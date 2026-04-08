#include"MoveExecutor.h"

MoveExecutor::MoveExecutor(Board& b):board(b){}

MoveType MoveExecutor::implementPromotion(Move& move, Piece& p) {

	if (p == Piece::W_Pawn && move.toPos.row == 0) {
		p = Piece::W_Queen;
	}
	else if (p == Piece::B_Pawn && move.toPos.row == 7) {
		p = Piece::B_Queen;
	}

	return MoveType::promotion;
}

MoveType MoveExecutor::implementCastle(Move& move, Piece& p) {

	if (p != Piece::W_King && p != Piece::B_King)return MoveType::none;

	if (std::abs(move.toPos.col - move.fromPos.col) != 2) return MoveType::none;

	bool isWhitePiece = isWhite(p);
	int row = move.fromPos.row;

	if (move.toPos.col == 6) {
		Position oldRookPos = { row, 7 };
		Position newRookPos = { row, 5 };
		Piece rook = board.getPiece(oldRookPos);

		board.setPiece(newRookPos, rook);
		board.setPiece(oldRookPos, Piece::Empty);

		board.updatePosInVector(oldRookPos, newRookPos, isWhitePiece);


		board.updateCastleState(p, move.fromPos);
		board.updateCastleState(rook, oldRookPos);
	}

	else if (move.toPos.col == 2) {
		Position oldRookPos = { row, 0 };
		Position newRookPos = { row, 3 };
		Piece rook = board.getPiece(oldRookPos);
		Piece pieceAtTarget = board.getPiece(newRookPos);
		board.setPiece(newRookPos, rook);
		board.setPiece(oldRookPos, Piece::Empty);

		board.updatePosInVector(oldRookPos, newRookPos, isWhitePiece);

		board.updateCastleState(p, move.fromPos);
		board.updateCastleState(rook, oldRookPos);
	}

	return MoveType::castle;
}

MoveType MoveExecutor::implementEnPassant(Move& move, Piece& piece) {
   
    if (getType(piece) != Piece::W_Pawn && getType(piece) != Piece::B_Pawn)
        return MoveType::none;

    if (move.fromPos.col == move.toPos.col)
        return MoveType::none;


    if (!board.isEmpty(move.toPos))
        return MoveType::none;

    LastMove lastMove = board.getLastMove();
    if (lastMove.from.row == -1) return MoveType::none; 

    Piece lastPiece = board.getPiece(lastMove.to);
    if (getType(lastPiece) != Piece::W_Pawn && getType(lastPiece) != Piece::B_Pawn)
        return MoveType::none;

    if (std::abs(lastMove.from.row - lastMove.to.row) != 2)
        return MoveType::none;

    if (lastMove.to.row != move.fromPos.row) return MoveType::none;
    if (std::abs(lastMove.to.col - move.fromPos.col) != 1) return MoveType::none;

    int direction = isWhite(piece) ? -1 : 1;
    if (move.toPos.row != lastMove.to.row + direction) return MoveType::none;
    if (move.toPos.col != lastMove.to.col) return MoveType::none;

    Position capturedPos = lastMove.to; 

    board.setPiece(capturedPos, Piece::Empty);

    bool isWhiteCaptured = isWhite(lastPiece);

    return MoveType::enPassant;
}

void MoveExecutor::applyMove(Move& move) {

	Piece piece = board.getPiece(move.fromPos);
	Piece targetPiece = board.getPiece(move.toPos);

	MoveType enPassantResult = implementEnPassant(move, piece);

	bool isWhiteMoving = isWhite(piece);

	board.movePiece(move.fromPos, move.toPos);

	board.updateCastleState(piece, move.fromPos);
	
	implementCastle(move, piece);
	implementPromotion(move, piece);

}