#include"NotationService.h"
#include"Board.h"
#include"State/Move.hpp"
#include"EventBus.h"
#include"State/GameState.hpp"
#include"MoveService.h"

ChessNotation::ChessNotation(){}

std::string ChessNotation::posToSAN(const Position pos) {

	char file = 'a' + pos.col;
	char rank = '8' - pos.row;

	return std::string(1, file) + rank;
}

char ChessNotation::pieceToSAN(const Piece piece) {

	switch (piece) {
	case Piece::W_Rook:return 'R';
	case Piece::W_Bishop:return 'B';
	case Piece::W_Knight:return 'N';
	case Piece::W_King:return 'K';
	case Piece::W_Queen:return 'Q';
	case Piece::B_Bishop:return 'B';
	case Piece::B_King:return 'K';
	case Piece::B_Knight:return 'N';
	case Piece::B_Rook:return 'R';
	case Piece::B_Queen: return 'Q';
	case Piece::Empty:
	default: return  ' ';
	}
}

std::string ChessNotation::disAmbiguation(Move& move, std::shared_ptr<Board> board,
	std::shared_ptr<MoveService> moveService) {

	Piece piece = move.movedPiece;

	if (piece == Piece::W_Pawn || piece == Piece::B_Pawn)return "";

	std::vector<Position> ambiguos;

	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {

			Position pos = { row,col };

			if (pos == move.fromPos)continue;
			if (board->getPiece(pos) != piece)continue;

			std::vector<Position> validMoves = moveService->getValidMoves(pos, *board);
			bool isCanReach = std::find(validMoves.begin(), validMoves.end(), move.toPos) == validMoves.end();

			if (!isCanReach) ambiguos.push_back(pos);
		}
	}

	if (ambiguos.empty())return"";

	bool sameFile = std::any_of(ambiguos.begin(), ambiguos.end(),
		[&](const Position& pos) {return pos.col == move.fromPos.col; });

	if (!sameFile)return std::string(1, 'a' + move.fromPos.col);

	bool sameRank = std::any_of(ambiguos.begin(), ambiguos.end(),
		[&](const Position& pos) {return pos.row == move.fromPos.row; });

	if (!sameRank)return std::string(1, '8' - move.fromPos.row);

	return posToSAN(move.fromPos);
}

std::string ChessNotation::fullSAN(Move& move, std::shared_ptr<GameState> gameState, 
	std::shared_ptr<Board> board, std::shared_ptr<MoveService> moveService) {

	if (move.moveType == MoveType::castle) return "O-O";
	if (move.moveType == MoveType::castleQueenSide)return "O-O-O";

	std::string SAN;

	Piece piece = move.movedPiece;
	bool isPawn = (piece == Piece::W_Pawn || piece == Piece::B_Pawn);

	if (!isPawn) SAN += pieceToSAN(piece);

	if (!isPawn) SAN += disAmbiguation(move, board, moveService);
	
	if (isPawn && (move.moveType == MoveType::capture || move.moveType == MoveType::enPassant)) {

		SAN += ('a' + move.fromPos.col);
	}
	
	if (move.moveType == MoveType::capture || move.moveType == MoveType::enPassant) SAN += 'x';
	
	SAN += posToSAN(move.toPos);
	
	if (move.moveType == MoveType::promotion) {

		SAN += '=' + std::string(1, pieceToSAN(move.promotionPiece));
	}
	
	if (gameState->getIsCheckMate()) SAN += '#';
	else if (gameState->getIsCheck()) SAN += '+';

	return SAN;

}
