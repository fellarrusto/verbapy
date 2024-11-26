# Compiler and base flags
CXX = g++
CXXFLAGS_BASE = -Wall -Wextra -std=c++17
CXXFLAGS_O3 = $(CXXFLAGS_BASE) -O3
CXXFLAGS_NOOPT = $(CXXFLAGS_BASE)

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = $(SRC_DIR)/include
SCRIPT_DIR = scripts

# Source files
SRCS_CLI = $(SRC_DIR)/anagrammer_cli.cpp
SRCS_LISTENER = $(SRC_DIR)/anagrammer_listener.cpp

# Object files
OBJS_CLI_NOOPT = $(BUILD_DIR)/anagrammer_cli.noopt.o
OBJS_LISTENER_NOOPT = $(BUILD_DIR)/anagrammer_listener.noopt.o
OBJS_CLI_O3 = $(BUILD_DIR)/anagrammer_cli.o3.o
OBJS_LISTENER_O3 = $(BUILD_DIR)/anagrammer_listener.o3.o

# Output binaries
BIN_CLI_NOOPT = $(BUILD_DIR)/anagrammer_cli
BIN_LISTENER_NOOPT = $(BUILD_DIR)/anagrammer_listener
BIN_CLI_O3 = $(BUILD_DIR)/anagrammer_cli_O3
BIN_LISTENER_O3 = $(BUILD_DIR)/anagrammer_listener_O3

# Default target
all: $(BIN_CLI_NOOPT) $(BIN_LISTENER_NOOPT) $(BIN_CLI_O3) $(BIN_LISTENER_O3)

# No optimization binaries
$(BIN_CLI_NOOPT): $(OBJS_CLI_NOOPT)
	$(CXX) $(CXXFLAGS_NOOPT) -I$(INCLUDE_DIR) -o $@ $^

$(BIN_LISTENER_NOOPT): $(OBJS_LISTENER_NOOPT)
	$(CXX) $(CXXFLAGS_NOOPT) -I$(INCLUDE_DIR) -o $@ $^

# Optimized (-O3) binaries
$(BIN_CLI_O3): $(OBJS_CLI_O3)
	$(CXX) $(CXXFLAGS_O3) -I$(INCLUDE_DIR) -o $@ $^

$(BIN_LISTENER_O3): $(OBJS_LISTENER_O3)
	$(CXX) $(CXXFLAGS_O3) -I$(INCLUDE_DIR) -o $@ $^

# Object file rules
$(BUILD_DIR)/%.noopt.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/anagrammer.h
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS_NOOPT) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o3.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/anagrammer.h
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS_O3) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
