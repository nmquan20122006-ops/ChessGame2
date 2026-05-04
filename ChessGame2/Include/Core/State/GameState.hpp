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
#include"Move.hpp"
#include"Type.hpp"
#include"Aistate.hpp"
#include"Undo.hpp"
#include"UiState.hpp"
#include<vector>

// GameState class to encapsulate the overall state of the chess game,
// NOTE::including the current turn, check/checkmate status, selected piece, valid moves, and other relevant information.
// NOTE::This class serves as a central repository for all game-related state information, allowing for easy access and management throughout the game's logic and UI.

class GameState {
public:
	//=================================================================================================
	//Getters
	//=================================================================================================
	const DragState&	 getDragState()				const { return drag; }
	const AiState&		 getAiState()				const { return aiState; }
	const GameMode&		 getGameMode()				const { return currentMode; }
	const GameStatus&	 getGameStatus()			const { return gameStatus; }
	const bool&			 getDualMode()				const { return isDualMode; }
	const int&			 getHalfMoveClockCount()	const { return halfMoveClockCount; }
	const int&			 getFullMoveNumberCount()	const { return fullMoveNumberCount; }
	const Position&		 getPendingTo()				const { return pendingTo; }
	const Color			 getCurrentTurn()			const { return currentTurn; }
	const Color			 getColor(Piece p)			const { if ((int)p >= 1 && (int)p <= 6)return Color::white; return Color::black; }
	const auto&			 getValidMoves()			const { return validMoves; }
	const Position		 getSelectPos()				const { return selectedPos; }
	const Position		 getCheckPos()				const { return checkPos; }
	const bool			 getIsCheck()				const { return isCheck; }
	const bool 			 getIsSelected()			const { return isSelected; }
	const bool 			 getIsCheckMate()			const { return isCheckMate; }
	const std::string&	 getCurrentFEN()			const { return currentFEN; }
	const auto&			 getUndoStack()				const { return undoStack; }
	const auto&			 getAnimating()				const { return isAnimating; }
	//===================================================================================================
	//Setters
	//===================================================================================================
	AiState&			 setAiState()						{ return aiState; }
	DragState&			 setDragState()						{ return drag; }
	void				 setGameMode(GameMode m)			{ currentMode = m; }
	void				 setGameStatus(GameStatus state)	{ gameStatus = state; }
	void				 setDualMode(bool a)				{ isDualMode = a; }
	void				 setSelectPos(Position pos)			{ selectedPos = pos; }
	void				 setHalfMoveClockCount(int a)		{ halfMoveClockCount = a; }
	void				 setFullMoveNumberCount(int a)		{ fullMoveNumberCount = a; }
	bool				 hasSelection()				const	{ return selectedPos.row != -1; }
	void				 setValidMoves(std::vector<Position> v) { validMoves = v; }
	void				 setIsCheck(bool state)				{ isCheck = state; }
	void				 setIsSelected(bool state)			{ isSelected = state; }
	void				 setIsCheckMate(bool state)			{ isCheckMate = state; }
	void				 setAnimating(bool state)			{ isAnimating = state; }
	void				 setCheckPos(Position pos)			{ checkPos = pos; }
	void				 setCurrentTurn(Color turn)			{ currentTurn = turn; }
	void				 setCurrentFEN(std::string fen)		{ currentFEN = fen; }
	//===================================================================================================
	//Action methods
	//===================================================================================================
	void				 clearSelection() {
		selectedPos = { -1,-1 };
		isSelected = false;
		validMoves.clear();
	}
	bool				 isPosInVector(Position pos) const {
		for (const auto& position : validMoves) {
			if (position == pos) {
				return true;
			}
		}
		return false;
	}
	void				 switchTurn() { currentTurn = (currentTurn == Color::white) ? Color::black : Color::white; }
	void				 pushToUndoStack(UndoEntry entry) { undoStack.push_back(entry); }
	bool				 canUndo() const { return !undoStack.empty(); }
	void				 reset() {
		currentTurn = Color::white;
		isCheck = false;
		isCheckMate = false;
		isSelected = false;
		selectedPos = { -1, -1 };
		checkPos = { -1, -1 };
		validMoves.clear();
		drag.reset();
		isDualMode = false;

	}
private:
	friend class GameControl;
	friend class Board_DEBUG;
	friend class BoardToFEN_DEBUG;
	//===================================================================================================
	//Turn and state
	//===================================================================================================
	Color				currentTurn = Color::white;
	GameStatus			gameStatus = GameStatus::Normal;
	GameMode			currentMode = GameMode::PlayerVsAi;
	//===================================================================================================
	//Flag
	//===================================================================================================
	bool				isCheck = false;
	bool				isCheckMate = false;
	bool				isSelected = false;
	bool				isDualMode = false;
	bool				isAnimating = false;
	//===================================================================================================
	//Position
	//===================================================================================================
	Position			selectedPos = { -1,-1 };
	Position			checkPos;
	Position			pendingFrom = { -1,-1 };
	Position			pendingTo = { -1,-1 };
	//===================================================================================================
	//Move data
	//===================================================================================================
	std::vector<Position>validMoves;
	std::vector<UndoEntry>undoStack;
	//===================================================================================================
	//Sub-state
	//===================================================================================================
	DragState			drag;
	AiState				aiState;
	//===================================================================================================
	//FEN/move counters
	//===================================================================================================
	int					halfMoveClockCount = 0;
	int					fullMoveNumberCount = 1;
	std::string			currentFEN;
};