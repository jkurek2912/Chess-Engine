#include "Perft.h"
#include "Board.h"
#include "Magic.h"
#include "MoveGen.h"
#include "Zobrist.h"
#include <algorithm>

#include <gtest/gtest.h>

class PerftTest : public ::testing::Test
{
protected:
    Board startingBoard;
    Board position2;
    Board position3;
    Board position4;
    Board position5;
    unsigned int threads = 1; // Default to multi threaded move validation
    bool benchmark = false;

    void SetUp() override
    {
        MoveGen::initAttackTables();
        initMagicBitboards();
        startingBoard.setBoard();
        position2.setCustomBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        position3.setCustomBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
        position4.setCustomBoard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
        position5.setCustomBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

        const char *env = std::getenv("PERFT_THREADS");
        if (env)
        {
            threads = std::clamp(std::atoi(env), 1, 8);
            if (threads == 1)
            {
                benchmark = true;
            }
        }
    }
};

TEST_F(PerftTest, Depth1)
{
    EXPECT_EQ(perftTest(startingBoard, 1, threads), 20u);
}

TEST_F(PerftTest, Depth2)
{
    EXPECT_EQ(perftTest(startingBoard, 2, threads), 400u);
}

TEST_F(PerftTest, Depth3)
{
    EXPECT_EQ(perftTest(startingBoard, 3, threads), 8902u);
}

TEST_F(PerftTest, Depth4)
{
    EXPECT_EQ(perftTest(startingBoard, 4, threads), 197281u);
}

TEST_F(PerftTest, Depth5)
{
    EXPECT_EQ(perftTest(startingBoard, 5, threads), 4865609u);
}

TEST_F(PerftTest, Depth6)
{
    EXPECT_EQ(perftTest(startingBoard, 6, threads), 119060324u);
}

// TEST_F(PerftTest, Depth7)
// {
//     EXPECT_EQ(perftTest(startingBoard, 7, threads), 3195901860u);
// }

// TEST_F(PerftTest, Depth8) {
//     EXPECT_EQ(perftTest(startingBoard, 8, threads), 84998978956u);
// }

TEST_F(PerftTest, Position2Depth1)
{
    EXPECT_EQ(perftTest(position2, 1, threads), 48u);
}

TEST_F(PerftTest, Position2Depth2)
{
    EXPECT_EQ(perftTest(position2, 2, threads), 2039u);
}

TEST_F(PerftTest, Position2Depth3)
{
    EXPECT_EQ(perftTest(position2, 3, threads), 97862u);
}

TEST_F(PerftTest, Position2Depth4)
{
    EXPECT_EQ(perftTest(position2, 4, threads), 4085603u);
}

TEST_F(PerftTest, Position2Depth5)
{
    EXPECT_EQ(perftTest(position2, 5, threads), 193690690u);
}

// TEST_F(PerftTest, Position2Depth6)
// {
//     if (benchmark)
//         GTEST_SKIP();
//     EXPECT_EQ(perftTest(position2, 6, threads), 8031647685u);
// }

TEST_F(PerftTest, Position3Depth1)
{
    EXPECT_EQ(perftTest(position3, 1, threads), 14u);
}

TEST_F(PerftTest, Position3Depth2)
{
    EXPECT_EQ(perftTest(position3, 2, threads), 191u);
}

TEST_F(PerftTest, Position3Depth3)
{
    EXPECT_EQ(perftTest(position3, 3, threads), 2812u);
}

TEST_F(PerftTest, Position3Depth4)
{
    EXPECT_EQ(perftTest(position3, 4, threads), 43238u);
}

TEST_F(PerftTest, Position3Depth5)
{
    EXPECT_EQ(perftTest(position3, 5, threads), 674624u);
}

TEST_F(PerftTest, Position3Depth6)
{
    EXPECT_EQ(perftTest(position3, 6, threads), 11030083u);
}

TEST_F(PerftTest, Position3Depth7)
{
    EXPECT_EQ(perftTest(position3, 7, threads), 178633661u);
}

// TEST_F(PerftTest, Position3Depth8)
// {
//     if (benchmark)
//         GTEST_SKIP();
//     EXPECT_EQ(perftTest(position3, 8, threads), 3009794393u);
// }

TEST_F(PerftTest, Position4Depth1)
{
    EXPECT_EQ(perftTest(position4, 1, threads), 6u);
}

TEST_F(PerftTest, Position4Depth2)
{
    EXPECT_EQ(perftTest(position4, 2, threads), 264u);
}

TEST_F(PerftTest, Position4Depth3)
{
    EXPECT_EQ(perftTest(position4, 3, threads), 9467u);
}

TEST_F(PerftTest, Position4Depth4)
{
    EXPECT_EQ(perftTest(position4, 4, threads), 422333u);
}

TEST_F(PerftTest, Position4Depth5)
{
    EXPECT_EQ(perftTest(position4, 5, threads), 15833292u);
}

// TEST_F(PerftTest, Position4Depth6)
// {
//     if (benchmark)
//         GTEST_SKIP();
//     EXPECT_EQ(perftTest(position4, 6, threads), 706045033u);
// }

TEST_F(PerftTest, Position5Depth1)
{
    EXPECT_EQ(perftTest(position5, 1, threads), 44u);
}

TEST_F(PerftTest, Position5Depth2)
{
    EXPECT_EQ(perftTest(position5, 2, threads), 1486u);
}

TEST_F(PerftTest, Position5Depth3)
{
    EXPECT_EQ(perftTest(position5, 3, threads), 62379u);
}

TEST_F(PerftTest, Position5Depth4)
{
    EXPECT_EQ(perftTest(position5, 4, threads), 2103487u);
}

TEST_F(PerftTest, Position5Depth5)
{
    EXPECT_EQ(perftTest(position5, 5, threads), 89941194u);
}