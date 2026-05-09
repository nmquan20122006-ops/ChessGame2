#include"PromotionController.h"
#include"MoveExecutor.h"
#include"MoveService.h"
#include"Board.h"

PromotionController::PromotionController(std::shared_ptr<Board> b, std::shared_ptr<GameState> g,
	std::shared_ptr<MoveService>m, MoveExecutor& r) : board(b), gameState(g), moveService(m), moveExecutor(r){ }

bool PromotionController::prepare(Position from, Position to) {
	gameState->setPendingFrom(from);
	gameState->setPendingTo(to);

	gameState->setGameStatus(GameStatus::WaitingForPromotion);
	return true;
}

Move PromotionController::execute(Piece selectedPiece) {

	Move move = moveService->createMove(gameState->getPendingFrom(), gameState->getPendingTo(), *board);

	moveExecutor.applyMove(move);

	gameState->setPendingFrom({-1,-1});
	gameState->setPendingTo({ -1,-1 });

	gameState->setGameStatus(GameStatus::Normal);

	return move;
}

bool PromotionController::isPending() const {
	return gameState->getGameStatus() == GameStatus::WaitingForPromotion;
}