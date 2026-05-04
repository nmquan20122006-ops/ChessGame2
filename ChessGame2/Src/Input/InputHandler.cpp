#include"InputHandler.h"
#include"Board.h"
#include"MoveService.h"
#include"EventBus.h"
#include<iostream>

InputHandler::InputHandler(std::shared_ptr<Board>b, std::shared_ptr<GameState>s, std::shared_ptr<MoveService> m):
	gameState(s),board(b), moveService(m) {}

bool InputHandler::IsCurrentTurnPiece(Piece p)const {

	if (p == Piece::Empty)return false;
	return gameState->getColor(p) == gameState->getCurrentTurn();
}

void InputHandler::Select(Position pos) {

	gameState->setSelectPos(pos);
	gameState->setIsSelected(true);
	gameState->setValidMoves(moveService->getValidMoves(pos, *board));
}

void InputHandler::Deselect() {

	gameState->clearSelection();
	gameState->setIsSelected(false);
}

void InputHandler::HandleSquareSelection(Position pos) {

	if (IsBlocking())return;
	if (!board->isInside(pos))return;

	Piece piece = board->getPiece(pos);
	bool isOwnPiece = IsCurrentTurnPiece(piece);

	if (!gameState->hasSelection()) {

		if (isOwnPiece) {
			Select(pos);
			EventBus::get().publish(GameEvent::Select);
		}
		return;
	}

	if (pos == gameState->getSelectPos()) {
		Deselect();
		return;
	}

	if (isOwnPiece) {
		Select(pos);
		return;
	}

	if (gameState->isPosInVector(pos)) {
		RequestClickMove(gameState->getSelectPos(), pos);
		Deselect();
		return;
	}

	EventBus::get().publish(GameEvent::UnValidMove);
	
	Deselect();
}

void InputHandler::HandlePress(Position pos, sf::Vector2f mousePos) {

	if (IsBlocking())return;
	if (!board->isInside(pos))return;

	Piece piece = board->getPiece(pos);
	bool isOwnPiece = IsCurrentTurnPiece(piece);

	auto& drag = gameState->setDragState();
	drag.isActive = true;
	drag.mousePosition= mousePos;

	if (isOwnPiece) {
		drag.draggingPiece = piece;
		drag.isDragging = true;
		drag.fromPos = pos;


	}
	else {
		drag.isDragging = false;
	}
}

void InputHandler::HandleMove(sf::Vector2f mousePos) {

	auto& drag = gameState->setDragState();
	if (drag.isDragging) {
		drag.mousePosition = mousePos;
	}

}

void InputHandler::HandleRelease(Position toPos) {

	if (IsBlocking())return;
	if (!board->isInside(toPos))return;

	auto& drag = gameState->setDragState();

	drag.isDragging = false;

	Position fromPos = drag.fromPos;
	
	bool isDifferentSquare = (fromPos != toPos);
	bool isSelectionMatch = gameState->hasSelection() && fromPos == gameState->getSelectPos();
	bool isTarget = gameState->isPosInVector(toPos);

	if (isDifferentSquare && isSelectionMatch && isTarget) {
		RequestDragMove(fromPos, toPos);
		drag.reset();
		Deselect();
		return;
	} 
	if (isDifferentSquare && isSelectionMatch && !isTarget) {
		EventBus::get().publish(GameEvent::UnValidMove);
	}

	drag.reset();
}

/*void InputHandler::HandleClickPromotionPanel(sf::Vector2f worldPos) {

	int col = static_cast<int>((worldPos.x - offset) / squareSize);
	int row = static_cast<int>((worldPos.y - offset) / squareSize);

	Position pendingTo = gameState->getPendingTo();
	Color turn = gameState->getCurrentTurn();

	if (col == pendingTo.col) {
		int startRow = (turn == Color::white) ? 0 : 4;
		int endRow = (turn == Color::white) ? 3 : 7;

		if (row >= startRow && row <= endRow) {

			int index = std::abs(row - startRow);

			Piece selected;
			if (turn == Color::white) {
				if (index == 0) selected = Piece::W_Queen;
				else if (index == 1) selected = Piece::W_Rook;
				else if (index == 2) selected = Piece::W_Bishop;
				else selected = Piece::W_Knight;
			}
			else {
				if (index == 0) selected = Piece::B_Queen;
				else if (index == 1) selected = Piece::B_Rook;
				else if (index == 2) selected = Piece::B_Bishop;
				else selected = Piece::B_Knight;
			}

			//send the selected promotion piece to game control to execute the promotion move
			gameControl->executePromotionMove(selected);
		}
		else {

			//logic->cancelPromotion();
		}
	}
	else {

		//logic->cancelPromotion();
	}
}*/