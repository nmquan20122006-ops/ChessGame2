#include"GameVideo.h"
#include<iostream>

GameVideo::GameVideo() :
	window(sf::VideoMode(windowWidth, windowHeight), "Chess"),

	board(),

	moveService(),state(),moveExecutor(board),

	gameControl(board,moveService,state,moveExecutor),

	inputController(gameControl),

	pieceRender(gameControl) {

	window.setFramerateLimit(144);

	initTexture();
	initSound();
	loadTextureForMember();

	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

}

void GameVideo::renderWindow(sf::RenderWindow& window) {

	float dt = static_cast<float>(clock.restart().asMilliseconds());

	window.clear(sf::Color::White);

	boardRenderer.drawPanel(window);

	boardRenderer.draw(window);

	renderCoordinate(window, textureManager);

	renderBoardHover(window);

	renderHighlightLastMove(window);

	renderCheckSquare(window);

	renderHightlight(window);

	renderHighlightValidMove(window);

	pieceRender.renderAll(window,board);

	executePieceAnim(window, dt);

	changeCursor(window);

	window.display();

}

void GameVideo::renderCoordinate(sf::RenderWindow& window, TextureManager& tex_m) {

	sf::Font& font = tex_m.getFont("font");

	boardRenderer.drawCoordinates(window);
}

void GameVideo::setupWindow() {

	sf::ContextSettings setting;
	setting.antialiasingLevel = 16;
	window.create(sf::VideoMode(windowWidth, windowHeight), "Chess", sf::Style::Titlebar | sf::Style::Close, setting);
	window.setFramerateLimit(144);
}

void GameVideo::initStockfish() {

	if (!gameAi.initAI(L"stockfish.exe")) {

		std::cout << "Failed to initialize Stockfish!" << std::endl;
		return;
	}
	gameAi.setDifficulty(20);
	
	gameAi.newGame(true);

}

void GameVideo::initTexture() {
	
	textureManager.loadTexture("PieceAtlas", "Assets/asset.png");
	textureManager.loadTexture("BoardTexture", "Assets/Board.png");
	textureManager.loadTexture("BorderTexture", "Assets/Border.png");
	textureManager.loadTexture("BoardTexture2", "Assets/Board2.png");
	textureManager.loadTexture("ChessSprite", "Assets/ChessSprite.png");
	textureManager.loadTexture("Piecesprite", "Assets/Piecesprite.png");
	textureManager.loadTexture("HighlightDot", "Assets/HighlightDot.png");
	textureManager.loadTexture("CaptureRing", "Assets/CaptureRing.png");
	textureManager.loadTexture("Minecraft Enchanted Netherite Sword", "Assets/Minecraft Enchanted Netherite Sword.png");
	textureManager.loadFont("font", "Assets/font1.ttf");

}

void GameVideo::initSound() {

	audio.loadSound("MoveSound", "Assets/Sound/move.wav");
	audio.loadSound("UnValidMoveSound", "Assets/Sound/illegalMove.wav");
	audio.loadSound("CaptureSound", "Assets/Sound/Capture.wav");
	audio.loadSound("CheckSound", "Assets/Sound/check.wav");
	audio.loadSound("CheckMateSound", "Assets/Sound/checkmate.wav");
	audio.loadSound("SelectSound", "Assets/Sound/select.wav");
}

void GameVideo::loadTextureForMember() {

	textureManager.setupChessAtlas();

	pieceRender.setupPieceSprites(textureManager);

	boardRenderer.initFont(textureManager.getFont("font"));

	boardRenderer.initHighlightTexture(textureManager.getTexture("HighlightDot"), textureManager.getTexture("CaptureRing"));

	boardRenderer.initCursorTexture(textureManager.getTexture("Minecraft Enchanted Netherite Sword"));

}

void GameVideo::renderHightlight(sf::RenderWindow& window) {

	const auto& state = gameControl.getState();


	if (state.hasSelection()) {

		boardRenderer.drawHighlight(window, state.selectedPos, sf::Color(255, 255, 0, 70));
	}

}

void GameVideo::renderHighlightLastMove(sf::RenderWindow& window) {
	
	LastMove lastMove = board.getLastMove();

	boardRenderer.drawHighlightLastMove(window, lastMove.from, lastMove.to);
}

void GameVideo::renderBoardHover(sf::RenderWindow& window) {

	Position hoverPos = inputController.getMouseHoverPosition(window);

	boardRenderer.drawHover(window, hoverPos);
}

