# Chess Engine V3 ♟️

A high-performance C++ chess engine built from scratch using **bitboards** and advanced search algorithms. Features a fully optimized move generation system, transposition tables, and sophisticated evaluation function.

## 🚀 Features

### Core Engine
- ✅ **Bitboard-based board representation** - Fast move generation using 64-bit integers
- ✅ **Magic bitboards** - Efficient sliding piece move generation
- ✅ **Fully legal move generation** - Handles checks, castling, en passant, and promotions correctly
- ✅ **Zobrist hashing** - Fast position hashing for transposition tables and repetition detection

### Search Algorithm
- ✅ **Negamax with alpha-beta pruning** - Core search algorithm
- ✅ **Quiescence search** - Captures-only search for stable evaluations
- ✅ **Transposition tables** - Caches search results (~4M entries, ~64MB)
- ✅ **Move ordering optimizations**:
  - Transposition table best move prioritization
  - MVV-LVA (Most Valuable Victim - Least Valuable Attacker) for captures
  - Killer moves heuristic
  - History heuristic
- ✅ **Late Move Reductions (LMR)** - Reduces search depth for late quiet moves
- ✅ **Repetition avoidance** - Penalizes repetitions when winning
- ✅ **Dynamic depth control** - Adjusts search depth based on material

### Evaluation Function
- ✅ **Material evaluation** - Piece values (Pawn=100, Knight=320, Bishop=330, Rook=500, Queen=900)
- ✅ **Piece-Square Tables (PST)** - Positional bonuses for piece placement
- ✅ **Bishop pair bonus** - +30 for having two bishops
- ✅ **Pawn structure evaluation**:
  - Doubled pawn penalty (-15 per additional pawn)
  - Isolated pawn penalty (-10 per isolated pawn)
- ✅ **Rook activity**:
  - Open file bonus (+15)
  - Semi-open file bonus (+10)
- ✅ **Endgame detection** - Adjusts king safety based on material count

### Performance Optimizations
- ✅ **Highly optimized move generation** - Precomputed attack tables, efficient bitboard operations
- ✅ **In-place move ordering** - Eliminates temporary allocations
- ✅ **Fast memory operations** - Uses `memset` for heuristic table clearing
- ✅ **Early exit optimizations** - Fast path checks for common cases
- ✅ **Efficient piece lookup** - Optimized `findPiece()` with occupancy checks
- ✅ **Material count caching** - Uses precomputed occupancy bitboards

## 📋 Requirements

- **C++17** or newer
- **GCC** (g++) or **Clang++** compiler
- **Make** build system
- **GoogleTest** (optional, for running tests)

### Platform Support
- ✅ Windows (MinGW/MSVC)
- ✅ Linux
- ✅ macOS

## 🏗️ Building

### Quick Build

```bash
make
```

This produces an executable `c` (or `c.exe` on Windows).

### Build Targets

```bash
# Release build (optimized)
make

# Debug build (with debug symbols)
make debug

# Run tests (requires GoogleTest)
make test

# Run perft benchmarks
make perft

# Clean build artifacts
make clean
```

