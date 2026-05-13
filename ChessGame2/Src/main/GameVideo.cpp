#include"GameVideo.h"
#include"EventBus.h"
#include<iostream>

GameVideo::GameVideo() :
	window				(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chess"),
	gui					(window),
	analysisPanel		(gui),
	evalBar				(gui),
	textureManager		(),
	board				(std::make_shared<Board>()),
	state				(std::make_shared<GameState>()),
	moveService			(std::make_shared<MoveService>()),
	uiManager			(windowSize, analysisPanel),
	pieceRender			(state,textureManager),
	informationPanel	(gui, pieceRender),
	audio				(),
	chessNotation		(),
	moveLog				(std::make_shared<MoveLog>(board, state, chessNotation, moveService))
	{
	moveExecutor =		std::make_unique<MoveExecutor>(board,state,*moveLog);
	inputHandler =		std::make_unique<InputHandler>(board, state, moveService);
	gameControl =		std::make_unique<GameControl>(board, state, moveService, moveExecutor,*moveLog);
	inputController =	std::make_unique<InputController>(*inputHandler, *state, gameControl.get());

	float rightEdge = uiManager.getBoardRightEdgeRatio();
	float leftEdge = uiManager.getBoardView().getViewport().left;

	analysisPanel.updateLayout(rightEdge);
	informationPanel.updateLayout(leftEdge);

	initAll();
	callBackAll();
	
	window.setMouseCursorVisible(true);
	window.setFramerateLimit(144);
	}

void GameVideo::initAll(){

	EngineConfig config;

	textureManager.initTexture();
	audio.initSound();
	loadTextureForMember();
	gameControl->initChessEngine(config);

	informationPanel.setFont("Assets/font1.ttf");

	evalBar.updateLayout(0.7f, uiManager.getBoardView(), WINDOW_WIDTH,WINDOW_HEIGHT);
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
	inputHandler->setOnPromotionSelected([&](Piece p) {
		gameControl->executePromotionMove(p);
		});

	inputHandler->setOnGetPendingTo([&]() {
		return state->getPendingTo(); 
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

	EventBus::get().subcribe(GameEvent::MoveRecord, [this] { informationPanel.update(moveLog->getMoveHistoryStack()); updateCapturedPieceList(); });
	EventBus::get().subcribe(GameEvent::Undo, [this] { informationPanel.update(moveLog->getMoveHistoryStack()); updateCapturedPieceList(); });
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

	renderHintPosition(window);

	renderEnemyBestMovePosition(window);

	renderPromotionPanel(window);

	animator.render(window);

	window.setView(uiManager.getUiView());

	gui.draw();

	window.display();

}

void GameVideo::renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m) {

	sf::Font& font = tex_m.getFont("font");

	boardRenderer.drawCoordinates(window);
}

void GameVideo::loadTextureForMember() {

	textureManager.setupChessAtlas();

	pieceRender.setupPieceSprites();

	boardRenderer.initFont(textureManager.getFont("font"));

	boardRenderer.initHighlightTexture(textureManager.getTexture("HighlightDot"), textureManager.getTexture("CaptureRing"));

	boardRenderer.initCursorTexture(textureManager.getTexture("Minecraft Enchanted Netherite Sword"));

}

void GameVideo::renderHightlight(sf::RenderWindow& window) {

	const auto& state = gameControl->getState();

	if (state.hasSelection()) {

		boardRenderer.drawHighlight(window, state.getSelectPos(), sf::Color(186, 202, 43, 70));
	}

}

void GameVideo::updateCapturedPieceList() {

	auto& moveExecutor = gameControl->getMoveExecutor();

	const auto& whitePieces = moveExecutor.getW_PieceCapture();
	const auto& blackPieces = moveExecutor.getB_CapturedPiece();

	informationPanel.updateCapturedList(whitePieces, blackPieces);
}

void GameVideo::renderHintPosition(sf::RenderWindow& window) {

	boardRenderer.drawHintLine(window, state->getHintPosition().from, state->getHintPosition().to, sf::Color(255, 255, 0, 120));

}

void GameVideo::renderHighlightLastMove(sf::RenderWindow& window) {
	
	LastMove lastMove = board->getLastMove();

	boardRenderer.drawHighlightLastMove(window, lastMove.from, lastMove.to);
}

void GameVideo::renderEnemyBestMovePosition(sf::RenderWindow& window) {

	boardRenderer.drawHintLine(window, state->getEnemyBestMovePosition().from, state->getEnemyBestMovePosition().to, sf::Color(180, 20, 20, 180));
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
	sf::Vector2f boardPos = window.mapPixelToCoords(mousePos, uiManager.getBoardView());
	
	window.setView(uiManager.getBoardView());
	boardRenderer.drawCursor(window,boardPos);

	sf::Vector2f uiPos = window.mapPixelToCoords(mousePos, uiManager.getUiView());
	window.setView(uiManager.getUiView());
	boardRenderer.drawCursor(window, uiPos);

}

void GameVideo::renderPromotionPanel(sf::RenderWindow& window) {

	if (state->getGameStatus() != GameStatus::WaitingForPromotion) return;

	boardRenderer.drawPromotionPanel(window, state->getCurrentTurn(), state->getPendingTo().col);
	pieceRender.drawListPromotionPiece(window, state->getCurrentTurn(), state->getPendingTo().col);
}

void GameVideo::gameLoop(float dt) {

	animator.update(dt);
	
	gameControl->updateChessEngineMove();

	evalBar.updateEval(state->getEngineEvaluation());

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

		evalBar.updateLayout(0.7f, uiManager.getBoardView(), e.size.width,e.size.height);

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