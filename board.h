#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <map>

#define NUM_ROWS 8
#define NUM_COLS 8
enum Piece
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Color
{
    WHITE,
    BLACK,
    BOTH
};

enum CastlingRights
{
    WHITEKING,
    WHITEQUEEN,
    BLACKKING,
    BLACKQUEEN
};

extern std::map<std::pair<Piece, Color>, char> pieceToChar;

class Move
{
public:
    Piece piece;
    Color color;
    int to;
    int from;
    bool caputure = false;
    bool check = false;
    bool checkMate = false;
    bool castle = false;
    bool doublePawnPush = false;
    int enPassantSquare = -1;

    Move() : piece(PAWN), color(WHITE), to(0), from(0) {}

    Move(Piece piece, Color color, int to, int from)
        : piece(piece), color(color), to(to), from(from) {}
};

class Board
{
public:
    std::array<uint64_t, 3> pawns;
    std::array<uint64_t, 3> knights;
    std::array<uint64_t, 3> bishops;
    std::array<uint64_t, 3> rooks;
    std::array<uint64_t, 3> queens;
    std::array<uint64_t, 3> kings;
    std::array<uint64_t, 3> occupancy;

    std::vector<bool> castlingRights;
    int moves;
    int movesSinceCapture;
    bool whiteKingInCheck;
    bool blackKingInCheck;
    bool whiteToMove;

    Move playedMove;

    void setBoard();
    void printBoard();
    void clearBoard();
    void setCustomBoard();

private:
    void setPieces(uint64_t, Piece p, Color color, std::vector<std::vector<char>> &board);
    void setPiece(int index, Piece piece, Color color);
};