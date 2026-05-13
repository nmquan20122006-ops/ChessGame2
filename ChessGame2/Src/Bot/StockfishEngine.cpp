
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

#include"ChessEngine/StockfishEngine.h"
#include"Constants.h"

StockfishEngine::StockfishEngine() : _m_hStdinWrite(NULL), _m_hStdoutRead(NULL), _m_pi{} {

    ZeroMemory(&_m_pi, sizeof(_m_pi));
}

StockfishEngine::~StockfishEngine() {
    stop();
}

bool StockfishEngine::start(const std::wstring& stockfishPath) {

    //khởi tạo inRead cho engine ghi vào, tạo outWrite để engine ghi ra
    HANDLE hStdinRead = NULL;
    HANDLE hStdoutWrite = NULL;

    //khởi tạo SECURITY_ATTRIBUTES với khả năng cho phép kế thừa
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    //tạo 2 ống ẩn danh cho giao tiếp stdin và stdout cho tiến trình con
    if (!CreatePipe(&hStdinRead, &_m_hStdinWrite, &sa, 0)) return false;

    if (!CreatePipe(&_m_hStdoutRead, &hStdoutWrite, &sa, 0)) {

        CloseHandle(hStdinRead);
        CloseHandle(_m_hStdinWrite);
        _m_hStdinWrite = NULL;
        return false;
    }

    //khởi tạo thông tin bắt đầu của tiến trình
    //tiến trình con sẽ đọc stdin từ hStdinRead và mọi output của tiến trình con sẽ được ghi vào hStdoutWrite
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hStdinRead;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = hStdoutWrite;
    si.dwFlags = STARTF_USESTDHANDLES;

    //khởi tạo command line an toàn
    std::wstring cmd = stockfishPath;
    if (cmd.find(L' ') != std::wstring::npos) {
        cmd = L"\"" + cmd + L"\"";
    }
    wchar_t* cmdLine = const_cast<wchar_t*>(cmd.c_str());

    //khởi tạo tiến trình chess engine
    bool success = CreateProcessW(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &_m_pi);

    //đóng các tiến trình mà cha không cần
    CloseHandle(hStdinRead);
    CloseHandle(hStdoutWrite);

    //xử lý nếu không thành công
    if (!success) {
        CloseHandle(_m_hStdinWrite);
        CloseHandle(_m_hStdoutRead);
        _m_hStdinWrite = NULL;
        _m_hStdoutRead = NULL;
        return false;
    }

    //thử gửi lệnh xem hoạt động không:)
    sendCommand("uci");
    if (readResponse().find("uciok") == std::string::npos) return false;

    sendCommand("isready");
    if (readResponse().find("readyok") == std::string::npos) return false;

    return true;
}

void StockfishEngine::stop() {

    if (isRunning()) {

        sendCommand("quit");
        DWORD wait = WaitForSingleObject(_m_pi.hProcess, PROCESS_TERMINATE_WAIT_MS);

        if (wait != WAIT_OBJECT_0) {
            TerminateProcess(_m_pi.hProcess, 1);
        }

        CloseHandle(_m_pi.hProcess);
        CloseHandle(_m_pi.hThread);
        _m_pi.hProcess = NULL;
        _m_pi.hThread = NULL;
    }

    if (_m_hStdinWrite) { CloseHandle(_m_hStdinWrite); _m_hStdinWrite = NULL; }
    if (_m_hStdoutRead) { CloseHandle(_m_hStdoutRead); _m_hStdoutRead = NULL; }

}

void StockfishEngine::stopSearch() {

    if (!isRunning()) {
        std::cerr << "[stopSearch] Can't send. Chess engine terminated!" << std::endl;
    }
    sendCommand("stop");
}

bool StockfishEngine::isRunning() {
    return _m_hStdinWrite != NULL && _m_hStdoutRead != NULL;
}

bool StockfishEngine::sendCommand(const std::string& cmd) {

    if (!isRunning()) {
        std::cerr << "[sendCommand] Can't send. Chess engine terminated!" << std::endl;
        return false;
    }
    
    std::cout << cmd << std::endl;

    std::string fullCmd = cmd + "\n";
    DWORD bytesWritten;
    return WriteFile(_m_hStdinWrite, fullCmd.c_str(), fullCmd.length(), &bytesWritten, NULL);
}

std::string StockfishEngine::readResponse() {

    if (!isRunning()) return "";

    char buffer[4096];
    DWORD bytesRead;
    std::string response;
    DWORD startTime = GetTickCount64();

    while (GetTickCount64() - startTime < PROCESS_TERMINATE_TIMEOUT_MS) {
        DWORD available = 0;
        if (!PeekNamedPipe(_m_hStdoutRead, NULL, 0, NULL, &available, NULL)) {
            break;
        }

        if (available > 0) {
            if (!ReadFile(_m_hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
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

    if (!isRunning()) { std::cerr << "[sendCommand] Can't send. Chess engine terminated!" << std::endl; return; }

    if (fen.empty()||fen == "startpos") {
        sendCommand("position startpos");
    }
    else {
        sendCommand("position fen " + fen);
    }

}

std::string StockfishEngine::getBestMoveByTime(int movetime) {
    if (!isRunning()) return "";

    sendCommand("go movetime " + std::to_string(movetime));
    std::string response = readResponse();

    praseEval(response);

    size_t pos = response.find("bestmove");
    if (pos == std::string::npos) return "";

    std::stringstream ss(response.substr(pos));
    std::string token, bestMove;
    ss >> token >> bestMove;

    if (bestMove == "(none)" || bestMove.empty()) return "";
    return bestMove;
}

std::string StockfishEngine::getBestMoveByTimeByDepth(int depth) {
    if (!isRunning())return"";

    sendCommand("go depth " + std::to_string(depth));
    std::string response = readResponse();

    size_t pos = response.find("bestmove");
    if (pos == std::string::npos)return"";

    std::stringstream ss(response.substr(pos));
    std::string token, bestmove;
    ss >> token >> bestmove;

    if (bestmove == "(none)" || bestmove.empty())return "";
    return bestmove;
}

void StockfishEngine::praseEval(const std::string& response) {

    size_t scorePos = response.rfind("score");
    if (scorePos == std::string::npos)return;

    std::stringstream ss(response.substr(scorePos));
    std::string token;
    
    ss >> token;//score;
    ss >> token;//mate or cp

    if (token == "cp") {
        ss >> _m_engineEvaluation.score;
        _m_engineEvaluation.isCheckMate = false;
    }
    else if (token == "mate") {
        ss >> _m_engineEvaluation.checkMateIn;
        _m_engineEvaluation.isCheckMate = true;
        _m_engineEvaluation.score = _m_engineEvaluation.checkMateIn > 0 ? 10000 : -10000;
    }
}

void StockfishEngine::setSkillLevel(int lv) {

    if (lv < 0) lv = 0;
    if (lv > 20)lv = 20;

    sendCommand("setoption name Skill Level value " + std::to_string(lv));
    sendCommand("isready");
    readResponse();

}
