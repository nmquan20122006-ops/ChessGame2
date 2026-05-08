#include "GameControl.h"
#include "Board.h"
#include "MoveExecutor.h"
#include "MoveService.h"
#include "StockfishAI.h" 
#include "Bot.h"
#include "ToFEN.h"
#include "EventBus.h"
#include "State/Move.hpp"

GameControl::GameControl(std::shared_ptr<Board> b, std::shared_ptr<GameState> s,
	std::shared_ptr<MoveService>& ms, std::unique_ptr<MoveExecutor>& me, MoveLog& l) : moveLog(l) {
	board = b;
	gameState = s;
	moveService = ms;
	moveExecutor = std::move(me);
	
	chessEngine = std::make_unique<StockfishGame>();

	subscribeToMove([this](const Move& move) {
		chessEngine->syncPosition(gameState->getCurrentFEN());
		});
}

GameControl::~GameControl() = default;

bool GameControl::requestMove(Position from, Position to) {

	if (!moveService->isValidMove(from, to, *board)) return false;

	Move move = moveService->createMove(from, to, *board);

	if (move.moveType == MoveType::promotion) {

		preparePromotion(from, to);
		return true;
	}
	else {

		moveExecutor->applyMove(move);
		finalizeMove(move);

		publishMoveEvent(move);
		return true;
	}
}

void GameControl::publishMoveEvent(Move& move) {

	auto& eventBus = EventBus::get();
	
	if (gameState->getIsCheckMate())						eventBus.publish(GameEvent::CheckMate);
	else if (gameState->getIsCheck())						eventBus.publish(GameEvent::Check);
	else if (move.moveType == MoveType::castle)				eventBus.publish(GameEvent::Castle);
	else if (move.moveType == MoveType::castleQueenSide)	eventBus.publish(GameEvent::CastleQueenSide);
	else if (move.moveType == MoveType::enPassant)			eventBus.publish(GameEvent::EnPassant);
	else if (move.moveType == MoveType::promotion)			eventBus.publish(GameEvent::Promotion);
	else if (move.moveType == MoveType::capture)			eventBus.publish(GameEvent::Capture);
	else if (move.moveType == MoveType::normal)				eventBus.publish(GameEvent::Move);
}

bool GameControl::requestAiMove(Position from, Position to, char promotionChar) {

	if (!moveService->isValidMove(from, to, *board))return false;

	Move move = moveService->createMove(from, to, *board);

	if (promotionChar != '\0' && move.moveType == MoveType::promotion) {
		move.promotionPiece = ToFEN::charToPiece(gameState->currentTurn, promotionChar);
	}

	moveExecutor->applyMove(move);

	finalizeMove(move);
	publishMoveEvent(move);

	return true;
}

void GameControl::preparePromotion(Position fromPos, Position toPos) {
	
	gameState->pendingFrom = fromPos;
	gameState->pendingTo = toPos;

	gameState->gameStatus = GameStatus::WaitingForPromotion;
}

void GameControl::executePromotionMove(Piece selectedPiece) {

	Position from = gameState->pendingFrom;
	Position to = gameState->pendingTo;

	Move move = moveService->createMove(from, to, *board);

	move.promotionPiece = selectedPiece;

	moveExecutor->applyMove(move);

	gameState->gameStatus = GameStatus::Normal;
	gameState->pendingFrom = { -1, -1 };
	gameState->pendingTo = { -1, -1 };

	finalizeMove(move);
	publishMoveEvent(move);

}

void GameControl::finalizeMove(Move& move) {
	
	updateGameState();

	gameState->currentFEN = ToFEN::FullFEN(*board, move, *gameState);

	moveLog.record(move);

	EventBus::get().publish(GameEvent::MoveRecord);

	notifyMoveExecuted(move);
}

