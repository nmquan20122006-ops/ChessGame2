#include"MoveValidator.h"
#include<cmath>

bool MoveValidator::checkPawn(Position fromPos, Position toPos, const Board& classBoard) {

	Piece pawn = classBoard.getPiece(fromPos);

	Piece targetPiece = classBoard.getPiece(toPos);

	int direction = isWhite(pawn) ? -1 : 1;

	int startRow = isWhite(pawn) ? 6 : 1;

	int colDiff = std::abs(toPos.col - fromPos.col);

	int nextMoveRow = fromPos.row + direction;

	int doubleMoveRow = fromPos.row + 2 * direction;

	//Pawn only move straight
	if (fromPos.col == toPos.col) {

		//take a step forward
		if (toPos.row == nextMoveRow) {

			return classBoard.isEmpty(toPos);
		}

		//take double step forward
		if (fromPos.row == startRow && toPos.row == doubleMoveRow) {

			Position midPos = { fromPos.row + direction,fromPos.col };

			return classBoard.isEmpty(midPos) && classBoard.isEmpty(toPos);
		}
	}
	
	//overridde other piece
	else if (colDiff == 1 && toPos.row == nextMoveRow) {

		//want override an other piece you need toPos isn't empty and they's color is opposite!

		return targetPiece !=Piece::Empty && isOpponent(pawn, targetPiece);
	}

	return false;
}

bool MoveValidator::isPathClear(Position fromPos, Position toPos, const Board& classBoard) {

	int rowStep = (toPos.row > fromPos.row) ? 1 : (toPos.row < fromPos.row ? -1 : 0);
	
	int colStep = (toPos.col > fromPos.col) ? 1 : (toPos.col < fromPos.col ? -1 : 0);

	int currentRow = fromPos.row + rowStep;

	int currentCol = fromPos.col + colStep;

	while (currentRow != toPos.row || currentCol != toPos.col) {

		if (!classBoard.isEmpty(Position{ currentRow,currentCol })) {
			return false;
		}

		currentRow += rowStep;
		currentCol += colStep;
	}
	return true;
}

bool MoveValidator::checkRook(Position fromPos, Position toPos, const Board& classBoard) {

	//ROOK STRAIGHTS ONLY
	if (toPos.col != fromPos.col && toPos.row != fromPos.row)return false;

	//Path muse clean
	if (!isPathClear(fromPos, toPos, classBoard))return false;

	Piece rook = classBoard.getPiece(fromPos);

	Piece targetPiece = classBoard.getPiece(toPos);

	return classBoard.isEmpty(toPos) || isOpponent(rook, targetPiece);
}

bool MoveValidator::checkBishop(Position fromPos, Position toPos, const Board& classBoard) {

	int rowDiff = std::abs(toPos.row - fromPos.row);

	int colDiff = std::abs(toPos.col - fromPos.col);

	//Bishop diagonals only
	if (rowDiff != colDiff)return false;

	//Path must clean
	if (!isPathClear(fromPos, toPos, classBoard))return false;

	Piece bishop = classBoard.getPiece(fromPos);
	 
	Piece targetPiece = classBoard.getPiece(toPos);

	return classBoard.isEmpty(toPos) || isOpponent(bishop, targetPiece);
}

bool MoveValidator::checkQueen(Position fromPos, Position toPos, const Board& classBoard) {

	return checkBishop(fromPos, toPos, classBoard) || checkRook(fromPos, toPos, classBoard);

}

bool MoveValidator::checkKing(Position fromPos, Position toPos, const Board& classBoard) {

	int rowDiff = std::abs(toPos.row - fromPos.row);

	int colDiff = std::abs(toPos.col - fromPos.col);

	if (rowDiff > 1 || colDiff > 1)return false;

	if (rowDiff == 0 && colDiff == 0)return false;

	Piece king = classBoard.getPiece(fromPos);

	Piece targetPiece = classBoard.getPiece(toPos);

	return classBoard.isEmpty(toPos) || isOpponent(king, targetPiece);
}

bool MoveValidator::checkKnight(Position fromPos, Position toPos, const Board& classBoard) {

	int rowDiff = std::abs(toPos.row - fromPos.row);

	int colDiff = std::abs(toPos.col - fromPos.col);

	bool isValidL = (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);

	if (!isValidL)return false;

	Piece knight = classBoard.getPiece(fromPos);

	Piece targetPiece = classBoard.getPiece(toPos);

	return classBoard.isEmpty(toPos) || isOpponent(knight, targetPiece);
}

