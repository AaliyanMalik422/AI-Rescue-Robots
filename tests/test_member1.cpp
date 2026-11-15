#include "test_harness.hpp"
#include "id_index_map.hpp"
#include "environment.hpp"
#include "pathfinder.hpp"
#include "scenario_parser.hpp"

// MEMBER 1 starter tests. They FAIL until you implement your modules.
// Add more: initially blocked roads, cut-off locations, malformed files
// (wrong counts, duplicate IDs, bad references, missing END), larger graphs.
// Needs Member 2's DynamicArray/LinkedList/MinHeap to be merged first.

static void build_square(Environment& env) {
    // 1 - 2
    // |   |      costs: 1-2:1  2-3:1  1-4:5  4-3:1  (road 4 = 1-4 is the long way)
    // 4 - 3
    env.add_location(Location(1, "A", LOCATION_JUNCTION));
    env.add_location(Location(2, "B", LOCATION_JUNCTION));
    env.add_location(Location(3, "C", LOCATION_BUILDING));
    env.add_location(Location(4, "D", LOCATION_CHARGING));
    env.add_connection(Connection(10, 1, 2, 1, false));
    env.add_connection(Connection(11, 2, 3, 1, false));
    env.add_connection(Connection(12, 1, 4, 5, false));
    env.add_connection(Connection(13, 4, 3, 1, false));
}

static void test_id_index_map() {
    IdIndexMap map;
    int index = -1;
    CHECK(map.insert(7, 0));
    CHECK(!map.insert(7, 1));                   // duplicate ID rejected
    CHECK(map.find(7, index) && index == 0);
    CHECK(!map.find(8, index));                 // inside the array, unused
    CHECK(!map.find(5000, index));              // beyond the array
    CHECK(!map.insert(-1, 0));
    CHECK(!map.insert(MAX_ID + 1, 0));
    for (int i = 100; i < 300; ++i) map.insert(i, i * 2);
    CHECK(map.size() == 201);
    CHECK(map.find(250, index) && index == 500);
    map.clear();
    CHECK(map.size() == 0 && !map.find(7, index));
}

static void test_environment() {
    Environment env;
    build_square(env);
    CHECK(env.get_location_count() == 4);
    CHECK(env.get_connection_count() == 4);
    CHECK(!env.add_location(Location(1, "dup", LOCATION_JUNCTION)));
    CHECK(!env.add_connection(Connection(99, 1, 77, 3, false)));   // unknown endpoint
    CHECK(env.get_location(3) != 0 && env.get_location(99) == 0);
    int index_of_one = env.get_location_index(1);
    const LinkedList<NeighborNode>* neighbours = env.get_neighbors_at(index_of_one);
    CHECK(neighbours != 0 && neighbours->size() == 2);
    CHECK(env.set_connection_blocked(10, true));
    REQUIRE(env.get_connection(10) != 0);
    CHECK(env.get_connection(10)->is_blocked);
    CHECK(env.set_station_available(4, false));
    CHECK(!env.set_station_available(1, false));        // not a station
    CHECK(!env.is_operational_station(4));
}

static void test_pathfinding() {
    Environment env;
    build_square(env);
    PathResult direct = find_shortest_path(env, 1, 3);
    CHECK(direct.exists && direct.total_cost == 2 && direct.route.size() == 3);
    env.set_connection_blocked(10, true);               // 1-2 closed
    PathResult detour = find_shortest_path(env, 1, 3);
    CHECK(detour.exists && detour.total_cost == 6 && detour.route.size() == 3 &&
          detour.route[1] == 4);
    env.set_connection_blocked(12, true);               // 1 now cut off
    CHECK(!find_shortest_path(env, 1, 3).exists);
    CHECK(env.is_cut_off(1));
    CHECK(find_shortest_path(env, 2, 2).exists);
}

static void test_parser_provided_scenario() {
    Environment env;
    DynamicArray<Robot> robots;
    DynamicArray<RescueRequest> requests;
    DynamicArray<Event> events;
    SimulationSettings settings;
    std::string error;
    bool ok = parse_scenario("data/scenario_provided.txt", env, robots, requests,
                             events, settings, error);
    CHECK(ok);
    CHECK(env.get_location_count() == 56 && env.get_connection_count() == 117);
    CHECK(robots.size() == 12 && requests.size() == 60 && events.size() == 20);
    CHECK(settings.max_steps == 250);
    CHECK(!parse_scenario("data/does_not_exist.txt", env, robots, requests,
                          events, settings, error));
}

int main() {
    test_id_index_map();
    test_environment();
    test_pathfinding();
    test_parser_provided_scenario();
    return finish_tests("Member 1");
}
