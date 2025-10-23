#include "Board.h"
#include "Magic.h"
#include "MoveGen.h"
#include "Search.h"
#include "Zobrist.h"

#include <gtest/gtest.h>

class SearchTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        // Called once for the entire test suite
        MoveGen::initAttackTables();
        initZobristKeys();
        initMagicBitboards();
    }

    void SetUp() override
    {
        board = Board();
    }

    Board board;
};

// ----------------- Mate in x Tests -----------------
TEST_F(SearchTest, White_MateIn1)
{
    board.setCustomBoard("6k1/R7/1R6/8/8/8/8/4K3 w - - 12 7");
    SearchResult result = Search::think(board);
    EXPECT_EQ(Move::moveToString(result.bestMove), "b6b8");
}

TEST_F(SearchTest, Black_MateIn1)
{
    board.setCustomBoard("1r2k3/8/8/8/8/8/r7/5K2 b - - 0 1");
    SearchResult result = Search::think(board);
    EXPECT_EQ(Move::moveToString(result.bestMove), "b8b1");
}

TEST_F(SearchTest, White_MateIn2)
{
    board.setCustomBoard("6k1/8/1R6/8/8/8/R7/4K3 w - - 12 7");
    SearchResult result = Search::think(board);
    EXPECT_TRUE(
        Move::moveToString(result.bestMove) == "b6b7" ||
        Move::moveToString(result.bestMove) == "a2a7");
}

TEST_F(SearchTest, Black_MateIn2)
{
    board.setCustomBoard("r4k2/8/8/8/8/1r6/8/7K b - - 2 2");
    SearchResult result = Search::think(board);
    EXPECT_TRUE(
        Move::moveToString(result.bestMove) == "b3b2" ||
        Move::moveToString(result.bestMove) == "a8a2");
}

TEST_F(SearchTest, White_MateIn3)
{
    board.setCustomBoard("8/6k1/8/8/8/8/R7/1R2K3 w - - 0 1");
    SearchResult result = Search::think(board);
    EXPECT_TRUE(
        Move::moveToString(result.bestMove) == "a2a6" ||
        Move::moveToString(result.bestMove) == "b1b6");
}

TEST_F(SearchTest, Black_MateIn3)
{
    board.setCustomBoard("r4k2/8/8/8/1r6/8/7K/8 b - - 4 3");
    SearchResult result = Search::think(board);
    EXPECT_TRUE(
        Move::moveToString(result.bestMove) == "a8a3" ||
        Move::moveToString(result.bestMove) == "b4b3");
}

// ----------------- Hanging Pieces -----------------
TEST_F(SearchTest, White_HangingBishop)
{
    board.setCustomBoard("rnbqk2r/pppp1ppp/5n2/4p3/1b2P3/2P2P2/PP1P2PP/RNBQKBNR w KQkq - 1 4");
    SearchResult result = Search::think(board);
    EXPECT_EQ(Move::moveToString(result.bestMove), "c3b4");
}

TEST_F(SearchTest, Black_HangingBishop)
{
    board.setCustomBoard("rnbqkbnr/pp1p1ppp/2p5/1B2p3/4P3/2N5/PPPP1PPP/R1BQK1NR b KQkq - 1 3");
    SearchResult result = Search::think(board);
    EXPECT_EQ(Move::moveToString(result.bestMove), "c6b5");
}