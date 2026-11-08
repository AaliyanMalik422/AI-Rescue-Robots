# AI Rescue Robots - Autonomous Disaster-Response Simulation

Data Structures Lab project, Fall 2026. The application is written in C++17.

## Group Work Division

- **Member 1 - Environment, Parser and Pathfinding:** `environment.hpp/.cpp`, `scenario_parser.hpp/.cpp`, and `pathfinder.hpp/.cpp`.
- **Member 2 - Robots, Requests and Custom Data Structures:** robot/request/event classes, `collections.hpp`, and `min_heap.hpp/.cpp`.
- **Member 3 - Simulation Controller and Integration:** `simulation.hpp/.cpp`, `logger.hpp/.cpp`, and `main.cpp`.

## Project Status

This repository is a starter implementation. It contains working foundations, but it is not yet a completed rescue simulation.

### Implemented in the current starter

- Custom `DynamicArray` and `LinkedList` implementations with explicit object lifetime management, copy/move operations and bounds checks.
- Custom deterministic binary min-heap.
- Undirected weighted environment graph with location and connection ID lookup, road blocking/unblocking, and charging-station availability.
- Dijkstra shortest-path search that avoids blocked roads, reconstructs routes, supports non-sequential location IDs, and uses `long long` route costs with overflow protection.
- Robot and rescue-request records with capability checks, battery validation, assignment checks and request lifecycle operations.
- Event record definitions.
- Logger storage and text-file output.
- Unit tests for the implemented data structures, graph, entities and pathfinding foundations.

### Not implemented yet

- The scenario parser is deliberately pending. The original provided scenario and five custom scenario files are included in `data/`; use their actual record format when implementing the parser. Until then, `ScenarioParser::parse` returns `false` and the application refuses to start a simulation.
- The simulation controller does not yet execute time steps, events, robot assignment, movement, rescue work or charging. Those functions are implementation work for the group.
- The `data/` directory contains the original provided scenario and five custom scenarios. The custom scenarios cover a basic run, blocked routes, coordination, stress testing and resource management. The stress scenario has 112 locations, 234 connections, 24 robots, 120 requests and 500 steps (twice the provided scenario's corresponding main size measures).
- Files in `output/` are placeholders and are not simulation results.

## Build Requirements

- A C++17 compiler (GCC recommended)
- GNU Make

## Build

From the project root:

```bash
make
```

This creates `bin/ai_rescue_simulation`.

## Run

Once the scenario parser and simulation have been implemented and a valid scenario file has been added:

```bash
./bin/ai_rescue_simulation data/scenario_provided.txt
```

At the current starter stage, scenario loading is expected to fail because the parser is intentionally pending.

## Tests

Build and run the unit tests:

```bash
make test
```

Parser and full simulation tests are pending until the input format and simulation loop are implemented. A pending test is not evidence that the corresponding feature works.

For extra memory checks on systems with GCC or Clang, a sanitizer build can be made by overriding `CXXFLAGS`, for example:

```bash
make clean
make test CXXFLAGS="-std=c++17 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-omit-frame-pointer"
```

## Data Structure Restriction

The project uses student-implemented data structures. STL containers such as `std::vector`, `std::list`, `std::map`, `std::queue` and `std::priority_queue` are not used for the core collections, graph or pathfinding heap. Standard strings and file streams are used for text and file handling.

## Repository Layout

- `include/` - class declarations and shared types
- `src/` - C++ implementations
- `data/` - scenario input files
- `tests/` - unit tests
- `output/` - generated logs, statistics and performance measurements
- `docs/` - algorithm pseudocode
