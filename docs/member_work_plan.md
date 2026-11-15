# Work Division

Internal freeze: **Tue 24 Nov**. 25-26 Nov is for emergencies only.
Each member has three milestones. **Minimum** = must be done. **Ideal** = do it if you are on track.

Every TODO in the code has pseudocode in a comment. Things marked `DONE (provided)` are already
written (getters, lookups, wiring, printing); you do not need to touch them.

## Member 1 - environment, parser, pathfinding
Files: `id_index_map`, `environment`, `pathfinder`, `scenario_parser`

| By | Minimum | Ideal |
| --- | --- | --- |
| Wed 18 Nov | `IdIndexMap.insert`, `add_location`, `add_connection`, `get_connection_between`, `is_cut_off`. Parser loads `scenario_provided.txt` with the right counts. | Parser also applies every validation rule in the design report. |
| Sat 21 Nov | Dijkstra (`find_shortest_path`), station costs, nearest station. 3 broken scenario files are rejected cleanly. | Tie-break rules, 8+ broken files, help Member 3 with route problems. |
| Mon 23 Nov | Report: graph, parser, Dijkstra complexity. README input-format section. | Alternatives table; build the 4 input sizes for the performance runs. |

## Member 2 - collections, entities, rules, log
Files: `collections.hpp`, `min_heap`, `event_queue`, `robot`, `rescue_request`, `dispatch_rules`, `logger`

| By | Minimum | Ideal |
| --- | --- | --- |
| Mon 16 Nov | `DynamicArray` (grow, push_back, insert, remove, deep copy, destructor) and `LinkedList`. Merge first, everyone needs it. | More edge-case tests. |
| Thu 19 Nov | `MinHeap`, `EventQueue`, request lifecycle, robot state rules, priority comparison and sort, mission battery, `save_to_file`. | Merge sort instead of insertion sort; a test for every illegal transition. |
| Mon 23 Nov | Memory check (no leaks), report: entities, collections, complexity. README output-format section. | Run with `-fsanitize=address`; alternatives table. |

## Member 3 - simulation and integration
Files: `simulation.*`, `main.cpp`

| By | Minimum | Ideal |
| --- | --- | --- |
| Wed 18 Nov | `apply_due_events` and `apply_event` for all seven event types; `assign_pending_requests`. | Early exit when no robot is available. |
| Fri 20 Nov | `travel`, `update_robot`, `decide_after_task`, `recheck_routes`, `interrupt_task`. Provided scenario runs all 250 steps and every request ends COMPLETED or FAILED. | Run twice and compare logs (determinism). |
| Mon 23 Nov | Run all six scenarios and the 4 input sizes. README, implementation/testing/results report sections, save the output for `scenario_provided.txt`. | Walk one request through the log in the report. |

## Working together
- Merge only when your own `make testN` passes and the project still compiles.
- If Member 3 is not done with dispatch by Wed 18 Nov, Member 1 takes `recheck_routes` and `interrupt_task`.
- If collections are not merged by Mon 16 Nov, Members 1 and 3 help Member 2 finish them.
- No new features after Sun 22 Nov.
- Everyone reads the other two modules before the demo.
