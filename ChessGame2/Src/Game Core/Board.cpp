#include"Board.h"

Board::Board() :board{} { initBoard(); };

Board::Board(const Board& other) {

	for (int row = 0; row < boardSize; row++) {
		for (int col = 0; col < boardSize; col++) {
			this->board[row][col] = other.board[row][col];
		}
	}

	this->castleState = other.castleState;
	this->lastMove = other.lastMove;
	
	this->whiteKingPos = other.whiteKingPos;
	this->blackKingPos = other.blackKingPos;
}

void Board::initBoard() {

	resetBoard();

	Piece majorW_Piece[] = { Piece::W_Rook,Piece::W_Knight,Piece::W_Bishop,Piece::W_Queen,Piece::W_King,Piece::W_Bishop,Piece::W_Knight,Piece::W_Rook };


	for (int col = 0; col < boardSize; col++) {

		setPiece({ 1,col }, Piece::B_Pawn);

		setPiece({ 6,col }, Piece::W_Pawn);

		setPiece({ 0, col }, getBlackPiece(majorW_Piece[col]));  // Hàng 0: quân đen

		setPiece({ 7, col }, majorW_Piece[col]);                 // Hàng 7: quân trắng

		if (majorW_Piece[col] == Piece::W_King) {

			whiteKingPos = { 7, col };
			blackKingPos = { 0, col };
		}
	}

	for (int r = 0; r < boardSize; r++) {
		for (int c = 0; c < boardSize; c++) {

			Position pos = { r,c };
			Piece p = getPiece(pos);
			if (p == Piece::Empty) {
				continue;
			}

		}
	}
}

void Board::setPiece(Position position,Piece piece) {

	if (!isInside(position))return;

	board[position.row][position.col] = piece;

}

bool Board::isInside(Position position) const {

	return (position.row >= 0 && position.row < boardSize && position.col >= 0 && position.col < boardSize);
}

Piece Board::getPiece(Position position) const {

	if (!isInside(position))return Piece::Empty;

	return board[position.row][position.col];
}

bool Board::isEmpty(Position position) const {

	if (!isInside(position))return false;

	return board[position.row][position.col] == Piece::Empty;
}

void Board::deletePiece(Position position) {

	if (!isInside(position))return;

	board[position.row][position.col] = Piece::Empty;
}

void Board::resetBoard() {

	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {

			board[i][j] = Piece::Empty;

		}
	}

	castleState.reset();
	lastMove.reset();
}

void Board::movePiece(Position fromPos, Position toPos) {

	Piece movingPiece = getPiece(fromPos);

	bool isWhiteMoving = isWhite(movingPiece);

	setPiece(toPos, movingPiece);

	deletePiece(fromPos);

	updateKingPosition(toPos, movingPiece);

	updateLastMove({ fromPos, toPos });

}

void Board::updateLastMove(LastMove move) {

	lastMove = move;
}

Position Board::findKing(bool isWhiteKing) const {

	return isWhiteKing ? whiteKingPos : blackKingPos;
}

void Board::updateCastleState(Piece p, Position fromPos, Position toPos, Piece capturedPiece) {

	if (p == Piece::W_King) {
		castleState.whiteKingMoved = true;
		castleState.whiteKingRookMoved = true;
		castleState.whiteQueenRookMoved = true;
		return;
	}
	if (p == Piece::B_King) {
		castleState.blackKingMoved = true;
		castleState.blackKingRookMoved = true;
		castleState.blackQueenRookMoved = true;
		return;
	}

	if (p == Piece::W_Rook) {
		if (fromPos.row == 7) {

			if (fromPos.col == 0) castleState.whiteQueenRookMoved = true;
			if (fromPos.col == 7) castleState.whiteKingRookMoved = true;
		}

	}

	if (p == Piece::B_Rook) {
		if (fromPos.row == 0) {

			if (fromPos.col == 0) castleState.blackQueenRookMoved = true;
			if (fromPos.col == 7) castleState.blackKingRookMoved = true;
		}

	}

	if (capturedPiece == Piece::W_Rook) {

		if (toPos.row == 7) {

			if (toPos.col == 0)castleState.whiteQueenRookMoved = true;
			if (toPos.col == 7)castleState.whiteKingRookMoved = true;
		}
	}

	if (capturedPiece == Piece::B_Rook) {

		if (toPos.row == 0) {
			
			if (toPos.col == 0)castleState.blackQueenRookMoved = true;
			if (toPos.col == 7)castleState.blackKingRookMoved = true;
		}
	}
}

void Board::updateKingPosition(Position toPos,const Piece &p) {

	if (p == Piece::W_King) whiteKingPos = toPos;
	else if (p == Piece::B_King) blackKingPos = toPos;

}

void Board::setLastMove(Position fromPos, Position toPos) {

	lastMove.from = fromPos;
	lastMove.to = toPos;
}