void GameVideo::renderCheckSquare(sf::RenderWindow& window) {

	const auto& state = gameControl.getState();

	if (state.isCheck) {

		boardRenderer.drawKingCheckSquare(window, state.checkPos);
	}

}

void GameVideo::renderHighlightValidMove(sf::RenderWindow& window) {

	const auto& state = gameControl.getState();
	const auto& drag = gameControl.getState().drag;

	if (state.hasSelection()||drag.isActive) {

		for (const auto& pos : state.validMoves) {

			Piece targetPiece = board.getPiece(pos);

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

void GameVideo::pieceAnimation(Position from, Position to, Piece p) {

	sf::Sprite s = pieceRender.getCopySprite(p);

	s.setScale(pieceRender.getBaseScale(), pieceRender.getBaseScale());

	sf::FloatRect bounds = s.getGlobalBounds();

	float centerX = (squareSize - bounds.width) / 2.f;

	float centerY = (squareSize - bounds.height) / 2.f;

	sf::Vector2f centerOffset(centerX, centerY);

	sf::Vector2f start = toPixel(from) + centerOffset;

	sf::Vector2f end = toPixel(to) + centerOffset;

	animator.addMove(s, start, end, 300);

}

void GameVideo::executePieceAnim(sf::RenderWindow& window, float dtMs) {

	if (animator.isAnimating()) {

        animator.update((int)dtMs);
        animator.render(window);

		isGameProcess.isAnimating = true;

    }
	else {

		isGameProcess.isAnimating = false;

		if (pendingMove.isActive) {

			gameControl.executeAIMove(pendingMove.from, pendingMove.to);

			pendingMove.isActive = false;
		}

	}

}

void GameVideo::handleAudioEvents() {

	moveType type = gameControl.consumeMoveType();
	if (type == moveType::none) return;

	
	bool isCheck = gameControl.getState().isCheck;

	
	if (type == moveType::checkMate) {
		audio.playSound("CheckMateSound", 200);
	}
	else if (isCheck) {
		
		audio.playSound("CheckSound", 200);
	}
	else if (type == moveType::capture) {
		audio.playSound("CaptureSound", 70);
	}
	else if (type == moveType::move) {
		audio.playSound("MoveSound", 100);
	}
	else if (type == moveType::unValid) {
		audio.playSound("UnValidMoveSound");
	}
	else if (type == moveType::select) {
		audio.playSound("SelectSound", 40);
	}
	
}

void GameVideo::syncAIWithGame() {

	const auto& history = gameControl.getMoveHistory();

	gameAi.syncFromHistory(history);
}

void GameVideo::executeAIMove(const std::string& uciMove) {

	if (uciMove.length() < 4) return;

	Position from, to;

	from.col = uciMove[0] - 'a';
	from.row = '8' - uciMove[1];

	to.col = uciMove[2] - 'a';
	to.row = '8' - uciMove[3];

	isGameProcess.isProcessing = true;

	Piece p = board.getPiece(from);

	pendingMove = { from,to,p,true };

	pieceAnimation(from, to, p);

	isGameProcess.isProcessing = false;
}

void GameVideo::handleAi() {

	if (isGameProcess.isAnimating || isGameProcess.isProcessing)return;

	moveType type = gameControl.peakMoveType();

	if (type != moveType::none && type != moveType::select && type != moveType::unValid) {

		gameAi.stopThinking();

		syncAIWithGame();
	}

	if (gameAi.isAITurn() && !gameAi.isGameOver()) {

		if (!gameAi.isThinking() && !gameAi.hasStartedThinking()) {

			gameAi.startThinking(800);
		}
	}


	std::string aiUCI = gameAi.getPendingMove();

	if (!aiUCI.empty()) {

		executeAIMove(aiUCI);

		const auto& historyAfterAIMove = gameControl.getMoveHistory();

		syncAIWithGame();
	}
}

void GameVideo::gameEvent(sf::Event e) {

	inputController.handleEvent(window, e);
}

void GameVideo::gameLoop() {

	handleAi();

	handleAudioEvents();

}

void GameVideo::Run() {

	initStockfish();

	syncAIWithGame();

	while (window.isOpen()) {

		sf::Event e;
		while (window.pollEvent(e)) {

			if (e.type == sf::Event::Closed) {
				window.close();
			}

			gameEvent(e);

		}

		gameLoop();

		renderWindow(window);
	}
}