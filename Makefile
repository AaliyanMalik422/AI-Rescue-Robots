CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -g
CPPFLAGS ?= -Iinclude
LDFLAGS ?=

APP_SOURCES := $(wildcard src/*.cpp)
APP_OBJECTS := $(patsubst src/%.cpp,build/%.o,$(APP_SOURCES))
TEST_SOURCES := $(wildcard tests/*.cpp)
TEST_OBJECTS := $(patsubst tests/%.cpp,build/tests_%.o,$(TEST_SOURCES))

APP := bin/ai_rescue_simulation
TEST_APP := bin/run_tests

.PHONY: all test clean

all: $(APP)

$(APP): $(APP_OBJECTS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(TEST_APP): $(filter-out build/main.o,$(APP_OBJECTS)) $(TEST_OBJECTS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

test: $(TEST_APP)
	./$(TEST_APP)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/tests_%.o: tests/%.cpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf build bin

-include $(APP_OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)
