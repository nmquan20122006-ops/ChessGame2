#pragma once
#include<SFML/Graphics.hpp>
#include"Constants.h"
#include"Utility.h"

enum class Color;
class PieceRenderer;

class BoardRenderer {

private:
	sf::Color light;
	sf::Color dark;
	sf::Color highlightColor;

	sf::RectangleShape hightlightSquare;

	sf::Sprite boardSprite;
	sf::Sprite borderSprite;
	sf::Text text;

	std::vector<sf::Text>coordTexts;

	sf::Sprite highlightDot;
	sf::Sprite captureRing;

	sf::RectangleShape panel;

	sf::Sprite cursorSprite;

public:
	BoardRenderer();

	void draw(sf::RenderWindow& window);

	void drawHighlight(sf::RenderWindow& window, Position pos, sf::Color Color);

	void drawValidMove(sf::RenderWindow& window, Position pos, bool isEnemy);

	void initHighlightTexture(sf::Texture& texture, sf::Texture& texture2);

	void drawHighlightLastMove(sf::RenderWindow& window, Position fromPos, Position toPos);

	void drawKingCheckSquare(sf::RenderWindow& window, Position kingPos);	

	void drawHover(sf::RenderWindow& window, Position pos);

	void drawCoordinates(sf::RenderWindow& window);

	void drawCursor(sf::RenderWindow& window,sf::Vector2f pos);

	void initBoardTexture(sf::Texture& texture);

	void initBorderTexture(sf::Texture& texture);

	void initCursorTexture(sf::Texture& texture);

	void drawPromotionPanel(sf::RenderWindow& window,Color turn, int col);

	void initFont(sf::Font& font);

};