#include "pathfinder.hpp"
#include "min_heap.hpp"

// MEMBER 1: Dijkstra and its helpers. Work with location INDICES internally
// (environment.get_location_index) and convert back to IDs only when building the route.
// Working arrays, all sized V = environment.get_location_count():
//   distance[i] (long long, start at UNREACHABLE_COST), previous_index[i] (-1),
//   previous_connection[i] (-1), settled[i] (false).

PathResult find_shortest_path(const Environment& environment, int start_id, int destination_id) {
    // TODO (Member 1)
    // PSEUDOCODE:
    //   result = empty PathResult
    //   s = index of start_id, d = index of destination_id; if either is -1: return result
    //   V = location count; fill distance/previous_*/settled; heap.reset(V)
    //   distance[s] = 0; heap.insert_or_decrease(s, 0)
    //   while heap not empty:
    //       extract_min -> u (index), cost
    //       if settled[u]: continue;  settled[u] = true
    //       if u == d: break                                   (destination finalised)
    //       for each node in environment.get_neighbors_at(u):
    //           conn = environment.get_connection_at(node.connection_index)
    //           if conn->is_blocked: continue                  (blocked roads never used)
    //           v = node.target_location_index; if settled[v]: continue
    //           result.edges_relaxed += 1
    //           candidate = distance[u] + conn->distance
    //           better = candidate < distance[v]
    //           tie    = candidate == distance[v] and (ID of u < ID of previous_index[v]'s location,
    //                                                  then conn->id < previous_connection[v]'s id)
    //           if better or tie: distance[v] = candidate; previous_index[v] = u;
    //                             previous_connection[v] = conn id;  heap.insert_or_decrease(v, candidate)
    //   if distance[d] == UNREACHABLE_COST: return result       (exists stays false)
    //   rebuild: walk previous_index from d back to s collecting location IDs and connection IDs,
    //            then reverse both lists into result.route / result.connection_ids
    //   result.total_cost = distance[d]; result.exists = true
    //   start == destination: route has one location, no connections, cost 0
    (void)environment; (void)start_id; (void)destination_id;
    return PathResult();
}

int compute_station_costs(const Environment& environment,
                          DynamicArray<long long>& cost_by_location_index) {
    // TODO (Member 1)
    // PSEUDOCODE: same loop as above, but
    //   * no destination and no early exit: run until the heap is empty
    //   * initial step: for every location index i that is an operational station:
    //         distance[i] = 0; heap.insert_or_decrease(i, 0)
    //   * roads are undirected, so distances from the stations equal distances to them
    //   * at the end copy distance into cost_by_location_index (resized to V)
    //   * return the number of edges relaxed
    (void)environment;
    cost_by_location_index.clear();
    return 0;
}

PathResult find_nearest_station(const Environment& environment, int start_id) {
    // TODO (Member 1)
    // PSEUDOCODE (simple version): best = none
    //   for every operational station s (by ascending ID, so ties go to the lower ID):
    //       r = find_shortest_path(environment, start_id, s.id)
    //       if r.exists and (best is none or r.total_cost < best.total_cost): best = r
    //   return best (exists == false if no station is reachable)
    //   Cheaper alternative: one Dijkstra from start_id that stops at the first station it settles.
    (void)environment; (void)start_id;
    return PathResult();
}

bool route_is_open(const Environment& environment,
                   const DynamicArray<int>& route, int from_index) {
    // DONE (provided; uses your get_connection_between)
    if (from_index < 0) return false;
    for (int i = from_index; i + 1 < route.size(); ++i) {
        const Connection* connection = environment.get_connection_between(route[i], route[i + 1]);
        if (connection == 0 || connection->is_blocked) return false;
    }
    return true;
}
