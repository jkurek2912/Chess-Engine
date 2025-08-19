#include "MoveGen.h"
#include "Board.h"

U64 MoveGen::arrPawnAttacks[2][64];

inline U64 squareBB(int sq) { return 1ULL << sq; }
const U64 notAFile = 0xfefefefefefefefe;
const U64 notHFile = 0x7f7f7f7f7f7f7f7f;
U64 shiftEast(U64 b) { return (b & notHFile) << 1; }
U64 shiftNortheast(U64 b) { return (b & notHFile) << 9; }
U64 shiftSoutheast(U64 b) { return (b & notHFile) >> 7; }
U64 shiftWest(U64 b) { return (b & notAFile) >> 1; }
U64 shiftSouthwest(U64 b) { return (b & notAFile) >> 9; }
U64 shiftNorthwest(U64 b) { return (b & notAFile) << 7; }

void MoveGen::initPawnAttacks()
{
   for (int sq = 0; sq < TOTAL_SQUARES; sq++)
   {
      U64 bb = squareBB(sq);
      arrPawnAttacks[WHITE][sq] = shiftNortheast(bb) | shiftNorthwest(bb);
      arrPawnAttacks[BLACK][sq] = shiftSoutheast(bb) | shiftSouthwest(bb);
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
   
}

std::vector<Move> MoveGen::generateBlackPawnAttacks(U64 blackPawns, U64 whiteOccupancy)
{
}

std::vector<Move> MoveGen::generateWhiteKnightMoves(U64 whiteKnights, U64 whiteOccupancy, U64 blackOccupancy)
{
}
