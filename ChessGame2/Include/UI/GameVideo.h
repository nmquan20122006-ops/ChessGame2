#pragma once

#include<SFML/Graphics.hpp>

#include"Constants.h"
#include"BoardRenderer.h"
#include"TextureManager.h"
#include"PieceRenderer.h"
#include"InputController.h"
#include"GameAudio.h"
#include"GameControl.h"
#include"Animation.h"
#include"StockfishAI.h"
#include"UIManager.h"
#include"AnalysisPanel.h"
#include"InformationPanel.h"

class GameVideo {

private:

	Board board;

	sf::RenderWindow window;
	tgui::GuiSFML gui;

	TextureManager textureManager;

	BoardRenderer boardRenderer;

	PieceRenderer pieceRender;

	InputController inputController;

	AudioManager audio;

	MoveExecutor moveExecutor;

	MoveService moveService;

	GameState state;
	StockfishGame stockfishGame;
	DumpBot dumpBot;
	
	GameControl gameControl;
	
	TweenyAnimator animator;
	sf::Clock clock;

	AnalysisPanel analysisPanel;
	InformationPanel informationPanel;
	
	sf::Vector2u windowSize = { windowWidth,windowHeight };
	UIManager uiManager;

	const sf::View& boardView = uiManager.getBoardView();
	const sf::View& uiView = uiManager.getUiView();

public:

	GameVideo();

	void renderWindow(sf::RenderWindow& window,float dt);

	void Run();

	void initStockfish();

	void renderHightlight(sf::RenderWindow& window);

	void renderHighlightValidMove(sf::RenderWindow& window);

	void renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m);

	void renderBoardHover(sf::RenderWindow& window);

	void renderHighlightLastMove(sf::RenderWindow& window);

	void renderCheckSquare(sf::RenderWindow& window);

	void changeCursor(sf::RenderWindow& window);

	void loadTextureForMember();

	void gameLoop(float dt);

	void updateEvent(sf::Event e);

};