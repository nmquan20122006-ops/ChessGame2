#include "StockfishAI.h"

StockfishGame::StockfishGame(std::shared_ptr<Board> b)
    : m_isPlayerWhite(true)
    , m_gameOver(false)
    , m_isThinking(false)
    , m_stopThinking(false)
    , m_aiStartedThinking(false) {

    board = b;
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

    board->initBoard();
    m_isPlayerWhite = playerIsWhite;
    m_gameOver = false;
    m_isThinking = false;
    m_stopThinking = false;
    m_aiStartedThinking = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pendingAIMove.clear();
    }

    engine.setPosition();

}

void StockfishGame::reset() {
    newGame(m_isPlayerWhite);
}

void StockfishGame::startThinking(int thinkingTimeMs) {
    if (m_isThinking || m_aiStartedThinking || m_gameOver) return;

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

void StockfishGame::syncMove(const std::string& currentFen) {
    engine.setPosition(currentFen);
}

std::string StockfishGame::toUCI(const Position& pos) {
    char file = 'a' + pos.col;
    char rank = '8' - pos.row;
    return std::string(1, file) + rank;
}

void StockfishGame::fromUCI(const std::string& uci, Position& pos) {
    if (uci.length() >= 2) {
        pos.col = uci[0] - 'a';
        pos.row = '8' - uci[1];
    }
}

std::string StockfishGame::moveToUCI(const Position& from, const Position& to, char promotion) {

    std::string uci = toUCI(from) + toUCI(to);
    if (promotion != '\0') {
        uci += promotion;
    }
    return uci;
}