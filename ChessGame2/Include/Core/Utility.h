#pragma once
#include<iostream>
#include<SFML/System/Vector2.hpp>
#include"Constants.h"
#include"Piece.h"
#include"State/Type.hpp"
#include"State/Aistate.hpp"

class GameState;
struct Move;

struct Position {

	int row;
	int col;

	bool operator==(const Position other) const;

	bool operator!=(const Position other)const;

	bool operator<(const Position& other) const {
		if (row != other.row) {
			return row < other.row;
		}
		return col < other.col;
	}


	Position(int r = -1, int c = -1);

};

inline Position pixelToCoordinate(sf::Vector2i pixelPos, int SQUARE_SIZE) {

	int c = static_cast<int>(std::floor((static_cast<float>(pixelPos.x) - OFFSET) / SQUARE_SIZE));
	int r = static_cast<int>(std::floor((static_cast<float>(pixelPos.y) - OFFSET) / SQUARE_SIZE));

	return { r, c };
}

inline sf::Vector2f inMiddleSquare(Position pos) {
	
	float sSize = static_cast<float>(SQUARE_SIZE);
	float off = static_cast<float>(OFFSET);

	float centerX = static_cast<float>(pos.col) * sSize + off + (sSize / 2.0f);
	float centerY = static_cast<float>(pos.row) * sSize + off + (sSize / 2.0f);

	return sf::Vector2f(centerX, centerY);
}

inline sf::Vector2f toPixel(Position pos) {
	
	float x = static_cast<float>(pos.col * SQUARE_SIZE + OFFSET);
	float y = static_cast<float>(pos.row * SQUARE_SIZE + OFFSET);

	return sf::Vector2f(x, y);
}

inline std::string moveToNotation(Position pos) {

	char file = 'a' + pos.col;
	char rank = '8' - pos.row;
	return std::string(1, file) + rank;

}

inline std::string toUCI(const Position& pos) {
	char file = 'a' + pos.col;
	char rank = '8' - pos.row;
	return std::string(1, file) + rank;
}

inline void fromUCI(const std::string& uci, Position& pos) {
	if (uci.length() >= 2) {
		pos.col = uci[0] - 'a';
		pos.row = '8' - uci[1];
	}
}

inline std::string moveToUCI(const Position& from, const Position& to, char promotion) {

	std::string uci = toUCI(from) + toUCI(to);
	if (promotion != '\0') {
		uci += promotion;
	}
	return uci;
}

inline Piece charToPiece(Color turn, char promotionChar) {

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

inline float EvalToRatio(const EngineEvaluation& Eval) {
	if (Eval.isCheckMate) {
		return Eval.checkMateIn > 0 ? 1.0f : 0.0f;
	}
	// Sigmoid — tự nhiên hơn linear
	float x = Eval.score / 400.0f;
	return 1.0f / (1.0f + std::exp(-x));
}