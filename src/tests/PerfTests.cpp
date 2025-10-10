#include "Perft.h"
#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"

#include <gtest/gtest.h>

class PerftTest : public ::testing::Test
{
protected:
    Board board;

    void SetUp() override
    {
        board.setBoard();
        MoveGen::initAttackTables();
    }
};

TEST_F(PerftTest, Depth1)
{
    EXPECT_EQ(perftTest(board, 1), 20u);
}

TEST_F(PerftTest, Depth2)
{
    EXPECT_EQ(perftTest(board, 2), 400u);
}

TEST_F(PerftTest, Depth3)
{
    EXPECT_EQ(perftTest(board, 3), 8902u);
}

TEST_F(PerftTest, Depth4)
{
    EXPECT_EQ(perftTest(board, 4), 197281u);
}

TEST_F(PerftTest, Depth5)
{
    EXPECT_EQ(perftTest(board, 5), 4865609u);
}

TEST_F(PerftTest, Depth6)
{
    EXPECT_EQ(perftTest(board, 6), 119060324u);
}

// TEST_F(PerftTest, Depth7)
// {
//     EXPECT_EQ(perftTest(board, 7), 3195901860u);
// }

// TEST_F(PerftTest, Depth8) {
//     EXPECT_EQ(perftTest(board, 8), 84998978956u);
// }