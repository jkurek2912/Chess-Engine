#include "MoveGen.h"
#include "Board.h"

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