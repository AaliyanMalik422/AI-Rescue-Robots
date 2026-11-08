#include "../include/pathfinder.hpp"
#include "../include/min_heap.hpp"

#include <limits>

namespace {
struct TrackerEntry {
    int location_id;
    long long cost;
    int previous_location;
    bool visited;
};

constexpr long long UNREACHABLE_COST = std::numeric_limits<long long>::max();

std::size_t get_tracker_index(DynamicArray<TrackerEntry>& trackers, int location_id) {
    for (std::size_t i = 0; i < trackers.size(); ++i) {
        if (trackers[i].location_id == location_id) return i;
    }
    trackers.push_back({location_id, UNREACHABLE_COST, -1, false});
    return trackers.size() - 1;
}
}

PathResult Pathfinder::find_shortest_path(const Environment& env, int start_id, int end_id) {
    PathResult result{};
    result.total_cost = 0;
    result.exists = false;

    if (env.get_location(start_id) == nullptr || env.get_location(end_id) == nullptr) {
        return result;
    }

    if (start_id == end_id) {
        result.exists = true;
        result.route.push_back(start_id);
        return result;
    }

    DynamicArray<TrackerEntry> trackers;
    MinHeap frontier;

    const std::size_t start_index = get_tracker_index(trackers, start_id);
    trackers[start_index].cost = 0;
    frontier.insert({start_id, 0});

    while (!frontier.empty()) {
        const PathNode current = frontier.extract_min();
        const std::size_t current_index = get_tracker_index(trackers, current.location_id);

        // Ignore outdated heap entries created before a shorter route was found.
        if (current.cost != trackers[current_index].cost || trackers[current_index].visited) {
            continue;
        }
        trackers[current_index].visited = true;

        if (current.location_id == end_id) break;

        const DynamicArray<Environment::NeighborNode> neighbors = env.get_neighbors(current.location_id);
        for (std::size_t i = 0; i < neighbors.size(); ++i) {
            const int neighbor_id = neighbors[i].target_location_id;
            const Connection* connection = env.get_connection(neighbors[i].connection_id);
            if (connection == nullptr || connection->is_blocked || connection->distance <= 0) {
                continue;
            }

            const long long edge_cost = static_cast<long long>(connection->distance);
            const long long current_cost = trackers[current_index].cost;
            if (current_cost > std::numeric_limits<long long>::max() - edge_cost) {
                // Do not allow arithmetic overflow to create a cheaper route.
                continue;
            }
            const long long candidate_cost = current_cost + edge_cost;

            const std::size_t neighbor_index = get_tracker_index(trackers, neighbor_id);
            if (!trackers[neighbor_index].visited && candidate_cost < trackers[neighbor_index].cost) {
                trackers[neighbor_index].cost = candidate_cost;
                trackers[neighbor_index].previous_location = current.location_id;
                frontier.insert({neighbor_id, candidate_cost});
            }
        }
    }

    const std::size_t end_index = get_tracker_index(trackers, end_id);
    if (trackers[end_index].cost == UNREACHABLE_COST) return result;

    // Reconstruct the route by following predecessor location IDs. Bound the
    // walk to protect against an inconsistent predecessor chain.
    DynamicArray<int> reverse_route;
    int location_id = end_id;
    bool reached_start = false;
    for (std::size_t steps = 0; steps <= trackers.size(); ++steps) {
        reverse_route.push_back(location_id);
        if (location_id == start_id) {
            reached_start = true;
            break;
        }

        bool found_predecessor = false;
        for (std::size_t i = 0; i < trackers.size(); ++i) {
            if (trackers[i].location_id == location_id) {
                location_id = trackers[i].previous_location;
                found_predecessor = location_id != -1;
                break;
            }
        }
        if (!found_predecessor) break;
    }

    if (!reached_start) return result;

    for (std::size_t i = reverse_route.size(); i > 0; --i) {
        result.route.push_back(reverse_route[i - 1]);
    }
    result.total_cost = trackers[end_index].cost;
    result.exists = true;
    return result;
}
