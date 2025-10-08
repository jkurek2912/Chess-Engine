#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <map>
#include <cassert>
#include <unordered_map>

class MoveState;

#define NUM_ROWS 8
#define NUM_COLS 8
enum Piece
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
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
    bool isCapture = false;
    bool isDoublePawnPush = false;
    bool isEnPassant = false;
    bool isPromotion = false;
    bool isCastle = false;
    bool isDraw = false;
    int score;

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

    std::vector<bool> castlingRights = {true, true, true, true};
    int moves;
    int movesSinceCapture;
    int enPassantSquare = -1;
    bool whiteKingInCheck;
    bool blackKingInCheck;
    bool whiteToMove;
    bool trackRepetitions = false; // zobrist hash flag. Keep false for perft testing, true for actual engine usage
    uint64_t hash;
    std::unordered_map<uint64_t, int> repetitionCount;

    void setBoard();
    void printBoard();
    void clearBoard();
    void setCustomBoard();
    void clearSquare(Piece piece, Color color, int square);
    void setPiece(Piece piece, Color color, int square);
    bool isDraw();
    void updateZobrist(const Move &move, const MoveState &state);
    uint64_t computeZobrist();
    std::pair<Piece, Color> findPiece(int square);

private:
};