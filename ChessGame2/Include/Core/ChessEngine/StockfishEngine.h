/**
* StockfishEngine.h
* @brief This file defines the StockfishEngine class, which provides an interface for communicating with the Stockfish chess engine. 
* @brief It includes methods for starting and stopping the engine, sending commands, reading responses, setting the game position, making moves, and retrieving the best move from the engine
* @brief The class manages the process and communication pipes to facilitate interaction with the Stockfish engine.
* @author [Nguyen Minh Quan]
* @date [21/3/2026]
* @version [1.0]
* copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use
*/

#ifndef STOCKFISH_ENGINE_H
#define STOCKFISH_ENGINE_H
#define NOMINMAX

#include <string>
#include <vector>
#include <windows.h>
#include <mutex>
#include <stdexcept>
#include "State/Aistate.hpp"

class ChessEngineException :public std::runtime_error {
public:
    explicit ChessEngineException(const std::string& msg):std::runtime_error(msg){}
};

class StockfishEngine {
private:
    HANDLE                  _m_hStdinWrite;
    HANDLE                  _m_hStdoutRead;
    PROCESS_INFORMATION     _m_pi;
    EngineEvaluation        _m_engineEvaluation;
    bool                    sendCommand(const std::string& cmd);
    std::string             readResponse();
public:
    StockfishEngine();
    ~StockfishEngine();
    bool                    start(const std::wstring& chessEnginePath);
    void                    stop();
    bool                    isRunning();
    void                    setPosition(const std::string& fen = "");
    std::string             getBestMoveByTime(int movetime);
    std::string             getBestMoveByTimeByDepth(int depth);
    void                    stopSearch();
    void                    praseEval(const std::string& response);
    EngineEvaluation        getEngineEvaluation()const { return _m_engineEvaluation; }
    void                    setSkillLevel(int level);
};
#endif