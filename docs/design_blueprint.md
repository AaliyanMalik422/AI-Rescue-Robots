# Design Blueprint

The binding design is the **Phase 1 Design Report** (`Phase1_Design_Report.pdf`).
This page maps it onto the code so each member knows where their piece lives.

| Design report item | Code | Owner |
|---|---|---|
| Graph, adjacency lists, ID lookup (3.1) | `environment.*`, `id_index_map.*` (array lookup instead of hash tables) | Member 1 |
| Dijkstra, multi-source station costs (4.1, 4.2) | `pathfinder.*` | Member 1 |
| Scenario input and validation (5) | `scenario_parser.*` | Member 1 |
| Dynamic array, linked list (3.2) | `collections.hpp` | Member 2 |
| Indexed min-heap, event heap (4.1, 7) | `min_heap.*`, `event_queue.*`, `event.*` | Member 2 |
| Robot and request states and rules (3.2, 4.3) | `robot.*`, `rescue_request.*` | Member 2 |
| Request ordering, battery rule (4.2) | `dispatch_rules.*` | Member 2 |
| CSV-style log container and file output (5) | `logger.*` | Member 2 |
| Time step, events, dispatch, movement, charging (4.3) | `simulation.*` | Member 3 |
| Statistics, outputs, CLI, README, performance runs | `simulation.*`, `main.cpp` | Member 3 |

## Agreed policies
1. Undirected connections.
2. Dijkstra = minimum total cost, ignores blocked roads.
3. Request priority: urgency desc, report time asc, ID asc.
4. Robot choice: lowest route cost, then lowest robot ID.
5. Battery: outbound + work + cheapest route to an UP station afterwards.
6. Equal-timestamp events: original file order.
7. One time step = one time unit; exactly `max_steps` updates (0 .. max_steps-1).
8. At `max_steps` every request not COMPLETED becomes FAILED (TIME_LIMIT).

## Known open decisions (record the answer here when settled)
- Execution time: not measured with a clock. Performance evidence uses work counters (route searches, edges relaxed, steps) vs input size. If the instructor approves `<ctime>`, a clock can be added later.
- How `speed` and partial road progress interact when `speed` exceeds a road's cost.
