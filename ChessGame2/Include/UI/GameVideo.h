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
	std::shared_ptr<Board> board;
	std::shared_ptr<GameState> state;

	std::unique_ptr<MoveExecutor> moveExecutor;
	std::unique_ptr<MoveService> moveService;
	std::unique_ptr<StockfishGame> stockfishGame;
	std::unique_ptr<DumpBot> dumpBot;

	std::unique_ptr<GameControl> gameControl;

	TextureManager textureManager;
	AudioManager audio;

	BoardRenderer boardRenderer;
	PieceRenderer pieceRender;
	InputController inputController;

	sf::RenderWindow window;
	tgui::GuiSFML gui;
	TweenyAnimator animator;
	sf::Clock clock;

	UIManager uiManager;
	AnalysisPanel analysisPanel;
	InformationPanel informationPanel;
	
	sf::Vector2u windowSize = { windowWidth,windowHeight };

	const sf::View& boardView = uiManager.getBoardView();
	const sf::View& uiView = uiManager.getUiView();

public:

	GameVideo();

	void renderWindow(sf::RenderWindow& window,float dt);

	void Run();

	void renderHightlight(sf::RenderWindow& window);

	void renderHighlightValidMove(sf::RenderWindow& window);

	void renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m);

	void renderBoardHover(sf::RenderWindow& window);

	void renderHighlightLastMove(sf::RenderWindow& window);

	void renderCheckSquare(sf::RenderWindow& window);

	void renderPromotionPanel(sf::RenderWindow& window);

	void changeCursor(sf::RenderWindow& window);

	void loadTextureForMember();

	void gameLoop(float dt);

	void updateEvent(sf::Event e);

};