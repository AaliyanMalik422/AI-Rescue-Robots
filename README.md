# AI Rescue Robots - Skeleton Template

**Course:** Data Structures Lab, NUCES Islamabad, Fall 2026 | **Standard:** C++17 (GCC)

This repository is a **skeleton**: the structure, headers, interfaces, tests and design docs are
in place, and the logic is left for the three members to implement. Every function that needs
writing is marked `TODO (Member N)` and currently returns a safe "not done" value, so the
project compiles from day one.

## Status

| Part | Status |
|---|---|
| Build, headers, interfaces, Makefile | done |
| Starter tests per member (all currently failing) | done |
| Scenarios (provided + 5 custom) | in `data/` |
| Simple getters, lookups, wiring and report printing | done (`DONE (provided)` in the code) |
| Logic of every module | **to implement**, pseudocode is in each TODO |

## Who builds what

See `docs/member_work_plan.md` for the full tables.

- **Member 1:** `id_index_map`, `environment`, `pathfinder`, `scenario_parser`
- **Member 2:** `collections.hpp`, `min_heap`, `event`, `event_queue`, `robot`, `rescue_request`, `dispatch_rules`, `logger`
- **Member 3:** `simulation`, statistics and outputs, `main`, integration, performance runs

Find your work: `grep -rn "TODO (Member 1)" src include` (use your number). The work division and dates are in `docs/member_work_plan.md`.

## Rules

- Allowed headers: `<iostream>`, `<fstream>`, `<sstream>`, `<string>`, `<cmath>`. No timing header: performance uses work counters. No STL containers,
  no `<algorithm>`, no libraries. Full list and audit: `docs/technical_constraints.md`.
- The binding design is the Phase 1 Design Report; `docs/design_blueprint.md` maps it to files.
- `data/scenario_provided.txt` must load unchanged.

## Build and test

```bash
make            # builds ./ai_rescue_robots
make test1      # Member 1 tests      make test2   # Member 2      make test3   # Member 3
make test       # all three
make clean
```

Tests start red. A test turning green means your piece matches the contract; add more tests as you go.

## Run (once implemented)

```bash
./ai_rescue_robots data/scenario_provided.txt [log_file] [stats_file]
```

Until the parser exists it prints `Invalid scenario: parse_scenario is not implemented yet`.

## Layout

```text
include/   headers = shared contracts (change signatures only after telling the others)
src/       implementations (stubs)
tests/     test_harness.hpp, test_member1.cpp, test_member2.cpp, test_member3.cpp
data/      scenario files
docs/      design blueprint, constraints, work plan, algorithm requirements
output/    generated logs and statistics (git-ignored)
```
