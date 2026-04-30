#include"InputController.h"
#include"GameControl.h"
#include<iostream>

InputController::InputController() {} 

void InputController::handleEvent(sf::RenderWindow& window, sf::Event& e, const sf::View& boardView) {
	
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos,boardView);
	Position mouseGridPos = getMouseHoverPosition(window,boardView);

	if (logic->getState().getGameStateEnum() == GameStateEnum::waitingForPromotion) {

		if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {

			handleClickPromotionPanel(worldPos);
		}

		return;
	}

	if (e.type == sf::Event::MouseButtonPressed) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			logic->handleSquareSelection(mouseGridPos);
			logic->handlePress(mouseGridPos, worldPos);
			
			
		}
	}
	
	if (e.type == sf::Event::MouseMoved) {
		 
		logic->handleMove(worldPos);
	}

	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.mouseButton.button == sf::Mouse::Left) {

			logic->handleRelease(mouseGridPos);

		}
	}


	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::Z) logic->executeUndoMove();
		
	}

	if (e.type == sf::Event::KeyPressed) {

		if(e.key.code==sf::Keyboard::Escape) logic->resetGame();
	}

	if (e.type == sf::Event::KeyPressed) {

		if (e.key.code == sf::Keyboard::Y) {
			
		}
	}

}

void InputController::handleClickPromotionPanel(sf::Vector2f worldPos) {

	int col = static_cast<int>((worldPos.x - offset) / squareSize);
	int row = static_cast<int>((worldPos.y - offset) / squareSize);

	Position pendingTo = logic->getState().getPendingTo();
	color turn = logic->getState().getCurrentTurn();

	if (col == pendingTo.col) {
		int startRow = (turn == color::white) ? 0 : 4;
		int endRow = (turn == color::white) ? 3 : 7;

		if (row >= startRow && row <= endRow) {
			
			int index = std::abs(row - startRow);

			Piece selected;
			if (turn == color::white) {
				if (index == 0) selected = Piece::W_Queen;
				else if (index == 1) selected = Piece::W_Rook;
				else if (index == 2) selected = Piece::W_Bishop;
				else selected = Piece::W_Knight;
			}
			else {
				if (index == 0) selected = Piece::B_Queen;
				else if (index == 1) selected = Piece::B_Rook;
				else if (index == 2) selected = Piece::B_Bishop;
				else selected = Piece::B_Knight;
			}

			//send the selected promotion piece to game control to execute the promotion move
			logic->executePromotionMove(selected);
		}
		else {
			
			//logic->cancelPromotion();
		}
	}
	else {
		
		//logic->cancelPromotion();
	}
}

Position InputController::getMouseHoverPosition(const sf::RenderWindow& window, const sf::View& boardView) {

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, boardView);

	float boardX = worldPos.x;
	float boardY = worldPos.y;

	return pixelToCoordinate(sf::Vector2i(boardX,boardY),squareSize);

}