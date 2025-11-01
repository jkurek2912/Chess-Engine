#pragma once
#include "board/Board.h"
#include "board/MoveGen.h"
#include "engine/Search.h"
#include <string>
#include <sstream>
#include <vector>

class UCI
{
public:
    static void run();
    static void processCommand(const std::string &command);
    
private:
    static Board currentBoard;
    static bool isInitialized;
    
    static void uci();
    static void debug(bool on);
    static void isready();
    static void setoption(const std::string &option);
    static void ucinewgame();
    static void position(const std::string &fen, const std::vector<std::string> &moves = {});
    static void go(const std::string &params);
    static void stop();
    static void quit();
    
    static int parseDepth(const std::string &params);
    static int parseTimeLimit(const std::string &params, bool isWhite);
    static int parseMoveTime(const std::string &params);
    
    static std::vector<std::string> split(const std::string &s);
};