### Manual Build

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -Isrc -Isrc/board -Isrc/engine \
    src/board/*.cpp src/engine/*.cpp src/main.cpp \
    -o chess_engine
```

## 🎮 Usage

### Interactive Mode

Run the executable and paste FEN strings:

```bash
./c
# or on Windows:
c.exe
```

**Example session:**
```
Simple Chess CLI
Paste a FEN each turn, or type 'q'/'quit'/'exit' to quit.

Enter FEN: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

White to move.

Best move: e2e4
Score: 45
Found 12345 nodes in 0.123 seconds
```

### Exit Commands
- Type `q`, `quit`, or `exit` to quit
- Press `Ctrl+C` to force exit

### FEN Format

The engine accepts standard FEN (Forsyth-Edwards Notation) strings with full validation:
- Piece placement (e.g., `rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR`)
- Side to move (`w` or `b`)
- Castling rights (`KQkq`, `-`, or any combination)
- En passant square (`-` or square like `e3`)
- Halfmove clock (0-100)
- Fullmove number (1+)

**Example FEN (starting position):**
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

## 🏛️ Architecture

### Project Structure

```
src/
├── board/          # Board representation and move generation
│   ├── Board.cpp   # Board state management, FEN parsing
│   ├── MoveGen.cpp # Legal move generation
│   ├── Magic.cpp   # Magic bitboard calculations
│   └── Zobrist.cpp # Zobrist hashing
├── engine/         # Search and evaluation
│   ├── Search.cpp  # Negamax search algorithm
│   ├── Evaluation.cpp # Position evaluation
│   └── Transposition.h # Transposition table
└── main.cpp        # CLI interface
```

### Key Components

#### Board Representation (`Board.h`, `Board.cpp`)
- Bitboard arrays for each piece type and color
- Occupancy bitboards for fast collision detection
- Zobrist hash for position identification
- Repetition tracking for draw detection

#### Move Generation (`MoveGen.cpp`)
- **Pseudo-legal move generation** - Generates moves without checking legality
- **Legal move filtering** - Filters out moves that leave king in check
- **Special moves**:
  - Castling (with path validation)
  - En passant captures
  - Pawn promotions
- Precomputed attack tables for all piece types

#### Search Algorithm (`Search.cpp`)
- **Negamax framework** - Recursive depth-first search
- **Alpha-beta pruning** - Eliminates unpromising branches
- **Quiescence search** - Continues searching captures only
- **Transposition table** - Caches and reuses search results
- **Move ordering** - Prioritizes likely best moves
- **LMR** - Reduces search depth for late quiet moves

#### Evaluation (`Evaluation.cpp`)
- Material count (weighted piece values)
- Piece-square tables for positional evaluation
- Pawn structure analysis
- Rook file control
- Endgame detection

## ⚡ Performance

The engine includes numerous optimizations for maximum performance:

### Search Optimizations
- Transposition table with 4M entries (~64MB)
- Aggressive move ordering reduces nodes searched
- Late Move Reductions save computation on quiet moves
- Efficient heuristic table management

### Move Generation Optimizations
- Precomputed attack tables (no runtime calculations)
- Bitwise operations for move generation
- Efficient legal move filtering
- Minimal memory allocations

### Memory Optimizations
- In-place move ordering (no temporary vectors)
- Precomputed lookup tables
- Efficient bitboard operations
- Zero-copy move structures where possible

## 🧪 Testing

### Perft Testing

Perft (Performance Test) validates move generation correctness by counting nodes at various depths.

```bash
make perft
```

This runs perft tests at multiple depths and outputs benchmark results.

### Unit Tests

Run unit tests for move generation and search algorithms:

```bash
make test
```

## 📊 Example Output

```
Simple Chess CLI
Paste a FEN each turn, or type 'q'/'quit'/'exit' to quit.

Enter FEN: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

White to move.

Best move: e2e4
Score: 45
Found 18608 nodes in 0.045 seconds

Enter FEN: [new FEN]
...
```

## 🔧 Configuration

### Adjusting Search Depth

Edit `src/engine/Search.cpp`:
- `DEPTH_FULL_BOARD` - Depth for full material positions (default: 6)
- `DEPTH_MIDGAME` - Depth for midgame positions (default: 8)
- `MATERIAL_THRESHOLD_FULL` - Material threshold for full board (default: 26)
- `MATERIAL_THRESHOLD_MID` - Material threshold for midgame (default: 10)

### Transposition Table Size

Edit `src/engine/Transposition.h`:
- `TABLE_SIZE` - Number of entries (default: 2^22 = ~4M entries)

## 🐛 Troubleshooting

### Build Issues
- **Missing includes**: Ensure all source files compile individually
- **Linker errors**: Check that all `.cpp` files are included in build
- **Optimization flags**: Ensure `-O2` or `-O3` is enabled for best performance

### Runtime Issues
- **Invalid FEN errors**: Check FEN format - engine validates all fields
- **No legal moves**: Position may be checkmate or stalemate
- **Repetition draws**: Engine now avoids repetitions when winning

## 🛣️ Roadmap / Future Improvements

- [ ] GUI interface (in progress - SFML-based)
- [ ] UCI (Universal Chess Interface) protocol support
- [ ] Opening book
- [ ] Endgame tablebase integration
- [ ] More sophisticated evaluation features
  - King safety in middlegame
  - Piece activity metrics
  - Advanced pawn structure evaluation
- [ ] Time management for timed games
- [ ] Parallel search improvements

## 📝 License

This project is open source. Feel free to use, modify, and distribute.

## 👨‍💻 Development

### Code Style
- C++17 standard
- Clear variable naming
- Extensive comments for complex algorithms
- Const correctness where applicable
- Modern C++ features (bitwise operations, STL containers)

### Contributing
Pull requests welcome! Please ensure:
- Code compiles with `-Wall -Wextra -Wpedantic`
- No new compiler warnings
- Tests pass (if applicable)
- Performance-critical code is optimized

## 📚 References

This engine implements standard chess engine techniques:
- **Bitboards**: Fast move generation using bitwise operations
- **Magic bitboards**: Efficient sliding piece attack generation
- **Negamax**: Unified minimax/alpha-beta algorithm
- **Transposition tables**: Result caching for performance
- **Move ordering**: MVV-LVA, killer moves, history heuristic
- **Late Move Reductions**: Depth reduction heuristic

## 🙏 Acknowledgments

Built as a learning project exploring chess engine development techniques and optimization strategies.

---

**Enjoy playing against the engine!** For questions or issues, please open an issue on the repository.