#include"InputHandler.h"
#include"Board.h"
#include"MoveService.h"
#include"EventBus.h"
#include"PromotionController.h"
#include"GameControl.h"

InputHandler::InputHandler(std::shared_ptr<Board>b, std::shared_ptr<GameState>s, std::shared_ptr<MoveService> m):
	m_gameState(s), m_board(b), m_moveService(m) {}

bool InputHandler::isCurrentTurnPiece(Piece p)const {

	if (p == Piece::Empty)return false;
	return m_gameState->getColor(p) == m_gameState->getCurrentTurn();
}

void InputHandler::select(Position pos) {

	m_gameState->setSelectPos(pos);
	m_gameState->setIsSelected(true);
	m_gameState->setValidMoves(m_moveService->getValidMoves(pos, *m_board));
}

void InputHandler::deselect() {

	m_gameState->clearSelection();
	m_gameState->setIsSelected(false);
}

void InputHandler::handleSquareSelection(Position pos) {

	if (isBlocking())return;
	if (!m_board->isInside(pos))return;

	Piece piece = m_board->getPiece(pos);
	bool isOwnPiece = isCurrentTurnPiece(piece);

	if (!m_gameState->hasSelection()) {

		if (isOwnPiece) {
			select(pos);
			EventBus::get().publish(GameEvent::Select);
		}
		return;
	}

	if (pos == m_gameState->getSelectPos()) {
		deselect();
		return;
	}

	if (isOwnPiece) {
		select(pos);
		return;
	}

	if (m_gameState->isPosInVector(pos)) {
		requestClickMove(m_gameState->getSelectPos(), pos);
		deselect();
		return;
	}

	EventBus::get().publish(GameEvent::UnValidMove);
	
	deselect();
}

void InputHandler::handlePress(Position pos, sf::Vector2f mousePos) {

	if (isBlocking())return;
	if (!m_board->isInside(pos))return;

	Piece piece = m_board->getPiece(pos);
	bool isOwnPiece = isCurrentTurnPiece(piece);

	auto& drag = m_gameState->setDragState();
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

void InputHandler::handleMove(sf::Vector2f mousePos) {

	auto& drag = m_gameState->setDragState();
	if (drag.isDragging) {
		drag.mousePosition = mousePos;
	}

}

void InputHandler::handleRelease(Position toPos) {

	if (isBlocking())return;
	if (!m_board->isInside(toPos))return;

	auto& drag = m_gameState->setDragState();

	drag.isDragging = false;

	Position fromPos = drag.fromPos;
	
	bool isDifferentSquare = (fromPos != toPos);
	bool isSelectionMatch = m_gameState->hasSelection() && fromPos == m_gameState->getSelectPos();
	bool isTarget = m_gameState->isPosInVector(toPos);

	if (isDifferentSquare && isSelectionMatch && isTarget) {
		requestDragMove(fromPos, toPos);
		drag.reset();
		deselect();
		return;
	} 
	if (isDifferentSquare && isSelectionMatch && !isTarget) {
		EventBus::get().publish(GameEvent::UnValidMove);
	}

	drag.reset();
}

void InputHandler::handleClickPromotionPanel(sf::Vector2f worldPos) {

	int col = static_cast<int>((worldPos.x - OFFSET) / SQUARE_SIZE);
	int row = static_cast<int>((worldPos.y - OFFSET) / SQUARE_SIZE);

	Position pendingTo = m_onGetPendingTo();
	Color turn = m_gameState->getCurrentTurn();

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

			m_onPromotionSelected(selected);
		}
		else {

			//logic->cancelPromotion();
		}
	}
	else {

		//logic->cancelPromotion();
	}
}