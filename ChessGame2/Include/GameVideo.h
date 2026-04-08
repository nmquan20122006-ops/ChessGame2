#pragma once

#include<SFML/Graphics.hpp>

#include"Constants.h"
#include"BoardRenderer.h"
#include"TextureManager.h"
#include"PieceRenderer.h"
#include"InputController.h"
#include"GameAudio.h"
#include"GameControl.h"
#include"HighlightRenderer.h"
#include"StockfishPlay.h"
#include"Animation.h"
#include"Define.h"

class GameVideo {

private:

	Board board;

	sf::RenderWindow window;

	TextureManager textureManager;

	BoardRenderer boardRenderer;

	PieceRenderer pieceRender;

	InputController inputController;

	MoveValidator moveValidator;

	AllValidMove allValidMove;

	AudioManager audio;

	MoveExecutor moveExecutor;

	GameControl gameControl;

	GameState state;

	MoveService moveService;

	HighlightRenderer highlightRenderer;

	color c;

	sf::Clock clock;

	TweenyAnimator animator;

	PendingMove pendingMove;

	StockfishGame gameAi;

	void syncAIWithGame();

	void executeAIMove(const std::string& uciMove);

	void handleAi();

	isOnProcess isGameProcess;

public:
	GameVideo();

	void renderWindow(sf::RenderWindow& window);

	void setupWindow();

	void Run();

	void initTexture();

	void initSound();

	void initStockfish();

	void renderHightlight(sf::RenderWindow& window);

	void renderHighlightValidMove(sf::RenderWindow& window);

	void renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m);

	void renderBoardHover(sf::RenderWindow& window);

	void renderHighlightLastMove(sf::RenderWindow& window);

	void renderCheckSquare(sf::RenderWindow& window);

	void changeCursor(sf::RenderWindow& window);

	void loadTextureForMember();

	void handleAudioEvents();

	void pieceAnimation(Position from, Position to, Piece p);

	void executePieceAnim(sf::RenderWindow& window, float dtMs);

	void gameLoop();

	void gameEvent(sf::Event e);
};