bool MoveValidator::isValidMove(Position fromPos, Position toPos, const Board& classBoard) {

	Piece fromPiece = classBoard.getPiece(fromPos);

	Piece targetPiece = classBoard.getPiece(toPos);

	if (fromPiece == Piece::Empty)return false;

	if (isSameColor(fromPiece, targetPiece))return false;

	Piece type = getType(fromPiece);

	switch (type) {

	case Piece::W_Pawn: return checkPawn(fromPos,toPos,classBoard)||isCanEnPassant(fromPos,toPos,classBoard);
	case Piece::W_Rook:return checkRook(fromPos, toPos, classBoard);
	case Piece::W_Knight:return checkKnight(fromPos, toPos, classBoard);
	case Piece::W_Bishop:return checkBishop(fromPos, toPos, classBoard);
	case Piece::W_Queen:return checkQueen(fromPos, toPos, classBoard);
	case Piece::W_King:return checkKing(fromPos, toPos, classBoard)||isCanCastle(fromPos,toPos,classBoard);

	default:return false;
	}

} 

bool MoveValidator::isInCheck(bool whiteTurn,const Board& classBoard) {

	Position kingPos = classBoard.findKing(whiteTurn);

	return isUnderAttack(kingPos, whiteTurn, classBoard);

}

bool MoveValidator::isUnderAttack(Position kingPos, bool isWhiteTarget, const Board& board) {

	const static int directions[8][2] = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };

	for (auto& dir : directions) {

		int r = kingPos.row;
		int c = kingPos.col;

		Position position = { r + dir[0],c + dir[1] };

		if (board.isInside(position)) {

			Piece piece = board.getPiece(position);

			if (piece != Piece::Empty && isWhite(piece) != isWhiteTarget) {

				if (getType(piece) == Piece::W_Knight||getType(piece)==Piece::B_Knight)return true;
			}
		}
	}
	static const int rookDir[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	for (auto& dir : rookDir) {
		int r = kingPos.row + dir[0];
		int c = kingPos.col + dir[1];


		while (board.isInside({ r, c })) {
			Piece piece = board.getPiece({ r, c });

			if (piece != Piece::Empty) {

				if (isWhite(piece) != isWhiteTarget) {
					Piece type = getType(piece);
					if (type == Piece::W_Rook ||type == Piece::W_Queen
						||type==Piece::B_Rook||type==Piece::B_Queen) {
						return true;
					}
				}

				break;
			}

			r += dir[0];
			c += dir[1];
		}
	}

	static const int diagonalDir[4][2] = { {1,1},{-1,1},{-1,-1},{1,-1} };

	for (auto& dir : diagonalDir) {
		int r = kingPos.row + dir[0];
		int c = kingPos.col + dir[1];

		while (board.isInside({ r,c })) {

			Piece piece = board.getPiece({ r,c });

			if (piece != Piece::Empty) {

				if (isWhite(piece) != isWhiteTarget) {

					Piece type = getType(piece);

					if (type == Piece::W_Bishop || type == Piece::W_Queen
						||type==Piece::B_Bishop||type==Piece::B_Queen) {
						return true;
					}
				}
				break;
			}

			r += dir[0];
			c += dir[1];
		}
	}

	static const int kingDir[8][2] = { {1,1}, {-1,-1}, {-1,1}, {1,-1}, {0,1}, {0,-1}, {1,0}, {-1,0} };

	for (auto& dir : kingDir) {

		Position pos = { kingPos.row + dir[0], kingPos.col + dir[1] };

		if (board.isInside(pos)) {

			Piece p = board.getPiece(pos);

			if (p != Piece::Empty && isWhite(p) != isWhiteTarget) {

				Piece type = getType(p);
				if (type == Piece::W_King || type == Piece::B_King) return true;
			}
		}
	}


	int pawnRow = isWhiteTarget ? kingPos.row - 1 : kingPos.row + 1;
	int pawnCol[] = { kingPos.col - 1, kingPos.col + 1 };

	for (int pCol : pawnCol) {

		Position pawnPos(pawnRow, pCol);

		if (board.isInside(pawnPos)) {

			Piece p = board.getPiece(pawnPos);

			if (isWhiteTarget && p == Piece::B_Pawn) return true;
			if (!isWhiteTarget && p == Piece::W_Pawn) return true;
		}
	}

	return false;
}

