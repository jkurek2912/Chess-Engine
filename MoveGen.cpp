#include "MoveGen.h"
#include "Board.h"

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

// bishops and rooks
U64 attack_table[107648]; // ~840 KiB all rook and bishop attacks

struct SMagic
{
   U64 *ptr;  // pointer to attack_table for each particular square
   U64 mask;  // to mask relevant squares of both lines (no outer squares)
   U64 magic; // magic 64-bit factor
   int shift; // shift right
};

SMagic mBishopTbl[64];
SMagic mRookTbl[64];

inline U64 bishopAttacks(U64 occupancy, int sq)
{
   U64 *aptr = mBishopTbl[sq].ptr;
   occupancy &= mBishopTbl[sq].mask;
   occupancy *= mBishopTbl[sq].magic;
   occupancy >>= mBishopTbl[sq].shift;
   return aptr[occupancy];
}

inline U64 rookAttacks(U64 occupancy, int sq)
{
   U64 *aptr = mRookTbl[sq].ptr;
   occupancy &= mRookTbl[sq].mask;
   occupancy *= mRookTbl[sq].magic;
   occupancy >>= mRookTbl[sq].shift;
   return aptr[occupancy];
}

std::vector<Move> MoveGen::generateBishopMoves(U64 bishops, U64 occupancy, U64 friendlyPieces)
{
   std::vector<Move> moves;

   while (bishops)
   {
      int sq = __builtin_ctzll(bishops);
      bishops &= bishops - 1;

      U64 attacks = bishopAttacks(occupancy, sq);

      attacks &= ~friendlyPieces;

      while (attacks)
      {
         int to = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, to});
      }
   }
   return moves;
}

std::vector<Move> MoveGen::generateRookMoves(U64 rooks, U64 occupancy, U64 friendlyPieces)
{
   std::vector<Move> moves;

   while (rooks)
   {
      int sq = __builtin_ctzll(rooks);
      rooks &= rooks - 1;

      U64 attacks = rookAttacks(occupancy, sq);

      attacks &= ~friendlyPieces;

      while (attacks)
      {
         int to = __builtin_ctzll(attacks);
         attacks &= attacks - 1;

         moves.push_back({sq, to});
      }
   }
   return moves;
}

std::vector<Move> MoveGen::generateQueenMoves(U64 queens, U64 occupancy, U64 friendlyPieces)
{
   std::vector<Move> moves;

   auto bMoves = generateBishopMoves(queens, occupancy, friendlyPieces);
   moves.insert(moves.end(), bMoves.begin(), bMoves.end());

   auto rMoves = generateRookMoves(queens, occupancy, friendlyPieces);
   moves.insert(moves.end(), rMoves.begin(), rMoves.end());

   return moves;
}
