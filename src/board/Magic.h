#include "Board.h"
#include "MoveGen.h"

static inline void setBit(uint64_t &bb, int sq) { bb |= (1ULL << sq); }
static inline void popBit(uint64_t &bb, int sq) { bb &= ~(1ULL << sq); }
static inline int countBits(uint64_t bb) { return __builtin_popcountll(bb); }
static inline int getLSBIndex(uint64_t bb) { return __builtin_ctzll(bb); }

extern const uint64_t notAFile;
extern const uint64_t notHFile;
extern const uint64_t notABFile;
extern const uint64_t notHGFile;

extern uint64_t bishopMasks[64];
extern uint64_t rookMasks[64];

extern uint64_t bishopAttacks[64][512];
extern uint64_t rookAttacks[64][4096];

extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];
extern const uint64_t rookMagicNumbers[64];
extern const uint64_t bishopMagicNumbers[64];

void initMagicBitboards();

uint64_t getBishopAttacks(int square, uint64_t occupancy);
uint64_t getRookAttacks(int square, uint64_t occupancy);
uint64_t getQueenAttacks(int square, uint64_t occupancy);