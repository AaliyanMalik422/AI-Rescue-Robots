# AI Rescue Robots

**Autonomous Disaster-Response Simulation**
C++17 · Data Structures Lab · NUCES Islamabad · Fall 2026

## Overview

AI Rescue Robots simulates a fleet of rescue robots in a disaster-struck city. The city is a weighted, undirected graph of locations and roads. Robots differ in capability and battery. Rescue requests arrive over time, roads and charging stations fail and recover, and robots go offline and come back. A central controller advances time one step at a time, dispatches the best robot to each request, routes it, runs the rescue, recharges robots, and records everything.

Everything is loaded from a scenario text file, so a larger scenario needs no code change. Identical input always gives identical output. All core data structures (dynamic array, linked list, indexed min-heap, event queue, ID lookup) are written by the team; no STL containers are used.

**Status:** complete. All three test suites pass, and all six scenarios in `data/` run to the end.

## Requirements

- GCC (or another compiler) with C++17 support
- GNU Make

No third-party libraries are used. On Windows, use WSL or MinGW so that `make` and `g++` are available.

## Build

From the repository root:

```bash
make          # builds ./ai_rescue_robots
make clean    # removes the executable and the test programs
```

## Run

```bash
./ai_rescue_robots <scenario_file> [log_file] [stats_file]
```

| Argument | Meaning | Default |
| --- | --- | --- |
| `scenario_file` | Scenario to load (required) | none |
| `log_file` | Event log (CSV) | `output/simulation_log.csv` |
| `stats_file` | Final statistics | `output/simulation_statistics.txt` |

Example with the provided scenario:

```bash
./ai_rescue_robots data/scenario_provided.txt output/simulation_log.csv output/simulation_statistics.txt
```

The program prints the final statistics to the console and writes both files. The `output/` folder must exist (it is in the repository). If the scenario file is invalid, the program prints a message naming the section and record at fault and exits with status 1. If the output files cannot be written, it prints an error and exits with status 1.

## Tests

```bash
make test1    # Member 1: environment, ID lookup, pathfinding, parser
make test2    # Member 2: collections, heap, event queue, entities, dispatch rules, logger
make test3    # Member 3: simulation and integration (all scenarios, determinism)
make test     # runs all three
```

Each program prints `[PASS]` or `[FAIL]` per check and a summary line. All checks should pass.

## Scenarios

All scenarios are in `data/` and use the format below. No scenario extends the format, so every file is compatible with `scenario_provided.txt`.

| File | Locations | Connections | Robots | Requests | Steps | Purpose |
| --- | --- | --- | --- | --- | --- | --- |
| `scenario_provided.txt` | 56 | 117 | 12 | 60 | 250 | University reference (never edit it) |
| `scenario_1_basic.txt` | 8 | 10 | 3 | 5 | 60 | Basic dispatch; one request is cut off and times out |
| `scenario_2_blocked_routes.txt` | 8 | 11 | 3 | 5 | 70 | Blocked roads and rerouting |
| `scenario_3_coordination.txt` | 10 | 15 | 4 | 8 | 100 | Several robots competing for requests |
| `scenario_5_resource_management.txt` | 12 | 17 | 4 | 8 | 120 | Battery limits, charging, stations going down |
| `scenario_4_stress.txt` | 112 | 234 | 24 | 120 | 500 | Stress and scalability (twice the provided size) |

Outcomes with the current code:

| Scenario | Completed | Failed |
| --- | --- | --- |
| provided | 60 | 0 |
| 1_basic | 4 | 1 |
| 2_blocked_routes | 5 | 0 |
| 3_coordination | 8 | 0 |
| 4_stress | 120 | 0 |
| 5_resource_management | 8 | 0 |

## Input format

Plain text. Blank lines and lines starting with `#` are ignored. Tokens are separated by spaces and names contain no spaces. Each section starts with a header `KEYWORD count` followed by exactly `count` records. The file ends with `END`.

