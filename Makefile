CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2

# List of source files (add more .cpp files here later)
SRC := main.cpp Board.cpp MoveGen.cpp
# Object files (automatically derived from SRC)
OBJ := $(SRC:.cpp=.o)

# Output program name
TARGET := chess

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile step (pattern rule)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)
