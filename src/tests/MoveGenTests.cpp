#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"

#include <gtest/gtest.h>

TEST(MoveGen, PawnPush)
{
    Board board;
    board.setCustomBoard("8/8/8/3P4/8/8/8/8 w - - 0 1");
    MoveGen::generateLegalMoves(board);
    EXPECT_EQ(board.legalMoves.size(), 1u);
}

// ----------------- Check Tests -----------------
TEST(MoveGen, WhitePawnCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/3P4/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4k3/5P2/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
}

TEST(MoveGen, BlackPawnCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/8/8/8/8/8/3p4/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/5p2/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}

TEST(MoveGen, WhiteBishopCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/8/8/1B6/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4k3/8/8/7B/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
}

TEST(MoveGen, BlackBishopCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/8/8/b7/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/7b/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}

TEST(MoveGen, WhiteQueenCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/8/8/8/8/8/4Q3/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4k3/8/8/8/Q7/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4k3/8/8/7Q/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4k2Q/8/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("Q3k3/8/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
    board.setCustomBoard("4Q3/8/8/4k3/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
}

TEST(MoveGen, BlackQueenCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/4q3/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/q7/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/7q/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/8/4K2q w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/8/q3K3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/4K3/8/8/4q3 w - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
