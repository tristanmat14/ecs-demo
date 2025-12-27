CXX := x86_64-w64-mingw32-g++
CXXFLAGS := -std=c++20

LINKFLAGS := -lgdi32

DEMO ?= sparse-set-ecs
BUILD_DIR := build
SRC_DIR := $(DEMO)/src

INCLUDE_DIRS := $(DEMO)/include $(filter %/, $(wildcard $(DEMO)/include/*/)) $(DEMO)/include/Components/Definitions
CXXFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

TARGET := $(BUILD_DIR)/main
SRC := $(wildcard $(SRC_DIR)/*.cpp)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -O3 $(LINKFLAGS) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
