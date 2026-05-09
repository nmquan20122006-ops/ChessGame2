#include"Board.h"

Board::Board() :m_board{} { init(); };

Board::Board(const Board& other) {

	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			this->m_board[row][col] = other.m_board[row][col];
		}
	}

	this->m_castleState = other.m_castleState;
	this->m_lastMove = other.m_lastMove;
	
	this->m_whiteKingPos = other.m_whiteKingPos;
	this->m_blackKingPos = other.m_blackKingPos;
}

void Board::init() {

	reset();

	Piece majorW_Piece[] = { Piece::W_Rook,Piece::W_Knight,Piece::W_Bishop,Piece::W_Queen,Piece::W_King,Piece::W_Bishop,Piece::W_Knight,Piece::W_Rook };


	for (int col = 0; col < BOARD_SIZE; col++) {

		setPiece({ 1,col }, Piece::B_Pawn);

		setPiece({ 6,col }, Piece::W_Pawn);

		setPiece({ 0, col }, getBlackPiece(majorW_Piece[col]));  // Hàng 0: quân đen

		setPiece({ 7, col }, majorW_Piece[col]);                 // Hàng 7: quân trắng

		if (majorW_Piece[col] == Piece::W_King) {

			m_whiteKingPos = { 7, col };
			m_blackKingPos = { 0, col };
		}
	}

	for (int r = 0; r < BOARD_SIZE; r++) {
		for (int c = 0; c < BOARD_SIZE; c++) {

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

	m_board[position.row][position.col] = piece;

}

bool Board::isInside(Position position) const {

	return (position.row >= 0 && position.row < BOARD_SIZE && position.col >= 0 && position.col < BOARD_SIZE);
}

Piece Board::getPiece(Position position) const {

	if (!isInside(position))return Piece::Empty;

	return m_board[position.row][position.col];
}

bool Board::isEmpty(Position position) const {

	if (!isInside(position))return false;

	return m_board[position.row][position.col] == Piece::Empty;
}

void Board::deletePiece(Position position) {

	if (!isInside(position))return;

	m_board[position.row][position.col] = Piece::Empty;
}

void Board::reset() {

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {

			m_board[i][j] = Piece::Empty;

		}
	}

	m_castleState.reset();
	m_lastMove.reset();
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

	m_lastMove = move;
}

Position Board::findKing(bool isWhiteKing) const {

	return isWhiteKing ? m_whiteKingPos : m_blackKingPos;
}

void Board::updateCastleState(Piece p, Position fromPos, Position toPos, Piece capturedPiece) {

	if (p == Piece::W_King) {
		m_castleState.whiteKingMoved = true;
		m_castleState.whiteKingRookMoved = true;
		m_castleState.whiteQueenRookMoved = true;
		return;
	}
	if (p == Piece::B_King) {
		m_castleState.blackKingMoved = true;
		m_castleState.blackKingRookMoved = true;
		m_castleState.blackQueenRookMoved = true;
		return;
	}

	if (p == Piece::W_Rook) {
		if (fromPos.row == 7) {

			if (fromPos.col == 0) m_castleState.whiteQueenRookMoved = true;
			if (fromPos.col == 7) m_castleState.whiteKingRookMoved = true;
		}

	}

	if (p == Piece::B_Rook) {
		if (fromPos.row == 0) {

			if (fromPos.col == 0) m_castleState.blackQueenRookMoved = true;
			if (fromPos.col == 7) m_castleState.blackKingRookMoved = true;
		}

	}

	if (capturedPiece == Piece::W_Rook) {

		if (toPos.row == 7) {

			if (toPos.col == 0) m_castleState.whiteQueenRookMoved = true;
			if (toPos.col == 7) m_castleState.whiteKingRookMoved = true;
		}
	}

	if (capturedPiece == Piece::B_Rook) {

		if (toPos.row == 0) {
			
			if (toPos.col == 0) m_castleState.blackQueenRookMoved = true;
			if (toPos.col == 7) m_castleState.blackKingRookMoved = true;
		}
	}
}

void Board::updateKingPosition(Position toPos,const Piece &p) {

	if (p == Piece::W_King) m_whiteKingPos = toPos;
	else if (p == Piece::B_King) m_blackKingPos = toPos;

}

void Board::setLastMove(Position fromPos, Position toPos) {

	m_lastMove.from = fromPos;
	m_lastMove.to = toPos;
}

