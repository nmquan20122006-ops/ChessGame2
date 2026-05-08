/**
* @file StockfishAI.h
* @brief This file defines the StockfishGame class, which serves as an interface between the chess game and the Stockfish chess engine
* @brief It manages the game state, communicates with the engine to get AI moves, and provides methods for starting new games, resetting the game, and synchronizing the game state with the engine.
* @brief The class also handles threading for AI move calculation to ensure a responsive user experience.
* @author [Nguyen Minh Quan]
* @date [30/4/2026]
* @version [1.0]
* copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
*/

#ifndef STOCKFISH_GAME_H
#define STOCKFISH_GAME_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>

#include "ChessEngineInterface.h"
#include "StockfishEngine.h"
#include "MoveExecutor.h"

class StockfishGame :public IChessEngine {

private:
    StockfishEngine                 engine;
    bool                            m_isPlayerWhite;
    std::atomic<bool>               m_gameOver;
    std::atomic<bool>               m_aiStartedThinking;
    std::thread                     aiThread;
    std::mutex                      m_mutex;
    std::atomic<bool>               m_isThinking;
    std::atomic<bool>               m_stopThinking;
    std::string                     pendingAIMove;

public:
    explicit StockfishGame();
    ~StockfishGame();

    bool            init(const std::wstring& stockfishPath)override;
    void            setDifficulty(int level)override;

    void            newGame(bool playerIsWhite = true)override;
    void            reset()override;

    void            startThinking(int thinkingTimeMs)override;
    void            stopThinking()override;
    bool            isThinking() const override { return m_isThinking.load(); }

    std::string     getPendingMove() override;
    void            syncPosition(const std::string& move)override;

    bool            isGameOver() const override { return m_gameOver.load(); }
};
#endif