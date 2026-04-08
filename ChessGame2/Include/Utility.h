#pragma once
#include"../Constants/Constants.h"
#include<SFML/Graphics.hpp>

struct Position {

	int row;
	int col;

	bool operator==(const Position other) const;

	bool operator!=(const Position other)const;

	Position(int r = -1, int c = -1);

};

inline Position pixelToCoordinate(sf::Vector2i pixelPos, int squareSize) {

	int c = (pixelPos.x - offset) / squareSize;
	int r = (pixelPos.y - offset) / squareSize;

	return { r, c };
}

inline sf::Vector2f inMiddleSquare(Position pos) {

	float centerX = pos.col * squareSize + offset + (squareSize / 2.0f);
	float centerY = pos.row * squareSize + offset + (squareSize / 2.0f);

	return sf::Vector2f({ centerX,centerY });
}

inline sf::Vector2f toPixel(Position pos) {
	
	float x = pos.col * squareSize + offset;
	float y = pos.row * squareSize + offset;

	return sf::Vector2f(x, y);
}

