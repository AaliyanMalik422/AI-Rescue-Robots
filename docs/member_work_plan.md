# Three-Member Work Plan

## Shared rule

Agree on all shared fields, method signatures and state-transition rules before independent coding. Do not change a shared interface without informing the other members.

## Member 1 — Environment, Parser and Pathfinding

Files: `environment.hpp/.cpp`, `pathfinder.hpp/.cpp`, `scenario_parser.hpp/.cpp`.

Coordinate with Member 2 on the `MinHeap` interface.

- Review and extend the implemented environment and adjacency-list graph.
- Implement the exact Appendix A scenario parser and validation.
- Add parser tests using the untouched official `scenario_provided.txt`.
- Test invalid counts, duplicate IDs, invalid references, blocked routes and unreachable locations.
- Document Dijkstra's correctness and complexity.

## Member 2 — Data Structures, Robots and Requests

Files: `collections.hpp`, `min_heap.hpp/.cpp`, `robot.hpp/.cpp`, `rescue_request.hpp/.cpp`.

- Understand and test the supplied DynamicArray and LinkedList.
- Extend the data structures only when the project requires additional operations.
- Review and test the supplied binary min-heap.
- Review robot capability, battery and state-transition rules.
- Review request assignment, work progress, completion, failure and reassignment.
- Ensure one robot cannot own multiple requests and one request cannot be owned by multiple robots.

## Member 3 — Simulation and Integration

Files: `simulation.hpp/.cpp`, `logger.hpp/.cpp`, `main.cpp`, integration tests and output documentation.

- Implement a deterministic discrete-time controller.
- Process events in timestamp and input-file order.
- Activate requests and apply deterministic request/robot selection.
- Coordinate outbound movement, rescue work, return travel and charging.
- Reassess routes after road, station or robot availability changes.
- Produce a reconstructable log and final statistics.

## Integration checkpoints

1. All modules compile together.
2. Collections, environment, heap and pathfinding tests pass.
3. Parser loads the untouched provided scenario and rejects malformed variants.
4. One simulation step produces deterministic state changes.
5. Complete simulation produces logs and final statistics.
6. Minimum-working, additional and stress scenarios are tested.
