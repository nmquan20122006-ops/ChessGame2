#include "StockfishPlay.h"
#include <iostream>

// ==================== CONSTRUCTOR / DESTRUCTOR ====================

StockfishGame::StockfishGame()
    : executor(board)
    , m_isPlayerWhite(true)
    , m_aiTurn(false)
    , m_gameOver(false)
    , m_isThinking(false)
    , m_stopThinking(false)
    , m_aiStartedThinking(false) {
}

StockfishGame::~StockfishGame() {
    stopThinking();
    engine.stop();
}

// ==================== INITIALIZATION ====================

bool StockfishGame::initAI(const std::wstring& stockfishPath) {
    if (!engine.start(stockfishPath)) {
        std::cerr << "Failed to start Stockfish!" << std::endl;
        return false;
    }

    engine.setSkillLevel(10);
    std::cout << "Stockfish initialized!" << std::endl;
    return true;
}

void StockfishGame::setDifficulty(int level) {
    if (level < 0) level = 0;
    if (level > 20) level = 20;
    engine.setSkillLevel(level);
}

// ==================== GAME CONTROL ====================

void StockfishGame::newGame(bool playerIsWhite) {
    stopThinking();

    board.initBoard();
    m_isPlayerWhite = playerIsWhite;
    m_gameOver = false;
    m_aiTurn = !m_isPlayerWhite;
    m_isThinking = false;
    m_stopThinking = false;
    m_aiStartedThinking = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        moveHistory.clear();
        lastAIMove.clear();
        pendingAIMove.clear();
    }

    engine.setPosition();

}

void StockfishGame::reset() {
    newGame(m_isPlayerWhite);
}

// ==================== PLAYER MOVE ====================

bool StockfishGame::playerMove(const Position& from, const Position& to) {
    if (m_aiTurn || m_gameOver || m_isThinking) {
        std::cout << "Not player's turn!" << std::endl;
        return false;
    }

    if (!validator.isCanExecuteMove(from, to, board)) {
        std::cout << "Invalid move!" << std::endl;
        return false;
    }

    Move move{ from, to };
    executor.applyMove(move);

    std::string uciMove = moveToUCI(from, to);

    engine.makeMove(uciMove);
    addToHistory(uciMove);

    std::cout << "Player move: " << uciMove << std::endl;

    m_aiTurn = true;
    return true;
}

// ==================== AI MOVE (SYNC) ====================

bool StockfishGame::aiMove(int thinkingTimeMs) {
    if (!m_aiTurn || m_gameOver || m_isThinking) {
        return false;
    }

    std::cout << "AI thinking..." << std::endl;

    m_isThinking = true;
    m_stopThinking = false;

    std::string uciMove = engine.getBestMove(thinkingTimeMs);

    m_isThinking = false;

    if (m_stopThinking || uciMove.empty()) {
        if (uciMove.empty()) {
            m_gameOver = true;
            std::cout << "AI has no moves!" << std::endl;
        }
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        lastAIMove = uciMove;
    }

    std::cout << "AI plays: " << uciMove << std::endl;

    Position from, to;
    fromUCI(uciMove.substr(0, 2), from);
    fromUCI(uciMove.substr(2, 2), to);

    Move move{ from, to };
    executor.applyMove(move);
    addToHistory(uciMove);

    m_aiTurn = false;
    return true;
}

// ==================== AI MOVE (ASYNC) ====================

void StockfishGame::startThinking(int thinkingTimeMs) {
    if (m_isThinking || m_aiStartedThinking || !m_aiTurn || m_gameOver) {
        return;
    }

    m_aiStartedThinking = true;
    m_isThinking = true;
    m_stopThinking = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pendingAIMove.clear();
    }

    if (aiThread.joinable()) {
        aiThread.join();
    }

    aiThread = std::thread([this, thinkingTimeMs]() {
        std::string uciMove = engine.getBestMove(thinkingTimeMs);

        if (!m_stopThinking && !uciMove.empty()) {
            std::lock_guard<std::mutex> lock(m_mutex);
            pendingAIMove = uciMove;
        }

        m_isThinking = false;
        });
}
void StockfishGame::stopThinking() {
    m_stopThinking = true;


    if (aiThread.joinable()) {
        aiThread.join();
    }

    m_isThinking = false;
    m_aiStartedThinking = false;
}

std::string StockfishGame::getPendingMove() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string move = pendingAIMove;
    pendingAIMove.clear();
    return move;
}

// ==================== SYNCHRONIZATION ====================

void StockfishGame::syncFromHistory(const std::vector<std::string>& moves) {
    stopThinking();

    engine.setPosition();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        moveHistory.clear();
    }

    for (const auto& move : moves) {
        engine.makeMove(move);
        addToHistory(move);
    }

    updateBoardFromHistory(moves);

    
    bool isWhiteTurn = (moves.size() % 2 == 0);

   
    bool aiIsWhite = !m_isPlayerWhite;
    m_aiTurn = (aiIsWhite == isWhiteTurn);

    m_gameOver = false;
    m_aiStartedThinking = false;
 
}

// ==================== PRIVATE METHODS ====================

void StockfishGame::addToHistory(const std::string& uciMove) {
    std::lock_guard<std::mutex> lock(m_mutex);
    moveHistory.push_back(uciMove);
}

void StockfishGame::updateBoardFromHistory(const std::vector<std::string>& moves) {
    board.initBoard();

    for (const auto& move : moves) {
        if (move.length() < 4) continue;

        Position from, to;
        fromUCI(move.substr(0, 2), from);
        fromUCI(move.substr(2, 2), to);

        Move boardMove{ from, to };
        executor.applyMove(boardMove);
    }
}

// ==================== STATIC UTILITY FUNCTIONS ====================

std::string StockfishGame::toUCI(const Position& pos) {
    char file = 'a' + pos.col;
    char rank = '8' - pos.row;
    return std::string(1, file) + rank;
}

std::string StockfishGame::moveToUCI(const Position& from, const Position& to) {
    return toUCI(from) + toUCI(to);
}

void StockfishGame::fromUCI(const std::string& uci, Position& pos) {
    if (uci.length() >= 2) {
        pos.col = uci[0] - 'a';
        pos.row = '8' - uci[1];
    }
}
