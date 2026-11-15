#pragma once

#include <string>
#include "environment.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"

// =====================================================================
// MEMBER 1 - Appendix A scenario parser and validator.
// =====================================================================

struct SimulationSettings {
    int max_steps;
    int speed;
    int battery_per_cost_unit;
    int battery_per_work_step;
    int recharge_per_step;
    std::string scenario_name;

    SimulationSettings()
        : max_steps(0), speed(0), battery_per_cost_unit(0),
          battery_per_work_step(0), recharge_per_step(0), scenario_name("") {}
};

// Reads file_path into the output containers. Returns true only if the WHOLE
// file is valid. On failure: return false and put a message naming the section
// and record in error_message; the outputs may be left partly filled.
// Events are appended in file order, each with original_order = its file position.
//
// TODO (Member 1): implement in src/scenario_parser.cpp.
bool parse_scenario(const std::string& file_path,
                    Environment& environment,
                    DynamicArray<Robot>& robots,
                    DynamicArray<RescueRequest>& requests,
                    DynamicArray<Event>& events,
                    SimulationSettings& settings,
                    std::string& error_message);