bool MoveValidator::isCanEnPassant(Position fromPos, Position toPos, const Board& board) {

	Piece p = board.getPiece(fromPos);

	if (getType(p) != Piece::W_Pawn)return false;

	LastMove lastMove = board.getLastMove();

	Position lastFrom = { lastMove.from.row,lastMove.from.col };

	Position lastTo = { lastMove.to.row,lastMove.to.col };

	Piece lastPiece = board.getPiece(lastTo);

	bool isLastPawnJump = (getType(lastPiece) == Piece::W_Pawn && std::abs(lastFrom.row - lastTo.row) == 2);

	if (!isLastPawnJump)return false;

	if (lastTo.row != fromPos.row || std::abs(lastTo.col - fromPos.col) != 1)return false;

	int direction = isWhite(p) ? -1 : 1;

	if (toPos.row == lastTo.row + direction && toPos.col == lastTo.col) {
		return true;
	}

	return false;
}

bool MoveValidator::isMoveLegal(Position fromPos, Position toPos, const Board& board) {

	Piece p = board.getPiece(fromPos);
	bool isWhiteKing = isWhite(p);

	Board tempBoard = board;
	tempBoard.movePiece(fromPos, toPos);

	Position kingPos = tempBoard.findKing(isWhiteKing);

	if (kingPos.row == -1) return false;

	if (isUnderAttack(kingPos, isWhiteKing, tempBoard)) {
		return false;
	}

	return true;
}

bool MoveValidator::isCanCastle(Position fromPos, Position toPos, const Board& board) {

	Piece fromPiece = board.getPiece(fromPos);

	if (fromPiece != Piece::W_King && fromPiece != Piece::B_King)return false;

	if (std::abs(toPos.col - fromPos.col) != 2)return false;

	if (toPos.row != fromPos.row)return false;

	bool isWhite = (fromPiece == Piece::W_King);
	int startRow = fromPos.row;

	if (isInCheck(isWhite, board))return false;

	if (fromPos.col != 4)return false;

	bool isKingSide = (toPos.col == 6);
	bool isQueenSide = (toPos.col == 2);

	if (!isKingSide && !isQueenSide)return false;

	const CastleState& state = board.getCastleState();

	if (isWhite) {

		if (state.whiteKingMoved) return false;
		if (isKingSide && state.whiteKingRookMoved)return false;
		if (isQueenSide && state.whiteQueenRookMoved)return false;

	}
	else {

		if (state.blackKingMoved)return false;
		if (isKingSide && state.blackKingRookMoved)return false;
		if (isQueenSide && state.blackQueenRookMoved)return false;

	}

	if (MoveValidator::isUnderAttack({ startRow, fromPos.row }, isWhite, board)) return false;

	if (isKingSide) {
		
		if (MoveValidator::isUnderAttack({ startRow, 5 }, isWhite, board) ||
			MoveValidator::isUnderAttack({ startRow, 6 }, isWhite, board))
			return false;
	}
	else if (isQueenSide) {
		
		if (MoveValidator::isUnderAttack({ startRow, 3 }, isWhite, board) ||
			MoveValidator::isUnderAttack({ startRow, 2 }, isWhite, board))
			return false;
	}

	Position rookPos;

	if (isKingSide) {

		rookPos = { startRow,7 };
	}
	else {
		rookPos = { startRow,0 };
	}

	Piece rook = board.getPiece(rookPos);
	Piece type = isWhite ? Piece::W_Rook : Piece::B_Rook;
	if (rook != type)return false;

	int startCol = std::min(fromPos.col, rookPos.col) + 1;
	int endCol = std::max(fromPos.col, rookPos.col);

	for (int col = startCol; col < endCol; col++) {

		if (board.getPiece({ startRow, col }) != Piece::Empty) return false;
	}
	return true;
}

bool MoveValidator::isCanExecuteMove(Position fromPos, Position toPos, const Board& board) {

	if (!isValidMove(fromPos, toPos, board)) return false;
	
	if (!isMoveLegal(fromPos, toPos, board))return false;

	return true;
}

