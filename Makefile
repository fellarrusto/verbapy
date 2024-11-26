# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Output binaries
BIN_CLI = $(BUILD_DIR)/anagrammer_cli
BIN_LISTENER = $(BUILD_DIR)/anagrammer_listener

# Default target
all: $(BIN_CLI) $(BIN_LISTENER)

# Build the CLI binary
$(BIN_CLI): $(BUILD_DIR)/anagrammer_cli.o $(BUILD_DIR)/anagrammer.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build the Listener binary
$(BIN_LISTENER): $(BUILD_DIR)/anagrammer_listener.o $(BUILD_DIR)/anagrammer.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
