#include"Board.h"

Board::Board() :board{} { initBoard(); };

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

			if (isWhite(p)) {
				w_PieceList.push_back(pos);
			}
			else {
				b_PieceList.push_back(pos);
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
	w_PieceList.clear();
	b_PieceList.clear();

}

void Board::movePiece(Position fromPos, Position toPos) {

	Piece movingPiece = getPiece(fromPos);
	Piece targetPiece = getPiece(toPos);
	bool isWhiteMoving = isWhite(movingPiece);

	if (targetPiece != Piece::Empty) {
		bool isWhiteTarget = isWhite(targetPiece);
		removeFromVector(isWhiteTarget ? b_PieceList : w_PieceList, toPos);
	}

	updatePosInVector(fromPos, toPos, isWhiteMoving);

	updateKingPosition(toPos, movingPiece);

	setPiece(toPos, movingPiece);

	deletePiece(fromPos);

	updateLastMove({ fromPos, toPos });
}

void Board::updateLastMove(LastMove move) {
	lastMove = move;
}

Position Board::findKing(bool isWhiteKing) const {

	return isWhiteKing ? whiteKingPos : blackKingPos;
}

void Board::updateCastleState(Piece p, Position fromPos) {

	if (p == Piece::W_King) {
		castleState.whiteKingMoved = true;
		return;
	}
	else if (p == Piece::B_King) {
		castleState.blackKingMoved = true;
		return;
	}


	if (p == Piece::W_Rook) {
		if (fromPos.row == 7) {

			if (fromPos.col == 0) castleState.whiteQueenRookMoved = true;
			if (fromPos.col == 7) castleState.whiteKingRookMoved = true;
		}
		
	}
	else if (p == Piece::B_Rook) {
		if (fromPos.row == 0) {

			if (fromPos.col == 0) castleState.blackQueenRookMoved = true;
			if (fromPos.col == 7) castleState.blackKingRookMoved = true;
		}
		
	}
}

void Board::updateKingPosition(Position toPos,Piece &p) {

	if (p == Piece::W_King) whiteKingPos = toPos;
	else if (p == Piece::B_King) blackKingPos = toPos;

}

void Board::removeFromVector(std::vector<Position>& list, Position Pos) {

	for (size_t i = 0; i < list.size(); ++i) {

		if (list[i].row == Pos.row && list[i].col == Pos.col) {

			list[i] = list.back();
			list.pop_back();
			return;
		}
	}
}

void Board::updatePosInVector(Position oldPos, Position newPos, bool isWhite) {

	auto& list = isWhite ? w_PieceList : b_PieceList;
	for (auto& pos : list) {
		if (pos.row == oldPos.row && pos.col == oldPos.col) {
			pos = newPos;
			return;
		}
	}
}

const std::vector<Position>& Board::getPositionExist(bool isWhite) const{
	return isWhite ? w_PieceList : b_PieceList;
}