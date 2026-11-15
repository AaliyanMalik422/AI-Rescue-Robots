# Algorithm Notes (requirements only)

Each member writes their own pseudocode and complexity argument for the report.
This file lists WHAT each algorithm must guarantee, not how to code it.
The authoritative rules are in the Phase 1 Design Report, sections 4 and 7.

## Dijkstra (Member 1)
- Minimises total road cost; BLOCKED roads are never used.
- Equal-cost ties: lower location ID, then lower connection ID.
- Heap holds one entry per location (decrease-key through the position array).
- Unreachable destination -> `exists == false`, never a partial route.
- Multi-source variant: start from every UP station at cost 0 to get each
  location's cost to its nearest station.
- Report: why Dijkstra is correct with positive costs, and O((V+E) log V).

## ID lookup (Member 1)
- Direct-address array: slots[id] = index or -1; O(1) insert/find, O(max ID) space.
- Report: why it replaces the hash table (IDs are small positive integers, no collisions).

## Parser (Member 1)
- Exactly the Appendix A format and the validation list in design report section 5.

## Heap, collections, event queue (Member 2)
- Amortised O(1) append, O(log n) heap operations; no leaks; deep copies.

## Request ordering and battery rule (Member 2)
- Urgency descending, report time ascending, request ID ascending.
- required battery = outbound*per_cost + duration*per_work + station_cost_after_task*per_cost
  (design report 4.2).

## Time step and dispatch (Member 3)
- Exactly the order in the header comment of `include/simulation.hpp`.
- Robot choice: lowest outbound route cost among feasible robots, then lowest robot ID.
- Offline / blocked-road / station-down behaviour: design report 4.3.
