#pragma once
	enum class Color { black, white, none };

	// MoveType enum to categorize different types of moves in chess
	enum class MoveType {
		normal, castle, enPassant, promotion, none
	};

	// GameStatus to represent the various states of the game
	enum class GameStatus {
		WaitingForStart, WhiteTurn, BlackTurn, CheckMate, Check, Paused, WaitingForPromotion, Normal, Animating
	};

	// GameMode enum to represent the different game modes available in the chess game
	enum class GameMode {
		PlayerVsAi, PlayerVsPlayer, AivsAi
	};