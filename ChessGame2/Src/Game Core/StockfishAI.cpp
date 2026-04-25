#include "StockfishAI.h"
#include <iostream>

StockfishGame::StockfishGame()
    : executor(board)
    , m_isPlayerWhite(true)
    , m_gameOver(false)
    , m_isThinking(false)
    , m_stopThinking(false)
    , m_aiStartedThinking(false) {
}

StockfishGame::~StockfishGame() {
    stopThinking();
    engine.stop();
}


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

void StockfishGame::newGame(bool playerIsWhite) {
    stopThinking();

    board.initBoard();
    m_isPlayerWhite = playerIsWhite;
    m_gameOver = false;
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


bool StockfishGame::aiMove(int thinkingTimeMs) {
    if (m_gameOver || m_isThinking) {
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

    return true;
}


void StockfishGame::startThinking(int thinkingTimeMs) {
    if (m_isThinking || m_aiStartedThinking || m_gameOver) {
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
        m_aiStartedThinking = false;
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

void StockfishGame::syncMove(const std::string& uciMove) {
    engine.makeMove(uciMove);
}

void StockfishGame::syncFromHistory(const std::vector<std::string>& moves) {
    stopThinking();

    engine.setPosition();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        moveHistory = moves; 
    }

    board.initBoard();

    for (const auto& moveStr : moves) {
        if (moveStr.length() < 4) continue;

        engine.makeMove(moveStr);

        Position from, to;
        fromUCI(moveStr.substr(0, 2), from);
        fromUCI(moveStr.substr(2, 2), to);

        Move boardMove{ from, to };
     
        executor.applyMove(boardMove);
    }

    m_gameOver = false;
    m_aiStartedThinking = false;

}

void StockfishGame::applyMove(const std::string& uciMove) {

    Position from, to;
    fromUCI(uciMove.substr(0, 2), from);
    fromUCI(uciMove.substr(2, 2), to);
    Move move{ from, to };
    executor.applyMove(move);  

    engine.makeMove(uciMove);
    addToHistory(uciMove);
    std::cout << "AI move sent to Stockfish: " << uciMove << std::endl;
}

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