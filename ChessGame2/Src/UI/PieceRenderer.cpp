#include"PieceRenderer.h"

PieceRenderer::PieceRenderer(): baseScale(0.f) {}

void PieceRenderer::setupPieceSprites(TextureManager& tm) {

	sf::Texture& atlas = tm.getTexture("Piecesprite");

	std::string pieceNames[6] = { "Pawn","Rook","Knight","Bishop","Queen","King" };

    //scale
	float scale = (squareSize / (float)FRAME_SIZE) * 1.f;

    this->baseScale = (squareSize / (float)FRAME_SIZE);

	for (int color = 0; color < 2; color++) {
		for (int type = 0; type < 6; type++) {

			std::string colorStr = (color == 0) ? "_WHITE" : "_BLACK";

			pieceSprite[color][type] = tm.createSprite("Piecesprite", pieceNames[type] + colorStr);

			pieceSprite[color][type].setScale(baseScale,baseScale);

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

    int colorIndex = white ? 0 : 1;

    Piece pieceType = getType(piece);
    int typeIndex = static_cast<int>(pieceType) - 1;

    if (colorIndex < 0 || colorIndex >= 2 || typeIndex < 0 || typeIndex >= 6) {
        return;
    }

    sf::Sprite& sprite = pieceSprite[colorIndex][typeIndex];

    sprite.setScale(baseScale, baseScale);
    sprite.setOrigin(0.f, 0.f);

    float offsetX = (squareSize - sprite.getGlobalBounds().width) / 2.f;
    float offsetY = (squareSize - sprite.getGlobalBounds().height) / 2.f;

    sprite.setPosition(pos.col * squareSize + offsetX + offset,
        pos.row * squareSize + offsetY + offset);

    window.draw(sprite);
}

void PieceRenderer::drawPieceAtPos(sf::RenderWindow& window, Piece piece, float row, int col) {

	if (piece == Piece::Empty) return;
    
	int colorIndex = isWhite(piece) ? 0 : 1;
	int typeIndex = static_cast<int>(getType(piece)) - 1;
    
	sf::Sprite& sprite = pieceSprite[colorIndex][typeIndex];
	sprite.setScale(baseScale, baseScale);

	float offsetX = (squareSize - sprite.getGlobalBounds().width) / 2.f;
	float offsetY = (squareSize - sprite.getGlobalBounds().height) / 2.f;

    sprite.setPosition(col * squareSize + offsetX + offset,
		row * squareSize + offsetY + offset);

	window.draw(sprite);

}

void PieceRenderer::drawListPromotionPiece(sf::RenderWindow& window, color turn, int col) {

    std::vector<Piece> promotionPieces = (turn == color::white) ?
        std::vector<Piece>{ Piece::W_Queen, Piece::W_Rook, Piece::W_Bishop, Piece::W_Knight } :
        std::vector<Piece>{ Piece::B_Queen, Piece::B_Rook, Piece::B_Bishop, Piece::B_Knight };

    for (int i = 0; i < promotionPieces.size(); i++) {

        float rowIndex = (turn == color::white) ? (float)i : (float)(4 + i);

        drawPieceAtPos(window, promotionPieces[i], rowIndex, col);
    }
}

void PieceRenderer::renderAll(sf::RenderWindow& window, const Board& board) {

    const auto& drag = gameControl->getState().getDragState();

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {

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

        dragSprite.setScale(baseScale * 1.2f, baseScale * 1.2f);
        
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
    
    int colorIndex = isWhite(p) ? 0 : 1;

    int typeIndex = static_cast<int>(getType(p)) - 1;

    return pieceSprite[colorIndex][typeIndex];
}

sf::Sprite PieceRenderer::getCopySprite(Piece p) {

    int colorIndex = isWhite(p) ? 0 : 1;

    int typeIndex = static_cast<int>(getType(p)) - 1;

    return pieceSprite[colorIndex][typeIndex];
}
