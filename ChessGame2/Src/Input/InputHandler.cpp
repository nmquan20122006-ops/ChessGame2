#include"InputHandler.h"
#include"Board.h"
#include"MoveService.h"
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
		if (isOwnPiece) Select(pos);
		
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

	drag.reset();
	
}