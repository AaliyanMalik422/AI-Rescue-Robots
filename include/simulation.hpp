#pragma once

#include <string>
#include "environment.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"
#include "scenario_parser.hpp"

struct SimulationStatistics {
    int completed_requests;
    int failed_requests;
    int assignments;
    int reassignments;
    long long total_movement_cost;
    long long total_battery_used;
    long long total_response_time;
    int maximum_response_time;

    SimulationStatistics()
        : completed_requests(0), failed_requests(0), assignments(0), reassignments(0),
          total_movement_cost(0), total_battery_used(0), total_response_time(0),
          maximum_response_time(0) {}
};

class SimulationController {
private:
    Environment environment;
    DynamicArray<Robot> robots;
    DynamicArray<RescueRequest> requests;
    DynamicArray<Event> events;
    SimulationSettings settings;
    SimulationStatistics statistics;

    int current_time;
    int next_event_index;
    bool initialized;
    bool finished;

    int find_robot_index(int robot_id) const;
    int find_request_index(int request_id) const;
    bool process_events();
    void assign_requests();
    void update_robots();
    void update_robot(Robot& robot);
    bool should_finish() const;
    void print_final_statistics() const;

public:
    SimulationController();

    bool load_scenario(const std::string& file_path);
    bool step();
    void run();
    void print_status() const;
};
