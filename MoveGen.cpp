#include "MoveGen.h"
#include "Board.h"

inline U64 squareBB(int sq) { return 1ULL << sq; }
const U64 notAFile = 0xfefefefefefefefeULL;
const U64 notHFile = 0x7f7f7f7f7f7f7f7fULL;
const U64 notGHFile = 0x3F3F3F3F3F3F3F3FULL;
const U64 notABFile = 0xFCFCFCFCFCFCFCFCULL;

// pawn move gen
U64 MoveGen::arrPawnAttacks[2][64];

inline U64 shiftNE(U64 b) { return (b & notHFile) << 9; }
inline U64 shiftSE(U64 b) { return (b & notHFile) >> 7; }
inline U64 shiftSW(U64 b) { return (b & notAFile) >> 9; }
inline U64 shiftNW(U64 b) { return (b & notAFile) << 7; }

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
U64 MoveGen::arrKnightAttacks[64];

inline U64 shiftNNE(U64 b) { return (b & notHFile) << 17; }
inline U64 shiftNEE(U64 b) { return (b & notGHFile) << 10; }
inline U64 shiftSEE(U64 b) { return (b & notGHFile) >> 6;  }
inline U64 shiftSSE(U64 b) { return (b & notHFile) >> 15; }

inline U64 shiftNNW(U64 b) { return (b & notAFile) << 15; }
inline U64 shiftNWW(U64 b) { return (b & notABFile) << 6;  }
inline U64 shiftSWW(U64 b) { return (b & notABFile) >> 10; }
inline U64 shiftSSW(U64 b) { return (b & notAFile) >> 17; }

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

      arrKnightAttacks[sq] = attacks;
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

std::vector<Move> MoveGen::generateWhitePawnPushes(U64 whitePawns, U64 occupancy)
{
   std::vector<Move> moves;
   U64 pushTargets = whiteSinglePushTargets(whitePawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare - 8;
      moves.push_back({fromSquare, toSquare});
   }

   pushTargets = whiteDoublePushTargets(whitePawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare - 16;
      moves.push_back({fromSquare, toSquare});
   }
   return moves;
}

std::vector<Move> MoveGen::generateBlackPawnPushes(U64 blackPawns, U64 occupancy)
{
   std::vector<Move> moves;
   U64 pushTargets = blackSinglePushTargets(blackPawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare + 8;
      moves.push_back({fromSquare, toSquare});
   }

   pushTargets = blackDoublePushTargets(blackPawns, occupancy);

   while (pushTargets)
   {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare + 16;
      moves.push_back({fromSquare, toSquare});
   }
   return moves;
}

std::vector<Move> MoveGen::generateWhitePawnAttacks(U64 whitePawns, U64 blackOccupancy)
{
   std::vector<Move> moves;

   while (whitePawns)
   {
      int sq = __builtin_ctzll(whitePawns);
      whitePawns &= whitePawns - 1;

      U64 attacks = MoveGen::arrPawnAttacks[WHITE][sq] & blackOccupancy;

      while (attacks)
      {
         int toSquare = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, toSquare});
      }
   }
   return moves;
}

std::vector<Move> MoveGen::generateBlackPawnAttacks(U64 blackPawns, U64 whiteOccupancy)
{
   std::vector<Move> moves;

   while (blackPawns)
   {
      int sq = __builtin_ctzll(blackPawns);
      blackPawns &= blackPawns - 1;

      U64 attacks = MoveGen::arrPawnAttacks[BLACK][sq] & whiteOccupancy;

      while (attacks)
      {
         int toSquare = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, toSquare});
      }
   }
   return moves;
}

std::vector<Move> MoveGen::generateWhiteKnightMoves(U64 whiteKnights, U64 whiteOccupancy)
{
   std::vector<Move> moves;

   while (whiteKnights) {
      int sq = __builtin_ctzll(whiteKnights);
      whiteKnights &= whiteKnights - 1;

      U64 attacks = MoveGen::arrKnightAttacks[sq] & ~whiteOccupancy;

      while (attacks) {
         int toSquare = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, toSquare});
      }
   }
   return moves;
}

std::vector<Move> MoveGen::generateBlackKnightMoves(U64 blackKnights, U64 blackOccupancy)
{
   std::vector<Move> moves;

   while (blackKnights) {
      int sq = __builtin_ctzll(blackKnights);
      blackKnights &= blackKnights - 1;

      U64 attacks = MoveGen::arrKnightAttacks[sq] & ~blackOccupancy;

      while (attacks) {
         int toSquare = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, toSquare});
      }
   }
   return moves;
}

