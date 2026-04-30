/**
* @file GameState.h
* @brief This file defines the GameState class and related
* Structures and enums that represent the state of a chess game, including the current turn, check/checkmate status, selected piece, valid moves, and other relevant information.
* The GameState class serves as a central repository for all game-related state information, allowing for easy access and management throughout the game's logic and UI.
* @author [Nguyen Minh Quan]
* @date [29/4/2026]
* @version [1.0]
* @copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
* The code is provided "as is" without any warranties or guarantees of functionality, performance, or suitability for any particular purpose
* The author shall not be liable for any damages arising out of the use or inability to use the software, even if advised of the possibility of such damage.
*/
#pragma once
#include"Utility.h"
#include"Piece.h"
#include"Board.h"

// color enum to represent the color of pieces and turns in the game. It includes three values: black, white, and none.
// NOTE::The 'none' value can be used to represent an uninitialized state or a situation where no color is applicable.
enum class color { black, white, none };

// MoveType enum to categorize different types of moves in chess
enum class MoveType {
	normal, castle, enPassant, promotion, none
};

// GameStateEnum to represent the various states of the game
enum class GameStateEnum {

	waitingForStart, whiteTurn, blackTurn, checkMate, check, Paused,waitingForPromotion, normal
};

// GameMode enum to represent the different game modes available in the chess game
enum class GameMode {
	PlayerVsAi, PlayerVsPlayer, AivsAi
};

// Move struct to encapsulate all relevant information about a chess move, including the move's origin and destination, the pieces involved, and any special move data.
struct Move {

	//Basic move data
	Position fromPos = { -1,-1 };
	Position toPos = { -1,-1 };
	Piece movedPiece = Piece::Empty;
	Piece capturedPiece = Piece::Empty;
	Piece promotionPiece = Piece::Empty;
	MoveType moveType = MoveType::none;
	
	//Special move data
	Position enPassantCapturedPos = { -1, -1 };
	bool isEnPassantCapture = false;

	//Board state snapshots
	std::string oldEnPassantTarger;

	//Updated state
	std::string newEnPassantTarger;
	
	/**
	* @brief Resets the move data to default values Useful for clearing move information after processing a move.
	* NOTE: This does not reset the board state snapshots, as they may be needed for undo operations or move logging.
	*/
	void reset() {
		*this = Move(); // Reset all members to their default values
	}
};

// BotMove struct to represent a move made by the AI bot
struct BotMove {

	Position fromPos, toPos;
};

// DragState struct to manage the state of piece dragging in the UI
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

// AiState struct to manage the state of the AI in the game
struct AiState {

	bool isAiEnabled = false;

	color aiTurn = color::black;

	std::string bestMoveSuggestion;
};

// UndoEntry struct to represent an entry in the undo stack, containing a snapshot of the board state before a move and the move itself. This allows for undoing moves and restoring the previous game state.
struct UndoEntry {

	std::shared_ptr<Board> boardBefore;
	Move moveBefore;
	color turnBefore = color::none;
	std::string fenBefore ="";
	int halfMoveClockCountBefore = 0;
	int fullMoveNumberCountBefore = 1;
};

// GameState class to encapsulate the overall state of the chess game,
// NOTE::including the current turn, check/checkmate status, selected piece, valid moves, and other relevant information.
// NOTE::This class serves as a central repository for all game-related state information, allowing for easy access and management throughout the game's logic and UI.
class GameState {
private:
	friend class GameControl;
	friend class MoveExecutor;
	friend class Board_DEBUG;
	friend class BoardToFEN_DEBUG;

	color currentTurn = color::white;

	bool isCheck = false;
	bool isCheckMate = false;
	bool isSelected = false;

	Position selectedPos = { -1,-1 };
	Position checkPos;

	Position pendingFrom = { -1,-1 };
	Position pendingTo = { -1,-1 };

	std::vector<Position>validMoves;
	std::vector<UndoEntry>undoStack;

	DragState drag;
	AiState aiState;

	GameStateEnum gameStateEnum = GameStateEnum::normal;
	GameMode currentMode = GameMode::PlayerVsAi;

	bool isDualMode = false;
	bool isAnimating = false;

	int halfMoveClockCount = 0;
	int fullMoveNumberCount = 1;
	
	std::string currentFEN = "";

public:
	const DragState& getDragState()const { return drag; }
	DragState& setDragState() { return drag; }

	const AiState& getAiState()const { return aiState; }
	AiState& setAiState() { return aiState; }

	const bool& getDualMode()const { return isDualMode; }
	void setDualMode(bool a) { isDualMode = a; }

	const GameMode& getGameMode()const { return currentMode; }
	GameMode& setGameMode() { return currentMode; }

	const GameStateEnum& getGameStateEnum()const { return gameStateEnum; }

	const int& getHalfMoveClockCount() const { return halfMoveClockCount; }
	const int& getFullMoveNumberCount() const { return fullMoveNumberCount; }

	const Position& getPendingTo()const { return pendingTo; }

	void setSelectPos(Position pos) {
		selectedPos = pos;
	}

	bool hasSelection()const {
		return selectedPos.row != -1;
	}

	void clearSelection() {
		selectedPos = { -1,-1 };
		isSelected = false;
		validMoves.clear();
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

	color getCurrentTurn()const { return currentTurn; }
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