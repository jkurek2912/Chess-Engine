#include "MoveGen.h"
#include "Board.h"

U64 MoveGen::arrPawnAttacks[2][64];

inline U64 squareBB(int sq) { return 1ULL << sq; }
inline U64 northEast(U64 b) { return (b & 0xfefefefefefefefeULL) << 9; }
inline U64 northWest(U64 b) { return (b & 0x7f7f7f7f7f7f7f7fULL) << 7; }
inline U64 southEast(U64 b) { return (b & 0xfefefefefefefefeULL) >> 7; }
inline U64 southWest(U64 b) { return (b & 0x7f7f7f7f7f7f7f7fULL) >> 9; }

void MoveGen::initPawnAttacks() {
    for (int sq = 0; sq < TOTAL_SQUARES; sq++) {
        U64 bb = squareBB(sq);
        arrPawnAttacks[WHITE][sq] = northEast(bb) | northWest(bb);
        arrPawnAttacks[BLACK][sq] = southEast(bb) | southWest(bb);
    }
}

U64 MoveGen::whiteSinglePushTargets(U64 whitePawns, U64 occupancy)
{
   return northOne(whitePawns) & ~occupancy;
}

U64 MoveGen::whiteDoublePushTargets(U64 whitePawns, U64 occupancy)
{
   const U64 rank4 = 0x00000000FF000000ULL;
   U64 singlePushs = whiteSinglePushTargets(whitePawns, occupancy);
   return northOne(singlePushs) & ~occupancy & rank4;
}

U64 MoveGen::blackSinglePushTargets(U64 blackPawns, U64 occupancy)
{
   return southOne(blackPawns) & ~occupancy;
}

U64 MoveGen::blackDoublePushTargets(U64 blackPawns, U64 occupancy)
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
   
   while (pushTargets) {
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
   
   while (pushTargets) {
      int toSquare = __builtin_ctzll(pushTargets);
      pushTargets &= pushTargets - 1;

      int fromSquare = toSquare + 16;
      moves.push_back({fromSquare, toSquare});
   }
   return moves;
}

std::vector<Move> MoveGen::generateWhitePawnCaptures(U64 whitePawns, U64 blackOccupancy) {

}

std::vector<Move> MoveGen::generateBlackPawnCaptures(U64 blackPawns, U64 whiteOccupancy) {
   
}


std::vector<Move> MoveGen::generateWhiteKnightMoves(U64 whiteKnights, U64 whiteOccupancy, U64 blackOccupancy) {

}


