#include "MoveGen.h"
#include "Board.h"

U64 MoveGen::whiteSinglePushTargets(U64 whitePawns, U64 occupancy)
{
    return northOne(whitePawns) & ~occupancy;
}

U64 MoveGen::whiteDoublePushTargets(U64 whitePawns, U64 occupancy) {
   const U64 rank4 = 0x00000000FF000000ULL;
   U64 singlePushs = whiteSinglePushTargets(whitePawns, occupancy);
   return northOne(singlePushs) & ~occupancy & rank4;
}

U64 MoveGen::blackSinglePushTargets(U64 blackPawns, U64 occupancy) {
   return southOne(blackPawns) & ~occupancy;
}

U64 MoveGen::blackDoublePushTargets(U64 blackPawns, U64 occupancy) {
   const U64 rank5 = 0x000000FF00000000ULL;
   U64 singlePushs = blackSinglePushTargets(blackPawns, occupancy);
   return southOne(singlePushs) & ~occupancy & rank5;
}