#include"BorderRenderer.h"

BorderRenderer::BorderRenderer() {

	border.setSize(sf::Vector2f(boardPixelSize + offset * 2, boardPixelSize + offset * 2));
	border.setPosition(0, 0);
	border.setFillColor(sf::Color(45, 25, 20));

	outSideBorder.setSize(sf::Vector2f(boardPixelSize + offset * 2, boardPixelSize + offset * 2));
	outSideBorder.setFillColor(sf::Color::Transparent);
	outSideBorder.setOutlineThickness(-8.f);
	outSideBorder.setOutlineColor(sf::Color(28, 16, 13));
	outSideBorder.setPosition(0, 0);

	text.setFillColor(sf::Color(245, 222, 179));
	text.setCharacterSize(15);
}

void BorderRenderer::draw(sf::RenderWindow& window) {

	window.draw(border);
	window.draw(outSideBorder);
}

void BorderRenderer::initFont(sf::Font& font) {

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

void BorderRenderer::drawCoordinates(sf::RenderWindow& window) {

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