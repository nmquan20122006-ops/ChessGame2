#include"BoardToFEN_Test.h"

BoardToFEN_DEBUG::BoardToFEN_DEBUG() {
	board_Real.initBoard();
}

char BoardToFEN_DEBUG::pieceToChar(Piece piece) {
	
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

std::string BoardToFEN_DEBUG::turnToString(color color) {

	switch (color) {
	case(color::black):return "black";
	case(color::white):return "white";
	case(color::none):
	default: return "none";
	}
}

char BoardToFEN_DEBUG::turnToChar(color turn) {

	switch (turn) {
	case(color::black):return 'b';
	case(color::white):return 'w';
	case(color::none):
	default: return 'n';
	}
}

void BoardToFEN_DEBUG::printBoard(Board& b) {

	for (int i = 0; i < boardSize; i++) {

		for (int j = 0; j < boardSize; j++) {
			std::cout << pieceToChar(b.board[i][j]) << " | ";
		}
		std::cout<<std::endl;
	}
}

Position BoardToFEN_DEBUG::getRandom() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<>dis(0, 7);
	return { dis(gen),dis(gen) };
}

void BoardToFEN_DEBUG::runTest(Board& b, int numMoves) {

	std::cout << "--- STARTING FEN STRESS TEST ---" << std::endl;
	for (int i = 0; i < numMoves; i++) {
		Position from = getRandom();
		Position to = getRandom();

		if (b.getPiece(from) == Piece::Empty && (from.row != to.row || from.col != to.col)) {
			b.movePiece(from, to);
		}

	}
	
}

std::string BoardToFEN_DEBUG::enPassantToString(Move& move) {

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

std::string BoardToFEN_DEBUG::castleToString(Board& b) {

	std::string castleString = "";

	auto& castleState_Real = b.castleState;

	if (castleState_Real.canWhiteCastleKingSide()) {
		castleString += "K";
	}
	if (castleState_Real.canWhiteCastleQueenSide()) {
		castleString += "Q";
	}
	if (castleState_Real.canBlackCastleKingSide()) {
		castleString += "k";
	}
	if (castleState_Real.canBlackCastleQueenSide()) {
		castleString += "q";
	}

	if (castleString.empty()) {
		castleString += "-";
	}
	return castleString;
}

std::string BoardToFEN_DEBUG::toFEN(Board& b,GameState& state_Real,Move& move) {

	std::string fen = ""; 
	char turnChar = turnToChar(state_Real.currentTurn);
	std::string castleChar = castleToString(b);
	std::string enPassantChar = enPassantToString(move);
	
	for (int row = 0; row < boardSize; ++row) {
		int emptyCount = 0;
		for (int col = 0; col < boardSize; ++col) {

			Piece p = b.board[row][col];	
			if (p == Piece::Empty) {
				emptyCount++;

			}
			else {
				if (emptyCount > 0) {
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}
				fen += pieceToChar(p);
			}
		}
		if (emptyCount > 0) {
			fen += std::to_string(emptyCount);
		}
		if (row < 7) {
			fen += "/";
		}
		
	}
	fen += ' ';
	fen += turnChar;
	fen += ' ';
	fen += castleChar;
	fen += ' ';  
	fen += enPassantChar;

	std::cout << fen << std::endl;
	return fen;
}