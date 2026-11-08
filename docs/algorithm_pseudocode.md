# Algorithm Pseudocode

## Dijkstra Shortest-Path Search

The implementation uses the environment's custom adjacency list, a custom
`DynamicArray` of tracker records, and the group's custom binary `MinHeap`.
Each heap entry contains a location ID and a `long long` tentative cost. Equal
cost entries are ordered by location ID for deterministic behavior.

```text
function find_shortest_path(environment, start_id, end_id):
    if either location does not exist:
        return no route

    if start_id equals end_id:
        return [start_id], cost 0

    trackers = empty DynamicArray of tracker records
    frontier = empty custom MinHeap

    add tracker(start_id, cost 0, predecessor -1, unvisited)
    insert (start_id, cost 0) into frontier

    while frontier is not empty:
        current = remove minimum entry from frontier
        current_index = find tracker index for current.location_id

        if current cost differs from tracker's current best cost:
            continue  // stale heap entry
        if tracker is already visited:
            continue

        mark current tracker visited
        if current location is end_id:
            stop search

        for each adjacency entry of current location:
            connection = look up the shared connection record by ID
            if connection is missing or blocked:
                continue

            edge_cost = connection distance
            if current cost + edge_cost would overflow long long:
                continue

            candidate = current cost + edge_cost
            neighbor_index = find or append tracker for neighbor location ID

            if neighbor is not visited and candidate is less than best cost:
                update neighbor cost and predecessor location ID
                insert (neighbor location ID, candidate) into frontier

    if end tracker has infinite cost:
        return no route

    reverse_route = empty DynamicArray of location IDs
    follow predecessor IDs from end_id until start_id is reached
    if predecessor chain is invalid:
        return no route

    reverse reverse_route into final route
    return final route and end tracker's total cost
```

The tracker lookup returns an array index rather than a pointer or reference.
This remains valid when the `DynamicArray` reallocates its storage. Heap entries
that no longer match the tracker's best cost are ignored when extracted.
