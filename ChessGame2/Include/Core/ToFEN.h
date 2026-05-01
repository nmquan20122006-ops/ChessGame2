/**
* @file ToFEN.h
* @brief This file defines the ToFEN class, which provides functionality to convert the current state of a chess game into Forsyth-Edwards Notation (FEN)
* @brief FEN is a standard notation for describing a particular board position of a chess game. The ToFEN class includes methods to convert pieces, turns, castling rights, en passant targets, and the overall board state into their corresponding FEN representations
* @brief This allows for easy serialization of the game state, which can be used for saving/loading games, interfacing with chess engines, or sharing positions.
* @author [Nguyen Minh Quan]
* @date [30/4/2026]
* @version [1.0]
* copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
*/
#pragma once
#include<string>
#include<iostream>

#include"Board.h"
#include"State/GameState.hpp"

class ToFEN {
private:
	static char PieceToChar(Piece piece);
	static char TurnToChar(Color turn);

	static std::string castleToFEN(const Board& board) ;
	static std::string enPassantToFEN(const Move& move);
	static std::string BoardToFEN(const Board& board);

public:
	ToFEN() = default;

	static std::string FullFEN(const Board& board, const Move& move,const GameState& gameState);

	static Piece charToPiece(Color turn, char promotionChar);
};