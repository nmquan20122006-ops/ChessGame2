#include "StockfishAI.h"

StockfishGame::StockfishGame()
    : m_isPlayerWhite(true)
    , m_gameOver(false)
    , m_isThinking(false)
    , m_stopThinking(false){

}

StockfishGame::~StockfishGame() {
    stopThinking();
    engine.stop();
}

bool StockfishGame::init(const std::wstring& stockfishPath) {
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

void StockfishGame::goDepth(int depth) {

    if (m_isThinking || m_gameOver) return;

    m_isThinking = true;
    m_stopThinking = false;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pendingEngineMove.clear();
    }
    if (engineThread.joinable()) {
        engineThread.join();
    }
    
    engineThread = std::thread([this, depth]() {
        std::string uciMove = engine.getBestMoveByDepth(depth);

        if (!m_stopThinking && !uciMove.empty()) {
            std::lock_guard<std::mutex> lock(m_mutex);
            pendingEngineMove = uciMove;
        }

        m_isThinking = false;
        });
}

void StockfishGame::newGame(bool playerIsWhite) {

    stopThinking();

    m_isPlayerWhite = playerIsWhite;
    m_gameOver = false;
    m_isThinking = false;
    m_stopThinking = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pendingEngineMove.clear();
    }

    engine.setPosition();

}

void StockfishGame::reset() {
    newGame(m_isPlayerWhite);
}

void StockfishGame::startThinking(int thinkingTimeMs) {

    if (m_isThinking || m_gameOver) return;

    m_isThinking = true;
    m_stopThinking = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pendingEngineMove.clear();
    }

    if (engineThread.joinable()) {
        engineThread.join();
    }

    engineThread = std::thread([this, thinkingTimeMs]() {
        std::string uciMove = engine.getBestMove(thinkingTimeMs);

        if (!m_stopThinking && !uciMove.empty()) {
            std::lock_guard<std::mutex> lock(m_mutex);
            pendingEngineMove = uciMove;
        }

        m_isThinking = false;
        });
}

void StockfishGame::stopThinking() {
    m_stopThinking = true;


    if (engineThread.joinable()) {
        engineThread.join();
    }

    m_isThinking = false;
}

std::string StockfishGame::getPendingMove() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string move = pendingEngineMove;
    pendingEngineMove.clear();
    return move;
}

void StockfishGame::syncPosition(const std::string& currentFen) {
    engine.setPosition(currentFen);
}