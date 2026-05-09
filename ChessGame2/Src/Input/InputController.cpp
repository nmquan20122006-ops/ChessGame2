#include"InputController.h"
#include"GameControl.h"
#include"InputHandler.h"
#include<iostream>

InputController::InputController(InputHandler& i, GameState& g, GameControl* c): inputHandler(i), gameState(g), gameControl(c) {}

void InputController::handleEvent(sf::RenderWindow& window, sf::Event& e, const sf::View& boardView) {
	
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos,boardView);
	Position mouseGridPos = getMouseHoverPosition(window,boardView);

	if (gameState.getGameStatus() == GameStatus::WaitingForPromotion) {

		if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {

			inputHandler.handleClickPromotionPanel(worldPos);
		}

		return;
	}

	if (e.type == sf::Event::MouseButtonPressed) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			inputHandler.handleSquareSelection(mouseGridPos);
			inputHandler.handlePress(mouseGridPos, worldPos);
			
		}
	}
	
	if (e.type == sf::Event::MouseMoved) {
		 
		inputHandler.handleMove(worldPos);
		
	}

	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			inputHandler.handleRelease(mouseGridPos);

		}
	}

	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::Z) gameControl->executeUndoMove();
		
	}

	if (e.type == sf::Event::KeyPressed) {

		if(e.key.code==sf::Keyboard::Escape) gameControl->resetGame();
	}

	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::L)gameControl->executeHint();

	}

}

Position InputController::getMouseHoverPosition(const sf::RenderWindow& window, const sf::View& boardView) {

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, boardView);

	float boardX = worldPos.x;
	float boardY = worldPos.y;

	return pixelToCoordinate(sf::Vector2i(boardX,boardY),SQUARE_SIZE);

}