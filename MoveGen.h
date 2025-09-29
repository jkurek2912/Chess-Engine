
#pragma once

#include "Board.h"
class MoveGen
{
public:
    static void generatePawnMoves(const Board &board, std::vector<Move> &moves);
    static void generateKnightMoves(const Board &board, std::vector<Move> &moves);
    static void generateBishopMoves(const Board &board, std::vector<Move> &moves);
    static void generateRookMoves(const Board &board, std::vector<Move> &moves);
    static void generateQueenMoves(const Board &board, std::vector<Move> &moves);
    static void generateKingMoves(const Board &board, std::vector<Move> &moves);
    static void applyMove(Board &board, Move &move);
    static void initAttackTables();
    static void generateLegalMoves(const Board &board, std::vector<Move> &moves);

private:
    static void generatePawnAttacks(const Board &board, std::vector<Move> &moves);
    static void generateSinglePawnPushes(const Board &board, std::vector<Move> &moves);
    static void generateDoublePawnPushes(const Board &board, std::vector<Move> &moves);
    static void initPawnAttacks();
    static void initKnightAttacks();
};