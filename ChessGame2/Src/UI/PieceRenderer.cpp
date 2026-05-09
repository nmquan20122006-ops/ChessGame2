#include"PieceRenderer.h"

PieceRenderer::PieceRenderer(std::shared_ptr<GameState>g, TextureManager& t): gameState(g), baseScale(0.f), tm(t) {}

void PieceRenderer::setupPieceSprites() {

	sf::Texture& atlas = tm.getTexture("Piecesprite");

	std::string pieceNames[6] = { "Pawn","Rook","Knight","Bishop","Queen","King" };

    //scale
	float scale = (SQUARE_SIZE / (float)FRAME_SIZE) * 1.f;

    this->baseScale = (SQUARE_SIZE / (float)FRAME_SIZE);

	for (int Color = 0; Color < 2; Color++) {
		for (int type = 0; type < 6; type++) {

			std::string ColorStr = (Color == 0) ? "_WHITE" : "_BLACK";

			pieceSprite[Color][type] = tm.createSprite("Piecesprite", pieceNames[type] + ColorStr);

			pieceSprite[Color][type].setScale(baseScale,baseScale);

		}
	}
}

void PieceRenderer::drawPiece(sf::RenderWindow& window, Position pos, const Board& board) {

    Piece piece = board.getPiece(pos);

    if (piece == Piece::Empty) return;

    bool white = isWhite(piece);
    bool black = isBlack(piece);

    if (!white && !black) {
        return;
    }

    int ColorIndex = white ? 0 : 1;

    Piece pieceType = getType(piece);
    int typeIndex = static_cast<int>(pieceType) - 1;

    if (ColorIndex < 0 || ColorIndex >= 2 || typeIndex < 0 || typeIndex >= 6) {
        return;
    }

    sf::Sprite& sprite = pieceSprite[ColorIndex][typeIndex];

    sprite.setScale(baseScale, baseScale);
    sprite.setOrigin(0.f, 0.f);

    float OFFSETX = (SQUARE_SIZE - sprite.getGlobalBounds().width) / 2.f;
    float OFFSETY = (SQUARE_SIZE - sprite.getGlobalBounds().height) / 2.f;

    sprite.setPosition(pos.col * SQUARE_SIZE + OFFSETX + OFFSET,
        pos.row * SQUARE_SIZE + OFFSETY + OFFSET);

    window.draw(sprite);
}

void PieceRenderer::drawPieceAtPos(sf::RenderWindow& window, Piece piece, float row, int col) {

	if (piece == Piece::Empty) return;
    
	int ColorIndex = isWhite(piece) ? 0 : 1;
	int typeIndex = static_cast<int>(getType(piece)) - 1;
    
	sf::Sprite& sprite = pieceSprite[ColorIndex][typeIndex];
	sprite.setScale(baseScale, baseScale);

	float OFFSETX = (SQUARE_SIZE - sprite.getGlobalBounds().width) / 2.f;
	float OFFSETY = (SQUARE_SIZE - sprite.getGlobalBounds().height) / 2.f;

    sprite.setPosition(col * SQUARE_SIZE + OFFSETX + OFFSET,
		row * SQUARE_SIZE + OFFSETY + OFFSET);

	window.draw(sprite);

}

void PieceRenderer::drawListPromotionPiece(sf::RenderWindow& window, Color turn, int col) {

    std::vector<Piece> promotionPieces = (turn == Color::white) ?
        std::vector<Piece>{ Piece::W_Queen, Piece::W_Rook, Piece::W_Bishop, Piece::W_Knight } :
        std::vector<Piece>{ Piece::B_Queen, Piece::B_Rook, Piece::B_Bishop, Piece::B_Knight };

    for (int i = 0; i < promotionPieces.size(); i++) {

        float rowIndex = (turn == Color::white) ? (float)i : (float)(4 + i);

        drawPieceAtPos(window, promotionPieces[i], rowIndex, col);
    }
}

void PieceRenderer::renderAll(sf::RenderWindow& window, const Board& board) {

    const auto& drag = gameState->getDragState();

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {

            Position pos(r, c);

            if (m_excludedPositions.count(pos)) continue;

            if (drag.isDragging && pos == drag.fromPos)continue;
           
            if (!board.isEmpty(pos)) {
                drawPiece(window, pos, board);
            }

        }
    }    
    if (drag.isActive && drag.draggingPiece != Piece::Empty) {

        sf::Sprite& dragSprite = getSpriteForPiece(drag.draggingPiece);

        dragSprite.setScale(baseScale * 1.4f, baseScale * 1.4f);
        
        float dragSpriteBoundWidth = dragSprite.getLocalBounds().width / 2;
        float dragSpriteBoundHeight = dragSprite.getLocalBounds().height / 2; 

        dragSprite.setOrigin(dragSpriteBoundWidth, dragSpriteBoundHeight);

        dragSprite.setPosition(drag.mousePosition);

        window.draw(dragSprite);

        dragSprite.setOrigin(0.f, 0.f);
        
        dragSprite.setScale(baseScale, baseScale);
    } 
}

sf::Sprite& PieceRenderer::getSpriteForPiece(Piece p) {
    
    int ColorIndex = isWhite(p) ? 0 : 1;

    int typeIndex = static_cast<int>(getType(p)) - 1;

    return pieceSprite[ColorIndex][typeIndex];
}

sf::Sprite PieceRenderer::getCopySprite(Piece p) {

    int ColorIndex = isWhite(p) ? 0 : 1;

    int typeIndex = static_cast<int>(getType(p)) - 1;

    return pieceSprite[ColorIndex][typeIndex];
}

sf::Texture& PieceRenderer::getCapturedPieceTexture(Piece piece) {

    int colorOFFSET = isWhite(piece) ? 0 : 6;
    int typeIndex = static_cast<int>(getType(piece)) - 1;
    int finalIndex = colorOFFSET + typeIndex;

    sf::Texture& capturedAtlast = tm.getTexture("CapturedPieceTexture");

    int row = isWhite(piece) ? 0 : 1;
    int col = typeIndex;
    
    sf::IntRect cutRect(col * FRAME_CAPTURE_SIZE, row * FRAME_CAPTURE_SIZE, FRAME_CAPTURE_SIZE, FRAME_CAPTURE_SIZE);

    sf::Image capturedImage = capturedAtlast.copyToImage();
    sf::Image pieceCapturedImage;
    pieceCapturedImage.create(FRAME_CAPTURE_SIZE, FRAME_CAPTURE_SIZE);
    pieceCapturedImage.copy(capturedImage, 0, 0, cutRect);

    pieceCapturedTexture[finalIndex].loadFromImage(pieceCapturedImage);
    pieceCapturedTexture[finalIndex].setSmooth(true);
    pieceCapturedTexture[finalIndex].generateMipmap();

    return pieceCapturedTexture[finalIndex];

}
