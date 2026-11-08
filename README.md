# AI Rescue Robots — Corrected Starter Template

**Course:** Data Structures Lab, NUCES Islamabad, Fall 2026  
**Language standard:** C++17  
**Purpose:** A modular, deliberately incomplete starter for a three-member student team.

## Current implementation status

This package is a starter, not a completed simulation.

### Implemented and tested foundations

- Custom `DynamicArray<T>` and singly linked `LinkedList<T>` in `include/collections.hpp`.
- Environment storage, duplicate checks, undirected adjacency lists and station/road updates.
- Custom binary min-heap.
- Dijkstra shortest-path implementation that skips blocked connections.
- Basic robot capability, battery and state-transition operations.
- Rescue-request lifecycle operations.
- Logger storage and file output.
- A build and foundation test suite.

### Deliberately unfinished project work

- Complete Appendix A scenario parser and validation.
- Discrete-time simulation loop and event processing.
- Request activation, robot selection and assignment coordination.
- Movement across multiple time steps, dynamic rerouting and return-to-charger movement.
- Full battery-feasibility calculation and charging coordination in the simulation controller.
- Complete simulation logging, final statistics and performance analysis.

The unfinished methods contain TODO comments. They are not claimed to be implemented.

## Technical rules currently agreed by the team

- Build target: C++17.
- Approved standard headers: `<iostream>`, `<fstream>`, `<sstream>`, `<string>`, `<cmath>`.
- Do not use STL containers (`vector`, `list`, `queue`, `stack`, `deque`, `map`, `set`, `priority_queue`, etc.).
- Implement required data structures manually.
- Basic templates, pointers, `new` and `delete` have been studied and may be used.
- Use named integer constants instead of enums for now.
- Do not add other standard headers or third-party libraries without instructor approval.

The official specification explicitly prohibits STL containers and requires students to implement core data structures. It also restricts external ML, pathfinding, scheduling and simulation libraries unless approved by the instructor.

## Build and test

From the project root:

```bash
make clean
make
make test
```

The build compiles the starter application. The tests exercise the implemented foundation modules. A passing build and test suite does **not** mean the parser or full simulation is complete.

## Run

After the parser and simulation have been implemented:

```bash
./ai_rescue_robots data/scenario_provided.txt
```

At present, the parser deliberately returns failure and prints a message because its implementation is a Member 1 task.

## Directory structure

```text
AI-Rescue-Robots-Starter/
├── include/
│   ├── collections.hpp
│   ├── types.hpp
│   ├── environment.hpp
│   ├── min_heap.hpp
│   ├── pathfinder.hpp
│   ├── event.hpp
│   ├── robot.hpp
│   ├── rescue_request.hpp
│   ├── scenario_parser.hpp
│   ├── logger.hpp
│   └── simulation.hpp
├── src/
│   ├── main.cpp
│   ├── environment.cpp
│   ├── min_heap.cpp
│   ├── pathfinder.cpp
│   ├── event.cpp
│   ├── robot.cpp
│   ├── rescue_request.cpp
│   ├── scenario_parser.cpp
│   ├── logger.cpp
│   └── simulation.cpp
├── tests/core_tests.cpp
├── data/README.md
├── output/.gitkeep
├── docs/
├── Makefile
└── README.md
```

`collections.hpp` is header-only because template definitions must be visible where template objects are used. There is no `Collections.cpp`.

## Agreed system policies

1. The graph uses undirected connections.
2. Dijkstra selects a minimum-total-distance route and ignores blocked connections.
3. Request priority: urgency descending, report time ascending, request ID ascending.
4. Robot selection: among feasible capable robots, outbound route cost ascending, then robot ID ascending.
5. A robot's mission must account for outbound travel, all rescue work and a return route to an operational charging station before assignment.
6. Events with equal timestamps are processed in their original file order.
7. Identical input and initial conditions must produce deterministic decisions.

The battery-feasibility rule is the team's design decision. The planned calculation is:

```text
required battery = outbound distance * battery_per_cost_unit
                 + rescue duration * battery_per_work_step
                 + return distance * battery_per_cost_unit
```

The simulation must recalculate routes and feasibility after relevant dynamic changes. The implementation details belong to Member 3 and must be documented.

## Work allocation

| Member | Main responsibility |
|---|---|
| 1 | Environment, scenario parser, pathfinding; coordinate with Member 2 on heap interfaces |
| 2 | Dynamic array and linked list review/testing, min-heap, robot and rescue-request behaviour |
| 3 | Simulation loop, event processing, assignment coordination, movement, charging, logging and statistics |

Agree on shared interfaces before changing function signatures.

## Input and scenario obligations

The official supplied `scenario_provided.txt` must be loaded unchanged. Copy the original file into `data/` before implementing parser tests.

The specification describes the provided scenario as 56 locations, 117 connections, 12 robots, 60 requests, 20 events and 250 time steps. The final project must also include at least three scenarios of increasing difficulty and a stress scenario satisfying the specification's size and duration requirements. Consult the official document in `docs/official_project_spec.md` for the exact requirements.
