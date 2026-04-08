#include"AllMoveValid.h"

std::vector<Position>AllValidMove::getPawnAllMoves(Position fromPos, const Board& board) {

	std::vector<Position>moves;

	Piece pawn = board.getPiece(fromPos);

	int direction = isWhite(pawn) ? -1 : 1;

	int startRow = isWhite(pawn) ? 6 : 1;

	Position epLeft = { fromPos.row + direction, fromPos.col - 1 };

	Position epRight = { fromPos.row + direction, fromPos.col + 1 };

	//next one step
	Position forward1{ fromPos.row + direction,fromPos.col };

	//check
	Position forward2{ fromPos.row + direction * 2,fromPos.col };

	if (board.isInside(forward1) && board.isEmpty(forward1)) {

		moves.push_back(forward1);

		if (fromPos.row == startRow && board.isInside(forward2) && board.isEmpty(forward2)) {

			moves.push_back(forward2);
		}
	}

	//capture other piece
	Position captureLeft{ fromPos.row + direction,fromPos.col - 1 };

	Position captureRight{ fromPos.row + direction,fromPos.col + 1 };

	//capture left
	if (board.isInside(captureLeft) && !board.isEmpty(captureLeft)) {

		Piece targetPiece = board.getPiece(captureLeft);

		if (isOpponent(pawn, targetPiece))moves.push_back(captureLeft);
	}

	//capture right
	if (board.isInside(captureRight) && !board.isEmpty(captureRight)) {

		Piece targetPiece = board.getPiece(captureRight);

		if (isOpponent(pawn, targetPiece))moves.push_back(captureRight);
	}

	//capture enPassant
	
	if (board.isInside(epLeft)) {

		if (MoveValidator::isCanEnPassant(fromPos, epLeft, board)) {

			moves.push_back(epLeft);
		}
	}

	if (board.isInside(epRight)) {

		if (MoveValidator::isCanEnPassant(fromPos, epRight, board)) {

			moves.push_back(epRight);
		}
	}

	return moves;

}

std::vector<Position>AllValidMove::getKnightAllMoves(Position fromPos, const Board& board) {

	std::vector<Position> moves;

	int directions[8][2] = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };

	for (auto& dir : directions) {

		int nextRow = fromPos.row + dir[0];
		int nextCol = fromPos.col + dir[1];

		Position toPos(nextRow, nextCol);

		if (board.isInside(toPos)) {

			Piece targetPiece = board.getPiece(toPos);
			Piece currentPiece = board.getPiece(fromPos);

			if (board.isEmpty(toPos) || isOpponent(targetPiece,currentPiece)) {
				moves.push_back(toPos);
			}
		}
	}
	return moves;

}

std::vector<Position>AllValidMove::getRookAllMoves(Position fromPos, const Board& board) {

	std::vector<Position>moves;

	std::vector<std::pair<int, int>>directions = { {1,0},{-1,0},{0,1},{0,-1} };

	for (auto dir : directions) {
		for (int i = 1; i < boardSize; i++) {

			Position to = { fromPos.row + i * dir.first,fromPos.col + i * dir.second };

			if (!board.isInside(to))break;

			if (board.isEmpty(to)) {
				moves.push_back(to);
			}

			else {
				Piece targetPiece = board.getPiece(to);
				Piece currentPiece = board.getPiece(fromPos);

				if (isOpponent(targetPiece,currentPiece)) {
					moves.push_back(to);
				}
				break;
			}
		}
	}
	return moves;
}

std::vector<Position>AllValidMove::getBishopAllMoves(Position fromPos, const Board& board) {

	std::vector<Position>moves;

	int direction[4][2] = { {1,1},{-1,1},{-1,-1},{1,-1} };

	for (auto& dir : direction) {
		for (int i = 1; i < boardSize; i++) {

			Position toPos = { fromPos.row + i * dir[0],fromPos.col + i * dir[1] };
			if (!board.isInside(toPos))break;

			if (board.isEmpty(toPos)) {
				moves.push_back(toPos);
			}
			else {

				Piece targetPiece = board.getPiece(toPos);
				Piece currentPiece = board.getPiece(fromPos);

				if (isOpponent(targetPiece, currentPiece)) {
					moves.push_back(toPos);
				}

				break;
			}
		}
	}

	return moves;

}

