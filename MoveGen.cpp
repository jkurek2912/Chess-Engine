#include "MoveGen.h"
#include "Board.h"

#include <iostream>

// pawn move gen
U64 MoveGen::arrPawnAttacks[2][64];

void MoveGen::initPawnAttacks()
{
   for (int sq = 0; sq < TOTAL_SQUARES; sq++)
   {
      U64 bb = squareBB(sq);
      arrPawnAttacks[WHITE][sq] = shiftNE(bb) | shiftNW(bb);
      arrPawnAttacks[BLACK][sq] = shiftSE(bb) | shiftSW(bb);
   }
}

// knight move gen
U64 MoveGen::arrKnightMoves[64];

void MoveGen::initKnightAttacks()
{
   for (int sq = 0; sq < TOTAL_SQUARES; sq++)
   {
      U64 bb = squareBB(sq);
      U64 attacks = 0ULL;

      attacks |= shiftNNE(bb);
      attacks |= shiftNEE(bb);
      attacks |= shiftSEE(bb);
      attacks |= shiftSSE(bb);
      attacks |= shiftNNW(bb);
      attacks |= shiftNWW(bb);
      attacks |= shiftSWW(bb);
      attacks |= shiftSSW(bb);

      arrKnightMoves[sq] = attacks;
   }
}

// king move gen
U64 MoveGen::arrKingMoves[64];

U64 kingAttacks(U64 kingSet)
{
   U64 attacks = eastOne(kingSet) | westOne(kingSet);
   kingSet |= attacks;
   attacks |= northOne(kingSet) | southOne(kingSet);
   return attacks;
}

void MoveGen::initKingMoves()
{
   U64 sqBB = 1;
   for (int sq = 0; sq < 64; sq++, sqBB <<= 1)
      arrKingMoves[sq] = kingAttacks(sqBB);
}

void MoveGen::initAttackTables()
{
   initPawnAttacks();
   initKnightAttacks();
   initKingMoves();
}

inline void addMovesFromBitboard(std::vector<Move> &moves, int from, U64 attacks)
{
   while (attacks)
   {
      int to = __builtin_ctzll(attacks);
      attacks &= attacks - 1;
      moves.push_back({from, to});
   }
}

U64 whiteSinglePushTargets(U64 whitePawns, U64 occupancy)
{
   return northOne(whitePawns) & ~occupancy;
}

U64 whiteDoublePushTargets(U64 whitePawns, U64 occupancy)
{
   const U64 rank4 = 0x00000000FF000000ULL;
   U64 singlePushs = whiteSinglePushTargets(whitePawns, occupancy);
   return northOne(singlePushs) & ~occupancy & rank4;
}

U64 blackSinglePushTargets(U64 blackPawns, U64 occupancy)
{
   return southOne(blackPawns) & ~occupancy;
}

U64 blackDoublePushTargets(U64 blackPawns, U64 occupancy)
{
   const U64 rank5 = 0x000000FF00000000ULL;
   U64 singlePushs = blackSinglePushTargets(blackPawns, occupancy);
   return southOne(singlePushs) & ~occupancy & rank5;
}

std::vector<Move> MoveGen::generatePawnPushes(U64 pawns, U64 occupancy, COLOR color)
{
   std::vector<Move> moves;
   U64 pushTargets = (color == WHITE) ? whiteSinglePushTargets(pawns, occupancy) : blackSinglePushTargets(pawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare - 8;
      moves.push_back({fromSquare, toSquare});
   }

   pushTargets = (color == WHITE) ? whiteDoublePushTargets(pawns, occupancy) : blackDoublePushTargets(pawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare - 16;
      moves.push_back({fromSquare, toSquare});
   }
   return moves;
}

std::vector<Move> MoveGen::generatePawnAttacks(U64 pawns, U64 occupancy, COLOR color)
{
   std::vector<Move> moves;

   while (pawns)
   {
      int sq = __builtin_ctzll(pawns);
      pawns &= pawns - 1;

      U64 attacks = (color == WHITE) ? MoveGen::arrPawnAttacks[WHITE][sq] & occupancy : MoveGen::arrPawnAttacks[BLACK][sq] & occupancy;

      addMovesFromBitboard(moves, sq, attacks);
   }
   return moves;
}

