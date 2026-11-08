#pragma once

#include <string>
#include "environment.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"

struct SimulationSettings {
    int max_steps;
    int speed;
    int battery_per_cost_unit;
    int battery_per_work_step;
    int recharge_per_step;

    SimulationSettings()
        : max_steps(0), speed(0), battery_per_cost_unit(0),
          battery_per_work_step(0), recharge_per_step(0) {}
};

class ScenarioParser {
public:
    // TODO: Implement Appendix A parsing and validation without changing
    // scenario_provided.txt. See docs/design_blueprint.md for the exact duties.
    static bool parse(const std::string& file_path,
                      Environment& environment,
                      DynamicArray<Robot>& robots,
                      DynamicArray<RescueRequest>& requests,
                      DynamicArray<Event>& events,
                      SimulationSettings& settings);
};
