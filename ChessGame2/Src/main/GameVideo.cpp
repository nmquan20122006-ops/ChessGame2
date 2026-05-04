#include"GameVideo.h"
#include"EventBus.h"
#include<iostream>

GameVideo::GameVideo() :
	window				(sf::VideoMode(windowWidth, windowHeight), "Chess"),
	gui					(window),
	analysisPanel		(gui),
	informationPanel	(gui),
	board				(std::make_shared<Board>()),
	state				(std::make_shared<GameState>()),
	moveService			(std::make_shared<MoveService>()),
	uiManager			(windowSize, analysisPanel),
	pieceRender			(state),
	audio				()
	{

	moveExecutor =		std::make_unique<MoveExecutor>(board,state);
	inputHandler =		std::make_unique<InputHandler>(board, state, moveService);
	gameControl =		std::make_unique<GameControl>(board, state, moveService, moveExecutor);
	inputController =	std::make_unique<InputController>(*inputHandler, *state, gameControl.get());

	float rightEdge = uiManager.getBoardRightEdgeRatio();
	float leftEdge = uiManager.getBoardView().getViewport().left;

	analysisPanel.updateLayout(rightEdge);
	informationPanel.updateLayout(leftEdge);

	initAll();
	callBackAll();
	
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(144);
	}

void GameVideo::initAll(){
	textureManager.initTexture();
	audio.initSound();
	loadTextureForMember();
	gameControl->initStockfishGame();
}

void GameVideo::callBackAll() {

	inputHandler->setOnClickMove([this](Position from, Position to) {
		gameControl->executePlayerMove(from, to);
		});

	inputHandler->setOnDragMove([this](Position from, Position to) {
		gameControl->requestMove(from, to);
		});

	inputHandler->setIsBlock([this]()->bool {
		return gameControl->isBlocking();
		});

	gameControl->setAnimationProvider([this](Position from, Position to, Piece piece, std::function<void()> onComplete) {

		if (piece == Piece::Empty) {
			if (onComplete) onComplete();
			return;
		}

		this->pieceRender.excludePosition(from);

		sf::Sprite pieceSprite = pieceRender.getCopySprite(piece);

		sf::Vector2f start = toPixel(from);
		sf::Vector2f end = toPixel(to);

		this->animator.addMove(pieceSprite, start, end, 100, [this, from, to, onComplete]() {

			this->pieceRender.includePosition(from);

			if (onComplete) onComplete();
			});
		});

}

void GameVideo::renderWindow(sf::RenderWindow& window,float dt) {

	window.setView(uiManager.getBoardView());

	window.clear(sf::Color::Black);

	boardRenderer.draw(window);

	renderCoordinate(window, textureManager);

	renderBoardHover(window);

	renderHighlightLastMove(window);

	renderCheckSquare(window);

	renderHightlight(window);

	renderHighlightValidMove(window);

	pieceRender.renderAll(window,*board);

	renderPromotionPanel(window);

	animator.render(window);

	changeCursor(window);

	gui.draw();

	window.display();

}

void GameVideo::renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m) {

	sf::Font& font = tex_m.getFont("font");

	boardRenderer.drawCoordinates(window);
}

void GameVideo::loadTextureForMember() {

	textureManager.setupChessAtlas();

	pieceRender.setupPieceSprites(textureManager);

	boardRenderer.initFont(textureManager.getFont("font"));

	boardRenderer.initHighlightTexture(textureManager.getTexture("HighlightDot"), textureManager.getTexture("CaptureRing"));

	boardRenderer.initCursorTexture(textureManager.getTexture("Minecraft Enchanted Netherite Sword"));

}

void GameVideo::renderHightlight(sf::RenderWindow& window) {

	const auto& state = gameControl->getState();

	if (state.hasSelection()) {

		boardRenderer.drawHighlight(window, state.getSelectPos(), sf::Color(255, 255, 0, 70));
	}

}

void GameVideo::renderHighlightLastMove(sf::RenderWindow& window) {
	
	LastMove lastMove = board->getLastMove();

	boardRenderer.drawHighlightLastMove(window, lastMove.from, lastMove.to);
}

void GameVideo::renderBoardHover(sf::RenderWindow& window) {

	Position hoverPos = inputController->getMouseHoverPosition(window, uiManager.getBoardView());

	boardRenderer.drawHover(window, hoverPos);
}

void GameVideo::renderCheckSquare(sf::RenderWindow& window) {

	const auto& state = gameControl->getState();

	if (state.getIsCheck()) {

		boardRenderer.drawKingCheckSquare(window, state.getCheckPos());
	}

}

void GameVideo::renderHighlightValidMove(sf::RenderWindow& window) {

	const auto& state = gameControl->getState();
	const auto& drag = gameControl->getState().getDragState();

	if (state.hasSelection()||drag.isActive) {

		for (const auto& pos : state.getValidMoves()) {

			Piece targetPiece = board->getPiece(pos);

			bool isEnemy = (targetPiece != Piece::Empty);

			boardRenderer.drawValidMove(window, pos, isEnemy);
		}

	}

}

void GameVideo::changeCursor(sf::RenderWindow& window) {

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

	boardRenderer.drawCursor(window,worldPos);

}

void GameVideo::renderPromotionPanel(sf::RenderWindow& window) {

	if (state->getGameStatus() != GameStatus::WaitingForPromotion) return;

	boardRenderer.drawPromotionPanel(window, state->getCurrentTurn(), state->getPendingTo().col);
	pieceRender.drawListPromotionPiece(window, state->getCurrentTurn(), state->getPendingTo().col);
}

void GameVideo::gameLoop(float dt) {

	animator.update(dt);
	
	//gameControl->updateAiMove();

	audio.update();

	EventBus::get().dispatch();

	renderWindow(window, dt);
}

void GameVideo::updateEvent(sf::Event e) {

	gui.handleEvent(e);

	if (e.type == sf::Event::Closed) {
		window.close();
	}

	if (e.type == sf::Event::Resized) {

		uiManager.onResize(e.size.width, e.size.height);

		analysisPanel.updateLayout(uiManager.getBoardRightEdgeRatio());

		informationPanel.updateLayout(uiManager.getBoardView().getViewport().left);

	}

	inputController->handleEvent(window, e, uiManager.getBoardView());
}

void GameVideo::Run() {

	while (window.isOpen()) {
		float dt = static_cast<float>(clock.restart().asMilliseconds());

		sf::Event e;

		while (window.pollEvent(e)) {

			updateEvent(e);

		}

		gameLoop(dt);

	}
}