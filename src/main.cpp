#include <iostream>
#include <string>
#include "simulation.hpp"

// MEMBER 3: command-line entry point.
// Usage: ./ai_rescue_robots <scenario_file> [log_file] [stats_file]
// Performance is reported with work counters (route searches, edges relaxed,
// steps simulated) instead of a clock, so no timing header is needed.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <scenario_file.txt> [log_file] [stats_file]\n";
        return 1;
    }

    std::string scenario_file = argv[1];
    std::string log_file = argc > 2 ? argv[2] : "output/simulation_log.txt";
    std::string stats_file = argc > 3 ? argv[3] : "output/simulation_statistics.txt";

    SimulationController simulation;
    if (!simulation.load_scenario(scenario_file)) {
        return 1;
    }

    simulation.run();
    simulation.print_final_statistics();

    if (!simulation.write_outputs(log_file, stats_file)) {
        std::cerr << "Could not write output files.\n";
        return 1;
    }
    return 0;
}
