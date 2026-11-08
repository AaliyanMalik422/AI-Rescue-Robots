#include <iostream>
#include "../include/simulation.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <scenario_file.txt>\n";
        return 1;
    }
    
    std::string scenario_file = argv[1];
    
    SimulationController sim;
    if (!sim.load_scenario(scenario_file)) {
        std::cerr << "Error: Scenario file '" << scenario_file << "' could not be loaded.\n";
        return 1;
    }
    
    sim.run();
    return 0;
}
