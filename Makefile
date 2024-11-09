# Recursive wildcard function
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Compiler
CC = gcc

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Directories for debug and release
DEBUG_BUILD_DIR = $(BUILD_DIR)/debug
RELEASE_BUILD_DIR = $(BUILD_DIR)/release

# Compiler flags
CFLAGS = -Wall -Wextra -MMD -MP
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm

# Source files (recursive)
SRC_FILES = $(call rwildcard, $(SRC_DIR), *.c)

# Object and dependency files
RELEASE_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(RELEASE_BUILD_DIR)/%.o, $(SRC_FILES))
DEBUG_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(DEBUG_BUILD_DIR)/%.o, $(SRC_FILES))
RELEASE_DEP_FILES = $(RELEASE_OBJ_FILES:.o=.d)
DEBUG_DEP_FILES = $(DEBUG_OBJ_FILES:.o=.d)

# Target executables
RELEASE_TARGET = $(BIN_DIR)/Overhead
DEBUG_TARGET = $(BIN_DIR)/Overhead_debug

# Default target (build both release and debug)
all: release run_release

# Release build target
release: $(RELEASE_TARGET)

# Debug build target
debug: $(DEBUG_TARGET)

# Build the release executable
$(RELEASE_TARGET): $(RELEASE_OBJ_FILES) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $(RELEASE_OBJ_FILES) $(LDFLAGS) -o $(RELEASE_TARGET)

# Build the debug executable (with -DOH_DEBUG flag)
$(DEBUG_TARGET): $(DEBUG_OBJ_FILES) | $(BIN_DIR)
	@$(CC) -DOH_DEBUG $(CFLAGS) $(DEBUG_OBJ_FILES) $(LDFLAGS) -o $(DEBUG_TARGET)

# Compile source files into object files for release
$(RELEASE_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(RELEASE_BUILD_DIR)
	@mkdir -p $(dir $@)  # Ensure the build directory exists
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile source files into object files for debug
$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_BUILD_DIR)
	@mkdir -p $(dir $@)  # Ensure the build directory exists
	@$(CC) -DOH_DEBUG $(CFLAGS) -c $< -o $@

# Create bin and build directories if they don't exist
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(RELEASE_BUILD_DIR):
	@mkdir -p $(RELEASE_BUILD_DIR)

$(DEBUG_BUILD_DIR):
	@mkdir -p $(DEBUG_BUILD_DIR)

# Clean the build
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the release executable
run_release:
	@./$(BIN_DIR)/Overhead

# Run the debug executable
run:
	@./$(BIN_DIR)/Overhead_debug

# Count lines of code
lines:
	@find $(git rev-parse --show-toplevel) -type f -not -path '*/build/*' -not -path '*/bin/*' -not -path '*/.git/*' -not -path '*/resources/*' -not -path '*/PythonTraining/model_weights_and_biases.npz*' | xargs wc -l

# Check for leaks in the debug version
leaks:
	@valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/Overhead_debug

# Check for leaks in the release version
leaks_release:
	@valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/Overhead_debug

tree:
	@tree -I "resources|bin|build"

tree_src:
	@tree -I "resources|bin|build|include|Makefile|README.md"

# Include dependency files
-include $(RELEASE_DEP_FILES)
-include $(DEBUG_DEP_FILES)

# Phony targets
.PHONY: all clean lines release debug run run_release leaks leaks_release
