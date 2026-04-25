#include "StockfishEngine.h"
#include <iostream>
#include <sstream>
#include <cstring>

StockfishEngine::StockfishEngine() : hStdinWrite(NULL), hStdoutRead(NULL) {
    ZeroMemory(&pi, sizeof(pi));
}

StockfishEngine::~StockfishEngine() {
    stop();
}

bool StockfishEngine::start(const std::wstring& stockfishPath) {
   
    HANDLE hStdinRead, hStdoutWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    if (!CreatePipe(&hStdinRead, &hStdinWrite, &sa, 0)) return false;

    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0)) return false;

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hStdinRead;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = hStdoutWrite;
    si.dwFlags = STARTF_USESTDHANDLES;

    std::wstring cmd = stockfishPath;

    if (cmd.find(L' ') != std::wstring::npos) {
        cmd = L"\"" + cmd + L"\""; 
    }

    wchar_t* cmdLine = const_cast<wchar_t*>(cmd.c_str());

    if (!CreateProcessW(NULL, cmdLine, NULL, NULL, TRUE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hStdinRead);
        CloseHandle(hStdoutWrite);
        return false;
    }

    CloseHandle(hStdinRead);
    CloseHandle(hStdoutWrite);

    sendCommand("uci");
    if (readResponse().find("uciok") == std::string::npos) return false;

    sendCommand("isready");
    if (readResponse().find("readyok") == std::string::npos) return false;

    return true;
}

void StockfishEngine::stop() {

    if (isRunning()) {
        sendCommand("quit");
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (hStdinWrite) CloseHandle(hStdinWrite);
    if (hStdoutRead) CloseHandle(hStdoutRead);

    hStdinWrite = NULL;
    hStdoutRead = NULL;
    moveHistory.clear();
}

bool StockfishEngine::isRunning() {
    return hStdinWrite != NULL && hStdoutRead != NULL;
}

bool StockfishEngine::sendCommand(const std::string& cmd) {
    if (!isRunning()) return false;
    std::cout << "[SENT TO STOCKFISH]: " << cmd << std::endl;

    std::string fullCmd = cmd + "\n";
    DWORD bytesWritten;
    return WriteFile(hStdinWrite, fullCmd.c_str(), fullCmd.length(), &bytesWritten, NULL);
}

std::string StockfishEngine::readResponse() {
    if (!isRunning()) return "";

    char buffer[4096];
    DWORD bytesRead;
    std::string response;
    DWORD startTime = GetTickCount64();
    const DWORD timeout = 5000; // 5 giây timeout

    while (GetTickCount64() - startTime < timeout) {
        DWORD available = 0;
        if (!PeekNamedPipe(hStdoutRead, NULL, 0, NULL, &available, NULL)) {
            break;
        }

        if (available > 0) {
            if (!ReadFile(hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
                break;

            if (bytesRead == 0) break;

            buffer[bytesRead] = '\0';
            response += buffer;

            if (response.find("bestmove") != std::string::npos ||
                response.find("readyok") != std::string::npos ||
                response.find("uciok") != std::string::npos) {
                break;
            }
        }

        Sleep(10);
    }

    return response;
}

void StockfishEngine::setPosition(const std::string& fen) {

    moveHistory.clear();

    if (fen.empty()) {
        sendCommand("position startpos");
    }
    else {
        sendCommand("position fen " + fen);
    }
}

void StockfishEngine::makeMove(const std::string& uciMove) {

    moveHistory.push_back(uciMove);

    std::string cmd = "position startpos moves";
    for (const auto& move : moveHistory) {
        cmd += " " + move;
    }
    sendCommand(cmd);
}

std::string StockfishEngine::getBestMove(int movetime) {
    if (!isRunning()) return "";

    sendCommand("go movetime " + std::to_string(movetime));
    std::string response = readResponse();

    size_t pos = response.find("bestmove");
    if (pos == std::string::npos) return "";

    std::stringstream ss(response.substr(pos));
    std::string token, bestMove;
    ss >> token >> bestMove;

    if (bestMove == "(none)" || bestMove.empty()) return "";
    return bestMove;
}


std::string StockfishEngine::toUCI(int row, int col) {

    char file = 'a' + col;    
    char rank = '8' - row;     

    return std::string(1, file) + rank;
}

void StockfishEngine::fromUCI(const std::string& uci, int& row, int& col) {

    if (uci.length() < 2) {
        row = col = -1; 
        return;
    }

    col = uci[0] - 'a';  
    row = '8' - uci[1];   

}

void StockfishEngine::setSkillLevel(int lv) {

    if (lv < 0) lv = 0;
    if (lv > 20)lv = 20;

    sendCommand("setoption name Skill Level value " + std::to_string(lv));
    sendCommand("isready");
    readResponse();

}
