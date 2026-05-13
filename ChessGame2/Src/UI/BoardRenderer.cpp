#include"BoardRenderer.h"
#include"State/GameState.hpp"
#include"PieceRenderer.h"

BoardRenderer::BoardRenderer() {

	light = sf::Color(132, 85, 68);
	dark = sf::Color(94, 55, 52);
	
}

void BoardRenderer::draw(sf::RenderWindow& window) {

	//border
	sf::RectangleShape border(sf::Vector2f(BOARD_PIXEL_SIZE + OFFSET * 2, BOARD_PIXEL_SIZE + OFFSET * 2));
	border.setPosition(0, 0);
	border.setFillColor(sf::Color(45, 25, 20));
	window.draw(border);
	window.draw(borderSprite);

	//outside border
	sf::RectangleShape outSideBorder(sf::Vector2f(BOARD_PIXEL_SIZE + OFFSET * 2, BOARD_PIXEL_SIZE + OFFSET * 2));
	outSideBorder.setFillColor(sf::Color::Transparent);
	outSideBorder.setOutlineThickness(-8.f);
	outSideBorder.setOutlineColor(sf::Color(28, 16, 13));
	outSideBorder.setPosition(0, 0);
	window.draw(outSideBorder);

	//board
	sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {

			square.setPosition(j * SQUARE_SIZE + OFFSET, i * SQUARE_SIZE + OFFSET);

			if ((i + j) % 2 == 0) {
				square.setFillColor(light);
			}
			else {
				square.setFillColor(dark);
			}

			window.draw(square);
		}
	}

	window.draw(boardSprite);
}

void BoardRenderer::drawHighlight(sf::RenderWindow& window,Position pos ,sf::Color Color) {

	hightlightSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));

	hightlightSquare.setPosition(pos.col * SQUARE_SIZE + OFFSET, pos.row * SQUARE_SIZE + OFFSET);

	hightlightSquare.setFillColor(Color);

	window.draw(hightlightSquare);
}

void BoardRenderer::initBoardTexture(sf::Texture& texture) {

	boardSprite.setTexture(texture);
	boardSprite.setPosition(OFFSET, OFFSET);
	
	boardSprite.setColor(sf::Color(255, 255, 255, 15));
	boardSprite.setScale(0.64f, 0.9711f);

}

void BoardRenderer::initBorderTexture(sf::Texture& texture) {

	borderSprite.setTexture(texture);
	borderSprite.setPosition(0, 0);

	borderSprite.setColor(sf::Color(255, 255, 255, 40));
	borderSprite.setScale(0.664f, 0.8302f);
}

void BoardRenderer::initFont(sf::Font& font) {

	this->text.setFont(font);

	std::string files[8] = { "a", "b", "c", "d", "e", "f", "g", "h" };

	for (int i = 0; i < 8; i++) {
		sf::Text t;

		t.setFont(font); 

		t.setString(files[i]);  

		t.setCharacterSize(20);

		coordTexts.push_back(t);
	}
	
}

void BoardRenderer::initHighlightTexture(sf::Texture& dot_Texture, sf::Texture& captureRing_Texture) {

	highlightDot.setTexture(dot_Texture);

	captureRing.setTexture(captureRing_Texture);

	sf::FloatRect dotBounds = highlightDot.getGlobalBounds();
	highlightDot.setOrigin(dotBounds.width / 2.f, dotBounds.height / 2.f);

	sf::FloatRect ringBounds = captureRing.getGlobalBounds();
	captureRing.setOrigin(ringBounds.width / 2.f, ringBounds.height / 2.f);


}

void BoardRenderer::drawCoordinates(sf::RenderWindow& window) {

	text.setFillColor(sf::Color(245, 222, 179));
	text.setCharacterSize(15);

	for (int i = 0; i < 8; i++) {
		
		std::string rowLabel = std::to_string(8 - i);
		text.setString(rowLabel);

		sf::FloatRect bNum = text.getLocalBounds();
		text.setOrigin(bNum.left + bNum.width / 2.0f, bNum.top + bNum.height / 2.0f);

		text.setPosition(OFFSET / 2.0f, OFFSET + i * SQUARE_SIZE + SQUARE_SIZE / 2.0f);
		window.draw(text);

		text.setPosition(OFFSET + BOARD_PIXEL_SIZE + OFFSET / 2.0f, OFFSET + i * SQUARE_SIZE + SQUARE_SIZE / 2.0f);
		window.draw(text);


		std::string colLabel(1, 'a' + i);
		text.setString(colLabel);

		sf::FloatRect bChar = text.getLocalBounds();
		text.setOrigin(bChar.left + bChar.width / 2.0f, bChar.top + bChar.height / 2.0f);

		text.setPosition(OFFSET + i * SQUARE_SIZE + SQUARE_SIZE / 2.0f, OFFSET + BOARD_PIXEL_SIZE + OFFSET / 2.0f);
		window.draw(text);

		text.setPosition(OFFSET + i * SQUARE_SIZE + SQUARE_SIZE / 2.0f, OFFSET / 2.0f);
		window.draw(text);
	}
}

