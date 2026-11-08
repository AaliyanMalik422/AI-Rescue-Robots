#include <iostream>
#include <climits>
#include "../include/pathfinder.hpp"

int run_pathfinding_tests() {
    int fails = 0;

    // Test 1: Star graph with 6 neighbors to test index stability & array resizing under ASan
    Environment env_star;
    env_star.add_location(1, "Center", false, false);
    for (int i = 2; i <= 7; ++i) {
        env_star.add_location(i, "Leaf", false, false);
        env_star.add_connection(100 + i, 1, i, 10 * i, false);
    }
    PathResult res_star = Pathfinder::find_shortest_path(env_star, 1, 7);
    if (!res_star.exists || res_star.total_cost != 70 || res_star.route.size() != 2) fails++;

    // Test 2: Unreachable destination
    Environment env_unreachable;
    env_unreachable.add_location(10, "A", false, false);
    env_unreachable.add_location(20, "B", false, false);
    PathResult res_unreach = Pathfinder::find_shortest_path(env_unreachable, 10, 20);
    if (res_unreach.exists || res_unreach.route.size() != 0) fails++;

    // Test 3: Start equals destination
    PathResult res_same = Pathfinder::find_shortest_path(env_star, 1, 1);
    if (!res_same.exists || res_same.total_cost != 0 || res_same.route.size() != 1 || res_same.route[0] != 1) fails++;

    // Test 4: Arbitrary non-sequential location IDs
    Environment env_arb;
    env_arb.add_location(101, "Start", false, false);
    env_arb.add_location(555, "Mid", false, false);
    env_arb.add_location(9999, "End", false, false);
    env_arb.add_connection(50, 101, 555, 15, false);
    env_arb.add_connection(51, 555, 9999, 25, false);

    PathResult res_arb = Pathfinder::find_shortest_path(env_arb, 101, 9999);
    if (!res_arb.exists || res_arb.total_cost != 40 || res_arb.route.size() != 3) fails++;
    if (res_arb.route.size() == 3 && (res_arb.route[0] != 101 || res_arb.route[1] != 555 || res_arb.route[2] != 9999)) fails++;

    // Test 5 & 6: Blocked shortest route and Alternative route
    Environment env_alt;
    env_alt.add_location(1, "A", false, false);
    env_alt.add_location(2, "B", false, false);
    env_alt.add_location(3, "C", false, false);
    env_alt.add_connection(1, 1, 2, 10, false);
    env_alt.add_connection(2, 2, 3, 10, false);
    env_alt.add_connection(3, 1, 3, 50, false);

    // Initial shortest path: 1 -> 2 -> 3 (cost 20)
    PathResult res_init = Pathfinder::find_shortest_path(env_alt, 1, 3);
    if (!res_init.exists || res_init.total_cost != 20 || res_init.route.size() != 3) fails++;

    // Block road 1 (1-2)
    env_alt.set_connection_blocked(1, true);
    PathResult res_blocked = Pathfinder::find_shortest_path(env_alt, 1, 3);
    if (!res_blocked.exists || res_blocked.total_cost != 50 || res_blocked.route.size() != 2) fails++;

    // Test 7: Larger branching graph
    Environment env_branch;
    for (int i = 1; i <= 10; ++i) env_branch.add_location(i, "Loc", false, false);
    env_branch.add_connection(1, 1, 2, 5, false);
    env_branch.add_connection(2, 2, 3, 5, false);
    env_branch.add_connection(3, 3, 10, 5, false);
    env_branch.add_connection(4, 1, 4, 2, false);
    env_branch.add_connection(5, 4, 5, 2, false);
    env_branch.add_connection(6, 5, 6, 2, false);
    env_branch.add_connection(7, 6, 10, 2, false); // 1->4->5->6->10 cost 8

    PathResult res_branch = Pathfinder::find_shortest_path(env_branch, 1, 10);
    if (!res_branch.exists || res_branch.total_cost != 8 || res_branch.route.size() != 5) fails++;

    // Route totals must not overflow 32-bit int when several valid edges are added.
    Environment env_large_cost;
    env_large_cost.add_location(301, "Large A", false, false);
    env_large_cost.add_location(302, "Large B", false, false);
    env_large_cost.add_location(303, "Large C", false, false);
    env_large_cost.add_connection(901, 301, 302, INT_MAX, false);
    env_large_cost.add_connection(902, 302, 303, INT_MAX, false);
    PathResult res_large_cost = Pathfinder::find_shortest_path(env_large_cost, 301, 303);
    if (!res_large_cost.exists || res_large_cost.total_cost != 2LL * INT_MAX || res_large_cost.route.size() != 3) fails++;

    // Invalid endpoint IDs must be rejected without producing a route.
    PathResult res_invalid = Pathfinder::find_shortest_path(env_large_cost, 999999, 303);
    if (res_invalid.exists || !res_invalid.route.empty()) fails++;

    if (fails) std::cerr << "[FAIL] Pathfinding tests (" << fails << " failures)." << std::endl;
    else std::cout << "[PASS] Pathfinding tests." << std::endl;

    return fails > 0 ? 1 : 0;
}
