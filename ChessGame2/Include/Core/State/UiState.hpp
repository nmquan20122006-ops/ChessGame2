#include"Piece.h"
#include"Type.hpp"
#include"Utility.h"


// DragState struct to manage the state of piece dragging in the UI
struct DragState {

	bool isActive = false;
	bool isDragging = false;

	Position fromPos = { -1,-1 };

	Piece draggingPiece = Piece::Empty;

	sf::Vector2f mousePosition;

	void reset() {
		isActive = false;
		isActive = false;
		fromPos = { -1,-1 };
		draggingPiece = Piece::Empty;
	}
};