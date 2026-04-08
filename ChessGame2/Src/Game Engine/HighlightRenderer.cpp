#include"HighlightRenderer.h"



HighlightRenderer::HighlightRenderer() {

	highlightSquare.setSize(sf::Vector2f(squareSize, squareSize));
}

void HighlightRenderer::drawSquare(sf::RenderWindow& window, Position pos, sf::Color color) {

	highlightSquare.setPosition(pos.col * squareSize + offset, pos.row * squareSize + offset);

	highlightSquare.setFillColor(color);

	window.draw(highlightSquare);
}

void HighlightRenderer::initTexture(sf::Texture& dot_Texture, sf::Texture& captureRing_Texture) {

	highlightDot.setTexture(dot_Texture);

	captureRing.setTexture(captureRing_Texture);

	sf::FloatRect dotBounds = highlightDot.getGlobalBounds();
	highlightDot.setOrigin(dotBounds.width / 2.f, dotBounds.height / 2.f);

	sf::FloatRect ringBounds = captureRing.getGlobalBounds();
	captureRing.setOrigin(ringBounds.width / 2.f, ringBounds.height / 2.f);

}

void HighlightRenderer::drawDot(sf::RenderWindow& window, Position pos) {

	sf::Vector2f center = inMiddleSquare(pos);

	highlightDot.setPosition(center.x,center.y);

	window.draw(highlightDot);
}

void HighlightRenderer::drawRing(sf::RenderWindow& window, Position pos) {

	sf::Vector2f center = inMiddleSquare(pos);

	captureRing.setPosition(center.x, center.y);

	window.draw(captureRing);
}