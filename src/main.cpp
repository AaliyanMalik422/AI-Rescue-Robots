#include <iostream>
#include <string>
#include "simulation.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <scenario_file.txt>\n";
        return 1;
    }

    std::string scenario_file = argv[1];
    SimulationController simulation;

    if (!simulation.load_scenario(scenario_file)) {
        std::cerr << "Scenario was not loaded. Check the parser implementation and input file.\n";
        return 1;
    }

    simulation.run();
    return 0;
}
