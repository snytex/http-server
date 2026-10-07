# Makefile – C++23, src/ → build/
# auto generated bc i cant be bothered doing ts myself

CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2 -g
LDFLAGS  :=

SRC_DIR  := src
BUILD_DIR:= build

# all .cpp files in src/
SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
# corresponding .o files in build/
OBJECTS  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# final binary
TARGET   := $(BUILD_DIR)/app

.PHONY: all clean run

all: $(TARGET)

# link
$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "built $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	@echo "cleaned build/"
