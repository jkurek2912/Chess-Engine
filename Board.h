#pragma once
#include <cstdint>
#include <vector>
#include <array>

#define TOTAL_SQUARES 64
#define NUM_ROWS 8
#define NUM_COLS 8

enum COLOR
{
    WHITE,
    BLACK,
    BOTH
};

enum PIECE
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

using U64 = std::uint64_t;

class Board
{
public:
    Board();
    void loadStartPosition();
    void clearBoard();
    void customSetBoard();
    void printBoard() const;
    void setOccupancy();

    U64 getPawns(COLOR c) const { return pawns[c]; }
    U64 getBishops(COLOR c) const { return bishops[c]; }
    U64 getKnights(COLOR c) const { return knights[c]; }
    U64 getRooks(COLOR c) const { return rooks[c]; }
    U64 getQueens(COLOR c) const { return queens[c]; }
    U64 getKings(COLOR c) const { return kings[c]; }
    U64 getOccupancy(COLOR c) const { return occupancy[c]; }
    U64 getEnPassantSquare() const { return enPassantSquare; }
    std::array<bool, 4> getCastlingRights() const { return castlingRights; }
    void setEnPassantSquare(int sq) { enPassantSquare = sq; }
    void clearSquare(int sq);
    void setPiece(PIECE piece, COLOR color, int sq);
    bool isEmptyBetween(int from, int to) const;


private:
    std::array<U64, 3> pawns{};
    std::array<U64, 3> knights{};
    std::array<U64, 3> bishops{};
    std::array<U64, 3> rooks{};
    std::array<U64, 3> queens{};
    std::array<U64, 3> kings{};
    std::array<U64, 3> occupancy{};

    std::array<bool, 4> castlingRights; // WK, WQ, BK, BQ
    bool whiteToMove;
    int enPassantSquare;
    int halfMoveClock;
    int fullMoveNumber;
};