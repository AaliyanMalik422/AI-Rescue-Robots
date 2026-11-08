# Design Blueprint — AI Rescue Robots

## 1. Core model

- **Location:** unique ID, name, type and charging-station availability.
- **Connection:** unique ID, two endpoint IDs, positive distance and blocked status.
- **Robot:** unique ID, current location, capability, operational state, battery and one optional request assignment.
- **Rescue request:** unique ID, location, category, urgency, report time, required work and lifecycle state.
- **Event:** event type, timestamp, target ID, optional value and original input order.

Named integer constants are used instead of enums in this starter.

## 2. Data structures

| Structure | Purpose | Main operations |
|---|---|---|
| Custom Dynamic Array | Store top-level records and paths | append, index, remove, resize |
| Custom singly linked list | Store each location's neighbours | append, traverse, delete |
| Custom binary min-heap | Dijkstra frontier | insert, extract minimum, heapify |

Request prioritization can initially use a deterministic scan of the request array. Do not add a second heap unless the group can justify it.

## 3. Pathfinding

Use Dijkstra with positive connection distances. Ignore blocked connections. The min-heap orders lower cost first and lower location ID second. The returned route contains the start and destination IDs. An invalid or unreachable destination returns `exists == false`.

## 4. Request selection

Among requests whose report time has arrived and whose state is pending:
1. Higher urgency first.
2. Earlier report time first.
3. Lower request ID first.

## 5. Robot selection

Consider idle, online, capability-compatible robots. A candidate must have a valid outbound route and enough battery for the entire planned mission. Select the candidate with the lowest outbound route cost, then the lowest robot ID.

## 6. Team-approved battery feasibility rule

Before assigning the request, calculate:

```text
outbound cost = outbound distance * battery_per_cost_unit
work cost     = duration_steps * battery_per_work_step
return cost   = return distance * battery_per_cost_unit
required      = outbound cost + work cost + return cost
```

The return destination must be an operational charging station reachable after the rescue. Select the station with the lowest return route cost; break equal-cost ties by station ID. The robot is eligible only when its current battery is at least `required`.

This is a team design decision, not a formula quoted verbatim from the university specification. The simulation must define how a road/station change during a mission affects the return plan.

## 7. Simulation time-step order (planned)

1. Process all events for the current timestamp in original file order.
2. Make newly reported requests eligible.
3. Assign pending requests using the deterministic priority rules.
4. Move robots and handle blocked-route changes.
5. Perform rescue work or charging as appropriate.
6. Log changes, update statistics and check termination.
7. Advance the simulation clock.

Document exact timing semantics for movement, work, battery consumption and charging before implementing the controller.

## 8. Parser work

Member 1 must parse the exact Appendix A format: comments, blank lines, section counts, PARAM values, locations, undirected connections, robots, requests, events and END. Reject invalid counts, malformed data, duplicate IDs, invalid references and inconsistent values. Preserve event order for equal timestamps. Do not modify the official scenario file.
