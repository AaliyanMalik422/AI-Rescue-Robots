#pragma once

#include "environment.hpp"

// =====================================================================
// MEMBER 1 - Route search (Dijkstra) on the Environment.
//
// Rules (design report 4.1): minimise total road cost; skip BLOCKED roads;
// equal-cost ties prefer the lower location ID, then the lower connection ID;
// unreachable -> exists == false. Uses Member 2's MinHeap (items are
// location INDICES, so call heap.reset(V) first).
// =====================================================================

struct PathResult {
    DynamicArray<int> route;          // location IDs from start to destination inclusive
    DynamicArray<int> connection_ids; // connection used for each hop (route.size()-1 entries)
    long long total_cost;
    bool exists;
    int edges_relaxed;                // work counter for the performance analysis

    PathResult() : total_cost(0), exists(false), edges_relaxed(0) {}
};

// TODO (Member 1): single-source shortest path from start_id to destination_id.
// Invalid IDs or no open route -> exists == false. start == destination -> cost 0.
PathResult find_shortest_path(const Environment& environment, int start_id, int destination_id);

// TODO (Member 1): multi-source Dijkstra starting from EVERY operational station.
// Fills cost_by_location_index[i] with the lowest cost from location i to a
// station that is UP, or UNREACHABLE_COST. Array is resized to V by the function.
// Returns the number of edges relaxed (work counter).
int compute_station_costs(const Environment& environment,
                           DynamicArray<long long>& cost_by_location_index);

// TODO (Member 1): cheapest route from start_id to any operational station
// (ties: lower station ID). exists == false when none is reachable.
PathResult find_nearest_station(const Environment& environment, int start_id);

// TODO (Member 1): true if every connection on route[from_index .. end] is open.
// Member 3 calls this after BLOCK events to decide whether to recalculate.
bool route_is_open(const Environment& environment,
                   const DynamicArray<int>& route, int from_index);
