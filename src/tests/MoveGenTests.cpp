#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"

#include <gtest/gtest.h>

// ----------------- Pawn Tests -----------------
TEST(MoveGen, WhitePawnPush)
{
    Board board;
    board.setCustomBoard("8/8/8/3P4/8/8/8/8 w - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_EQ(moves.size(), 1u);
}

TEST(MoveGen, BlackPawnPush)
{
    Board board;
    board.setCustomBoard("8/8/8/4p3/8/8/8/8 b - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_EQ(moves.size(), 1u);
}

TEST(MoveGen, WhitePawnDoublePush)
{
    Board board;
    board.setCustomBoard("8/8/8/8/8/8/4P3/8 w - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_EQ(moves.size(), 2u);
}

TEST(MoveGen, BlackPawnDoublePush)
{
    Board board;
    board.setCustomBoard("8/4p3/8/8/8/8/8/8 b - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_EQ(moves.size(), 2u);
}

TEST(MoveGen, WhitePawnBlocked)
{
    Board board;
    board.setCustomBoard("8/8/8/4n3/4P3/8/8/8 w - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_TRUE(moves.empty());
}

TEST(MoveGen, BlackPawnBlocked)
{
    Board board;
    board.setCustomBoard("8/8/8/4p3/4N3/8/8/8 b - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_TRUE(moves.empty());
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
    board.setCustomBoard("4k3/8/8/8/8/8/3p4/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/5p2/4K3 b - - 0 1");
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
    board.setCustomBoard("4k3/8/8/b7/8/8/8/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/7b/8/8/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}

TEST(MoveGen, WHITE_QUEENCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/8/8/8/8/8/4Q3/4K3 w - - 0 1");
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

TEST(MoveGen, BLACK_QUEENCheckingKing)
{
    Board board;
    board.setCustomBoard("4k3/4q3/8/8/8/8/8/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/q7/8/8/8/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/7q/8/8/4K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/8/4K2q b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/8/8/8/q3K3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
    board.setCustomBoard("4k3/8/8/8/4K3/8/8/4q3 b - - 0 1");
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}

// ----------------- Check Mate Tests -----------------
TEST(MoveGen, CheckMate1)
{
    Board board;
    board.setCustomBoard("1R3k2/R7/8/8/8/8/8/4K3 b - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(MoveGen::inCheck(board, BLACK));
}

TEST(MoveGen, CheckMate2)
{
    Board board;
    board.setCustomBoard("5k2/8/8/8/8/8/r7/1r2K3 w - - 0 1");
    std::vector<Move> moves;
    MoveGen::generateLegalMoves(board, moves);
    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(MoveGen::inCheck(board, WHITE));
}