bool GameControl::executePlayerMove(Position from, Position to) {

	if (animationProvider) {

		gameState->setAnimating(true);

		Piece piece = board->getPiece(from);

		animationProvider(from, to, piece, [this, from, to]() {

			this->requestMove(from, to);

			gameState->setAnimating(false);
			});
	}
	else {
		this->requestMove(from, to);
		gameState->setAnimating(false);
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

void GameControl::updateGameState() {

	gameState->switchTurn();

	const auto& Turn = gameState->getCurrentTurn();

	gameState->setIsCheck(moveService->Check(*board, Turn));

	if (gameState->getIsCheck())gameState->setCheckPos(board->findKing(Turn == Color::white));

	gameState->setIsCheckMate(moveService->CheckMate(*board, Turn));
}

void GameControl::initStockfishGame() {

	chessEngine->init(L"stockfish.exe");

	chessEngine->setDifficulty(20);
	chessEngine->newGame(true);

	gameState->setAiState().isAiEnabled = true;
	gameState->setAiState().aiTurn = Color::black;

	gameState->setDualMode(false);
}

bool GameControl::executeAiMove(std::string& uciMove) {

	Position from, to;

	fromUCI(uciMove.substr(0, 2), from);
	fromUCI(uciMove.substr(2, 2), to);

	char promotionChar = (uciMove.length() == 5) ? uciMove[4] : '\0';

	Piece piece = board->getPiece(from);

	gameState->setAnimating(true);
	
	if (animationProvider) {

		animationProvider(from, to, piece, [this, from, to, promotionChar]() {

			this->requestAiMove(from, to, promotionChar);

			gameState->setAnimating(false);

			});
	}
	else {

		this->requestAiMove(from, to, promotionChar);

		gameState->setAnimating(false);

	}
	return true;
}

void GameControl::updateAiMove() {

	Color currentTurn = gameState->currentTurn;
	auto& ai = gameState->getAiState();

	if (!ai.isAiEnabled || currentTurn != ai.aiTurn) return;

	if (isBlocking())return;

	std::string uciMove = chessEngine->getPendingMove();

	if (!uciMove.empty()) {

		executeAiMove(uciMove);
	}
	else{
		chessEngine->startThinking(2000);
	}
}

void GameControl::syncAfterUndo(UndoEntry undoEntry) {

	moveLog.syncGame(undoEntry);
	chessEngine->syncPosition(gameState->getCurrentFEN());

}

bool GameControl::executeUndoMove() {

	if (moveLog.getUndoStack().empty() || gameState->getAnimating() == true)return false;

	gameState->setAnimating(true);

	UndoEntry lastEntry = moveLog.popUndo();
	moveExecutor->undoCapturedPiece(lastEntry.moveBefore);

	Position toPos = lastEntry.moveBefore.toPos;
	Position fromPos = lastEntry.moveBefore.fromPos;

	Piece pieceToMoveBack = board->getPiece(toPos);

	if (animationProvider) {
		animationProvider(toPos, fromPos, pieceToMoveBack, [this, lastEntry]() {
			
			this->syncAfterUndo(lastEntry);
			moveLog.undoHistory();

			EventBus::get().publish(GameEvent::Undo);

			gameState->setAnimating(false);

			if (gameState->getAiState().isAiEnabled &&

				gameState->currentTurn == gameState->getAiState().aiTurn) {

				this->executeUndoMove();
				
			}
			});
	}
	else {
		this->syncAfterUndo(lastEntry);

		EventBus::get().publish(GameEvent::Undo);

		if (gameState->getAiState().isAiEnabled &&
			gameState->currentTurn == gameState->getAiState().aiTurn) {

			this->executeUndoMove();
		}
		gameState->setAnimating(false);
	}
	return true;
}

void GameControl::resetGame() {

	board->resetBoard();

	board->initBoard();

	gameState->reset();

	chessEngine->newGame(true);

}

bool GameControl::isBlocking()const {

	return chessEngine->isThinking() || gameState->getAnimating() == true;
}

void GameControl::stopStockfish() {
	chessEngine->stopThinking();
}