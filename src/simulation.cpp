#include "simulation.hpp"
#include "logger.hpp"
#include <iostream>

SimulationController::SimulationController()
    : current_time(0), next_event_index(0), initialized(false), finished(false) {}

bool SimulationController::load_scenario(const std::string& file_path) {
    environment.clear();
    robots.clear();
    requests.clear();
    events.clear();
    settings = SimulationSettings();
    statistics = SimulationStatistics();
    current_time = 0;
    next_event_index = 0;
    initialized = false;
    finished = false;

    initialized = ScenarioParser::parse(file_path, environment, robots,
                                        requests, events, settings);
    return initialized;
}

int SimulationController::find_robot_index(int robot_id) const {
    for (int i = 0; i < robots.size(); ++i) {
        if (robots[i].id == robot_id) return i;
    }
    return -1;
}

int SimulationController::find_request_index(int request_id) const {
    for (int i = 0; i < requests.size(); ++i) {
        if (requests[i].id == request_id) return i;
    }
    return -1;
}

bool SimulationController::process_events() {
    // TODO (Member 3): Process events for current_time in original file order.
    // Return false if an event references an invalid target or cannot be applied.
    return true;
}

void SimulationController::assign_requests() {
    // TODO (Member 3):
    // - Consider only requests whose report_time <= current_time and state is PENDING.
    // - Priority: urgency descending, report_time ascending, request ID ascending.
    // - Candidate robot: IDLE, online and capability-compatible.
    // - Check outbound + full work + return-to-operational-station battery.
    // - Select lowest outbound route cost, then robot ID ascending.
}

void SimulationController::update_robots() {
    // TODO (Member 3): Update every robot exactly once per simulation time step.
    for (int i = 0; i < robots.size(); ++i) {
        update_robot(robots[i]);
    }
}

void SimulationController::update_robot(Robot& robot) {
    // TODO (Member 3): Implement route movement, road-block rerouting, rescue work,
    // return travel and charging. Use settings.speed and the battery parameters.
    (void)robot;
}

bool SimulationController::should_finish() const {
    // TODO (Member 3): Finish when all requests are terminal and all events have
    // been processed, or when settings.max_steps has been reached.
    return false;
}

bool SimulationController::step() {
    if (!initialized || finished) return false;

    // TODO (Member 3): One deterministic step:
    // process events -> assign requests -> update robots -> record results
    // -> check termination -> advance current_time.
    return false;
}

void SimulationController::run() {
    if (!initialized) {
        Logger::log("Cannot run: scenario was not loaded successfully.");
        return;
    }

    // TODO (Member 3): Call step() until it reports that no further step can run.
    Logger::log("Simulation loop is not implemented yet.");
}

void SimulationController::print_status() const {
    std::cout << "Simulation time: " << current_time << '\n';
    std::cout << "Locations: " << environment.get_location_count() << '\n';
    std::cout << "Connections: " << environment.get_connection_count() << '\n';
    std::cout << "Robots: " << robots.size() << '\n';
    std::cout << "Requests: " << requests.size() << '\n';
}

void SimulationController::print_final_statistics() const {
    std::cout << "Completed requests: " << statistics.completed_requests << '\n';
    std::cout << "Failed requests: " << statistics.failed_requests << '\n';
    std::cout << "Assignments: " << statistics.assignments << '\n';
    std::cout << "Reassignments: " << statistics.reassignments << '\n';
    std::cout << "Total movement cost: " << statistics.total_movement_cost << '\n';
    std::cout << "Total battery used: " << statistics.total_battery_used << '\n';
}
