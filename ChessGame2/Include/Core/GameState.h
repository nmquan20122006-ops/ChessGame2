#pragma once
#include"Utility.h"

enum class color { black, white, none };

enum class MoveType {
	normal, castle, enPassant, promotion, none
};

struct Move {

	Position fromPos = { -1,-1 };
	Position toPos = { -1,-1 };

	MoveType type = MoveType::none;

	Piece promotionPiece = Piece::Empty;;
	Piece movedPiece = Piece::Empty;
	Piece capturedPiece = Piece::Empty;

	std::string enPassantTarget = "";
	Position enPassantCapturedPos = { -1, -1 };
	bool isEnPassantCapture = false;

	void reset() {

		Position fromPos = { -1,-1 };
		Position toPos = { -1,-1 };

		MoveType type = MoveType::none;

		Piece promotionPiece = Piece::Empty;;
		Piece movedPiece = Piece::Empty;
		Piece capturedPiece = Piece::Empty;
	}

};

struct BotMove {

	Position fromPos, toPos;
};

enum class GameStateEnum {

	waitingForStart, whiteTurn, blackTurn, checkMate, check, Paused
};

struct DragState {

	bool isActive = false;
	bool isDragging = false;
	
	Position fromPos = { -1,-1 };

	Piece draggingPiece = Piece::Empty;

	sf::Vector2f mousePosition;

	void reset() {
		isActive = false;
		fromPos = { -1,-1 };
		draggingPiece = Piece::Empty;
	}
};

struct AiState {

	bool isAiEnabled = false;

	color aiTurn = color::black;

	std::string bestMoveSuggestion;
};

enum class GameMode {
	PlayerVsAi, PlayerVsPlayer, AivsAi
};

class GameState {

private:

	friend class GameControl;
	friend class Board_DEBUG;
	friend class BoardToFEN_DEBUG;

	color currentTurn = color::white;

	bool isCheck = false;
	bool isCheckMate = false;
	bool isSelected = false;

	Position selectedPos = { -1,-1 };
	Position checkPos;

	std::vector<Position>validMoves;

	DragState drag;
	AiState aiState;

	GameMode currentMode = GameMode::PlayerVsAi;

	bool isDualMode = false;
	bool isAnimating = false;

public:

	const DragState& getDragState()const { return drag; }
	DragState& setDragState() { return drag; }

	const AiState& getAiState()const { return aiState; }
	AiState& setAiState() { return aiState; }

	const bool& getDualMode()const { return isDualMode; }
	void setDualMode(bool a) { isDualMode = a; }

	const GameMode& getGameMode()const { return currentMode; }
	GameMode& setGameMode() { return currentMode; }

	void setSelectPos(Position pos) {
		selectedPos = pos;
	}
	void clearSelection() {
		selectedPos = { -1,-1 };
		isSelected = false;
		validMoves.clear();
	}
	bool hasSelection()const {
		return selectedPos.row != -1;
	}

	color getColor(Piece p) const {

		if ((int)p >= 1 && (int)p <= 6)return color::white;

		return color::black;
	}

	void setValidMoves(std::vector<Position> v) {
		validMoves = v;
	}
	const auto& getValidMoves()const { return validMoves; }
	
	bool isPosInVector(Position pos) const {

		for (const auto& position : validMoves) {

			if (position == pos) {

				return true;
			}
		}

		return false;
	}

	const Position getSelectPos()const { return selectedPos; }
	const Position getCheckPos()const { return checkPos; }

	const bool getIsCheck()const { return isCheck; }
	const bool getIsSelected()const { return isSelected; }
	const bool getIsCheckMate()const { return isCheckMate; }

	void setIsCheck(bool state) { isCheck = state; }
	void setIsSelected(bool state) { isSelected = state; }
	void setIsCheckMate(bool state) { isCheckMate = state; }

	void switchTurn() { currentTurn = (currentTurn == color::white) ? color::black : color::white; }

	void reset() {
		currentTurn = color::white;
		isCheck = false;
		isCheckMate = false;
		isSelected = false;
		selectedPos = { -1, -1 };
		checkPos = { -1, -1 };
		validMoves.clear();
		drag.reset();
		isDualMode = false;
	}
};