#pragma once

#include "Board.h"
#include "MoveGen.h"

uint64_t perft(Board &board, int depth);
uint64_t perftTest(Board &board, int depth);