| Section | Record | Notes |
| --- | --- | --- |
| `SCENARIO <name>` | first line | Scenario name |
| `PARAM <key> <value>` | one line per key, before `LOCATIONS` | Keys: `max_steps`, `speed`, `battery_per_cost_unit`, `battery_per_work_step`, `recharge_per_step`. Each exactly once. `max_steps` and `speed` must be above 0, the others 0 or more |
| `LOCATIONS n` | `id name type` | `type`: `JUNCTION`, `BUILDING`, `HOSPITAL`, `DEPOT`, `CHARGING`, `COMMAND`. IDs unique |
| `CONNECTIONS n` | `id location_a location_b cost status` | Undirected. `status`: `OPEN` or `BLOCKED`. Both endpoints must exist and differ; cost above 0 |
| `ROBOTS n` | `id start_location capability max_battery initial_battery` | `capability`: `MEDICAL`, `FIRE`, `SUPPLY`, `SEARCH`, `GENERAL` (serves any category). `initial_battery` between 0 and `max_battery` |
| `REQUESTS n` | `id report_time location category urgency duration_steps` | `category`: `MEDICAL`, `FIRE`, `SUPPLY`, `SEARCH`. `urgency` 1 (low) to 5 (critical). `duration_steps` above 0. `report_time` below `max_steps` |
| `EVENTS n` | `time TYPE target [value]` | See below |
| `END` | last line | Nothing but comments or blank lines may follow |

Event types:

| Type | Target | Effect |
| --- | --- | --- |
| `BLOCK` / `UNBLOCK` | connection ID | Closes or reopens a road |
| `STATION_DOWN` / `STATION_UP` | charging location ID | Takes a charging station out of or back into service |
| `SET_URGENCY <value>` | request ID | Changes a request's urgency (1 to 5) |
| `ROBOT_OFFLINE` / `ROBOT_ONLINE` | robot ID | Takes a robot out of or back into service |

Events at the same time step are applied in the order they appear in the file.

The parser stops at the first problem and reports it as `<SECTION> record <n>: <what is wrong>`. It rejects, among others: a record count that does not match the header, duplicate IDs, unknown location or connection references, self-loops, out-of-range values, unknown keywords, wrong token counts, and a missing `END`.

## Simulation rules

**Time.** Time advances in steps `0 .. max_steps-1`. Each step runs these stages in order:

1. Apply the events due at this step.
2. Mark requests whose report time has arrived as pending.
3. Re-check routes: reroute or interrupt robots whose road was blocked, whose charging station went down, or whose battery can no longer cover the mission.
4. Advance every robot: move, work, complete, or recharge.
5. Assign pending requests. A new assignment starts moving on the next step.

After the last step, every request that is not completed becomes `FAILED` with reason `TIME_LIMIT`.

**Routing.** Shortest route by total road cost (Dijkstra with a custom indexed min-heap). Blocked roads are never used. Equal-cost ties prefer the lower location ID, then the lower connection ID.

**Dispatch.** Pending requests are served in this order: higher urgency, then earlier report time, then lower request ID. A robot is a candidate only if it is available, its capability matches the category, a route exists, and its battery covers the whole mission. Among candidates, the lowest route cost wins and a tie goes to the lower robot ID. A request nobody can serve (for example one at a cut-off location) stays pending until it can be served or the time limit ends.

**Battery rule.** Required battery for a mission = outbound route cost x `battery_per_cost_unit` + `duration_steps` x `battery_per_work_step` + cost of the cheapest route to an operational charging station x `battery_per_cost_unit`. A mission is not started unless a charging station is reachable afterwards.

**Movement.** A robot covers up to `speed` cost units per step and spends `battery_per_cost_unit` battery per unit. It keeps its progress on a road between steps. A robot that arrives at the request starts working on the next step; each working step does one unit of work and spends `battery_per_work_step`. When a task ends, a robot at or below 30 percent battery (or unable to afford any pending request) goes to the nearest operational charging station and gains `recharge_per_step` per step until full.

