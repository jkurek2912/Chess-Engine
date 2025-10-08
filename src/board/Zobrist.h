#pragma once
#include <cstdint>

extern uint64_t zobristPiece[2][6][64]; // color, piece, square
extern uint64_t zobristCastling[4];     // white K, white Q, black K, black Q
extern uint64_t zobristEnPassant[8];    // file A–H
extern uint64_t zobristSide;            // side to move

void initZobristKeys(); // call once at program start
