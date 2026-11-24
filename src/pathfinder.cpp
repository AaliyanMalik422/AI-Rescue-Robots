#include "pathfinder.hpp"
#include "min_heap.hpp"

static bool predecessor_is_better(const Environment& environment,
                                  int candidate_parent, int candidate_connection,
                                  int old_parent, int old_connection) {
    if (old_parent == -1) return true;

    const Location* candidate = environment.get_location_at(candidate_parent);
    const Location* old = environment.get_location_at(old_parent);
    if (candidate != 0 && old != 0 && candidate->id != old->id)
        return candidate->id < old->id;

    const Connection* candidate_road = environment.get_connection_at(candidate_connection);
    const Connection* old_road = environment.get_connection(old_connection);
    if (candidate_road == 0) return false;
    if (old_road == 0) return true;
    return candidate_road->id < old_road->id;
}

PathResult find_shortest_path(const Environment& environment, int start_id, int destination_id) {
    PathResult result;
    int start = environment.get_location_index(start_id);
    int destination = environment.get_location_index(destination_id);
    if (start == -1 || destination == -1) return result;

    int count = environment.get_location_count();
    DynamicArray<long long> distance;
    DynamicArray<int> previous_index;
    DynamicArray<int> previous_connection;
    DynamicArray<bool> settled;

    for (int i = 0; i < count; ++i) {
        distance.push_back(UNREACHABLE_COST);
        previous_index.push_back(-1);
        previous_connection.push_back(-1);
        settled.push_back(false);
    }

    MinHeap heap;
    heap.reset(count);
    distance[start] = 0;
    heap.insert_or_decrease(start, 0);

    PathNode current;
    while (heap.extract_min(current)) {
        int u = current.item;
        if (u < 0 || u >= count || settled[u]) continue;
        settled[u] = true;
        if (u == destination) break;

        const LinkedList<NeighborNode>* neighbours = environment.get_neighbors_at(u);
        if (neighbours == 0) continue;
        const ListNode<NeighborNode>* node = neighbours->get_head();

        while (node != 0) {
            int v = node->data.target_location_index;
            const Connection* road = environment.get_connection_at(node->data.connection_index);
            if (road != 0 && !road->is_blocked && v >= 0 && v < count && !settled[v]) {
                ++result.edges_relaxed;
                long long candidate = distance[u] + road->distance;
                bool better = candidate < distance[v];
                bool tie = candidate == distance[v] &&
                    predecessor_is_better(environment, u, node->data.connection_index,
                                          previous_index[v],
                                          environment.get_connection_index(
                                              previous_connection[v]));
                if (better || tie) {
                    distance[v] = candidate;
                    previous_index[v] = u;
                    previous_connection[v] = road->id;
                    heap.insert_or_decrease(v, candidate);
                }
            }
            node = node->next;
        }
    }

    if (distance[destination] == UNREACHABLE_COST) return result;

    DynamicArray<int> reverse_route;
    DynamicArray<int> reverse_connections;
    int current_index = destination;
    while (current_index != start) {
        const Location* location = environment.get_location_at(current_index);
        if (location == 0 || previous_index[current_index] == -1) return PathResult();
        reverse_route.push_back(location->id);
        reverse_connections.push_back(previous_connection[current_index]);
        current_index = previous_index[current_index];
    }
    const Location* start_location = environment.get_location_at(start);
    if (start_location == 0) return PathResult();
    reverse_route.push_back(start_location->id);

    for (int i = reverse_route.size() - 1; i >= 0; --i)
        result.route.push_back(reverse_route[i]);
    for (int i = reverse_connections.size() - 1; i >= 0; --i)
        result.connection_ids.push_back(reverse_connections[i]);

    result.total_cost = distance[destination];
    result.exists = true;
    return result;
}

int compute_station_costs(const Environment& environment,
                          DynamicArray<long long>& cost_by_location_index) {
    int count = environment.get_location_count();
    cost_by_location_index.clear();

    DynamicArray<long long> distance;
    DynamicArray<bool> settled;
    for (int i = 0; i < count; ++i) {
        distance.push_back(UNREACHABLE_COST);
        settled.push_back(false);
        cost_by_location_index.push_back(UNREACHABLE_COST);
    }

    MinHeap heap;
    heap.reset(count);
    for (int i = 0; i < count; ++i) {
        const Location* location = environment.get_location_at(i);
        if (location != 0 && environment.is_operational_station(location->id)) {
            distance[i] = 0;
            heap.insert_or_decrease(i, 0);
        }
    }

    int edges_relaxed = 0;
    PathNode current;
    while (heap.extract_min(current)) {
        int u = current.item;
        if (u < 0 || u >= count || settled[u]) continue;
        settled[u] = true;

        const LinkedList<NeighborNode>* neighbours = environment.get_neighbors_at(u);
        if (neighbours == 0) continue;
        const ListNode<NeighborNode>* node = neighbours->get_head();
        while (node != 0) {
            int v = node->data.target_location_index;
            const Connection* road = environment.get_connection_at(node->data.connection_index);
            if (road != 0 && !road->is_blocked && v >= 0 && v < count && !settled[v]) {
                ++edges_relaxed;
                long long candidate = distance[u] + road->distance;
                if (candidate < distance[v]) {
                    distance[v] = candidate;
                    heap.insert_or_decrease(v, candidate);
                }
            }
            node = node->next;
        }
    }

    for (int i = 0; i < count; ++i) cost_by_location_index[i] = distance[i];
    return edges_relaxed;
}

PathResult find_nearest_station(const Environment& environment, int start_id) {
    PathResult best;
    int start_index = environment.get_location_index(start_id);
    if (start_index == -1) return best;

    for (int i = 0; i < environment.get_location_count(); ++i) {
        const Location* station = environment.get_location_at(i);
        if (station == 0 || !environment.is_operational_station(station->id)) continue;

        PathResult candidate = find_shortest_path(environment, start_id, station->id);
        if (!candidate.exists) continue;

        const Location* best_station = 0;
        if (best.exists && best.route.size() > 0)
            best_station = environment.get_location(best.route[best.route.size() - 1]);

        if (!best.exists || candidate.total_cost < best.total_cost ||
            (candidate.total_cost == best.total_cost && best_station != 0 &&
             station->id < best_station->id)) {
            best = candidate;
        }
    }
    return best;
}

bool route_is_open(const Environment& environment,
                   const DynamicArray<int>& route, int from_index) {
    if (from_index < 0) return false;
    for (int i = from_index; i + 1 < route.size(); ++i) {
        const Connection* connection = environment.get_connection_between(route[i], route[i + 1]);
        if (connection == 0 || connection->is_blocked) return false;
    }
    return true;
}
