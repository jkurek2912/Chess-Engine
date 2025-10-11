# ChessEngineV3 ♟️

A high-performance C++ chess engine built from scratch using **bitboards** and a **negamax search** with **alpha–beta pruning**.  
It supports full move legality (including castling, en passant, promotions, and check detection), multi-threaded search, and perft validation up to billions of nodes.

---

## ⚙️ Features

- ✅ **Bitboard-based board representation** for fast move generation  
- ✅ **Fully legal move generation** (checks, castling, en passant, promotions)  
- ✅ **Negamax search** with alpha–beta pruning  
- ✅ **Quiescence search** for stable evaluations  
- ✅ **Parallel search** across multiple threads  
- ✅ **Perft validation** to verify correctness of move generation  
- ✅ **Simple CLI interface** for testing and analysis

---

## 🧠 Engine Overview

The engine works in two main layers:

1. **Move Generation (`MoveGen`)**  
   Generates all legal moves from a position

2. **Search (`Search`)**  
   Explores possible moves using a recursive **negamax** algorithm with **alpha–beta pruning** to eliminate unpromising lines.  
   The evaluation function estimates which side is better at each leaf node. 

## 🧰 Building

### Requirements
- C++17 or newer  
- `make`  
- `g++` or `clang++`  
- (Optional) [GoogleTest](https://github.com/google/googletest) for running unit tests  


#### 🧱 Build the main engine

```bash```
Run make 
This produces an executable c
Run ./c and paste in a FEN to begin the engine