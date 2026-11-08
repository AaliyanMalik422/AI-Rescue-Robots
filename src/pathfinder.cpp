#include "pathfinder.hpp"
#include "min_heap.hpp"

namespace {
const long long UNREACHABLE_COST = 9000000000000000000LL;

struct TrackerEntry {
    int location_id;
    long long cost;
    int previous_location_id;
    bool visited;

    TrackerEntry()
        : location_id(-1), cost(UNREACHABLE_COST), previous_location_id(-1), visited(false) {}

    explicit TrackerEntry(int id)
        : location_id(id), cost(UNREACHABLE_COST), previous_location_id(-1), visited(false) {}
};

int find_tracker_index(const DynamicArray<TrackerEntry>& trackers, int location_id) {
    for (int i = 0; i < trackers.size(); ++i) {
        if (trackers[i].location_id == location_id) return i;
    }
    return -1;
}

int add_tracker_if_missing(DynamicArray<TrackerEntry>& trackers, int location_id) {
    int index = find_tracker_index(trackers, location_id);
    if (index >= 0) return index;

    TrackerEntry entry(location_id);
    if (!trackers.push_back(entry)) return -1;
    return trackers.size() - 1;
}
}

PathResult Pathfinder::find_shortest_path(const Environment& environment,
                                          int start_id,
                                          int destination_id) {
    PathResult result;

    if (environment.get_location(start_id) == 0 ||
        environment.get_location(destination_id) == 0) {
        return result;
    }

    if (start_id == destination_id) {
        result.route.push_back(start_id);
        result.exists = true;
        result.total_cost = 0;
        return result;
    }

    DynamicArray<TrackerEntry> trackers;
    MinHeap frontier;

    int start_index = add_tracker_if_missing(trackers, start_id);
    if (start_index < 0) return result;
    trackers[start_index].cost = 0;
    if (!frontier.insert(PathNode(start_id, 0))) return result;

    while (!frontier.empty()) {
        PathNode current;
        if (!frontier.extract_min(current)) break;

        int current_index = find_tracker_index(trackers, current.location_id);
        if (current_index < 0) continue;
        if (trackers[current_index].visited) continue;
        if (current.cost != trackers[current_index].cost) continue;

        trackers[current_index].visited = true;
        if (current.location_id == destination_id) break;

        const LinkedList<NeighborNode>* neighbors = environment.get_neighbors(current.location_id);
        if (neighbors == 0) continue;

        const ListNode<NeighborNode>* neighbor_node = neighbors->get_head();
        while (neighbor_node != 0) {
            int neighbor_id = neighbor_node->data.target_location_id;
            const Connection* connection = environment.get_connection(neighbor_node->data.connection_id);

            if (connection != 0 && !connection->is_blocked && connection->distance > 0) {
                long long edge_cost = connection->distance;
                long long current_cost = trackers[current_index].cost;

                // Guard the addition so an overflow cannot create a false short route.
                if (current_cost <= UNREACHABLE_COST - edge_cost) {
                    long long candidate_cost = current_cost + edge_cost;
                    int neighbor_index = add_tracker_if_missing(trackers, neighbor_id);

                    if (neighbor_index < 0) return result;

                    if (!trackers[neighbor_index].visited &&
                        candidate_cost < trackers[neighbor_index].cost) {
                        trackers[neighbor_index].cost = candidate_cost;
                        trackers[neighbor_index].previous_location_id = current.location_id;
                        if (!frontier.insert(PathNode(neighbor_id, candidate_cost))) return result;
                    } else if (!trackers[neighbor_index].visited &&
                               candidate_cost == trackers[neighbor_index].cost &&
                               (trackers[neighbor_index].previous_location_id == -1 ||
                                current.location_id < trackers[neighbor_index].previous_location_id)) {
                        // Deterministic predecessor tie-break for equal-cost routes.
                        trackers[neighbor_index].previous_location_id = current.location_id;
                    }
                }
            }

            neighbor_node = neighbor_node->next;
        }
    }

    int destination_index = find_tracker_index(trackers, destination_id);
    if (destination_index < 0 || trackers[destination_index].cost == UNREACHABLE_COST) {
        return result;
    }

    DynamicArray<int> reverse_route;
    int location_id = destination_id;
    bool reached_start = false;

    for (int steps = 0; steps <= trackers.size(); ++steps) {
        if (!reverse_route.push_back(location_id)) return PathResult();
        if (location_id == start_id) {
            reached_start = true;
            break;
        }

        int index = find_tracker_index(trackers, location_id);
        if (index < 0 || trackers[index].previous_location_id < 0) break;
        location_id = trackers[index].previous_location_id;
    }

    if (!reached_start) return result;

    for (int i = reverse_route.size() - 1; i >= 0; --i) {
        if (!result.route.push_back(reverse_route[i])) return PathResult();
    }

    result.total_cost = trackers[destination_index].cost;
    result.exists = true;
    return result;
}
