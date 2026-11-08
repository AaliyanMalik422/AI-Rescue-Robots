#include "../include/simulation.hpp"
#include "../include/logger.hpp"
#include <iostream>

SimulationController::SimulationController() : current_time(0), is_initialized(false) {
    settings.max_time_steps = 0;
}

bool SimulationController::load_scenario(const std::string& file_path) {
    if (ScenarioParser::parse(file_path, env, robots, requests, events, settings)) {
        is_initialized = true;
        Logger::log("Scenario successfully loaded: " + file_path);
        return true;
    }
    is_initialized = false;
    Logger::log("Failed to load scenario: " + file_path + " (parser pending scenario format).");
    return false;
}

void SimulationController::run() {
    if (!is_initialized) {
        Logger::log("Cannot run simulation: scenario loading failed or missing.");
        return;
    }
    Logger::log("Simulation execution is pending actual scenario format implementation.");
}
