#include <gtest/gtest.h>
#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"

TEST(MoveGen, PawnPush)
{
    Board board;
    board.setCustomBoard("8/8/8/3P4/8/8/8/8 w - - 0 1");
    std::vector<Move> moves;
    MoveGen::generatePseudoLegalMoves(board, moves);
    EXPECT_EQ(moves.size(), 1u);
}

TEST(MoveGen, KingInCheck)
{
    Board board;
    board.setCustomBoard("4k3/8/8/8/8/8/4Q3/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
