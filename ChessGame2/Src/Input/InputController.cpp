#include"InputController.h"
#include<iostream>

InputController::InputController(GameControl& g) :logic(g){} 


void InputController::handleEvent(sf::RenderWindow& window, sf::Event& e, const sf::View& boardView) {
	
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos,boardView);
	Position mouseGridPos = getMouseHoverPosition(window,boardView);

	if (e.type == sf::Event::MouseButtonPressed) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			logic.handleSquareSelection(mouseGridPos);
			logic.handlePress(mouseGridPos, worldPos);
			
			
		}
	}
	
	if (e.type == sf::Event::MouseMoved) {
		 
		logic.handleMove(worldPos);
	}

	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			logic.handleRelease(mouseGridPos);

		}
	}


	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::Z) logic.executeUndoMove();
		
	}

	if (e.type == sf::Event::KeyPressed) {

		if(e.key.code==sf::Keyboard::Escape) logic.resetGame();
	}

	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::Y) {
			logic.executeRedoMove();
		}
	}

}

Position InputController::getMouseHoverPosition(const sf::RenderWindow& window, const sf::View& boardView) {

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, boardView);

	float boardX = worldPos.x;
	float boardY = worldPos.y;

	return pixelToCoordinate(sf::Vector2i(boardX,boardY),squareSize);

}