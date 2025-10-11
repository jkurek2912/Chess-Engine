
#pragma once

#include "Board.h"
#ifdef UNIT_TESTING
#include <gtest/gtest.h>
#endif

struct MoveState
{
    std::array<bool, 4> castlingRights{};
    int enPassantSquare = -1;
    int movesSinceCapture = 0;
    int moves = 0;
    bool whiteToMove = true;
    Piece capturedPiece = NONE;
    Color capturedColor = BOTH;
    int capturedSquare = -1;
};

class MoveGen
{
public:
    static void initAttackTables();
    static void generateLegalMoves(Board &board, std::vector<Move> &moves);
    static bool isSquareAttacked(const Board &board, int sq, Color attacker);
    static void printAttackMap(const Board &board, Color attacker);
    static void makeMove(Board &board, const Move &move, MoveState &state);
    static void unmakeMove(Board &board, const Move &move, const MoveState &state);
    static bool inCheck(const Board &board, Color color);

private:
    static void generatePawnAttacks(const Board &board, std::vector<Move> &moves);
    static void generateSinglePawnPushes(const Board &board, std::vector<Move> &moves);
    static void generateDoublePawnPushes(const Board &board, std::vector<Move> &moves);
    static void initPawnAttacks();
    static void initKnightAttacks();
    static void initKingAttacks();
    static void generatePawnMoves(const Board &board, std::vector<Move> &moves);
    static void generateKnightMoves(const Board &board, std::vector<Move> &moves);
    static void generateBishopMoves(const Board &board, std::vector<Move> &moves, bool isQueen);
    static void generateRookMoves(const Board &board, std::vector<Move> &moves, bool isQueen);
    static void generateQueenMoves(const Board &board, std::vector<Move> &moves);
    static void generateKingMoves(const Board &board, std::vector<Move> &moves);
    static void applyMove(Board &board, const Move &move);
    static void generatePseudoLegalMoves(const Board &board, std::vector<Move> &moves);

#ifdef UNIT_TESTING
#include <gtest/gtest.h>
    FRIEND_TEST(MoveGen, WhiteKingCastle);
    FRIEND_TEST(MoveGen, BlackKingCastle);
    FRIEND_TEST(MoveGen, WhitePawnPush);
    FRIEND_TEST(MoveGen, BlackPawnPush);
#endif
};