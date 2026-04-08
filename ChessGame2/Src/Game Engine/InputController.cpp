#include"InputController.h"

InputController::InputController(GameControl& g) :logic(g){}


void InputController::handleEvent(sf::RenderWindow& window, sf::Event& e) {
	
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
	Position mouseGridPos = getMouseHoverPosition(window);

	if (e.type == sf::Event::MouseButtonPressed) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			logic.handlePress(mouseGridPos,worldPos);
			
			
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



}

Position InputController::getMouseHoverPosition(const sf::RenderWindow& window) {

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	return pixelToCoordinate(mousePos, squareSize);

}