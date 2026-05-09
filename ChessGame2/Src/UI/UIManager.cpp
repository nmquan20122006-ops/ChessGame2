#include"UIManager.h"

UIManager::UIManager(sf::Vector2u windowSize,AnalysisPanel& a): analysisPanel(a){

	boardView.setSize(BOARD_PIXEL_SIZE + 2 * OFFSET, BOARD_PIXEL_SIZE + 2 * OFFSET);
	boardView.setCenter((BOARD_PIXEL_SIZE + 2 * OFFSET) / 2.f, (BOARD_PIXEL_SIZE + 2 * OFFSET) / 2.f);
	
	onResize(windowSize.x, windowSize.y);
}

void UIManager::onResize(unsigned int width, unsigned int height) {

	this->boardView = getLetterBox(this->boardView, width, height);

}

sf::View UIManager::getLetterBox(sf::View& boardView, unsigned int width, unsigned int height) {

	if (width == 0 || height == 0)return boardView;

	float windowRatio = (float)width / height;
	float viewRatio = boardView.getSize().x / boardView.getSize().y;

	float posX, posY, sizeX, sizeY;

	posX = posY = 0.f;	sizeX = sizeY = 1.f;

	bool horizontalSpacing = windowRatio > viewRatio;

	if (horizontalSpacing) {

		sizeX = viewRatio / windowRatio;
		posX = (1.f - sizeX) / 2.f;
	}
	else {

		sizeY = windowRatio / viewRatio;
		posY = (1.f - sizeY) / 2.f;
	}

	boardView.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

	return boardView;
}
