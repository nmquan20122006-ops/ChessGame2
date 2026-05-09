#include "GameControl.h"
#include "Board.h"
#include "MoveExecutor.h"
#include "MoveService.h"
#include "StockfishAI.h" 
#include "Bot.h"
#include "ToFEN.h"
#include "EventBus.h"
#include "State/Move.hpp"
#include "PromotionController.h"
#include"ChessEngineController.h"

GameControl::GameControl(
	std::shared_ptr<Board> b,
	std::shared_ptr<GameState> s,
	std::shared_ptr<MoveService>& ms,
	std::unique_ptr<MoveExecutor>& me,
	MoveLog& l) : 
	m_moveLog(l) {
	m_board = b;
	m_gameState = s;
	m_moveService = ms;
	m_moveExecutor = std::move(me);
	m_promotionController = std::make_unique<PromotionController>(m_board, m_gameState, m_moveService, *m_moveExecutor);
	m_chessEngineController = std::make_unique<ChessEngineController>(m_board, m_gameState, m_moveService);

	m_chessEngineController->setOnHintReady([this](Position from, Position to) {
		m_gameState->setHintPosition(from, to);
		});
	executeEngineMove();

	subscribeToMove([this](const Move& move) {
		m_chessEngineController->syncPosition(m_gameState->getCurrentFEN());
		});
}

GameControl::~GameControl() = default;

bool GameControl::requestMove(Position from, Position to) {

	if (!m_moveService->isValidMove(from, to, *m_board)) return false;

	Move move = m_moveService->createMove(from, to, *m_board);

	if (move.moveType == MoveType::promotion) {

		m_promotionController->prepare(from,to);
		return true;
	}
	else {

		m_moveExecutor->applyMove(move);
		finalizeMove(move);

		publishMoveEvent(move);
		return true;
	}
}

void GameControl::publishMoveEvent(Move& move) {

	auto& eventBus = EventBus::get();
	
	if (m_gameState->getIsCheckMate())						eventBus.publish(GameEvent::CheckMate);
	else if (m_gameState->getIsCheck())						eventBus.publish(GameEvent::Check);
	else if (move.moveType == MoveType::castle)				eventBus.publish(GameEvent::Castle);
	else if (move.moveType == MoveType::castleQueenSide)	eventBus.publish(GameEvent::CastleQueenSide);
	else if (move.moveType == MoveType::enPassant)			eventBus.publish(GameEvent::EnPassant);
	else if (move.moveType == MoveType::promotion)			eventBus.publish(GameEvent::Promotion);
	else if (move.moveType == MoveType::capture)			eventBus.publish(GameEvent::Capture);
	else if (move.moveType == MoveType::normal)				eventBus.publish(GameEvent::Move);
}

void GameControl::executePromotionMove(Piece selectedPiece) {

	Move move = m_promotionController->execute(selectedPiece);

	finalizeMove(move);
	publishMoveEvent(move);

}

void GameControl::finalizeMove(Move& move) {
	
	updateGameState();

	m_gameState->setCurrentFEN(ToFEN::FullFEN(*m_board, move, *m_gameState));
	m_moveLog.record(move);

	EventBus::get().publish(GameEvent::MoveRecord);

	notifyMoveExecuted(move);
}

bool GameControl::executePlayerMove(Position from, Position to) {

	if (m_animationProvider) {

		m_gameState->setAnimating(true);

		Piece piece = m_board->getPiece(from);

		m_animationProvider(from, to, piece, [this, from, to]() {

			this->requestMove(from, to);

			m_gameState->setAnimating(false);
			});
	}
	else {
		this->requestMove(from, to);
		m_gameState->setAnimating(false);
	}

	return true;
}

void GameControl::notifyMoveExecuted(const Move& move) {

	for (auto& callBack : m_onMoveExecutedListeners) {
		
		if (callBack) {
			callBack(move);
		}
	}
}

void GameControl::notifyStateChanged(const GameStatus& newState) {

	for (auto& callBack : m_onGameStateChangedListeners) {

		if (callBack) {
			callBack(newState);
		}
	}
}

void GameControl::executeEngineMove() {

	m_chessEngineController->setOnMoveReady([this](Move& move) {

		if (m_animationProvider) {

			m_gameState->setAnimating(true);
			Piece piece = m_board->getPiece(move.fromPos);

			m_animationProvider(move.fromPos, move.toPos, piece,
				[this, move]() mutable {
					m_moveExecutor->applyMove(move);
					finalizeMove(move);
					publishMoveEvent(move);
					m_gameState->setAnimating(false);
				});
		}
		else {
			m_moveExecutor->applyMove(move);
			finalizeMove(move);
			publishMoveEvent(move);
		}
		});
}

void GameControl::executeHint() {
	m_chessEngineController->requestHint();
}

void GameControl::updateGameState() {

	m_gameState->switchTurn();

	m_gameState->clearHintPosition();

	const auto& Turn = m_gameState->getCurrentTurn();

	m_gameState->setIsCheck(m_moveService->Check(*m_board, Turn));

	if (m_gameState->getIsCheck())m_gameState->setCheckPos(m_board->findKing(Turn == Color::white));

	m_gameState->setIsCheckMate(m_moveService->CheckMate(*m_board, Turn));
}

void GameControl::initChessEngine(EngineConfig& config) {

	m_chessEngineController->init(config);
}

void GameControl::updateChessEngineMove() {

	m_chessEngineController->update();
}

void GameControl::syncAfterUndo(UndoEntry undoEntry) {

	m_moveLog.syncGame(undoEntry);
	m_chessEngineController->syncPosition(m_gameState->getCurrentFEN());

}

bool GameControl::executeUndoMove() {

	if (m_moveLog.getUndoStack().empty() || m_gameState->getAnimating() == true)return false;

	m_gameState->setAnimating(true);

	UndoEntry lastEntry = m_moveLog.popUndo();
	m_moveExecutor->undoCapturedPiece(lastEntry.moveBefore);

	Position toPos = lastEntry.moveBefore.toPos;
	Position fromPos = lastEntry.moveBefore.fromPos;

	Piece pieceToMoveBack = m_board->getPiece(toPos);

	if (m_animationProvider) {
		m_animationProvider(toPos, fromPos, pieceToMoveBack, [this, lastEntry]() {
			
			this->syncAfterUndo(lastEntry);
			m_moveLog.undoHistory();

			EventBus::get().publish(GameEvent::Undo);

			m_gameState->setAnimating(false);

			if (m_gameState->getEngineConfig().enabled &&

				m_gameState->getCurrentTurn() == m_gameState->getEngineConfig().turn) {

				this->executeUndoMove();
				
			}
			});
	}
	else {
		this->syncAfterUndo(lastEntry);

		EventBus::get().publish(GameEvent::Undo);

		if (m_gameState->getEngineConfig().enabled &&
			m_gameState->getCurrentTurn() == m_gameState->getEngineConfig().turn) {

			this->executeUndoMove();
		}
		m_gameState->setAnimating(false);
	}
	return true;
}

bool GameControl::isBlocking() const {

	return m_chessEngineController->isThinking() || m_gameState->getAnimating();
}

void GameControl::resetGame() {

	m_board->reset();

	m_board->init();

	m_gameState->reset();

	m_chessEngineController->newGame();

}