std::vector<Position>AllValidMove::getQueenAllMoves(Position fromPos, const Board& board) {

	std::vector<Position>moves;

	int directions[8][2] = { {-1,0},{1,0},{0,1},{0,-1},{1,1},{-1,-1},{-1,1},{1,-1} };

	for (auto& dir : directions) {
		for (int i = 1; i < boardSize; i++) {

			Position toPos = { fromPos.row + i * dir[0],fromPos.col + i * dir[1] };

			if (!board.isInside(toPos))break;

			if (board.isEmpty(toPos)) {
				moves.push_back(toPos);
			}
			else {

				Piece targetPiece = board.getPiece(toPos);
				Piece currentPiece = board.getPiece(fromPos);

				if (isOpponent(targetPiece, currentPiece)) {
					moves.push_back(toPos);
				}

				break;
			}
		}
	}

	return moves;
}

std::vector<Position>AllValidMove::getAllValidMoves(Position fromPos, const Board& board) {

	std::vector<Position> rawMoves = getRawMoves(fromPos, board);
	std::vector<Position> legalMoves;

	
	for (const auto& toPos : rawMoves) {

		if (moveValidator.isMoveLegal(fromPos, toPos, board)) {
			legalMoves.push_back(toPos);
		}
	}
	
	return legalMoves;
}

std::vector<Position> AllValidMove::getKingPhysicalMoves(Position fromPos, const Board& board) {

	std::vector<Position> moves;
	Piece currentKing = board.getPiece(fromPos);
	bool isWhiteKing = isWhite(currentKing);

	
	static const int directions[8][2] = {
		{1,1}, {-1,-1}, {-1,1}, {1,-1}, {0,1}, {0,-1}, {1,0}, {-1,0}
	};

	for (auto& dir : directions) {
		Position toPos(fromPos.row + dir[0], fromPos.col + dir[1]);

		if (board.isInside(toPos)) {
			Piece targetPiece = board.getPiece(toPos);

			
			if (targetPiece == Piece::Empty || isWhite(targetPiece) != isWhiteKing) {
				if (!MoveValidator::isUnderAttack(toPos, isWhiteKing, board)) {
					moves.push_back(toPos);
				}
				
			}
		}
	}

	int startRow = isWhiteKing ? 7 : 0;
	if (fromPos.row == startRow && fromPos.col == 4) {

		// Nhập thành gần (Kingside)
		if (moveValidator.isCanCastle(fromPos, { startRow, 6 }, board)) {
			moves.push_back({ startRow, 6 });
		}

		// Nhập thành xa (Queenside)
		if (moveValidator.isCanCastle(fromPos, { startRow, 2 }, board)) {
			moves.push_back({ startRow, 2 });
		}
	}

	
	return moves;
}

std::vector<Position>AllValidMove::getRawMoves(Position fromPos, const Board& board) {

	Piece fromPiece = board.getPiece(fromPos);
	if (fromPiece == Piece::Empty)return{};

	switch (getType(fromPiece)) {

	case Piece::W_Pawn:		return getPawnAllMoves(fromPos, board);
	case Piece::W_Rook:     return getRookAllMoves(fromPos, board);
	case Piece::W_Knight:	return getKnightAllMoves(fromPos, board);
	case Piece::W_Bishop:	return getBishopAllMoves(fromPos, board);
	case Piece::W_Queen:    return getQueenAllMoves(fromPos, board);
	case Piece::W_King:		return getKingPhysicalMoves(fromPos, board);
	case Piece::B_King: return getKingPhysicalMoves(fromPos, board);

	default: return {};
	}
}
