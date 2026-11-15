# AI Rescue Robots

**Autonomous Disaster-Response Simulation**  
C++17 · Data Structures Lab · NUCES Islamabad · Fall 2026

## Overview

AI Rescue Robots is a C++ simulation of an autonomous response fleet operating in a disaster-affected urban area. The system models a road network, rescue robots with different capabilities and battery limits, incoming emergency requests, and environmental changes that affect movement and resource availability.

The simulation is scenario-driven: environment data, robot and request definitions, operating parameters, and scheduled events are loaded from external text files. A central controller coordinates request dispatch, route selection, robot activity, and the recording of simulation results.

The project is being developed with a focus on data-structure implementation, algorithmic reasoning, deterministic behaviour, and measurable performance.

## System Design

### Environment and routing

The disaster area is represented as a **weighted, undirected graph**. Locations are vertices and roads are edges. The graph uses an adjacency-list representation, and blocked roads are excluded from route searches.

Shortest routes are calculated with **Dijkstra's algorithm**. A custom binary min-heap supports efficient selection of the next location to process. The route result records both the sequence of locations and the connections used.

### Rescue dispatch

Requests have a category, urgency, report time, location, and required work duration. The dispatch policy orders requests by:

1. Higher urgency first.
2. Earlier report time first.
3. Lower request ID first.

A robot is considered only when it is available, has a compatible capability, can reach the request, and has sufficient battery for the planned mission. Among feasible candidates, the robot with the lowest outbound route cost is selected; ties are resolved by robot ID.

### Robot resources and task lifecycle

The model tracks robot location, operational state, capability, battery level, and assigned request. Movement, rescue work, and charging affect the robot's resources.

Mission feasibility accounts for the outbound journey, the required rescue work, and a return route to an operational charging station. Requests move through a defined lifecycle from reporting and assignment to completion or failure.

### Time and environmental events

The controller advances the simulation in discrete time steps. Scheduled events can block or restore roads, change charging-station availability, raise request urgency, or take robots offline and bring them back online.

Events with the same timestamp retain their order from the scenario file. The simulation uses explicit dispatch and tie-breaking rules so that identical input produces reproducible decisions.

## Implementation Status

This repository is under active development. The project structure, interfaces, scenario data, build system, and module-level test suites are maintained in the repository. Core parsing, graph, dispatch, and simulation behaviours are being implemented and verified against those tests.

A successful compilation confirms that the current source builds; it does not by itself mean that every scenario or simulation behaviour is complete. Consult the test results and implementation notes when assessing current functionality.

## Requirements

- GCC or another compiler with C++17 support
- GNU Make

The implementation follows the course constraints: core data structures are student-implemented, STL containers are not used, and third-party libraries are not required.

## Build

From the repository root:

```bash
make
```

This builds the executable:

```text
./ai_rescue_robots
```

## Run

The command-line interface accepts a scenario file and optional paths for the log and statistics outputs:

```bash
./ai_rescue_robots data/scenario_provided.txt
```

To specify output locations:

```bash
./ai_rescue_robots data/scenario_provided.txt output/simulation_log.csv output/simulation_statistics.txt
```

The parser and end-to-end simulation are still under development, so full scenario execution is not yet guaranteed.

## Tests

The Makefile provides separate test targets and an aggregate target:

```bash
make test1    # environment, indexing, pathfinding, and parser tests
make test2    # collections, heap, entities, dispatch rules, and logging tests
make test3    # simulation and integration tests
make test     # run all test suites
make clean    # remove generated executables
```

During development, tests associated with unfinished functionality may fail. Test output should be checked after each implementation change.

## Scenario Format

Scenario files are plain text. Blank lines and lines beginning with `#` are ignored. The file contains a scenario name, simulation parameters, counted data sections, scheduled events, and a final `END` marker.

The sections are:

| Section | Contents |
| --- | --- |
| `SCENARIO` | Scenario name |
| `PARAM` | Simulation limits, movement speed, battery consumption, and recharge rate |
| `LOCATIONS` | Location IDs, names, and types |
| `CONNECTIONS` | Connection IDs, endpoints, costs, and initial availability |
| `ROBOTS` | Robot IDs, starting locations, capabilities, and battery limits |
| `REQUESTS` | Request IDs, report times, locations, categories, urgency, and work duration |
| `EVENTS` | Time-stamped environmental and operational changes |
| `END` | End-of-file marker |

Each counted section must contain exactly the number of records specified in its header. Connections are undirected. The parser is expected to validate IDs, references, ranges, section counts, event parameters, and file termination.

The university-provided `data/scenario_provided.txt` is the reference input and must be used unchanged. The `data/` directory also contains additional scenarios covering basic operation, blocked routes, coordination, stress, and resource management.

## Outputs

The simulator is designed to produce two files:

- **Event log:** a chronological, CSV-style record with the fields `time,event,ids,detail`.
- **Statistics report:** summary metrics including request outcomes, response times, movement cost, battery use, assignments, reassignments, conflicts, route-search work, and per-robot utilisation.

The output files are intended to support reproducibility, debugging, and performance analysis.

## Repository Structure

```text
include/    Header files and shared interfaces
src/        C++ implementations
tests/      Module-level and integration test suites
data/       Reference and custom scenario files
docs/       Project specification and design documentation
output/     Generated simulation logs and statistics
Makefile    Build and test targets
```

## Project Documentation

Additional technical material is available in `docs/`:

- `official_project_spec.md` — course project requirements
- `design_blueprint.md` — mapping of the design to the codebase
- `algorithm_pseudocode.md` — algorithm requirements and implementation notes
- `technical_constraints.md` — implementation constraints for the course

