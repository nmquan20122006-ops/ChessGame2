#pragma once

#include<SFML/Graphics.hpp>
#include"Board.h"
#include"TextureManager.h"
#include"Piece.h"
#include"GameControl.h"


class PieceRenderer {

private:
	sf::Sprite pieceSprite[2][6]; //mảng sprite cho 2 màu và 6 loại quân cờ

	GameControl& game;

	std::set<Position> m_excludedPositions;

	float baseScale;

public:
	PieceRenderer(GameControl& g);

	void setupPieceSprites(TextureManager& tm);

	void drawPiece(sf::RenderWindow& window, Position pos, const Board& board);

	void renderAll(sf::RenderWindow& window, const Board& board);

	sf::Sprite& getSpriteForPiece(Piece p);

	sf::Sprite getCopySprite(Piece p);

	float getBaseScale()const { return baseScale; }

	void excludePosition(Position pos) { m_excludedPositions.insert(pos); }
	void includePosition(Position pos) { m_excludedPositions.erase(pos); }
	void clearExclusions() { m_excludedPositions.clear(); }
};