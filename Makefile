CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude

APP = ai_rescue_robots
TEST_APP = core_tests

SOURCES = src/main.cpp src/environment.cpp src/min_heap.cpp src/pathfinder.cpp \
          src/robot.cpp src/rescue_request.cpp src/event.cpp \
          src/scenario_parser.cpp src/logger.cpp src/simulation.cpp

TEST_SOURCES = tests/core_tests.cpp src/environment.cpp src/min_heap.cpp \
               src/pathfinder.cpp src/robot.cpp src/rescue_request.cpp

all: $(APP)

$(APP): $(SOURCES) $(wildcard include/*.hpp)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(APP)

$(TEST_APP): $(TEST_SOURCES) $(wildcard include/*.hpp)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_APP)

test: $(TEST_APP)
	./$(TEST_APP)

clean:
	rm -f $(APP) $(TEST_APP)

.PHONY: all test clean
