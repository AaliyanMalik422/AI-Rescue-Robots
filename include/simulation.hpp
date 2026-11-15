#pragma once

#include <iostream>
#include <string>
#include "environment.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "event.hpp"
#include "event_queue.hpp"
#include "scenario_parser.hpp"
#include "logger.hpp"
#include "dispatch_rules.hpp"
#include "pathfinder.hpp"

// =====================================================================
// MEMBER 3 - Simulation controller and integration.
//
// ONE STEP, for current_time = 0 .. max_steps-1, in this order (design report 4.3):
//   1. apply_due_events      events with timestamp == t, in file order
//   2. release_due_requests  UNREPORTED -> PENDING when report_time <= t
//   3. recheck_routes        reroute / interrupt robots affected by events
//   4. advance_robots        move, work, complete, recharge (robots active at this point)
//   5. assign_pending_requests   new assignments start moving on the NEXT step
//   After step max_steps-1: every request not COMPLETED -> FAILED (TIME_LIMIT).
//
// Log event names to use (column 2 of the CSV log):
//   EVENT, REQUEST_REPORTED, ASSIGN, REQUEST_STATE, ROBOT_STATE, MOVE, ARRIVE, WORK,
//   COMPLETE, REROUTE, INTERRUPT, CONFLICT, REJECTED, BATTERY, CHARGE, END
//
// Movement model (decided here so the three members agree):
//   * `speed` cost units can be covered per step along the route; each unit costs
//     battery_per_cost_unit. A robot crossing a road keeps its progress between steps.
//   * Arriving at the request location sets the robot PERFORMING and the request IN_PROGRESS;
//     the first unit of work happens on the NEXT step. Same for arriving at a station:
//     RECHARGING starts on the next step.
//   * Each PERFORMING step: perform 1 unit of work and spend battery_per_work_step.
//   * Utilisation = busy_steps / online_steps, busy = steps started TRAVELLING or PERFORMING.
//   * conflicts_prevented = number of requests for which MORE THAN ONE robot was feasible
//     (only one is chosen).
// =====================================================================

struct SimulationStatistics {
    int completed_requests;
    int failed_requests;
    int unresolved_at_end;
    int assignments;
    int reassignments;
    int conflicts_prevented;
    long long total_movement_cost;
    long long total_battery_used;
    long long total_response_time;
    int maximum_response_time;
    long long route_searches;     // work counters for the performance analysis
    long long edges_relaxed;

    SimulationStatistics()
        : completed_requests(0), failed_requests(0), unresolved_at_end(0), assignments(0),
          reassignments(0), conflicts_prevented(0), total_movement_cost(0),
          total_battery_used(0), total_response_time(0), maximum_response_time(0),
          route_searches(0), edges_relaxed(0) {}
};

class SimulationController {
private:
    Environment environment;
    DynamicArray<Robot> robots;
    DynamicArray<RescueRequest> requests;
    EventQueue event_queue;
    SimulationSettings settings;
    SimulationStatistics statistics;
    Logger logger;

    int current_time;
    bool initialized;
    bool finished;

    int find_robot_index(int robot_id) const;
    int find_request_index(int request_id) const;

    // --- the five pipeline stages (apply_due_events, apply_event, recheck_routes and
    //     assign_pending_requests are TODO Member 3; release_due_requests and advance_robots are provided) ---
    void apply_due_events();
    bool apply_event(const Event& event);
    void release_due_requests();
    void recheck_routes();
    void advance_robots();
    void assign_pending_requests();

    // --- the movement and dispatch logic (TODO, Member 3; search_route, search_nearest_station
    //     and change_robot_state are provided) ---
    void update_robot(Robot& robot);
    // Moves up to `speed` units along robot.planned_route / the road it is on.
    // Returns true if the robot is now at the END of its route (not on a road).
    bool travel(Robot& robot);
    // Robot is at a location with no task: choose AVAILABLE, or RETURNING to a station,
    // or UNAVAILABLE (still on a road, or no station reachable).
    void decide_after_task(Robot& robot);
    // Request goes back to PENDING (work reset), robot released, then decide_after_task.
    void interrupt_task(Robot& robot, const std::string& reason);
    // Wrappers around Member 1's searches that also count work and log nothing.
    PathResult search_route(int from_id, int to_id);
    PathResult search_nearest_station(int from_id);
    // set_state that logs the change, or logs REJECTED when the transition is illegal.
    bool change_robot_state(Robot& robot, int new_state, const std::string& reason);
    // True if some PENDING request is compatible with the robot but the robot is feasible for none.
    bool blocked_by_battery_for_all_pending(const Robot& robot);

    void fail_unresolved_requests();
    void compute_final_statistics();
    void write_statistics(std::ostream& out) const;

public:
    SimulationController();

    // Loads and validates the scenario, fills event_queue. (Already wired to Member 1's parser.)
    bool load_scenario(const std::string& file_path);

    // DONE (provided): run exactly one time step. False if not loaded or already finished.
    bool step();

    // DONE (provided): call step() until finished.
    void run();

    // DONE (provided): print environment summary, robots, and requests grouped by state.
    void print_status() const;
    void print_final_statistics() const;

    // DONE (provided): save the log and a statistics file. False on I/O failure.
    bool write_outputs(const std::string& log_path, const std::string& stats_path) const;

    bool is_finished() const { return finished; }
    int get_current_time() const { return current_time; }
    const SimulationStatistics& get_statistics() const { return statistics; }
    const DynamicArray<RescueRequest>& get_requests() const { return requests; }
    const DynamicArray<Robot>& get_robots() const { return robots; }
    const Logger& get_logger() const { return logger; }
};
