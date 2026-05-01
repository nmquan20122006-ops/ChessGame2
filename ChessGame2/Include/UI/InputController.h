#pragma once

#include<SFML/Graphics.hpp>
#include"Utility.h"
#include"Constants.h"

class InputHandler;
class GameControl;

class InputController {
private:

	InputHandler& inputHandler;
	GameState& gameState;
	GameControl* gameControl;

public:

	InputController(InputHandler& inputHandler, GameState& gameState, GameControl* gameControl);

	void handleEvent(sf::RenderWindow& window, sf::Event& e,const sf::View& view);

	void handleClickPromotionPanel(sf::Vector2f worldPos);
	
	Position getMouseHoverPosition(const sf::RenderWindow& window,const sf::View& view);

};
