#include"GameControl.h"
#include<iostream>

GameControl::GameControl(Board& b, MoveExecutor& m, MoveService& c,
	GameState& g, StockfishGame& s, DumpBot d) :

	board(b), moveExecutor(m),

	moveService(c), gameState(g), stockfishGame(s), dumpBot(d)
{

	m_onMoveExecutedListeners.push_back([this](const Move& move) {

		std::string uciMove = StockfishGame::moveToUCI(move.fromPos, move.toPos);
		this->stockfishGame.syncMove(uciMove);

		});
}

bool GameControl::requestMove(Position from, Position to) {

	if (!moveService.isValidMove(from, to, board)) return false;

	Move move = moveService.createMove(from, to, board);
	moveExecutor.applyMove(move);
	undoStack.push_back(move);
	redoStack.clear();

	notifyMoveExecuted(move);
	updateGameState();
	return true;
}

void GameControl::handleSquareSelection(Position pos) {

	if (stockfishGame.isThinking())return;

	if (!board.isInside(pos))return;

	if (!gameState.isSelected) {

		Piece p = board.getPiece(pos);
		color pieceColor = gameState.getColor(p);

		if (pieceColor == gameState.currentTurn) {

			gameState.setSelectPos(pos);
			gameState.setValidMoves(moveService.getValidMoves(pos, board));

			gameState.isSelected = true;
			return;
		}
		
	}

	else {

		Piece p = board.getPiece(pos);
		color pieceColor = gameState.getColor(p);

		if (pos == gameState.getSelectPos()) {

			deselect();

		}

		else if (p != Piece::Empty && pieceColor == gameState.currentTurn) {

			gameState.setSelectPos(pos);

			gameState.setValidMoves(moveService.getValidMoves(pos, board));

		}

		else if (gameState.isPosInVector(pos)) {

			Position from = gameState.getSelectPos();
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

	if (stockfishGame.isThinking())return;

	if (!board.isInside(pos))return;
	
	Piece p = board.getPiece(pos);
	color pieceColor = gameState.getColor(p);

	dragState.isActive = true;
	dragState.mousePosition = mousePos;

	if (p != Piece::Empty && pieceColor == gameState.currentTurn) {

		dragState.draggingPiece = p;
		dragState.isDragging = true;
		dragState.fromPos = pos;
		dragState.isActive = true;

	}

	else {

		dragState.isDragging = false;
	}

}

void GameControl::handleMove(sf::Vector2f mousePos) {

	if (dragState.isActive) {

		dragState.mousePosition = mousePos;
	}
	
}

void GameControl::handleRelease(Position toPos) {

	if (!board.isInside(toPos))return;
	if (isBlocking()) return;

	dragState.isActive = false;

	Position from = dragState.fromPos;

	if (from != toPos) {

		if (gameState.hasSelection()&&from==gameState.selectedPos) {

			if (gameState.isPosInVector(toPos)) {

				requestMove(from, toPos);

				dragState.reset();
				deselect();
			}
			else {

				deselect();
			}
			
		}
	}

	dragState.reset();
}

bool GameControl::executePlayerMove(Position from, Position to) {

	if (animationProvider) {

		m_isAnimating = true;

		Piece piece = board.getPiece(from);

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

bool GameControl::executeUndoMove() {

	if (undoStack.size() < 2 || isBlocking()) return false;

	Move move2 = undoStack.back();
	Move move1 = undoStack[undoStack.size() - 2];

	m_isAnimating = true;
	m_isUndoing = true;

	if (animationProvider) {

		animationProvider(move2.toPos, move2.fromPos, move2.movedPiece, [this, move1, move2]() {

			moveExecutor.undoMove(undoStack.back());
			redoStack.push_back(move2);        
			undoStack.pop_back();
			gameState.switchTurn();

			
			animationProvider(move1.toPos, move1.fromPos, move1.movedPiece, [this, move1]() {

				moveExecutor.undoMove(undoStack.back());
				redoStack.push_back(move1);     
				undoStack.pop_back();
				gameState.switchTurn();

				if (!undoStack.empty()) {
					Move currentLast = undoStack.back();
					board.setLastMove(currentLast.fromPos, currentLast.toPos);
				}
				else {
					board.resetLastMove();
				}

				auto uciHistory = stockfishGame.getMoveHistory();
				if (!uciHistory.empty()) uciHistory.pop_back();
				if (!uciHistory.empty()) uciHistory.pop_back();
				stockfishGame.syncFromHistory(uciHistory);

				m_isAnimating = false;
				m_isUndoing = false;
				});
			});
	}
	else {
		requestUndo();
	}
	return true;
}

void GameControl::executeRedoMove() {
	
}

void GameControl::requestUndo() {
	if (undoStack.empty()) {
		std::cout << "moveHistory EMPTY" << std::endl;
		return;
	}

	std::cout << "moveHistory size before undo: " << undoStack.size() << std::endl;

	int steps = (undoStack.size() >= 2) ? 2 : 1;

	for (int i = 0; i < steps; ++i) {
		if (undoStack.empty()) break;

		Move lastMove = undoStack.back();
		undoStack.pop_back();
		moveExecutor.undoMove(lastMove);
		redoStack.push_back(lastMove);

	}

	if (!undoStack.empty()) {
		Move& current = undoStack.back();
		board.setLastMove(current.fromPos, current.toPos);
	}
	else {
		board.resetLastMove();
	}

	gameState.switchTurn();

	auto uciHistory = stockfishGame.getMoveHistory();
	for (int i = 0; i < steps; ++i) {
		if (!uciHistory.empty()) uciHistory.pop_back();
	}
	stockfishGame.syncFromHistory(uciHistory);
}

void GameControl::requestRedo(){



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

	gameState.switchTurn();

	auto& Turn = gameState.currentTurn;

	gameState.isCheck = moveService.Check(board, Turn);

	if (gameState.isCheck)gameState.checkPos = board.findKing(Turn==color::white);

	gameState.isCheckMate = moveService.CheckMate(board, Turn);
}

void GameControl::updateAiMove() {

	color currentTurn = gameState.currentTurn;
	auto& ai = gameState.setAiState();

	if (!ai.isAiEnabled || currentTurn != ai.aiTurn) return;

	if (stockfishGame.isThinking() || isBlocking())return;


	std::string uciMove = stockfishGame.getPendingMove();

	if (!uciMove.empty()) {

		executeAiMove(uciMove);
	}
	else{

		stockfishGame.startThinking(1500);
	}
}

bool GameControl::executeAiMove(std::string& uciMove) {

	Position from, to;

	StockfishGame::fromUCI(uciMove.substr(0, 2), from);
	StockfishGame::fromUCI(uciMove.substr(2, 2), to);

	Piece piece = board.getPiece(from);

	m_isAnimating = true;
	
	if (animationProvider) {

		animationProvider(from, to, piece, [this, from, to, uciMove]() {

			this->requestMove(from, to);
		
			m_isAnimating = false;

			});
	}
	else {

		this->requestMove(from, to);

		
		m_isAnimating = false;

	}
	return true;
}

/*void GameControl::updateAiDualMode() {

	color currentTurn = gameState.currentTurn;
	auto& ai = gameState.setAiState();

	if (!ai.isAiEnabled)return;
	if (stockfishGame.isThinking())return;

	bool isAllowedToThink = gameState.isDualMode || (currentTurn == ai.aiTurn);
	if (!isAllowedToThink)return;

	if (currentTurn == color::white) {

		ai.isAiThinking = true;
		BotMove move = dumpBot.bot_makeMove(board);
		std::string uciMove = StockfishGame::moveToUCI(move.fromPos, move.toPos);
		executeAiMove(uciMove);
	}
	else {

		if (!stockfishGame.isThinking()) {

			std::string uciMove = stockfishGame.getPendingMove();

			if (!uciMove.empty()) {
				ai.isAiThinking = true;
				executeAiMove(uciMove);
			}
			else {
				stockfishGame.startThinking(1500);
			}
		}
	}
}*/

void GameControl::resetGame() {

	board.resetBoard();

	board.initBoard();

	gameState.reset();

	stockfishGame.newGame(true);

	resetMoveHistory();

}

void GameControl::resetMoveHistory() {

	undoStack.clear();

	std::cerr << "Reset Move History" << std::endl;
}

bool GameControl::isBlocking()const {

	return stockfishGame.isThinking() || m_isAnimating || m_isUndoing;
}