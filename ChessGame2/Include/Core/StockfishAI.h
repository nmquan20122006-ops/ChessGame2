#ifndef STOCKFISH_GAME_H
#define STOCKFISH_GAME_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include "StockfishEngine.h"
#include "MoveValidator.h"
#include "MoveExecutor.h"

class StockfishGame {

private:

    friend class GameControl;
    
    Board board;
    MoveValidator validator;
    MoveExecutor executor;
    StockfishEngine engine;

    bool m_isPlayerWhite;
    std::atomic<bool> m_gameOver;
    std::atomic<bool> m_aiStartedThinking;

    std::vector<std::string> moveHistory;
    std::string lastAIMove;

    std::thread aiThread;
    std::mutex m_mutex;

    std::atomic<bool> m_isThinking;
    std::atomic<bool> m_stopThinking;

    std::string pendingAIMove;

    void addToHistory(const std::string& uciMove);
    void updateBoardFromHistory(const std::vector<std::string>& moves);

public:
    StockfishGame();
    ~StockfishGame();

    bool initAI(const std::wstring& stockfishPath);
    void setDifficulty(int level);

    void newGame(bool playerIsWhite = true);
    void applyMove(const std::string& uciMove);
    void reset();

    bool aiMove(int thinkingTimeMs = 1000);

    void startThinking(int thinkingTimeMs);
    void stopThinking();
    bool isThinking() const { return m_isThinking.load(); }
    std::string getPendingMove();

    void syncFromHistory(const std::vector<std::string>& moves);
    void syncMove(const std::string& move);

    bool isGameOver() const { return m_gameOver.load(); }

    const Board& getBoard() const { return board; }

    std::vector<std::string> getMoveHistory() const { return moveHistory; }
    std::string getLastAIMove() const { return lastAIMove; }

    bool isPlayerWhite() const { return m_isPlayerWhite; }
    bool hasStartedThinking() const { return m_aiStartedThinking.load(); }

    static std::string toUCI(const Position& pos);
    static std::string moveToUCI(const Position& from, const Position& to);
    static void fromUCI(const std::string& uci, Position& pos);
};

#endif