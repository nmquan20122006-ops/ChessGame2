#pragma once
#include <functional>
#include <vector>
#include <memory>
#include"Piece.h"
#include "Utility.h"
#include "GameState.h"
#include"MoveExecutor.h"
#include"MoveService.h"
#include"StockfishAI.h"
#include"Bot.h"

class GameControl {
public:

    using MoveEventCallback = std::function<void(const Move& move)>;
    using StateChangeCallback = std::function<void(GameStateEnum newState)>;
    using AnimRequest = std::function<void(Position, Position,Piece, std::function<void()>) >;
    
private:

    Board& board;
    MoveExecutor& moveExecutor;
    MoveService& moveService;
    GameState& gameState;
    StockfishGame& stockfishGame;
    DumpBot dumpBot;

    DragState& dragState = gameState.drag;

    AnimRequest animationProvider = nullptr;
    
    std::vector<MoveEventCallback> m_onMoveExecutedListeners;
    std::vector<StateChangeCallback> m_onGameStateChangedListeners;

    void deselect() { gameState.clearSelection(); }

    void updateGameState();
    void notifyMoveExecuted(const Move& move);
    void notifyStateChanged(const GameStateEnum& newState);

    std::vector<Move>undoStack;
    std::vector<Move>redoStack;

    bool m_isAnimating = getState().isAnimating;
    bool m_isUndoing = false;

public:
    GameControl(Board& b, MoveExecutor& m, MoveService& c, GameState& g, StockfishGame& s,DumpBot d);

    void handleSquareSelection(Position pos);
    void handlePress(Position, sf::Vector2f mousePos);
    void handleMove(sf::Vector2f mousePos);
    void handleRelease(Position pos);

    void subscribeToMove(MoveEventCallback callback) {
        m_onMoveExecutedListeners.push_back(callback);
    }
    void subscribeToStateChange(StateChangeCallback callback) {
        m_onGameStateChangedListeners.push_back(callback);
    }

    void setAnimationProvider(AnimRequest provider) { animationProvider = provider; }

    bool requestMove(Position from, Position to);
    bool executeAiMove(std::string& uciMove);
    bool executePlayerMove(Position from, Position to);
    void updateAiMove();

    void requestUndo();
    void requestRedo();

    bool executeUndoMove();
    void executeRedoMove();

    void resetGame();
    void resetMoveHistory();

    GameState& getState() { return gameState; }

    bool isBlocking()const;

};