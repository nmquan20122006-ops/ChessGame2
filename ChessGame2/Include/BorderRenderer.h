#pragma once

#include<SFML/Graphics.hpp>
#include<vector>
#include"Utility.h"

class BorderRenderer {
private:
	
	sf::RectangleShape border;
	sf::RectangleShape outSideBorder;
	
	sf::Text text;
	std::vector<sf::Text>coordTexts;

public:

	BorderRenderer();

	void draw(sf::RenderWindow& window);

	void initFont(sf::Font& font);

	void drawCoordinates(sf::RenderWindow& window);
};