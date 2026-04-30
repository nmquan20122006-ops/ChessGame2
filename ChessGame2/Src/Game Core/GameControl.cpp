#include "GameControl.h"
#include "Board.h"
#include "MoveExecutor.h"
#include "MoveService.h"
#include "StockfishAI.h" 
#include "Bot.h"
#include "ToFEN.h"

GameControl::GameControl(std::shared_ptr<Board> b, std::shared_ptr<GameState> s,
	std::unique_ptr<MoveService>& ms, std::unique_ptr<MoveExecutor>& me) {

	board = b;
	gameState = s;
	moveService = std::move(ms);
	moveExecutor = std::move(me);
	
	stockfishGame = std::make_unique<StockfishGame>(board);

	m_isAnimating = gameState->isAnimating;

	m_onMoveExecutedListeners.push_back([this](const Move& move) {

		std::string fen = gameState->currentFEN;
		this->stockfishGame->syncMove(fen);

		});
}

bool GameControl::requestMove(Position from, Position to) {

	//---before move

	if (!moveService->isValidMove(from, to, *board)) return false;

	//create move object
	Move move = moveService->createMove(from, to, *board);

	if (move.moveType == MoveType::promotion) {

		preparePromotion(from, to);

		return true;
	}
	//normal move
	else {

		//record the state before move for undo functionality
		moveExecutor->recordPrevBoard(move);

		//---after move
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
}

void GameControl::preparePromotion(Position fromPos, Position toPos) {
	
	gameState->pendingFrom = fromPos;
	gameState->pendingTo = toPos;

	gameState->gameStateEnum = GameStateEnum::waitingForPromotion;
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

	gameState->gameStateEnum = GameStateEnum::normal;
	gameState->pendingFrom = { -1, -1 };
	gameState->pendingTo = { -1, -1 };
}

void GameControl::finalizeMove(const Move& move) {
	//update half move clock and full move number
	gameState->halfMoveClockCount = halfMoveClockProcess(gameState->halfMoveClockCount, move);
	gameState->fullMoveNumberCount = fullMoveNumberProcess(gameState->fullMoveNumberCount, gameState->currentTurn);

	//update game state (check, checkmate, turn switch)
	updateGameState();

	//update FEN
	gameState->currentFEN = ToFEN::FullFEN(*board, move, *gameState);

	notifyMoveExecuted(move);
}

bool GameControl::executePlayerMove(Position from, Position to) {

	if (animationProvider) {

		m_isAnimating = true;

		Piece piece = board->getPiece(from);

		animationProvider(from, to, piece, [this, from, to]() {

			this->requestMove(from, to);

			m_isAnimating = false;
			});
	}
	else {
		this->requestMove(from, to);
	}

	return true;
}

void GameControl::handleSquareSelection(Position pos) {

	if (stockfishGame->isThinking())return;

	if (!board->isInside(pos))return;

	if (!gameState->isSelected) {

		Piece p = board->getPiece(pos);
		color pieceColor = gameState->getColor(p);

		if (pieceColor == gameState->currentTurn) {

			gameState->setSelectPos(pos);
			gameState->setValidMoves(moveService->getValidMoves(pos, *board));

			gameState->isSelected = true;
			return;
		}
		
	}

	else {

		Piece p = board->getPiece(pos);
		color pieceColor = gameState->getColor(p);

		if (pos == gameState->getSelectPos()) {

			deselect();

		}

		else if (p != Piece::Empty && pieceColor == gameState->currentTurn) {

			gameState->setSelectPos(pos);

			gameState->setValidMoves(moveService->getValidMoves(pos, *board));

		}

		else if (gameState->isPosInVector(pos)) {

			Position from = gameState->getSelectPos();
			Position to = pos;

			this->executePlayerMove(from, to);

			deselect();
		}

		else {

			deselect();

		}
	}
}

void GameControl::handlePress(Position pos, sf::Vector2f mousePos) {

	if (stockfishGame->isThinking())return;

	if (!board->isInside(pos))return;
	
	Piece p = board->getPiece(pos);
	color pieceColor = gameState->getColor(p);

	gameState->drag.isActive = true;
	gameState->drag.mousePosition = mousePos;

	if (p != Piece::Empty && pieceColor == gameState->currentTurn) {

		gameState->drag.draggingPiece = p;
		gameState->drag.isDragging = true;
		gameState->drag.fromPos = pos;
		gameState->drag.isActive = true;

	}

	else {

		gameState->drag.isDragging = false;
	}

}

void GameControl::handleMove(sf::Vector2f mousePos) {

	if (gameState->drag.isActive) {

		gameState->drag.mousePosition = mousePos;
	}
	
}

void GameControl::handleRelease(Position toPos) {

	if (!board->isInside(toPos))return;
	if (isBlocking()) return;

	gameState->drag.isActive = false;

	Position from = gameState->drag.fromPos;

	if (from != toPos) {

		if (gameState->hasSelection()&&from==gameState->selectedPos) {

			if (gameState->isPosInVector(toPos)) {

				requestMove(from, toPos);

				gameState->drag.reset();
				deselect();
			}
			else {

				deselect();
			}
			
		}
	}

	gameState->drag.reset();
}

void GameControl::notifyMoveExecuted(const Move& move) {

	for (auto& callBack : m_onMoveExecutedListeners) {
		
		if (callBack) {
			callBack(move);
		}
	}
}

void GameControl::notifyStateChanged(const GameStateEnum& newState) {

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

	if (gameState->isCheck)gameState->checkPos = board->findKing(Turn==color::white);

	gameState->isCheckMate = moveService->CheckMate(*board, Turn);
}

void GameControl::initStockfishGame() {

	stockfishGame->initAI(L"stockfish.exe");

	stockfishGame->setDifficulty(20);
	stockfishGame->newGame(true);

	gameState->setAiState().isAiEnabled = true;
	gameState->setAiState().aiTurn = color::black;

	gameState->setDualMode(false);
}

bool GameControl::executeAiMove(std::string& uciMove) {

	Position from, to;

	StockfishGame::fromUCI(uciMove.substr(0, 2), from);
	StockfishGame::fromUCI(uciMove.substr(2, 2), to);

	char promotionChar = (uciMove.length() == 5) ? uciMove[4] : '\0';

	Piece piece = board->getPiece(from);

	m_isAnimating = true;
	
	if (animationProvider) {

		animationProvider(from, to, piece, [this, from, to, promotionChar]() {

			this->requestAiMove(from, to, promotionChar);

			m_isAnimating = false;

			});
	}
	else {

		this->requestAiMove(from, to, promotionChar);

		m_isAnimating = false;

	}
	return true;
}

void GameControl::updateAiMove() {

	color currentTurn = gameState->currentTurn;
	auto& ai = gameState->getAiState();

	if (!ai.isAiEnabled || currentTurn != ai.aiTurn) return;

	if (isBlocking())return;

	std::string uciMove = stockfishGame->getPendingMove();

	if (!uciMove.empty()) {

		executeAiMove(uciMove);
	}
	else{

		stockfishGame->startThinking(1500);
	}
}

void GameControl::syncAfterUndo(const UndoEntry& undoEntry) {
	//Restore the board state, turn, FEN, half move clock, and full move number from the undo entry
	moveExecutor->syncAfterUndo(undoEntry);

	//update stockfish fen to match with the restored game state after undoing the move
	stockfishGame->syncMove(gameState->currentFEN);
}

bool GameControl::executeUndoMove() {

	if (gameState->undoStack.empty() || m_isAnimating)return false;

	m_isAnimating = true;

	UndoEntry lastEntry = gameState->undoStack.back();
	gameState->undoStack.pop_back();

	Position toPos = lastEntry.moveBefore.toPos;
	Position fromPos = lastEntry.moveBefore.fromPos;
	Piece pieceToMoveBack = board->getPiece(toPos);

	if (animationProvider) {
		animationProvider(toPos, fromPos, pieceToMoveBack, [this, lastEntry]() {
			
			this->syncAfterUndo(lastEntry);
			
			this->m_isAnimating = false;

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
		this->m_isAnimating = false;
	}
	return true;
}

int GameControl::halfMoveClockProcess(int prevClock, const Move& move) {

	if (move.movedPiece == Piece::B_Pawn || move.movedPiece == Piece::W_Pawn || move.capturedPiece != Piece::Empty) {
		return 0;
	}
	return prevClock + 1;
}

int GameControl::fullMoveNumberProcess(int prevClock, const color currentTurn) {

	if (currentTurn == color::black)return prevClock + 1;
	return prevClock;
}

void GameControl::resetGame() {

	board->resetBoard();

	board->initBoard();

	gameState->reset();

	stockfishGame->newGame(true);

}

bool GameControl::isBlocking()const {

	return stockfishGame->isThinking() || m_isAnimating || m_isUndoing;
}