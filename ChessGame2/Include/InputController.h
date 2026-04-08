#pragma once

#include<SFML/Graphics.hpp>
#include"Utility.h"
#include"../Constants/Constants.h"
#include"GameControl.h"

class InputController {
private:

	GameControl& logic;

public:

	InputController(GameControl& g);

	void handleEvent(sf::RenderWindow& window, sf::Event& e);
	
	Position getMouseHoverPosition(const sf::RenderWindow& window);

};