void BoardRenderer::drawHover(sf::RenderWindow& window, Position pos) {

	if (pos.row < 0 || pos.row>7 || pos.col < 0 || pos.col>7)return;

	sf::RectangleShape hoverSquare(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));

	hoverSquare.setFillColor(sf::Color(255, 255, 255, 50));

	hoverSquare.setPosition(pos.col * SQUARE_SIZE + OFFSET, pos.row * SQUARE_SIZE + OFFSET);

	window.draw(hoverSquare);
}

void BoardRenderer::drawHighlightLastMove(sf::RenderWindow& window, Position fromPos, Position toPos) {

	drawHighlight(window, fromPos, sf::Color(245, 246, 130, 40));

	drawHighlight(window, toPos, sf::Color(245, 246, 130, 40));

}

void BoardRenderer::drawKingCheckSquare(sf::RenderWindow& window, Position kingPos) {

	drawHighlight(window, kingPos, sf::Color(255, 0, 0, 40));
}

void BoardRenderer::drawValidMove(sf::RenderWindow& window, Position pos,bool isEnemy) {

	float centerX = pos.col * SQUARE_SIZE + OFFSET + (SQUARE_SIZE / 2.0f);
	float centerY = pos.row * SQUARE_SIZE + OFFSET + (SQUARE_SIZE / 2.0f);

	highlightDot.setPosition(centerX, centerY);
	captureRing.setPosition(centerX, centerY);

	if (isEnemy) {

		window.draw(captureRing);
	}

	else {

		window.draw(highlightDot);
	}

}

void BoardRenderer::initCursorTexture(sf::Texture& cursorTexture) {

	cursorSprite.setTexture(cursorTexture);

	cursorSprite.setScale(0.8f,0.8f);
}

void BoardRenderer::drawCursor(sf::RenderWindow& window,sf::Vector2f worldPos) {

	cursorSprite.setPosition(worldPos);

	window.draw(cursorSprite);
}

void BoardRenderer::drawHintLine(sf::RenderWindow& window, Position from, Position to, sf::Color color) {
	if (from == to) return;

	float thickness = 15.0f;
	float arrowHeadSize = thickness * 1.5f;

	auto getCenter = [&](Position p) {
		return sf::Vector2f(
			p.col * SQUARE_SIZE + OFFSET + SQUARE_SIZE / 2.0f,
			p.row * SQUARE_SIZE + OFFSET + SQUARE_SIZE / 2.0f
		);
		};

	sf::Vector2f fromPos = getCenter(from);
	sf::Vector2f toPos = getCenter(to);

	sf::Vector2f direction = toPos - fromPos;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length < arrowHeadSize) return; 

	sf::Vector2f dir = direction / length;
	sf::Vector2f perp(-dir.y, dir.x);

	sf::VertexArray lineBody(sf::TriangleStrip, 4);
	sf::Vector2f halfThick = perp * (thickness / 2.0f);
	sf::Vector2f lineEndPos = toPos - dir * arrowHeadSize;
	sf::Color lineColor(color);

	lineBody[0].position = fromPos + halfThick;
	lineBody[1].position = fromPos - halfThick;
	lineBody[2].position = lineEndPos + halfThick;
	lineBody[3].position = lineEndPos - halfThick;

	for (int i = 0; i < 4; i++) lineBody[i].color = lineColor;
	window.draw(lineBody);

	sf::ConvexShape arrow;
	arrow.setPointCount(3);
	arrow.setPoint(0, toPos);
	arrow.setPoint(1, lineEndPos + perp * arrowHeadSize);
	arrow.setPoint(2, lineEndPos - perp * arrowHeadSize);
	arrow.setFillColor(color);

	window.draw(arrow);
}

void BoardRenderer::drawPromotionPanel(sf::RenderWindow& window, Color turn, int col) {

	int direction = (turn == Color::white) ? 1 : -1;

	sf::RectangleShape promotionPanel(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE * 4));
	promotionPanel.setFillColor(sf::Color(255, 255, 255, 170));
	promotionPanel.setPosition(OFFSET + col * SQUARE_SIZE, OFFSET + (turn == Color::white ? 0 : 4 * SQUARE_SIZE));
	promotionPanel.setOutlineThickness(2.f);
	promotionPanel.setOutlineColor(sf::Color::Black);

	window.draw(promotionPanel);

}