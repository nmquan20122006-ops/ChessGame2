#pragma once
#include<SFML/Graphics.hpp>
#include"Utility.h"

class HighlightRenderer {
private:

	sf::RectangleShape highlightSquare;

	sf::Sprite highlightDot;
	sf::Sprite captureRing;

public:

	HighlightRenderer();

	void initTexture(sf::Texture& texture, sf::Texture& texture2);

	void drawSquare(sf::RenderWindow& window, Position pos, sf::Color color);

	void drawDot(sf::RenderWindow& window, Position pos);

	void drawRing(sf::RenderWindow& window, Position pos);

};