#include "test_harness.hpp"
#include "simulation.hpp"

// MEMBER 3 starter tests. They FAIL until the whole team's modules are merged and
// your controller works. Add more: simultaneous requests, two robots competing
// for one request, a road blocked while a robot travels, stop at max_steps,
// stress scenario, determinism (two runs give identical logs).

static void testProvidedScenarioRuns() {
	SimulationController simulation;
	REQUIRE(simulation.loadScenario("data/scenario_provided.txt"));
	simulation.run();
	CHECK(simulation.isFinished());
	const SimulationStatistics& stats = simulation.getStatistics();
	CHECK(stats.completedRequests + stats.failedRequests == 60);
	CHECK(stats.completedRequests > 0);
	for (int i = 0; i < simulation.getRequests().size(); ++i) {
		CHECK(simulation.getRequests()[i].is_terminal());   // nothing left half-done
	}
}

static void testDeterminism() {
	SimulationController first;
	SimulationController second;
	REQUIRE(first.loadScenario("data/scenario_provided.txt"));
	REQUIRE(second.loadScenario("data/scenario_provided.txt"));
	first.run();
	second.run();
	REQUIRE(first.getLogger().size() > 0);
	CHECK(first.getLogger().size() == second.getLogger().size());
	CHECK(first.getStatistics().totalMovementCost == second.getStatistics().totalMovementCost);
	CHECK(first.getStatistics().completedRequests == second.getStatistics().completedRequests);
}

int main() {
	testProvidedScenarioRuns();
	testDeterminism();
	return finish_tests("Member 3");
}