std::vector<Move> MoveGen::generateKnightMoves(U64 knights, U64 occupancy)
{
   std::vector<Move> moves;

   while (knights)
   {
      int sq = __builtin_ctzll(knights);
      knights &= knights - 1;

      U64 attacks = MoveGen::arrKnightMoves[sq] & ~occupancy;

      addMovesFromBitboard(moves, sq, attacks);
   }
   return moves;
}

std::vector<Move> MoveGen::generateKingMoves(U64 king, U64 occupancy)
{
   std::vector<Move> moves;

   int sq = __builtin_ctzll(king);
   king &= king - 1;

   U64 attacks = MoveGen::arrKingMoves[sq] & ~occupancy;

   addMovesFromBitboard(moves, sq, attacks);

   return moves;
}

std::vector<Move> bishopMovesFromSquare(int sq, U64 friendly, U64 enemy)
{
   std::vector<Move> moves;
   int r = sq / 8;
   int f = sq % 8;

   int dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // NE, NW, SE, SW

   for (auto &d : dirs)
   {
      int i = r + d[0];
      int j = f + d[1];
      while (i >= 0 && i < NUM_ROWS && j >= 0 && j < NUM_COLS)
      {
         int targetSq = i * 8 + j;
         U64 targetMask = 1ULL << targetSq;

         if (friendly & targetMask)
            break;                        // stop: friendly blocks movement
         moves.push_back({sq, targetSq}); // legal move (empty or enemy)

         if (enemy & targetMask)
            break; // stop after capturing enemy
         i += d[0];
         j += d[1];
      }
   }

   return moves;
}

std::vector<Move> MoveGen::generateBishopMoves(U64 bishops, U64 friendly, U64 enemy)
{
   std::vector<Move> moves;

   while (bishops)
   {
      int sq = __builtin_ctzll(bishops);
      bishops &= bishops - 1;
      auto sqMoves = bishopMovesFromSquare(sq, friendly, enemy);
      moves.insert(moves.end(), sqMoves.begin(), sqMoves.end());
   }

   return moves;
}

std::vector<Move> rookMovesFromSquare(int sq, U64 friendly, U64 enemy)
{
   std::vector<Move> moves;
   int r = sq / 8;
   int f = sq % 8;

   // Directions: N, S, E, W
   int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

   for (auto &d : dirs)
   {
      int i = r + d[0];
      int j = f + d[1];
      while (i >= 0 && i < 8 && j >= 0 && j < 8)
      {
         int targetSq = i * 8 + j;
         U64 targetMask = 1ULL << targetSq;

         // Blocked by friendly
         if (friendly & targetMask)
            break;

         // Legal move (empty or enemy)
         moves.push_back({sq, targetSq});

         // Stop after capturing enemy
         if (enemy & targetMask)
            break;

         i += d[0];
         j += d[1];
      }
   }

   return moves;
}

std::vector<Move> MoveGen::generateRookMoves(U64 rooks, U64 friendly, U64 enemy)
{
   std::vector<Move> moves;

   while (rooks)
   {
      int sq = __builtin_ctzll(rooks);
      rooks &= rooks - 1;
      auto sqMoves = rookMovesFromSquare(sq, friendly, enemy);
      moves.insert(moves.end(), sqMoves.begin(), sqMoves.end());
   }
   return moves;
}

std::vector<Move> MoveGen::generateQueenMoves(U64 queens, U64 friendly, U64 enemy)
{
   std::vector<Move> moves;

   while (queens)
   {
      int sq = __builtin_ctzll(queens);
      queens &= queens - 1;

      auto bishopMoves = bishopMovesFromSquare(sq, friendly, enemy);
      auto rookMoves = rookMovesFromSquare(sq, friendly, enemy);

      moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
      moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
   }
   
   return moves;
}
