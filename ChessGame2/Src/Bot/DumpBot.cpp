#include"Bot.h"
#include"Board.h"
#include<algorithm>
#include<ctime>

DumpBot::DumpBot(MoveService& move,color c):moveService(move),botColor(c){

	rng.seed(std::time(0));
}

std::vector<BotMove>DumpBot::bot_getAllValidMove(const Board& board) {

	std::vector<BotMove> bot_allMoves;

	for (int r = 0; r < boardSize; r++) {
		for (int c = 0; c < boardSize; c++) {

			Position currentPos = { r,c };
			Piece currentPiece = board.getPiece(currentPos);

			if (currentPiece != Piece::Empty && gameState.getColor(currentPiece) == botColor) {

				std::vector<Position> validDestination = moveService.getValidMoves(currentPos,board);

				for (const auto& dest : validDestination) {

					bot_allMoves.push_back({currentPos,dest});
				}
			}
		}
	}
	return bot_allMoves;
}

BotMove DumpBot::bot_makeMove(const Board& board) {

	std::vector<BotMove> moves = bot_getAllValidMove(board);

	if (moves.empty()) {

		return { Position{ -1,-1 }, Position{ -1,-1 } };
	}

	std::uniform_int_distribution<int> dist(0, moves.size() - 1);

	int randomIndex = dist(rng);

	return moves[randomIndex];
}