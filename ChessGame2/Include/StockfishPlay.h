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
    // ==================== CORE COMPONENTS ====================
    Board board;
    MoveValidator validator;
    MoveExecutor executor;
    StockfishEngine engine;

    // ==================== GAME STATE ====================
    bool m_isPlayerWhite;
    std::atomic<bool> m_aiTurn;
    std::atomic<bool> m_gameOver;
    std::atomic<bool> m_aiStartedThinking;

    // ==================== MOVE HISTORY ====================
    std::vector<std::string> moveHistory;
    std::string lastAIMove;

    // ==================== AI THREAD ====================
    std::thread aiThread;
    std::mutex m_mutex;

    std::atomic<bool> m_isThinking;
    std::atomic<bool> m_stopThinking;

    std::string pendingAIMove;

    // ==================== PRIVATE METHODS ====================
    void addToHistory(const std::string& uciMove);
    void updateBoardFromHistory(const std::vector<std::string>& moves);

public:
    // ==================== CONSTRUCTOR / DESTRUCTOR ====================
    StockfishGame();
    ~StockfishGame();

    // ==================== INITIALIZE ====================
    bool initAI(const std::wstring& stockfishPath);
    void setDifficulty(int level);

    // ==================== GAME CONTROL ====================
    void newGame(bool playerIsWhite = true);
    void reset();

    // ==================== PLAYER MOVE ====================
    bool playerMove(const Position& from, const Position& to);

    // ==================== AI MOVE ====================
    bool aiMove(int thinkingTimeMs = 1000);

    // ==================== AI ASYNC ====================
    void startThinking(int thinkingTimeMs);
    void stopThinking();
    bool isThinking() const { return m_isThinking.load(); }
    std::string getPendingMove();

    // ==================== SYNCHRONIZATION ====================
    void syncFromHistory(const std::vector<std::string>& moves);

    // ==================== GETTERS ====================
    bool isPlayerTurn() const { return !m_aiTurn.load() && !m_gameOver.load(); }
    bool isAITurn() const { return m_aiTurn.load() && !m_gameOver.load(); }
    bool isGameOver() const { return m_gameOver.load(); }

    const Board& getBoard() const { return board; }

    // Trả bản sao để an toàn thread
    std::vector<std::string> getMoveHistory() const { return moveHistory; }
    std::string getLastAIMove() const { return lastAIMove; }

    bool isPlayerWhite() const { return m_isPlayerWhite; }
    bool hasStartedThinking() const { return m_aiStartedThinking.load(); }

    // ==================== UTILITY ====================
    static std::string toUCI(const Position& pos);
    static std::string moveToUCI(const Position& from, const Position& to);
    static void fromUCI(const std::string& uci, Position& pos);
};

#endif