#pragma once
#include <string>
#include "environment.hpp"
#include "collections.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"
#include "scenario_parser.hpp"

class SimulationController {
private:
    Environment env;
    DynamicArray<Robot> robots;
    DynamicArray<RescueRequest> requests;
    DynamicArray<Event> events;
    SimulationSettings settings;
    int current_time;
    bool is_initialized;

public:
    SimulationController();
    bool load_scenario(const std::string& file_path);
    void run();
};
