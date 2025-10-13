#include "Perft.h"
#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"

#include <gtest/gtest.h>

class PerftTest : public ::testing::Test
{
protected:
    Board startingBoard;
    Board position2;
    Board position3;
    Board position4;
    Board position5;

    void SetUp() override
    {
        MoveGen::initAttackTables();
        startingBoard.setBoard();
        position2.setCustomBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        position3.setCustomBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
        position4.setCustomBoard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
        position5.setCustomBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    }
};

TEST_F(PerftTest, Depth1)
{
    EXPECT_EQ(perftTest(startingBoard, 1), 20u);
}

TEST_F(PerftTest, Depth2)
{
    EXPECT_EQ(perftTest(startingBoard, 2), 400u);
}

TEST_F(PerftTest, Depth3)
{
    EXPECT_EQ(perftTest(startingBoard, 3), 8902u);
}

TEST_F(PerftTest, Depth4)
{
    EXPECT_EQ(perftTest(startingBoard, 4), 197281u);
}

TEST_F(PerftTest, Depth5)
{
    EXPECT_EQ(perftTest(startingBoard, 5), 4865609u);
}

TEST_F(PerftTest, Depth6)
{
    EXPECT_EQ(perftTest(startingBoard, 6), 119060324u);
}

// TEST_F(PerftTest, Depth7)
// {
//     EXPECT_EQ(perftTest(startingBoard, 7), 3195901860u);
// }

// TEST_F(PerftTest, Depth8) {
//     EXPECT_EQ(perftTest(startingBoard, 8), 84998978956u);
// }

TEST_F(PerftTest, Position2Depth1)
{
    EXPECT_EQ(perftTest(position2, 1), 48u);
}

TEST_F(PerftTest, Position2Depth2)
{
    EXPECT_EQ(perftTest(position2, 2), 2039u);
}

TEST_F(PerftTest, Position2Depth3)
{
    EXPECT_EQ(perftTest(position2, 3), 97862u);
}

TEST_F(PerftTest, Position2Depth4)
{
    EXPECT_EQ(perftTest(position2, 4), 4085603u);
}

TEST_F(PerftTest, Position2Depth5)
{
    EXPECT_EQ(perftTest(position2, 5), 193690690u);
}

TEST_F(PerftTest, Position2Depth6)
{
    EXPECT_EQ(perftTest(position2, 6), 8031647685u);
}

TEST_F(PerftTest, Position3Depth1)
{
    EXPECT_EQ(perftTest(position3, 1), 14u);
}

TEST_F(PerftTest, Position3Depth2)
{
    EXPECT_EQ(perftTest(position3, 2), 191u);
}

TEST_F(PerftTest, Position3Depth3)
{
    EXPECT_EQ(perftTest(position3, 3), 2812u);
}

TEST_F(PerftTest, Position3Depth4)
{
    EXPECT_EQ(perftTest(position3, 4), 43238u);
}

TEST_F(PerftTest, Position3Depth5)
{
    EXPECT_EQ(perftTest(position3, 5), 674624u);
}

TEST_F(PerftTest, Position3Depth6)
{
    EXPECT_EQ(perftTest(position3, 6), 11030083u);
}

TEST_F(PerftTest, Position3Depth7)
{
    EXPECT_EQ(perftTest(position3, 7), 178633661u);
}

TEST_F(PerftTest, Position3Depth8)
{
    EXPECT_EQ(perftTest(position3, 8), 3009794393u);
}

TEST_F(PerftTest, Position4Depth1)
{
    EXPECT_EQ(perftTest(position4, 1), 6u);
}

TEST_F(PerftTest, Position4Depth2)
{
    EXPECT_EQ(perftTest(position4, 2), 264u);
}

TEST_F(PerftTest, Position4Depth3)
{
    EXPECT_EQ(perftTest(position4, 3), 9467u);
}

TEST_F(PerftTest, Position4Depth4)
{
    EXPECT_EQ(perftTest(position4, 4), 422333u);
}

TEST_F(PerftTest, Position4Depth5)
{
    EXPECT_EQ(perftTest(position4, 5), 15833292u);
}

TEST_F(PerftTest, Position4Depth6)
{
    EXPECT_EQ(perftTest(position4, 6), 706045033u);
}

TEST_F(PerftTest, Position5Depth1)
{
    EXPECT_EQ(perftTest(position5, 1), 44u);
}

TEST_F(PerftTest, Position5Depth2)
{
    EXPECT_EQ(perftTest(position5, 2), 1486u);
}

TEST_F(PerftTest, Position5Depth3)
{
    EXPECT_EQ(perftTest(position5, 3), 62379u);
}

TEST_F(PerftTest, Position5Depth4)
{
    EXPECT_EQ(perftTest(position5, 4), 2103487u);
}

TEST_F(PerftTest, Position5Depth5)
{
    EXPECT_EQ(perftTest(position5, 5), 89941194u);
}