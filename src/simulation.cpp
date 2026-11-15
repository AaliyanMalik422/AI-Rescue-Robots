#include "simulation.hpp"
#include "pathfinder.hpp"
#include "names.hpp"
#include <fstream>
#include <iostream>

// MEMBER 3: everything below except the constructor, load_scenario and the two lookup
// helpers is yours. Read design report section 4 and the header comment in simulation.hpp.
// Notation: pc = settings.battery_per_cost_unit, pw = settings.battery_per_work_step.

SimulationController::SimulationController()
    : current_time(0), initialized(false), finished(false) {}

bool SimulationController::load_scenario(const std::string& file_path) {
    environment.clear();
    robots.clear();
    requests.clear();
    event_queue.clear();
    logger.clear();
    settings = SimulationSettings();
    statistics = SimulationStatistics();
    current_time = 0;
    initialized = false;
    finished = false;

    DynamicArray<Event> parsed_events;
    std::string error_message;
    if (!parse_scenario(file_path, environment, robots, requests,
                        parsed_events, settings, error_message)) {
        std::cerr << "Invalid scenario: " << error_message << '\n';
        return false;
    }

    for (int i = 0; i < parsed_events.size(); ++i) {
        if (!event_queue.push(parsed_events[i])) {
            std::cerr << "Could not store events.\n";
            return false;
        }
    }
    initialized = true;
    return true;
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

// ---------------------------------------------------------------------
// STEP and RUN
// ---------------------------------------------------------------------

bool SimulationController::step() {
    // DONE (provided): the order of the five stages is the design decision (see simulation.hpp).
    if (!initialized || finished) return false;

    apply_due_events();
    release_due_requests();
    recheck_routes();
    advance_robots();
    assign_pending_requests();

    current_time = current_time + 1;
    if (current_time >= settings.max_steps) {
        finished = true;
        fail_unresolved_requests();
        compute_final_statistics();
        logger.log(current_time, "END", "", "simulation finished");
    }
    return true;
}

void SimulationController::run() {
    // DONE (provided)
    if (!initialized) {
        std::cerr << "Cannot run: scenario was not loaded.\n";
        return;
    }
    while (step()) {
    }
}

// ---------------------------------------------------------------------
// STAGE 1: events
// ---------------------------------------------------------------------

void SimulationController::apply_due_events() {
    // TODO (Member 3)
    // PSEUDOCODE: loop:
    //     if event_queue.peek(next) fails or next.timestamp != current_time: stop
    //     event_queue.pop(next)
    //     ok = apply_event(next)
    //     log(current_time, ok ? "EVENT" : "REJECTED", "target=<id>", event_type_name(type))
    // (EventQueue returns equal-timestamp events in file order, which is the required rule.)
}

bool SimulationController::apply_event(const Event& event) {
    // TODO (Member 3)
    // PSEUDOCODE by event.type (return false when the target does not exist / is invalid):
    //   BLOCK, UNBLOCK : return environment.set_connection_blocked(target, type == BLOCK)
    //                    (the blocking is NOT acted on here; recheck_routes does that in stage 3)
    //   STATION_DOWN/UP: return environment.set_station_available(target, type == STATION_UP)
    //   SET_URGENCY    : i = find_request_index(target); if i == -1 return false
    //                    return requests[i].set_urgency(event.value)
    //   ROBOT_OFFLINE  : i = find_robot_index(target); if i == -1 or robot already OFFLINE: return false
    //                    request_id = robots[i].go_offline()
    //                    if request_id != -1: j = find_request_index(request_id);
    //                        requests[j].return_to_pending();  statistics.reassignments++
    //                    log ROBOT_STATE (old -> OFFLINE) and REQUEST_STATE if it changed
    //   ROBOT_ONLINE   : i = find_robot_index(target); if i == -1 or robot is not OFFLINE: return false
    //                    robot.go_online()                     -> state AVAILABLE
    //                    if robot.is_on_road(): change_robot_state(UNAVAILABLE)
    //                         (update_robot keeps crossing the road once it is open)
    //                    else if robot is at an operational station and battery < max_battery:
    //                         change_robot_state(RECHARGING)
    //                    else decide_after_task(robot)
    (void)event;
    return false;
}

// ---------------------------------------------------------------------
// STAGE 2: requests
// ---------------------------------------------------------------------

void SimulationController::release_due_requests() {
    // DONE (provided)
    for (int i = 0; i < requests.size(); ++i) {
        RescueRequest& request = requests[i];
        if (request.state == REQUEST_UNREPORTED && request.report_time <= current_time) {
            if (request.report()) {
                logger.log(current_time, "REQUEST_REPORTED", "request=" + int_to_string(request.id),
                           std::string("location=") + int_to_string(request.location_id) +
                           ";category=" + category_name(request.category) +
                           ";urgency=" + int_to_string(request.urgency));
            }
        }
    }
}

// ---------------------------------------------------------------------
// STAGE 3: routes after changes
// ---------------------------------------------------------------------

void SimulationController::recheck_routes() {
    // TODO (Member 3)
    // PSEUDOCODE: station_costs = empty; computed = false
    //   for each robot that is not OFFLINE and not on a road:
    //     case TRAVELLING (has a task):
    //         if route_is_open(planned_route, next_route_index - 1):  route still fine
    //         else: r = search_route(robot.current_location_id, robot.mission_target_id)
    //               if not r.exists: interrupt_task(robot, "no route"); continue
    //               robot.set_route(r.route, r.connection_ids); log REROUTE
    //         then the battery reserve check (below)
    //     case PERFORMING: battery reserve check (below)
    //     case RETURNING:
    //         if the station robot.mission_target_id is still operational and the route is open: fine
    //         else r = search_nearest_station(current location)
    //              if r.exists: robot.set_route(...); log REROUTE
    //              else: robot.clear_route(); change_robot_state(UNAVAILABLE)
    //     case RECHARGING:
    //         if the station at its location is no longer operational:
    //              r = search_nearest_station(current location) -> RETURNING with that route,
    //              or UNAVAILABLE when no station is reachable
    //   Battery reserve check (TRAVELLING with a route, PERFORMING):
    //         compute station_costs once per call (compute_station_costs, count edges_relaxed)
    //         remaining_cost = cost of the rest of the route (0 when PERFORMING)
    //         need = mission_battery_required(remaining_cost, request.remaining_work,
    //                                         station_costs[index of the request location], pc, pw)
    //         if need == -1 or robot.battery_level < need: interrupt_task(robot, "battery reserve")
    //   Robots mid-road are skipped: they finish the road, and are checked at the next location.
}

// ---------------------------------------------------------------------
// STAGE 4: movement, work, charging
// ---------------------------------------------------------------------

void SimulationController::advance_robots() {
    // DONE (provided)
    for (int i = 0; i < robots.size(); ++i) {
        Robot& robot = robots[i];
        if (robot.state == ROBOT_OFFLINE) continue;
        robot.online_steps += 1;
        if (robot.state == ROBOT_TRAVELLING || robot.state == ROBOT_PERFORMING) robot.busy_steps += 1;
        update_robot(robot);
    }
}

void SimulationController::update_robot(Robot& robot) {
    // TODO (Member 3)
    // PSEUDOCODE by robot.state:
    //   TRAVELLING:
    //       if travel(robot):                                  (reached the request location)
    //           request.start_work()  (ASSIGNED -> IN_PROGRESS); change_robot_state(PERFORMING)
    //           log ARRIVE
    //   PERFORMING:
    //       if not robot.consume_battery(pw): interrupt_task(robot, "battery"); return
    //       request.perform_work(1); log WORK
    //       if request.remaining_work == 0:
    //           request.complete(current_time); statistics.completed_requests++ (or compute later)
    //           log COMPLETE; robot.release_request(); decide_after_task(robot)
    //   RETURNING:
    //       if travel(robot): change_robot_state(RECHARGING); log ARRIVE      (charging starts next step)
    //   RECHARGING:
    //       robot.charge(settings.recharge_per_step); log CHARGE
    //       if robot.battery_level == robot.max_battery: change_robot_state(AVAILABLE)
    //   UNAVAILABLE:
    //       if robot.is_on_road(): if travel(robot) finished the road: decide_after_task(robot)
    //       else: decide_after_task(robot)     (retry every step: a road or station may have changed)
    //   AVAILABLE: nothing to do
    (void)robot;
}

bool SimulationController::travel(Robot& robot) {
    // TODO (Member 3)
    // PSEUDOCODE:
    //   budget = settings.speed
    //   while budget > 0:
    //       if robot is NOT on a road:
    //           if next_route_index >= planned_route.size(): break               (end of route)
    //           id = planned_connections[next_route_index - 1]; conn = environment.get_connection(id)
    //           if conn is blocked: break                                        (wait; stage 3 reroutes)
    //           robot.current_connection_id = id; distance_on_current_connection = 0
    //           robot.road_target_id = planned_route[next_route_index]
    //       conn = environment.get_connection(robot.current_connection_id)
    //       if conn is blocked: break                       (paused on the road until it reopens)
    //       units = min(budget, conn->distance - distance_on_current_connection)
    //       if not robot.consume_battery(units * pc): log REJECTED "no battery to move"; break
    //       distance_on_current_connection += units; budget -= units
    //       statistics.total_movement_cost += units
    //       if distance_on_current_connection == conn->distance:             (finished the road)
    //           robot.current_location_id = robot.road_target_id
    //           clear current_connection_id, road_target_id, progress
    //           if robot has a route: next_route_index += 1
    //   log MOVE (location or road progress, battery) if the robot moved
    //   return (not on a road) and (planned_route is empty or next_route_index >= planned_route.size())
    //   NOTE: a robot with an EMPTY route that is not on a road (e.g. already at the station)
    //         counts as "at the end of its route".
    (void)robot;
    return false;
}

void SimulationController::decide_after_task(Robot& robot) {
    // TODO (Member 3)
    // PSEUDOCODE:
    //   if robot.is_on_road(): change_robot_state(UNAVAILABLE); return
    //   need_charge = battery_is_low(robot, 30) or blocked_by_battery_for_all_pending(robot)
    //   if not need_charge: change_robot_state(AVAILABLE); return
    //   r = search_nearest_station(robot.current_location_id)
    //   if not r.exists: change_robot_state(UNAVAILABLE); return
    //   robot.set_route(r.route, r.connection_ids); robot.return_station_id = last stop of the route
    //   change_robot_state(RETURNING)       (a zero-length route means: already at a station)
    (void)robot;
}

void SimulationController::interrupt_task(Robot& robot, const std::string& reason) {
    // TODO (Member 3)
    // PSEUDOCODE: i = find_request_index(robot.assigned_request_id)
    //   if i != -1: requests[i].return_to_pending()  (resets work to full duration)
    //               statistics.reassignments++; log INTERRUPT with reason and REQUEST_STATE
    //   robot.release_request()
    //   decide_after_task(robot)
    (void)robot; (void)reason;
}

PathResult SimulationController::search_route(int from_id, int to_id) {
    // DONE (provided): counts the work for the performance analysis.
    PathResult result = find_shortest_path(environment, from_id, to_id);
    statistics.route_searches += 1;
    statistics.edges_relaxed += result.edges_relaxed;
    return result;
}

PathResult SimulationController::search_nearest_station(int from_id) {
    // DONE (provided)
    PathResult result = find_nearest_station(environment, from_id);
    statistics.route_searches += 1;
    statistics.edges_relaxed += result.edges_relaxed;
    return result;
}

bool SimulationController::change_robot_state(Robot& robot, int new_state,
                                              const std::string& reason) {
    // DONE (provided): the only place robot states change, so every change is logged.
    int old_state = robot.state;
    std::string ids = "robot=" + int_to_string(robot.id);
    if (robot.set_state(new_state)) {
        logger.log(current_time, "ROBOT_STATE", ids,
                   std::string(robot_state_name(old_state)) + "->" + robot_state_name(new_state) +
                   ";" + reason);
        return true;
    }
    logger.log(current_time, "REJECTED", ids,
               std::string("illegal transition ") + robot_state_name(old_state) + "->" +
               robot_state_name(new_state));
    return false;
}

bool SimulationController::blocked_by_battery_for_all_pending(const Robot& robot) {
    // TODO (Member 3)
    // PSEUDOCODE: compatible = 0; feasible = 0; station_costs = compute_station_costs(...)
    //   for each PENDING request whose category the robot can serve (robot.check_capability):
    //       compatible++; r = search_route(robot location, request location)
    //       if r.exists and mission_battery_required(r.total_cost, duration, station cost of the
    //             request location, pc, pw) in 0..robot.battery_level: feasible++
    //   return compatible > 0 and feasible == 0
    (void)robot;
    return false;
}

// ---------------------------------------------------------------------
// STAGE 5: dispatch
// ---------------------------------------------------------------------

void SimulationController::assign_pending_requests() {
    // TODO (Member 3)
    // PSEUDOCODE:
    //   if no robot is AVAILABLE: return                       (cheap exit: most steps)
    //   indices = indices of requests with state PENDING
    //   sort_request_indices(requests, indices)                (Member 2: priority order)
    //   compute_station_costs(environment, station_costs) once; add its return value to edges_relaxed
    //   for each request index q in `indices` (highest priority first):
    //       best_robot = -1; best_cost = none; feasible_count = 0
    //       for each robot that is AVAILABLE and check_capability(request.category):
    //           r = search_route(robot.current_location_id, request.location_id)
    //           if not r.exists: continue
    //           need = mission_battery_required(r.total_cost, request.duration_steps,
    //                    station_costs[index of request.location_id], pc, pw)
    //           if need == -1 or robot.battery_level < need: continue
    //           feasible_count++
    //           if best_robot == -1 or r.total_cost < best_cost
    //                  or (r.total_cost == best_cost and robot.id < best robot's id):
    //               remember this robot and its route
    //       if feasible_count > 1: statistics.conflicts_prevented++; log CONFLICT
    //       if best_robot != -1:
    //           request.assign(robot.id, current_time)  AND  robot.assign_request(request.id)
    //           (do both or neither: if one fails, undo the other and log REJECTED)
    //           robot.set_route(route, connection_ids); statistics.assignments++
    //           log ASSIGN with robot, request, route cost, battery   (robot starts moving next step)
    //       if no robot is AVAILABLE any more: stop
    //   Requests nobody can serve simply stay PENDING (e.g. a cut-off location).
}

// ---------------------------------------------------------------------
// End of run and reporting
// ---------------------------------------------------------------------

void SimulationController::fail_unresolved_requests() {
    // DONE (provided)
    for (int i = 0; i < requests.size(); ++i) {
        if (requests[i].state != REQUEST_COMPLETED && requests[i].fail(FAIL_TIME_LIMIT)) {
            logger.log(current_time, "REQUEST_STATE", "request=" + int_to_string(requests[i].id),
                       "->FAILED;TIME_LIMIT");
        }
    }
}

void SimulationController::compute_final_statistics() {
    // DONE (provided). Movement cost, assignments, reassignments, conflicts and search counters
    // were counted while running; this fills in the rest from the final records.
    statistics.completed_requests = 0;
    statistics.failed_requests = 0;
    statistics.unresolved_at_end = 0;
    statistics.total_response_time = 0;
    statistics.maximum_response_time = 0;
    for (int i = 0; i < requests.size(); ++i) {
        const RescueRequest& request = requests[i];
        if (request.state == REQUEST_COMPLETED) {
            int response = request.completion_time - request.report_time;
            statistics.completed_requests += 1;
            statistics.total_response_time += response;
            if (response > statistics.maximum_response_time) statistics.maximum_response_time = response;
        } else if (request.state == REQUEST_FAILED) {
            statistics.failed_requests += 1;
            if (request.fail_reason == FAIL_TIME_LIMIT) statistics.unresolved_at_end += 1;
        }
    }
    statistics.total_battery_used = 0;
    for (int i = 0; i < robots.size(); ++i) statistics.total_battery_used += robots[i].battery_used;
}

void SimulationController::write_statistics(std::ostream& out) const {
    // DONE (provided): every metric listed in spec section 8.
    out << "scenario: " << settings.scenario_name << '\n';
    out << "steps_run: " << current_time << '\n';
    out << "requests_total: " << requests.size() << '\n';
    out << "requests_completed: " << statistics.completed_requests << '\n';
    out << "requests_failed: " << statistics.failed_requests << '\n';
    out << "requests_unresolved_at_end: " << statistics.unresolved_at_end << '\n';
    if (statistics.completed_requests > 0) {
        out << "average_response_time: "
            << (double)statistics.total_response_time / statistics.completed_requests << '\n';
    } else {
        out << "average_response_time: n/a\n";
    }
    out << "maximum_response_time: " << statistics.maximum_response_time << '\n';
    out << "total_movement_cost: " << statistics.total_movement_cost << '\n';
    out << "total_battery_used: " << statistics.total_battery_used << '\n';
    out << "assignments: " << statistics.assignments << '\n';
    out << "reassignments: " << statistics.reassignments << '\n';
    out << "conflicts_prevented: " << statistics.conflicts_prevented << '\n';
    out << "route_searches: " << statistics.route_searches << '\n';
    out << "edges_relaxed: " << statistics.edges_relaxed << '\n';
    for (int i = 0; i < robots.size(); ++i) {
        const Robot& robot = robots[i];
        out << "robot " << robot.id << ": busy_steps=" << robot.busy_steps
            << " online_steps=" << robot.online_steps << " utilisation_percent=";
        if (robot.online_steps > 0) out << 100.0 * robot.busy_steps / robot.online_steps;
        else out << "n/a";
        out << " battery_used=" << robot.battery_used << '\n';
    }
}

void SimulationController::print_status() const {
    // DONE (provided)
    std::cout << "Simulation time: " << current_time << '\n';
    std::cout << "Locations: " << environment.get_location_count()
              << "  Connections: " << environment.get_connection_count() << '\n';
    for (int i = 0; i < robots.size(); ++i) {
        std::cout << "Robot " << robots[i].id << " " << robot_state_name(robots[i].state)
                  << " at " << robots[i].current_location_id
                  << " battery " << robots[i].battery_level << "/" << robots[i].max_battery << '\n';
    }
    for (int i = 0; i < requests.size(); ++i) {
        std::cout << "Request " << requests[i].id << " " << request_state_name(requests[i].state)
                  << " " << category_name(requests[i].category)
                  << " urgency " << requests[i].urgency << '\n';
    }
}

void SimulationController::print_final_statistics() const {
    // DONE (provided)
    write_statistics(std::cout);
}

bool SimulationController::write_outputs(const std::string& log_path,
                                         const std::string& stats_path) const {
    // DONE (provided)
    bool log_ok = logger.save_to_file(log_path);
    std::ofstream out(stats_path.c_str());
    if (!out.is_open()) return false;
    write_statistics(out);
    out.close();
    return log_ok && !out.fail();
}
