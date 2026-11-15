#include "test_harness.hpp"
#include "simulation.hpp"

// MEMBER 3 starter tests. They FAIL until the whole team's modules are merged and
// your controller works. Add more: simultaneous requests, two robots competing
// for one request, a road blocked while a robot travels, stop at max_steps,
// stress scenario, determinism (two runs give identical logs).

static void test_provided_scenario_runs() {
    SimulationController simulation;
    REQUIRE(simulation.load_scenario("data/scenario_provided.txt"));
    simulation.run();
    CHECK(simulation.is_finished());
    const SimulationStatistics& stats = simulation.get_statistics();
    CHECK(stats.completed_requests + stats.failed_requests == 60);
    CHECK(stats.completed_requests > 0);
    for (int i = 0; i < simulation.get_requests().size(); ++i) {
        CHECK(simulation.get_requests()[i].is_terminal());   // nothing left half-done
    }
}

static void test_determinism() {
    SimulationController first;
    SimulationController second;
    REQUIRE(first.load_scenario("data/scenario_provided.txt"));
    REQUIRE(second.load_scenario("data/scenario_provided.txt"));
    first.run();
    second.run();
    REQUIRE(first.get_logger().size() > 0);
    CHECK(first.get_logger().size() == second.get_logger().size());
    CHECK(first.get_statistics().total_movement_cost == second.get_statistics().total_movement_cost);
    CHECK(first.get_statistics().completed_requests == second.get_statistics().completed_requests);
}

int main() {
    test_provided_scenario_runs();
    test_determinism();
    return finish_tests("Member 3");
}
