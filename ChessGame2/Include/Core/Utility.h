#pragma once
#include<iostream>
#include<SFML/System/Vector2.hpp>
#include"Constants.h"

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

inline Position pixelToCoordinate(sf::Vector2i pixelPos, int squareSize) {

	int c = static_cast<int>(std::floor((static_cast<float>(pixelPos.x) - offset) / squareSize));
	int r = static_cast<int>(std::floor((static_cast<float>(pixelPos.y) - offset) / squareSize));

	return { r, c };
}

inline sf::Vector2f inMiddleSquare(Position pos) {
	
	float sSize = static_cast<float>(squareSize);
	float off = static_cast<float>(offset);

	float centerX = static_cast<float>(pos.col) * sSize + off + (sSize / 2.0f);
	float centerY = static_cast<float>(pos.row) * sSize + off + (sSize / 2.0f);

	return sf::Vector2f(centerX, centerY);
}

inline sf::Vector2f toPixel(Position pos) {
	
	float x = static_cast<float>(pos.col * squareSize + offset);
	float y = static_cast<float>(pos.row * squareSize + offset);

	return sf::Vector2f(x, y);
}


