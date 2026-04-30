#pragma once

#include<SFML/Graphics.hpp>
#include"Utility.h"
#include"Constants.h"

class GameControl;

class InputController {
private:

	GameControl* logic = nullptr;

public:

	InputController();

	void setTarget(GameControl* control) { logic = control; }

	void handleEvent(sf::RenderWindow& window, sf::Event& e,const sf::View& view);

	void handleClickPromotionPanel(sf::Vector2f worldPos);
	
	Position getMouseHoverPosition(const sf::RenderWindow& window,const sf::View& view);

};
