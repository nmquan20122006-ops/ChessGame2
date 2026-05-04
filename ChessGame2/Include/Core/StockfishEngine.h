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

class StockfishEngine {
private:
    HANDLE              hStdinWrite;
    HANDLE              hStdoutRead;
    PROCESS_INFORMATION pi;
    std::vector<std::string> moveHistory;

    bool                sendCommand(const std::string& cmd);
    std::string         readResponse();

public:
    StockfishEngine();
    ~StockfishEngine();

    bool                start(const std::wstring& stockfishPath);
    void                stop();
    bool                isRunning();

    void                setPosition(const std::string& fen = "");
    void                makeMove(const std::string& uciMove);
    std::string         getBestMove(int movetime);

    static std::string  toUCI(int row, int col);
    static void         fromUCI(const std::string& uci, int& row, int& col);

    void                setSkillLevel(int level);
};

#endif