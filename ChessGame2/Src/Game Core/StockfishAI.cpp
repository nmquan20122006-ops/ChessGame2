#include "ChessEngine/StockfishAI.h"

StockfishGame::StockfishGame()
    : _m_isPlayerWhite      (true)
    , _m_gameOver           (false)
    , _m_isThinking         (false)
    , _m_stopThinking       (false)
    , _m_isChessEngineReady (false){}

StockfishGame::~StockfishGame() {
    stopThinking();
    _m_chessEngine.stop();
}

bool StockfishGame::init(const std::wstring& chessEnginePath) {
    try {
        _m_chessEngine.start(chessEnginePath);
        _m_isChessEngineReady = true;
        std::cerr << "Chess engine initialized!" << std::endl;
        return true;
    }
    catch(const ChessEngineException& e){
        _m_isChessEngineReady = false;
        std::cerr << "Failed to init chess engine" << e.what() << std::endl;
        return false;

    }
}

void StockfishGame::setDifficulty(int level) {

    if (!_m_isChessEngineReady)return;
    if (level < 0) level = 0;
    if (level > 20) level = 20;
    try {
        _m_chessEngine.setSkillLevel(level);
    }
    catch (const ChessEngineException& error) {
        std::cerr << "setDifficulty error: " << error.what() << std::endl;
    }
}

void StockfishGame::_runEngineTask(std::function<std::string()>task) {
    _m_isThinking = true;
    _m_stopThinking = false;
    {
        std::lock_guard<std::mutex> lock(_m_mutex);
        _m_pendingEngineMove.clear();
    }

    if (_m_chessEngineThread.joinable())_m_chessEngineThread.join();

    _m_chessEngineThread = std::thread([this, task] {
        std::string uciMove;
        try {
            uciMove = task();
        }
        catch (const ChessEngineException& error) {
            std::cerr << "Engine task error: " << error.what() << std::endl;
        }

        if (!_m_stopThinking.load() && !uciMove.empty()) {
            std::lock_guard<std::mutex> lock(_m_mutex);
            _m_pendingEngineMove = uciMove;
        }

        _m_isThinking = false;

        });
}

void StockfishGame::goDepth(int depth) {

    if (!_m_isChessEngineReady || _m_isThinking.load() || _m_gameOver.load())return;

    _runEngineTask([this, depth]() {
        return _m_chessEngine.getBestMoveByTimeByDepth(depth);
        });
}

void StockfishGame::stopSearch() {

    if (_m_isChessEngineReady) {
        _m_chessEngine.stopSearch();
    }
}

void StockfishGame::newGame(bool playerIsWhite) {

    stopThinking();

    _m_isPlayerWhite = playerIsWhite;
    _m_gameOver = false;
    _m_isThinking = false;
    _m_stopThinking = false;

    {
        std::lock_guard<std::mutex> lock(_m_mutex);
        _m_pendingEngineMove.clear();
    }

    if (_m_isChessEngineReady) {
        try {
            _m_chessEngine.setPosition();
        }
        catch (const ChessEngineException& error) {
            std::cerr << "New game set position error: " << error.what() << std::endl;
        }
    }

}

void StockfishGame::reset() {
    newGame(_m_isPlayerWhite);
}

void StockfishGame::goMoveTime(int thinkingTimeMs) {

    if (!_m_isChessEngineReady || _m_isThinking.load() || _m_gameOver.load())return;

    _runEngineTask([this, thinkingTimeMs]() {
        return _m_chessEngine.getBestMoveByTime(thinkingTimeMs);
        });
}

void StockfishGame::stopThinking() {

    _m_stopThinking = true;

    if (_m_isChessEngineReady && _m_isThinking.load()) {
        _m_chessEngine.stopSearch();
    }

    if (_m_chessEngineThread.joinable()) {
        _m_chessEngineThread.join();
    }

    _m_isThinking = false;
}

std::string StockfishGame::getPendingMove() {
    std::lock_guard<std::mutex> lock(_m_mutex);
    std::string move = _m_pendingEngineMove;
    _m_pendingEngineMove.clear();
    return move;
}

void StockfishGame::syncPosition(const std::string& currentFen) {
    _m_chessEngine.setPosition(currentFen);
}