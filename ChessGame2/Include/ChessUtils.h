#pragma once
#include<string>
#include<Utility.h>
class ChessUtils {
public:
    
    static std::string toUCI(int row, int col) {
        char file = 'a' + col;     
        char rank = '8' - row;     
        return std::string(1, file) + rank;
    }

    
    static void fromUCI(const std::string& uci, int& row, int& col) {
        col = uci[0] - 'a';      
        row = '8' - uci[1];     
    }

    
    static void moveFromUCI(const std::string& uciMove,
        Position& fromPos, Position& toPos) {
        fromUCI(uciMove.substr(0, 2), fromPos.row, fromPos.col);
        fromUCI(uciMove.substr(2, 2), toPos.row, toPos.col);
    }

    static std::string moveToUCI(const Position& from, const Position& to) {
        return toUCI(from.row, from.col) + toUCI(to.row, to.col);
    }
};