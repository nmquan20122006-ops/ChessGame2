#include "GameControl.h"
#include "Board.h"
#include "MoveExecutor.h"
#include "MoveService.h"
#include "StockfishAI.h" 
#include "Bot.h"
#include "ToFEN.h"

GameControl::GameControl(std::shared_ptr<Board> b, std::shared_ptr<GameState> s,
	std::shared_ptr<MoveService>& ms, std::unique_ptr<MoveExecutor>& me) {
	board = b;
	gameState = s;
	moveService = ms;
	moveExecutor = std::move(me);
	
	stockfishGame = std::make_unique<StockfishGame>(board);

	subscribeToMove([this](const Move& move) {
		stockfishGame->syncMove(gameState->getCurrentFEN());
		});
}

GameControl::~GameControl() = default;

bool GameControl::requestMove(Position from, Position to) {
	//---before move
	if (!moveService->isValidMove(from, to, *board)) return false;
	//create move object
	Move move = moveService->createMove(from, to, *board);

	if (move.moveType == MoveType::promotion) {
		preparePromotion(from, to);
		return true;
	}
	else {
		moveExecutor->applyMove(move);
		finalizeMove(move);
		return true;
	}
}

bool GameControl::requestAiMove(Position from, Position to, char promotionChar) {

	if (!moveService->isValidMove(from, to, *board))return false;

	Move move = moveService->createMove(from, to, *board);

	if (promotionChar != '\0' && move.moveType == MoveType::promotion) {
		move.promotionPiece = ToFEN::charToPiece(gameState->currentTurn, promotionChar);
	}

	moveExecutor->recordPrevBoard(move);
	moveExecutor->applyMove(move);

	finalizeMove(move);

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

	//record the state before move for undo functionality
	moveExecutor->recordPrevBoard(move);

	moveExecutor->applyMove(move);

	updateGameState();

	gameState->gameStatus = GameStatus::Normal;
	gameState->pendingFrom = { -1, -1 };
	gameState->pendingTo = { -1, -1 };
}

void GameControl::finalizeMove(const Move& move) {
	//update game state (check, checkmate, turn switch)
	updateGameState();
	//update FEN
	gameState->currentFEN = ToFEN::FullFEN(*board, move, *gameState);
	//notify all the listeners
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

	auto& Turn = gameState->currentTurn;

	gameState->isCheck = moveService->Check(*board, Turn);

	if (gameState->isCheck)gameState->checkPos = board->findKing(Turn==Color::white);

	gameState->isCheckMate = moveService->CheckMate(*board, Turn);
}

void GameControl::initStockfishGame() {

	stockfishGame->initAI(L"stockfish.exe");

	stockfishGame->setDifficulty(20);
	stockfishGame->newGame(true);

	gameState->setAiState().isAiEnabled = true;
	gameState->setAiState().aiTurn = Color::black;

	gameState->setDualMode(false);
}

bool GameControl::executeAiMove(std::string& uciMove) {

	Position from, to;

	StockfishGame::fromUCI(uciMove.substr(0, 2), from);
	StockfishGame::fromUCI(uciMove.substr(2, 2), to);

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

	std::string uciMove = stockfishGame->getPendingMove();

	if (!uciMove.empty()) {

		executeAiMove(uciMove);
	}
	else{

		stockfishGame->startThinking(2000);
	}
}

void GameControl::syncAfterUndo(const UndoEntry& undoEntry) {
	//Restore the board state, turn, FEN, half move clock, and full move number from the undo entry
	moveExecutor->syncAfterUndo(undoEntry);

	//update stockfish fen to match with the restored game state after undoing the move
	stockfishGame->syncMove(gameState->currentFEN);
}

bool GameControl::executeUndoMove() {

	if (gameState->undoStack.empty() || gameState->getAnimating() == true)return false;

	gameState->setAnimating(true);

	UndoEntry lastEntry = gameState->undoStack.back();
	gameState->undoStack.pop_back();

	Position toPos = lastEntry.moveBefore.toPos;
	Position fromPos = lastEntry.moveBefore.fromPos;
	Piece pieceToMoveBack = board->getPiece(toPos);

	if (animationProvider) {
		animationProvider(toPos, fromPos, pieceToMoveBack, [this, lastEntry]() {
			
			this->syncAfterUndo(lastEntry);
			
			gameState->setAnimating(false);

			if (gameState->getAiState().isAiEnabled && gameState->currentTurn == gameState->getAiState().aiTurn) {
				this->executeUndoMove();
			}
			});
	}
	else {

		this->syncAfterUndo(lastEntry);
		if (gameState->getAiState().isAiEnabled && gameState->currentTurn == gameState->getAiState().aiTurn) {
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

	stockfishGame->newGame(true);

}

bool GameControl::isBlocking()const {

	return stockfishGame->isThinking() || gameState->getAnimating() == true;
}