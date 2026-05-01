#include"ToFEN.h"

char ToFEN::PieceToChar(Piece piece) {

	switch (piece) {
	case Piece::W_Pawn:return 'P';
	case Piece::W_Rook:return 'R';
	case Piece::W_Bishop:return 'B';
	case Piece::W_Knight:return 'N';
	case Piece::W_King:return 'K';
	case Piece::W_Queen:return 'Q';
	case Piece::B_Bishop:return 'b';
	case Piece::B_King:return 'k';
	case Piece::B_Knight:return 'n';
	case Piece::B_Pawn:return 'p';
	case Piece::B_Rook:return 'r';
	case Piece::B_Queen: return 'q';
	case Piece::Empty:
	default: return  ' ';
	}
}

Piece ToFEN::charToPiece(Color turn, char promotionChar) {

	if (turn == Color::white) {
		if (promotionChar == 'q') return Piece::W_Queen;
		if (promotionChar == 'r') return Piece::W_Rook;
		if (promotionChar == 'b') return Piece::W_Bishop;
		if (promotionChar == 'n') return Piece::W_Knight;
	}
	else {
		if (promotionChar == 'q') return Piece::B_Queen;
		if (promotionChar == 'r') return Piece::B_Rook;
		if (promotionChar == 'b') return Piece::B_Bishop;
		if (promotionChar == 'n') return Piece::B_Knight;
	}
	return Piece::Empty;
}

char ToFEN::TurnToChar(Color turn) {

	switch (turn) {
	case(Color::black):return 'b';
	case(Color::white):return 'w';
	case(Color::none):
	default: return 'n';
	}
}

std::string ToFEN::castleToFEN(const Board& board) {

	std::string castleString = "";

	auto& castleState = board.getCastleState();

	if (castleState.canWhiteCastleKingSide()) {
		castleString += "K";
	}
	if (castleState.canWhiteCastleQueenSide()) {
		castleString += "Q";
	}
	if (castleState.canBlackCastleKingSide()) {
		castleString += "k";
	}
	if (castleState.canBlackCastleQueenSide()) {
		castleString += "q";
	}

	if (castleString.empty()) {
		castleString = "-";
	}
	return castleString;
}

std::string ToFEN::enPassantToFEN(const Move& move) {

	if ((move.movedPiece == Piece::W_Pawn || move.movedPiece == Piece::B_Pawn) &&
		std::abs(move.fromPos.row - move.toPos.row) == 2) {
		int enPassantRow = (move.fromPos.row + move.toPos.row) / 2;
		int enPassantCol = move.fromPos.col;

		char file = 'a' + enPassantCol;
		char rank = '8' - enPassantRow;

		std::string result = "";

		result += file;
		result += rank;

		return result;
	}
	else {
		return "-";
	}
}

std::string ToFEN::BoardToFEN(const Board& board) {

	std::string fen;
	for (int row = 0; row < boardSize; ++row) {

		int emptyCount = 0;
		for (int col = 0; col < boardSize; ++col) {

			Piece p = board.getPiece({ row,col });
			if (p == Piece::Empty) {
				emptyCount++;

			}
			else {
				if (emptyCount > 0) {
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}
				fen += PieceToChar(p);
			}
		}
		if (emptyCount > 0) {
			fen += std::to_string(emptyCount);
		}
		if (row < boardSize-1) {
			fen += "/";
		}

	}

	return fen;
}

std::string ToFEN::FullFEN(const Board& board, const Move& move, const GameState& gameState) {

	std::string fullFEN = "";

	fullFEN += BoardToFEN(board);
	fullFEN += " ";
	fullFEN += TurnToChar(gameState.getCurrentTurn());
	fullFEN += " ";
	fullFEN += castleToFEN(board);
	fullFEN += " ";
	fullFEN += enPassantToFEN(move);
	fullFEN += " ";
	fullFEN += std::to_string(gameState.getHalfMoveClockCount());
	fullFEN += " ";
	fullFEN += std::to_string(gameState.getFullMoveNumberCount());



	return fullFEN;
}