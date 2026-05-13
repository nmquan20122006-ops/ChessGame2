#include"ChessEngine/ChessEngineController.h"
#include"ChessEngine/ChessEngineInterface.h"
#include"MoveService.h"
#include"Board.h"
#include"ChessEngine/StockfishAI.h"
#include"State/GameState.hpp"

ChessEngineController::ChessEngineController(
	std::shared_ptr<Board>			board,
	std::shared_ptr<GameState>		gameState,
	std::shared_ptr<MoveService>	moveService)
	: m_board(board), m_gameState(gameState), m_moveService(moveService)
{
	m_chessEngine = std::make_unique<StockfishGame>();
	m_engineThinkingMode = EngineThinkingMode::IDLE;
}

void ChessEngineController::init(EngineConfig& config) {

	m_engineConfig = config;

	m_chessEngine->init(m_engineConfig.Path);
	m_chessEngine->setDifficulty(m_engineConfig.difficulty);
	m_chessEngine->newGame(true);
	m_gameState->setEngineConfig().enabled = true;
	m_gameState->setEngineConfig().turn = m_engineConfig.turn;
	
}

void ChessEngineController::newGame() { m_chessEngine->newGame(true); }

void ChessEngineController::stop() { m_chessEngine->stopThinking(); }

void ChessEngineController::syncPosition(const std::string& fen) { m_chessEngine->syncPosition(fen); }

void ChessEngineController::setEngine(std::unique_ptr<IChessEngine>engine) {

	if (m_chessEngine->isThinking())m_chessEngine->stopThinking();
	m_chessEngine = std::move(engine);
}

bool ChessEngineController::isThinking() const {
	return m_chessEngine && m_chessEngine->isThinking();
}

bool ChessEngineController::isEnable()const {
	return m_gameState->getEngineConfig().enabled;
}

void ChessEngineController::updateEngineMove() {

	const auto& config = m_gameState->getEngineConfig();
	if (!config.enabled)								return;
	if (config.turn != m_gameState->getCurrentTurn())	return;
	if (m_gameState->getAnimating())					return;
	if (m_chessEngine->isThinking())					return;

	std::string uciMove = m_chessEngine->getPendingMove();

	if (uciMove.empty()) {
		m_chessEngine->goMoveTime(config.thinkingMs);
	}
	else {
		processMove(uciMove);
	}
}

void ChessEngineController::updateEngineHint() {

	const auto& config = m_gameState->getEngineConfig();
	if (m_engineThinkingMode != EngineThinkingMode::HINT)return;
	if (!config.enabled)								 return;
	if (config.turn == m_gameState->getCurrentTurn())	 return;
	if (m_gameState->getAnimating())					 return;
	if (m_chessEngine->isThinking())					 return;

	std::string uciMove = m_chessEngine->getPendingMove();

	if (!uciMove.empty()) {
		processHint(uciMove);
		m_engineThinkingMode = EngineThinkingMode::IDLE;
	}
}

void ChessEngineController::updateEngineEnemyBestMove() {

	const auto& engine = m_gameState->getEngineConfig();
	if (m_engineThinkingMode != EngineThinkingMode::ENEMY_BEST_MOVE_HINT)return;
	if (!engine.enabled)								 return;
	if (engine.turn == m_gameState->getCurrentTurn())	 return;
	if (m_gameState->getAnimating())					 return;
	if (m_chessEngine->isThinking())					 return;

	std::string uciMove = m_chessEngine->getPendingMove();

	if (!uciMove.empty()) {
		processEnemyBestMove(uciMove);
		m_engineThinkingMode = EngineThinkingMode::IDLE;
	}
}

void ChessEngineController::update() {
	updateEngineHint();
	updateEngineMove();
	updateEngineEnemyBestMove();
}

void ChessEngineController::requestEnemyBestMove() {

	if (m_chessEngine->isThinking())						 return;
	if (m_gameState->getCurrentTurn() == m_engineConfig.turn)return;
	if (m_gameState->getAnimating())						 return;

	m_engineThinkingMode = EngineThinkingMode::ENEMY_BEST_MOVE_HINT;
	m_chessEngine->syncPosition(m_gameState->getOppositeFEN());
	m_chessEngine->goDepth(m_engineConfig.goDepth);

}

void ChessEngineController::requestHint() {

	if (m_chessEngine->isThinking())						 return;
	if (m_gameState->getCurrentTurn() == m_engineConfig.turn)return;
	if (m_gameState->getAnimating())						 return;

	m_chessEngine->syncPosition(m_gameState->getCurrentFEN());
	m_engineThinkingMode = EngineThinkingMode::HINT;
	m_chessEngine->goDepth(m_engineConfig.goDepth);
}

void ChessEngineController::processHint(const std::string& uciMove) {
	if (uciMove.length() < 4)return;

	Position from, to;
	fromUCI(uciMove.substr(0, 2), from);
	fromUCI(uciMove.substr(2, 2), to);

	if (m_onHintReady) m_onHintReady(from, to);
}

void ChessEngineController::processEnemyBestMove(const std::string& uciMove) {

	if (uciMove.length() < 4)return;

	Position from, to;
	fromUCI(uciMove.substr(0, 2), from);
	fromUCI(uciMove.substr(2, 2), to);

	if (m_onEnemyBestMoveReady)m_onEnemyBestMoveReady(from, to);
}

EngineEvaluation ChessEngineController::getEngineEvaluation() const {
	return m_chessEngine->getEngineEvaluation();
}

void ChessEngineController::processMove(const std::string& uciMove) {
	Position from, to;
	fromUCI(uciMove.substr(0, 2), from);
	fromUCI(uciMove.substr(2, 2), to);

	char promotionChar = (uciMove.length() == 5) ? uciMove[4] : '\0';

	Move move = m_moveService->createMove(from, to, *m_board);

	if (promotionChar != '\0' && move.moveType == MoveType::promotion) {
		move.promotionPiece = charToPiece(m_gameState->getCurrentTurn(), promotionChar);
	}

	if (m_onMoveReady) m_onMoveReady(move);
}