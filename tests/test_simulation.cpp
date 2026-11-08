#include <iostream>
#include "../include/simulation.hpp"

int run_simulation_tests() {
    int fails = 0;
    SimulationController sim;

    // Until the scenario format is implemented, loading a file must fail closed.
    if (sim.load_scenario("non_existent_file.txt")) ++fails;

    if (fails) std::cerr << "[FAIL] Simulation loading guard tests (" << fails << " failures).\n";
    else std::cout << "[PASS] Simulation loading guard tests.\n";
    std::cout << "[PENDING] Full simulation behavior tests (time loop not implemented).\n";
    return fails > 0 ? 1 : 0;
}