**Failures and events.** If a road on a robot's route is blocked, it is rerouted; if there is no route or its battery reserve is no longer enough, its request returns to pending and is reassigned. A robot taken offline drops its request back to pending, keeps its position and any progress on the road, and does not resume the old task when it returns. A robot crossing a road finishes the road before it receives new work.

**Determinism.** There is no randomness. Event order, dispatch order and all tie-breaks are fixed, so two runs on the same scenario produce identical logs.

## Output format

### Event log (CSV)

One header line, then one line per record: `time,event,ids,detail`.

```text
time,event,ids,detail
0,REQUEST_REPORTED,request=1,location=17;category=MEDICAL;urgency=1
0,ASSIGN,robot=9;request=2,cost=5;battery=72;required=24
1,MOVE,robot=1,distance=1;battery=59;connection=1;progress=1;target=2
```

| Event | Meaning |
| --- | --- |
| `EVENT` | A scenario event was applied |
| `REJECTED` | An action or event was refused (for example urgency change on a finished request) |
| `REQUEST_REPORTED` | A request became pending |
| `ASSIGN` | A robot was assigned to a request |
| `CONFLICT` | More than one robot was feasible; one was chosen |
| `REQUEST_STATE` | A request changed state |
| `ROBOT_STATE` | A robot changed state |
| `MOVE` | A robot moved (distance, battery, road progress) |
| `ARRIVE` | A robot reached a request or a charging station |
| `WORK` | One unit of rescue work was done |
| `COMPLETE` | A request was completed |
| `CHARGE` | A robot gained battery |
| `REROUTE` | A robot was given a new route |
| `INTERRUPT` | A task was interrupted and the request returned to pending |
| `END` | The simulation finished |

Request states: `UNREPORTED`, `PENDING`, `ASSIGNED`, `IN_PROGRESS`, `COMPLETED`, `FAILED`. Robot states: `AVAILABLE`, `TRAVELLING`, `PERFORMING`, `RETURNING`, `RECHARGING`, `UNAVAILABLE`, `OFFLINE`.

### Statistics file

`key: value` lines, then one line per robot.

| Field | Meaning |
| --- | --- |
| `scenario`, `steps_run` | Scenario name and steps simulated |
| `requests_total`, `requests_completed`, `requests_failed`, `requests_unresolved_at_end` | Request outcomes |
| `average_response_time`, `maximum_response_time` | Completion time minus report time, over completed requests |
| `total_movement_cost` | Road cost units travelled by all robots |
| `total_battery_used` | Battery spent by all robots |
| `assignments`, `reassignments` | Assignments made; tasks returned to pending |
| `conflicts_prevented` | Requests for which more than one robot was feasible |
| `route_searches`, `edges_relaxed` | Work counters for performance analysis (used instead of a clock) |
| `robot <id>: busy_steps online_steps utilisation_percent battery_used` | Per-robot utilisation = busy steps / online steps |

## Repository structure

```text
src/        Implementations (main.cpp and one .cpp per module)
include/    Headers
tests/      Test programs for each member's modules and integration
data/       Scenario files
docs/       Project specification and design notes
output/     Generated logs and statistics
Makefile    Build and test targets
```

| Module | Files | Role |
| --- | --- | --- |
| Collections | `collections.hpp`, `min_heap.*`, `event_queue.*`, `event.*` | Dynamic array, linked list, indexed min-heap, event queue |
| Entities and rules | `robot.*`, `rescue_request.*`, `dispatch_rules.*`, `logger.*` | State rules, request lifecycle, priority and battery rules, log |
| Environment | `environment.*`, `id_index_map.*`, `pathfinder.*` | Graph, ID lookup, Dijkstra, nearest station |
| Parser | `scenario_parser.*`, `line_reader.*`, `names.*` | Scenario loading and validation |
| Controller | `simulation.*`, `main.cpp` | Time steps, events, dispatch, movement, statistics |

## Documentation

Additional material is in `docs/`:

- `official_project_spec.md` - course project requirements
- `design_blueprint.md` - mapping of the design to the code
- `algorithm_pseudocode.md` - algorithm requirements
- `technical_constraints.md` - implementation constraints
