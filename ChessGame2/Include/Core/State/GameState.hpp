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
#include"UiState.hpp"
#include<vector>

// GameState class to encapsulate the overall state of the chess game,
// NOTE::including the current turn, check/checkmate status, selected piece, valid moves, and other relevant information.
// NOTE::This class serves as a central repository for all game-related state information, allowing for easy access and management throughout the game's logic and UI.

class GameState {
public:
	const DragState&		getDragState()				const { return m_drag; }
	const EngineConfig&		getEngineConfig()			const { return m_engineConfig; }
	const GameMode&			getGameMode()				const { return m_currentMode; }
	const GameStatus&		getGameStatus()				const { return m_gameStatus; }
	const bool&				getDualMode()				const { return m_isDualMode; }
	const int&				getHalfMoveClockCount()		const { return m_halfMoveClockCount; }
	const int&				getFullMoveNumberCount()	const { return m_fullMoveNumberCount; }
	const Position&			getPendingTo()				const { return m_pendingTo; }
	const Position&			getPendingFrom()			const { return m_pendingFrom; }
	const Color				getCurrentTurn()			const { return m_currentTurn; }
	const Color				getColor(Piece p)			const { if ((int)p >= 1 && (int)p <= 6)return Color::white; return Color::black; }
	const auto&				getValidMoves()				const { return m_validMoves; }
	const Position			getSelectPos()				const { return m_selectedPos; }
	const Position			getCheckPos()				const { return m_checkPos; }
	const bool				getIsCheck()				const { return m_isCheck; }
	const bool 				getIsSelected()				const { return m_isSelected; }
	const bool 				getIsCheckMate()			const { return m_isCheckMate; }
	const std::string&		getCurrentFEN()				const { return m_currentFEN; }
	const auto&				getAnimating()				const { return m_isAnimating; }
	const HintPosition& getHintPosition()				const { return m_hintPosition; }

	EngineConfig&			setEngineConfig()					{ return m_engineConfig; }
	DragState&				setDragState()						{ return m_drag; }
	void					setGameMode(GameMode m)				{ m_currentMode = m; }
	void					setGameStatus(GameStatus state)		{ m_gameStatus = state; }
	void					setDualMode(bool a)					{ m_isDualMode = a; }
	void					setSelectPos(Position pos)			{ m_selectedPos = pos; }
	void					setHalfMoveClockCount(int a)		{ m_halfMoveClockCount = a; }
	void					setFullMoveNumberCount(int a)		{ m_fullMoveNumberCount = a; }
	bool					hasSelection()				const	{ return m_selectedPos.row != -1; }
	void					setValidMoves(std::vector<Position> v) { m_validMoves = v; }
	void					setIsCheck(bool state)				{ m_isCheck = state; }
	void					setIsSelected(bool state)			{ m_isSelected = state; }
	void					setIsCheckMate(bool state)			{ m_isCheckMate = state; }
	void					setAnimating(bool state)			{ m_isAnimating = state; }
	void					setCheckPos(Position pos)			{ m_checkPos = pos; }
	void					setCurrentTurn(Color turn)			{ m_currentTurn = turn; }
	void					setCurrentFEN(std::string fen)		{ m_currentFEN = fen; }
	void					setPendingFrom(Position pos)		{ m_pendingFrom = pos; }
	void					setPendingTo(Position pos)			{ m_pendingTo = pos; }
	void					setHintPosition(Position from, Position to) { m_hintPosition.from = from; m_hintPosition.to = to; }
	void					clearSelection() {
		m_selectedPos = { -1,-1 };
		m_isSelected = false;
		m_validMoves.clear();
	}
	void					clearHintPosition() { m_hintPosition.from = { -1,-1 }; m_hintPosition.to = { -1,-1 }; }
	bool					isPosInVector(Position pos) const {
		for (const auto& position : m_validMoves) {
			if (position == pos) {
				return true;
			}
		}
		return false;
	}
	void					switchTurn() { m_currentTurn = (m_currentTurn == Color::white) ? Color::black : Color::white; }
	void					reset() {
		m_currentTurn = Color::white;
		m_isCheck = false;
		m_isCheckMate = false;
		m_isSelected = false;
		m_selectedPos = { -1, -1 };
		m_checkPos = { -1, -1 };
		m_validMoves.clear();
		m_drag.reset();
		m_isDualMode = false;

	}

private:
	Color					m_currentTurn = Color::white;
	GameStatus				m_gameStatus = GameStatus::Normal;
	GameMode				m_currentMode = GameMode::PlayerVsAi;
	bool					m_isCheck = false;
	bool					m_isCheckMate = false;
	bool					m_isSelected = false;
	bool					m_isDualMode = false;
	bool					m_isAnimating = false;
	Position				m_selectedPos = { -1,-1 }; 
	Position				m_checkPos;
	Position				m_pendingFrom = { -1,-1 };
	Position				m_pendingTo = { -1,-1 };
	std::vector<Position>	m_validMoves;
	DragState				m_drag;
	EngineConfig			m_engineConfig;
	HintPosition			m_hintPosition;
	int						m_halfMoveClockCount = 0;
	int						m_fullMoveNumberCount = 1;
	std::string				m_currentFEN;
};