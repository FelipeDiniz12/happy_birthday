# Makefile configurable variables.
# To override them, set before running make
.DELETE_ON_ERROR:

# User-configurable variables
DEBUG			?= 1
OPTIMIZE		?= 0
LINKER			?= lld
SANITIZE		?=
LIB_DIRS		?=
PREFIX			?= /usr/local
CLANG_TIDY		?= clang-tidy
CLANG_TIDY_ARGS	?= -- $(CXXFLAGS)
CLANG_FORMAT	?= clang-format

# Internal variables
CXX 			:= g++
CXXFLAGS		:= -std=c++23 -Wall -Wextra -Wpedantic
INCLUDE=
LDFLAGS			:= -lssl -lcrypto -lsqlite3
TARGET			:= happy_birthday

# Files
SRC_DIR			:= .
BUILD_DIR		:= build
SRCS			:= $(wildcard $(SRC_DIR)/*.cpp)
OBJS 			:= $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS			:= $(OBJS:.o=.d)

# Check for required tools
ifeq ($(shell command -v $(CXX) 2>/dev/null),)
	$(error $(CXX) not found)
endif

# Based on configurable variables, completing CXXFLAGS values
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -DNDEBUG
	ifeq ($(OPTIMIZE), 1)
		CXXFLAGS += -O3
	endif
endif

ifneq ($(SANITIZE),)
	ifeq ($(SANITIZE), ASAN)
		CXXFLAGS += -fsanitize=address
		LDFLAGS += -fsanitize=address
	else ifeq ($(SANITIZE), UBSAN)
		CXXFLAGS += -fsanitize=undefined
		LDFLAGS += -fsanitize=undefined
	else ifeq ($(SANITIZE), TSAN)
		CXXFLAGS += -fsanitize=thread
		LDFLAGS += -fsanitize=thread
	else
		$(error Unknown sanitizer: $(SANITIZE))
	endif
endif

# Library paths
LDFLAGS += $(addprefix -L, $(LIB_DIRS))

ifeq ($(LINKER), lld)
	LDFLAGS += -fuse-ld=lld
else ifeq ($(LINKER), gold)
	LDFLAGS += -fuse-ld=gold
else
	$(error Unknown linker: $(LINKER))
endif

# Recipes
.PHONY: all clean help install lint format format-check

# Build the executable (default target)
all: format-check $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MMD -MP -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Remove build artifacts
clean:
	$(RM) -rf $(TARGET) $(BUILD_DIR)

# Install the executable to PREFIX/bin
install: $(TARGET)
	install -m 755 $< $(PREFIX)/bin

# Run clang-tidy on project sources
lint:
	$(CLANG_TIDY) $(SRCS) $(CLANG_TIDY_ARGS)

# Format project sources using clang-format
format:
	$(CLANG_FORMAT) -i -style=file $(SRCS)

# Check if project sources are formatted correctly
format-check:
	$(CLANG_FORMAT) -n --Werror -style=file $(SRCS)

# Show help
help:
	@echo "Available targets:"
	@echo "  all           - Build the project (runs format-check first)"
	@echo "  clean         - Remove build artifacts"
	@echo "  format-check  - Check if files are formatted correctly (used by all)"
	@echo "  format        - Format source files with clang-format"
	@echo "  install       - Install the executable to $(PREFIX)/bin"
	@echo "  lint          - Run clang-tidy on source files"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Variables (set before make, e.g., make DEBUG=0):"
	@echo "  DEBUG=1/0			- Enable/disable debug mode"
	@echo "  OPTIMIZE=1/0			- Enable/disable optimization"
	@echo "  LINKER=lld/gold		- Choose linker"
	@echo "  SANITIZE=ASAN/UBSAN/TSAN		- Enable address/undefined/thread sanitizers"
	@echo "  LIB_DIRS			- Additional library directories (-L paths)"
	@echo "  PREFIX			- Install prefix (default: /usr/local)"
	@echo "  CLANG_TIDY			- Lint binary (default: clang-tidy)"
	@echo "  CLANG_FORMAT		- Formatter binary (default: clang-format)"
