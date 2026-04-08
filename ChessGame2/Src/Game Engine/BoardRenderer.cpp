#include"BoardRenderer.h"

BoardRenderer::BoardRenderer() {

	light = sf::Color(132, 85, 68);
	dark = sf::Color(94, 55, 52);
	
	panel.setFillColor(sf::Color(32, 28, 24));
	panel.setSize(sf::Vector2f(windowWidth, windowHeight));

	
}

void BoardRenderer::draw(sf::RenderWindow& window) {

	//border
	
	sf::RectangleShape border(sf::Vector2f(boardPixelSize + offset * 2, boardPixelSize + offset * 2));
	border.setPosition(0, 0);
	border.setFillColor(sf::Color(45, 25, 20));
	window.draw(border);
	window.draw(borderSprite);

	//outside border
	sf::RectangleShape outSideBorder(sf::Vector2f(boardPixelSize + offset * 2, boardPixelSize + offset * 2));
	outSideBorder.setFillColor(sf::Color::Transparent);
	outSideBorder.setOutlineThickness(-8.f);
	outSideBorder.setOutlineColor(sf::Color(28, 16, 13));
	outSideBorder.setPosition(0, 0);
	window.draw(outSideBorder);

	//board
	sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));

	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {

			square.setPosition(j * squareSize + offset, i * squareSize + offset);

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

void BoardRenderer::drawHighlight(sf::RenderWindow& window,Position pos ,sf::Color color) {

	hightlightSquare.setSize(sf::Vector2f(squareSize, squareSize));

	hightlightSquare.setPosition(pos.col * squareSize + offset, pos.row * squareSize + offset);

	hightlightSquare.setFillColor(color);

	window.draw(hightlightSquare);
}

void BoardRenderer::initBoardTexture(sf::Texture& texture) {

	boardSprite.setTexture(texture);
	boardSprite.setPosition(offset, offset);
	
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

		text.setPosition(offset / 2.0f, offset + i * squareSize + squareSize / 2.0f);
		window.draw(text);

		text.setPosition(offset + boardPixelSize + offset / 2.0f, offset + i * squareSize + squareSize / 2.0f);
		window.draw(text);


		std::string colLabel(1, 'a' + i);
		text.setString(colLabel);

		sf::FloatRect bChar = text.getLocalBounds();
		text.setOrigin(bChar.left + bChar.width / 2.0f, bChar.top + bChar.height / 2.0f);

		text.setPosition(offset + i * squareSize + squareSize / 2.0f, offset + boardPixelSize + offset / 2.0f);
		window.draw(text);

		text.setPosition(offset + i * squareSize + squareSize / 2.0f, offset / 2.0f);
		window.draw(text);
	}
}

void BoardRenderer::drawHover(sf::RenderWindow& window, Position pos) {

	if (pos.row < 0 || pos.row>7 || pos.col < 0 || pos.col>7)return;

	sf::RectangleShape hoverSquare(sf::Vector2f(squareSize, squareSize));

	hoverSquare.setFillColor(sf::Color(255, 255, 255, 80));

	hoverSquare.setPosition(pos.col * squareSize + offset, pos.row * squareSize + offset);

	window.draw(hoverSquare);
}

void BoardRenderer::drawPanel(sf::RenderWindow& window) {

	panel.setPosition(0, 0);
	
	window.draw(panel);
}

void BoardRenderer::drawHighlightLastMove(sf::RenderWindow& window, Position fromPos, Position toPos) {

	drawHighlight(window, fromPos, sf::Color(167, 224, 108,80));

	drawHighlight(window, toPos, sf::Color(167, 224, 108,70));

}

void BoardRenderer::drawKingCheckSquare(sf::RenderWindow& window, Position kingPos) {

	drawHighlight(window, kingPos, sf::Color(255, 0, 0, 150));
}

void BoardRenderer::drawValidMove(sf::RenderWindow& window, Position pos,bool isEnemy) {

	float centerX = pos.col * squareSize + offset + (squareSize / 2.0f);
	float centerY = pos.row * squareSize + offset + (squareSize / 2.0f);

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