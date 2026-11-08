#pragma once
#include <string>
#include "environment.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"

struct SimulationSettings {
    int max_time_steps;
};

class ScenarioParser {
public:
    static bool parse(const std::string& file_path, 
                      Environment& env, 
                      DynamicArray<Robot>& robots, 
                      DynamicArray<RescueRequest>& requests, 
                      DynamicArray<Event>& events, 
                      SimulationSettings& settings);
};
