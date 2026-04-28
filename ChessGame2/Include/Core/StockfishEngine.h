#ifndef STOCKFISH_ENGINE_H
#define STOCKFISH_ENGINE_H
#define NOMINMAX

#include <string>
#include <vector>
#include <windows.h>

class StockfishEngine {
private:
    HANDLE hStdinWrite;
    HANDLE hStdoutRead;
    PROCESS_INFORMATION pi;
    std::vector<std::string> moveHistory;

    bool sendCommand(const std::string& cmd);
    std::string readResponse();

public:
    StockfishEngine();
    ~StockfishEngine();

    bool start(const std::wstring& stockfishPath);
    void stop();
    bool isRunning();

    void setPosition(const std::string& fen = "");
    void makeMove(const std::string& uciMove);
    std::string getBestMove(int movetime);

    static std::string toUCI(int row, int col);
    static void fromUCI(const std::string& uci, int& row, int& col);

    void setSkillLevel(int level);
};

#endif