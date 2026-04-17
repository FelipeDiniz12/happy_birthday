# Makefile configurable variables.
# To override them, set before running make
DEBUG			?= 1
OPTIMIZE		?= 0
LINKER			?= lld

# Internal variables
CXX 			:= g++
CXX_FLAGS		:= -std=c++23 -Wall -Wextra -Wpedantic
INCLUDE=
LDFLAGS			:= -lssl -lcrypto -lsqlite3
TARGET			:= happy_birthday

# Files
SRC_DIR			:= .
BUILD_DIR		:= build
SRCS			:= $(wildcard $(SRC_DIR)/*.cpp)
OBJS 			:= $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS			:= $(OBJS:.o=.d)

# Based on configurable variables, completing CXX_FLAGS values
ifeq ($(DEBUG), 1)
	CXX_FLAGS += -g -O0
else
	CXX_FLAGS += -DNDEBUG
endif

ifeq ($(OPTIMIZE), 1)
	ifeq ($(DEBUG), 0)
		CXX_FLAGS += -O3
	endif
endif

ifeq ($(LINKER), lld)
	LDFLAGS += -fuse-ld=lld
else ifeq ($(LINKER), gold)
	LDFLAGS += -fuse-ld=gold
else
	$(error Unknown linker: $(LINKER))
endif

# Receipts
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(TARGET) $(BUILD_DIR)
