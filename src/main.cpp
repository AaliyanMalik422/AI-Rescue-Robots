#include <iostream>
#include <string>
#include "simulation.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 4) {
		cerr << "Usage: " << argv[0]
			<< " <scenario_file.txt> [log_file] [stats_file]\n";
		return 1;
	}

	string scenarioFile = argv[1];
	string logFile = argc >= 3 ? argv[2] : "output/simulation_log.csv";
	string statsFile = argc >= 4 ? argv[3] : "output/simulation_statistics.txt";

	SimulationController simulation;

	if (!simulation.loadScenario(scenarioFile)) {
		return 1;
	}

	simulation.run();

	if (!simulation.writeOutputs(logFile, statsFile)) {
		cerr << "Could not write simulation output files.\n";
		return 1;
	}

	simulation.printFinalStatistics();

	return 0;
}