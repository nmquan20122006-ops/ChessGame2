#pragma once
#include<SFML/Graphics.hpp>
#include"Constants.h"
#include"AnalysisPanel.h"

class UIManager {

private:
	sf::View boardView;
	sf::View uiView;

	AnalysisPanel& analysisPanel;
public:

	UIManager(sf::Vector2u windowSize, AnalysisPanel& a);

	const sf::View& getBoardView()const { return boardView; }
	const sf::View& getUiView()const { return uiView; }

	void onResize(unsigned int width, unsigned int height);

	sf::View getLetterBox(sf::View& view, unsigned int width, unsigned int height);
	sf::View updateUiLayout(sf::View& uiView, unsigned int width, unsigned int height);

	float getBoardRightEdgeRatio()const {
		sf::FloatRect viewport = boardView.getViewport();
		return viewport.left + viewport.width;
	}
};