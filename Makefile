CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude

APP = ai_rescue_robots

# Shared building blocks (Member 2 modules, Member 1 modules, Member 3 modules)
M2_SRC = src/min_heap.cpp src/event.cpp src/event_queue.cpp src/robot.cpp src/rescue_request.cpp \
          src/dispatch_rules.cpp src/logger.cpp
M1_SRC = src/id_index_map.cpp src/environment.cpp src/pathfinder.cpp src/scenario_parser.cpp
M3_SRC = src/simulation.cpp
SHARED_SRC = src/names.cpp src/line_reader.cpp

LIB_SRC = $(SHARED_SRC) $(M1_SRC) $(M2_SRC) $(M3_SRC)
HEADERS = $(wildcard include/*.hpp) $(wildcard tests/*.hpp)

all: $(APP)

$(APP): src/main.cpp $(LIB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) src/main.cpp $(LIB_SRC) -o $(APP)

# Per-member test programs. Each links every module so tests can use the others.
test1: tests/test_member1.cpp $(LIB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_member1.cpp $(LIB_SRC) -o test_member1
	./test_member1

test2: tests/test_member2.cpp $(LIB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_member2.cpp $(LIB_SRC) -o test_member2
	./test_member2

test3: tests/test_member3.cpp $(LIB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_member3.cpp $(LIB_SRC) -o test_member3
	./test_member3

# Runs all three even if one fails; exit code is non-zero if any failed.
test:
	-@$(MAKE) --no-print-directory test1
	-@$(MAKE) --no-print-directory test2
	-@$(MAKE) --no-print-directory test3

clean:
	rm -f $(APP) test_member1 test_member2 test_member3

.PHONY: all test test1 test2 test3 clean
