#include "simulation.hpp"
#include "pathfinder.hpp"
#include "names.hpp"
#include <fstream>
#include <iostream>

// MEMBER 3: everything below except the constructor, loadScenario and the two lookup
// helpers is yours. Read design report section 4 and the header comment in simulation.hpp.
// Notation: pc = settings.battery_per_cost_unit, pw = settings.battery_per_work_step.

SimulationController::SimulationController()
	: currentTime(0), initialized(false), finished(false) {}

bool SimulationController::loadScenario(const string& filePath) {
	environment.clear();
	robots.clear();
	requests.clear();
	eventQueue.clear();
	logger.clear();
	settings = SimulationSettings();
	statistics = SimulationStatistics();
	currentTime = 0;
	initialized = false;
	finished = false;

	DynamicArray<Event> parsedEvents;
	string errorMessage;
	if (!parse_scenario(filePath, environment, robots, requests,
						parsedEvents, settings, errorMessage)) {
		cerr << "Invalid scenario: " << errorMessage << '\n';
		return false;
	}

	for (int i = 0; i < parsedEvents.size(); ++i) {
		if (!eventQueue.push(parsedEvents[i])) {
			cerr << "Could not store events.\n";
			return false;
		}
	}
	initialized = true;
	return true;
}

int SimulationController::findRobotIndex(int robotId) const {
	for (int i = 0; i < robots.size(); ++i) {
		if (robots[i].id == robotId) return i;
	}
	return -1;
}

int SimulationController::findRequestIndex(int requestId) const {
	for (int i = 0; i < requests.size(); ++i) {
		if (requests[i].id == requestId) return i;
	}
	return -1;
}

// ---------------------------------------------------------------------
// STEP and RUN
// ---------------------------------------------------------------------

bool SimulationController::step() {
	// DONE (provided): the order of the five stages is the design decision (see simulation.hpp).
	if (!initialized || finished) return false;

	applyDueEvents();
	releaseDueRequests();
	recheckRoutes();
	advanceRobots();
	assignPendingRequests();

	currentTime = currentTime + 1;
	if (currentTime >= settings.max_steps) {
		finished = true;
		failUnresolvedRequests();
		computeFinalStatistics();
		logger.log(currentTime, "END", "", "simulation finished");
	}
	return true;
}

void SimulationController::run() {
	// DONE (provided)
	if (!initialized) {
		cerr << "Cannot run: scenario was not loaded.\n";
		return;
	}
	while (step()) {
	}
}

// ---------------------------------------------------------------------
// STAGE 1: events
// ---------------------------------------------------------------------

void SimulationController::applyDueEvents() {
	Event next;

	// Process all events whose scheduled time has arrived.
	// EventQueue maintains timestamp order and file order for ties.
	while (eventQueue.peek(next)) {

		// The next event is scheduled for a future time.
		if (next.timestamp > currentTime) {
			break;
		}

		// Remove the next due event from the queue.
		if (!eventQueue.pop(next)) {
			break;
		}

		// Apply the event.
		bool applied = applyEvent(next);

		// Prepare the event description for logging.
		string detail = event_type_name(next.type);

		if (next.type == EVENT_SET_URGENCY) {
			detail += ";urgency=";
			detail += int_to_string(next.value);
		}

		// Record whether the event was successful.
		logger.log(
			currentTime,
			applied ? "EVENT" : "REJECTED",
			"target=" + int_to_string(next.target_id),
			detail
		);
	}
}

bool SimulationController::applyEvent(const Event& event) {

	// BLOCK: Block a road connection.
	if (event.type == EVENT_BLOCK) {
		return environment.set_connection_blocked(event.target_id, true);
	}

	// UNBLOCK: Reopen a road connection.
	if (event.type == EVENT_UNBLOCK) {
		return environment.set_connection_blocked(event.target_id, false);
	}

	// STATION_DOWN: Make a charging station unavailable.
	if (event.type == EVENT_STATION_DOWN) {
		return environment.set_station_available(event.target_id, false);
	}

	// STATION_UP: Make a charging station available.
	if (event.type == EVENT_STATION_UP) {
		return environment.set_station_available(event.target_id, true);
	}

	// SET_URGENCY: Change the urgency of a rescue request.
	if (event.type == EVENT_SET_URGENCY) {

		int index = findRequestIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		return requests[index].set_urgency(event.value);
	}

	// ROBOT_OFFLINE: Take a robot out of service.
	if (event.type == EVENT_ROBOT_OFFLINE) {

		int index = findRobotIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		Robot& robot = robots[index];

		// A robot already offline cannot be taken offline again.
		if (robot.state == ROBOT_OFFLINE) {
			return false;
		}

		int requestId = robot.assigned_request_id;
		int requestIndex = -1;
		int oldRequestState = -1;

		// Validate the robot's current request before changing anything.
		if (requestId != -1) {

			requestIndex = findRequestIndex(requestId);

			if (requestIndex == -1) {
				return false;
			}

			RescueRequest& request = requests[requestIndex];

			// Confirm that this request actually belongs to this robot.
			if (request.assigned_robot_id != robot.id) {
				return false;
			}

			// Only an assigned or in-progress request can be interrupted.
			if (request.state != REQUEST_ASSIGNED &&
				request.state != REQUEST_IN_PROGRESS) {
				return false;
			}

			oldRequestState = request.state;
		}

		int oldRobotState = robot.state;

		// Take the robot offline.
		// go_offline() clears its assignment and route but preserves
		// its physical location and any progress on its current road.
		int releasedRequestId = robot.go_offline();

		if (releasedRequestId != requestId ||
			robot.state != ROBOT_OFFLINE) {
			return false;
		}

		// Return its interrupted request to PENDING.
		if (requestIndex != -1) {

			if (!requests[requestIndex].return_to_pending()) {
				return false;
			}

			statistics.reassignments++;

			logger.log(
				currentTime,
				"REQUEST_STATE",
				"request=" + int_to_string(requestId),
				string(request_state_name(oldRequestState)) +
				"->PENDING;ROBOT_OFFLINE"
			);
		}

		// Record the robot's state transition.
		logger.log(
			currentTime,
			"ROBOT_STATE",
			"robot=" + int_to_string(robot.id),
			string(robot_state_name(oldRobotState)) +
			"->OFFLINE;ROBOT_OFFLINE"
		);

		return true;
	}

	// ROBOT_ONLINE: Bring an offline robot back into service.
	if (event.type == EVENT_ROBOT_ONLINE) {

		int index = findRobotIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		Robot& robot = robots[index];

		// Only an offline robot can be brought online.
		if (robot.state != ROBOT_OFFLINE) {
			return false;
		}

		if (!robot.go_online()) {
			return false;
		}

		// go_online() changes OFFLINE directly to AVAILABLE.
		logger.log(
			currentTime,
			"ROBOT_STATE",
			"robot=" + int_to_string(robot.id),
			"OFFLINE->AVAILABLE;ROBOT_ONLINE"
		);

		// If the robot is still crossing a road, it must finish
		// crossing before receiving another task.
		if (robot.is_on_road()) {
			return changeRobotState(
				robot,
				ROBOT_UNAVAILABLE,
				"online;finish current road"
			);
		}

		// If it is at an operational charging station and its battery
		// is not full, it should start recharging.
		if (environment.is_operational_station(robot.current_location_id) &&
			robot.battery_level < robot.max_battery) {

			return changeRobotState(
				robot,
				ROBOT_RECHARGING,
				"online at charging station"
			);
		}

		// Otherwise, decide whether it should become available
		// or return to a charging station.
		decideAfterTask(robot);

		return true;
	}

	// Unknown event type.
	return false;
}

// ---------------------------------------------------------------------
// STAGE 2: requests
// ---------------------------------------------------------------------

void SimulationController::releaseDueRequests() {
	// DONE (provided)
	for (int i = 0; i < requests.size(); ++i) {
		RescueRequest& request = requests[i];
		if (request.state == REQUEST_UNREPORTED && request.report_time <= currentTime) {
			if (request.report()) {
				logger.log(currentTime, "REQUEST_REPORTED", "request=" + int_to_string(request.id),
					string("location=") + int_to_string(request.location_id) +
						";category=" + category_name(request.category) +
							";urgency=" + int_to_string(request.urgency));
			}
		}
	}
}

// ---------------------------------------------------------------------
// STAGE 3: routes after changes
// ---------------------------------------------------------------------

void SimulationController::recheckRoutes() {
	DynamicArray<long long> stationCosts;
	bool stationCostsComputed = false;

	// Check every robot against the updated environment.
	for (int i = 0; i < robots.size(); ++i) {
		Robot& robot = robots[i];

		// Offline robots and robots crossing roads are checked later.
		if (robot.state == ROBOT_OFFLINE || robot.is_on_road()) {
			continue;
		}

		// ---------------------------------------------------------
		// CASE 1: ROBOT_TRAVELLING
		// ---------------------------------------------------------
		if (robot.state == ROBOT_TRAVELLING) {

			// Check whether the remaining route is still open.
			if (!route_is_open(
				environment,
				robot.planned_route,
				robot.next_route_index - 1
			)) {
				// Find an alternative route to the rescue location.
				PathResult replacement = searchRoute(
					robot.current_location_id,
					robot.mission_target_id
				);

				// No alternative route exists.
				if (!replacement.exists) {
					interruptTask(robot, "no route");
					continue;
				}

				// Store the replacement route.
				if (!robot.set_route(
					replacement.route,
					replacement.connection_ids
				)) {
					interruptTask(robot, "reroute failed");
					continue;
				}

				// Record the new route.
				logger.log(
					currentTime,
					"REROUTE",
					"robot=" + int_to_string(robot.id),
					"target=" + int_to_string(robot.mission_target_id) +
					";cost=" + int_to_string(replacement.total_cost)
				);
			}
		}

		// ---------------------------------------------------------
		// CASE 2: ROBOT_RETURNING
		// ---------------------------------------------------------
		else if (robot.state == ROBOT_RETURNING) {

			bool stationAvailable =
				environment.is_operational_station(robot.mission_target_id);

			bool routeOpen = route_is_open(
				environment,
				robot.planned_route,
				robot.next_route_index - 1
			);

			// Existing destination and route are still valid.
			if (stationAvailable && routeOpen) {
				continue;
			}

			// Find another operational charging station.
			PathResult replacement = searchNearestStation(
				robot.current_location_id
			);

			if (!replacement.exists || replacement.route.size() == 0) {
				robot.clear_route();

				changeRobotState(
					robot,
					ROBOT_UNAVAILABLE,
					"no operational station reachable"
				);
				continue;
			}

			if (!robot.set_route(
				replacement.route,
				replacement.connection_ids
			)) {
				robot.clear_route();

				changeRobotState(
					robot,
					ROBOT_UNAVAILABLE,
					"station reroute failed"
				);
				continue;
			}

			robot.return_station_id =
				replacement.route[replacement.route.size() - 1];

			logger.log(
				currentTime,
				"REROUTE",
				"robot=" + int_to_string(robot.id),
				"station=" + int_to_string(robot.return_station_id) +
				";cost=" + int_to_string(replacement.total_cost)
			);

			continue;
		}

		// ---------------------------------------------------------
		// CASE 3: ROBOT_RECHARGING
		// ---------------------------------------------------------
		else if (robot.state == ROBOT_RECHARGING) {

			// Its current charging station is still operational.
			if (environment.is_operational_station(
				robot.current_location_id
			)) {
				continue;
			}

			// The current station went down. Find another one.
			PathResult replacement = searchNearestStation(
				robot.current_location_id
			);

			if (!replacement.exists || replacement.route.size() == 0) {
				robot.clear_route();

				changeRobotState(
					robot,
					ROBOT_UNAVAILABLE,
					"charging station unavailable"
				);
				continue;
			}

			if (!robot.set_route(
				replacement.route,
				replacement.connection_ids
			)) {
				robot.clear_route();

				changeRobotState(
					robot,
					ROBOT_UNAVAILABLE,
					"station reroute failed"
				);
				continue;
			}

			robot.return_station_id =
				replacement.route[replacement.route.size() - 1];

			changeRobotState(
				robot,
				ROBOT_RETURNING,
				"current station unavailable"
			);

			logger.log(
				currentTime,
				"REROUTE",
				"robot=" + int_to_string(robot.id),
				"station=" + int_to_string(robot.return_station_id) +
				";cost=" + int_to_string(replacement.total_cost)
			);

			continue;
		}

		// Only travelling and performing robots need mission reserves.
		else if (robot.state != ROBOT_PERFORMING) {
			continue;
		}

		// ---------------------------------------------------------
		// CASE 4: BATTERY RESERVE CHECK
		// Applies to TRAVELLING and PERFORMING robots.
		// ---------------------------------------------------------

		int requestIndex = findRequestIndex(robot.assigned_request_id);

		if (requestIndex == -1) {
			interruptTask(robot, "missing assigned request");
			continue;
		}

		// Calculate the cost of reaching the nearest operational
		// charging station only once during this function call.
		if (!stationCostsComputed) {
			statistics.edgesRelaxed +=
				compute_station_costs(environment, stationCosts);

			stationCostsComputed = true;
		}

		RescueRequest& request = requests[requestIndex];

		int requestLocationIndex =
			environment.get_location_index(request.location_id);

		// Validate the station-cost lookup index.
		if (requestLocationIndex < 0 ||
			requestLocationIndex >= stationCosts.size()) {

			interruptTask(robot, "battery reserve unavailable");
			continue;
		}

		// PERFORMING robots have already reached their destination.
		long long remainingCost = 0;

		// TRAVELLING robots still need to reach the rescue location.
		if (robot.state == ROBOT_TRAVELLING) {

			int firstConnectionIndex = robot.next_route_index - 1;

			if (firstConnectionIndex < 0 ||
				firstConnectionIndex > robot.planned_connections.size()) {

				interruptTask(robot, "invalid remaining route");
				continue;
			}

			// Add the distance of every remaining road.
			for (int j = firstConnectionIndex;
				j < robot.planned_connections.size(); ++j) {

				const Connection* connection =
					environment.get_connection(robot.planned_connections[j]);

				if (connection == 0) {
					remainingCost = -1;
					break;
				}

				remainingCost += connection->distance;
			}

			if (remainingCost < 0) {
				interruptTask(robot, "invalid remaining route");
				continue;
			}
		}

		// Calculate battery required for the remaining mission,
		// including the return to an operational charging station.
		long long requiredBattery = mission_battery_required(
			remainingCost,
			request.remaining_work,
			stationCosts[requestLocationIndex],
			settings.battery_per_cost_unit,
			settings.battery_per_work_step
		);

		// Interrupt if no return station is reachable or
		// the robot cannot afford the remaining mission.
		if (requiredBattery == -1 ||
			robot.battery_level < requiredBattery) {

			interruptTask(robot, "battery reserve");
			continue;
		}
	}
}

// ---------------------------------------------------------------------
// STAGE 4: movement, work, charging
// ---------------------------------------------------------------------

void SimulationController::advanceRobots() {
	// DONE (provided)
	for (int i = 0; i < robots.size(); ++i) {
		Robot& robot = robots[i];
		if (robot.state == ROBOT_OFFLINE) continue;
		robot.online_steps += 1;
		if (robot.state == ROBOT_TRAVELLING || robot.state == ROBOT_PERFORMING) robot.busy_steps += 1;
		updateRobot(robot);
	}
}

void SimulationController::updateRobot(Robot& robot) {
	// TODO (Member 3)
	// PSEUDOCODE by robot.state:
	//   TRAVELLING:
	//       if travel(robot):                                  (reached the request location)
	//           request.start_work()  (ASSIGNED -> IN_PROGRESS); changeRobotState(PERFORMING)
	//           log ARRIVE
	//   PERFORMING:
	//       if not robot.consume_battery(pw): interruptTask(robot, "battery"); return
	//       request.perform_work(1); log WORK
	//       if request.remaining_work == 0:
	//           request.complete(currentTime); statistics.completedRequests++ (or compute later)
	//           log COMPLETE; robot.release_request(); decideAfterTask(robot)
	//   RETURNING:
	//       if travel(robot): changeRobotState(RECHARGING); log ARRIVE      (charging starts next step)
	//   RECHARGING:
	//       robot.charge(settings.recharge_per_step); log CHARGE
	//       if robot.battery_level == robot.max_battery: changeRobotState(AVAILABLE)
	//   UNAVAILABLE:
	//       if robot.is_on_road(): if travel(robot) finished the road: decideAfterTask(robot)
	//       else: decideAfterTask(robot)     (retry every step: a road or station may have changed)
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
	//       statistics.totalMovementCost += units
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

void SimulationController::decideAfterTask(Robot& robot) {
	// TODO (Member 3)
	// PSEUDOCODE:
	//   if robot.is_on_road(): changeRobotState(UNAVAILABLE); return
	//   needCharge = battery_is_low(robot, 30) or batteryBlocksAllPending(robot)
	//   if not needCharge: changeRobotState(AVAILABLE); return
	//   r = searchNearestStation(robot.current_location_id)
	//   if not r.exists: changeRobotState(UNAVAILABLE); return
	//   robot.set_route(r.route, r.connection_ids); robot.return_station_id = last stop of the route
	//   changeRobotState(RETURNING)       (a zero-length route means: already at a station)
	(void)robot;
}

void SimulationController::interruptTask(Robot& robot, const string& reason) {
	// TODO (Member 3)
	// PSEUDOCODE: i = findRequestIndex(robot.assigned_request_id)
	//   if i != -1: requests[i].return_to_pending()  (resets work to full duration)
	//               statistics.reassignments++; log INTERRUPT with reason and REQUEST_STATE
	//   robot.release_request()
	//   decideAfterTask(robot)
	(void)robot; (void)reason;
}

PathResult SimulationController::searchRoute(int fromId, int toId) {
	// DONE (provided): counts the work for the performance analysis.
	PathResult result = find_shortest_path(environment, fromId, toId);
	statistics.routeSearches += 1;
	statistics.edgesRelaxed += result.edges_relaxed;
	return result;
}

PathResult SimulationController::searchNearestStation(int fromId) {
	// DONE (provided)
	PathResult result = find_nearest_station(environment, fromId);
	statistics.routeSearches += 1;
	statistics.edgesRelaxed += result.edges_relaxed;
	return result;
}

bool SimulationController::changeRobotState(Robot& robot, int newState,
	const string& reason) {
	// DONE (provided): the only place robot states change, so every change is logged.
	int oldState = robot.state;
	string ids = "robot=" + int_to_string(robot.id);
	if (robot.set_state(newState)) {
		logger.log(currentTime, "ROBOT_STATE", ids,
			string(robot_state_name(oldState)) + "->" + robot_state_name(newState) +
				";" + reason);
		return true;
	}
	logger.log(currentTime, "REJECTED", ids,
		string("illegal transition ") + robot_state_name(oldState) + "->" +
			robot_state_name(newState));
	return false;
}

bool SimulationController::batteryBlocksAllPending(const Robot& robot) {
	// TODO (Member 3)
	// PSEUDOCODE: compatible = 0; feasible = 0; stationCosts = compute_station_costs(...)
	//   for each PENDING request whose category the robot can serve (robot.check_capability):
	//       compatible++; r = searchRoute(robot location, request location)
	//       if r.exists and mission_battery_required(r.total_cost, duration, station cost of the
	//             request location, pc, pw) in 0..robot.battery_level: feasible++
	//   return compatible > 0 and feasible == 0
	(void)robot;
	return false;
}

// ---------------------------------------------------------------------
// STAGE 5: dispatch
// ---------------------------------------------------------------------

void SimulationController::assignPendingRequests() {
	// TODO (Member 3)
	// PSEUDOCODE:
	//   if no robot is AVAILABLE: return                       (cheap exit: most steps)
	//   indices = indices of requests with state PENDING
	//   sort_request_indices(requests, indices)                (Member 2: priority order)
	//   compute_station_costs(environment, stationCosts) once; add its return value to edgesRelaxed
	//   for each request index q in `indices` (highest priority first):
	//       bestRobot = -1; bestCost = none; feasibleCount = 0
	//       for each robot that is AVAILABLE and check_capability(request.category):
	//           r = searchRoute(robot.current_location_id, request.location_id)
	//           if not r.exists: continue
	//           need = mission_battery_required(r.total_cost, request.duration_steps,
	//                    stationCosts[index of request.location_id], pc, pw)
	//           if need == -1 or robot.battery_level < need: continue
	//           feasibleCount++
	//           if bestRobot == -1 or r.total_cost < bestCost
	//                  or (r.total_cost == bestCost and robot.id < best robot's id):
	//               remember this robot and its route
	//       if feasibleCount > 1: statistics.conflictsPrevented++; log CONFLICT
	//       if bestRobot != -1:
	//           request.assign(robot.id, currentTime)  AND  robot.assign_request(request.id)
	//           (do both or neither: if one fails, undo the other and log REJECTED)
	//           robot.set_route(route, connection_ids); statistics.assignments++
	//           log ASSIGN with robot, request, route cost, battery   (robot starts moving next step)
	//       if no robot is AVAILABLE any more: stop
	//   Requests nobody can serve simply stay PENDING (e.g. a cut-off location).
}

// ---------------------------------------------------------------------
// End of run and reporting
// ---------------------------------------------------------------------

void SimulationController::failUnresolvedRequests() {
	// DONE (provided)
	for (int i = 0; i < requests.size(); ++i) {
		if (requests[i].state != REQUEST_COMPLETED && requests[i].fail(FAIL_TIME_LIMIT)) {
			logger.log(currentTime, "REQUEST_STATE", "request=" + int_to_string(requests[i].id),
				"->FAILED;TIME_LIMIT");
		}
	}
}

void SimulationController::computeFinalStatistics() {
	// DONE (provided). Movement cost, assignments, reassignments, conflicts and search counters
	// were counted while running; this fills in the rest from the final records.
	statistics.completedRequests = 0;
	statistics.failedRequests = 0;
	statistics.unresolvedAtEnd = 0;
	statistics.totalResponseTime = 0;
	statistics.maximumResponseTime = 0;
	for (int i = 0; i < requests.size(); ++i) {
		const RescueRequest& request = requests[i];
		if (request.state == REQUEST_COMPLETED) {
			int response = request.completion_time - request.report_time;
			statistics.completedRequests += 1;
			statistics.totalResponseTime += response;
			if (response > statistics.maximumResponseTime) statistics.maximumResponseTime = response;
		} else if (request.state == REQUEST_FAILED) {
			statistics.failedRequests += 1;
			if (request.fail_reason == FAIL_TIME_LIMIT) statistics.unresolvedAtEnd += 1;
		}
	}
	statistics.totalBatteryUsed = 0;
	for (int i = 0; i < robots.size(); ++i) statistics.totalBatteryUsed += robots[i].battery_used;
}

void SimulationController::writeStatistics(ostream& out) const {
	// DONE (provided): every metric listed in spec section 8.
	out << "scenario: " << settings.scenario_name << '\n';
	out << "steps_run: " << currentTime << '\n';
	out << "requests_total: " << requests.size() << '\n';
	out << "requests_completed: " << statistics.completedRequests << '\n';
	out << "requests_failed: " << statistics.failedRequests << '\n';
	out << "requests_unresolved_at_end: " << statistics.unresolvedAtEnd << '\n';
	if (statistics.completedRequests > 0) {
		out << "average_response_time: "
			<< (double)statistics.totalResponseTime / statistics.completedRequests << '\n';
	} else {
		out << "average_response_time: n/a\n";
	}
	out << "maximum_response_time: " << statistics.maximumResponseTime << '\n';
	out << "total_movement_cost: " << statistics.totalMovementCost << '\n';
	out << "total_battery_used: " << statistics.totalBatteryUsed << '\n';
	out << "assignments: " << statistics.assignments << '\n';
	out << "reassignments: " << statistics.reassignments << '\n';
	out << "conflicts_prevented: " << statistics.conflictsPrevented << '\n';
	out << "route_searches: " << statistics.routeSearches << '\n';
	out << "edges_relaxed: " << statistics.edgesRelaxed << '\n';
	for (int i = 0; i < robots.size(); ++i) {
		const Robot& robot = robots[i];
		out << "robot " << robot.id << ": busy_steps=" << robot.busy_steps
			<< " online_steps=" << robot.online_steps << " utilisation_percent=";
		if (robot.online_steps > 0) out << 100.0 * robot.busy_steps / robot.online_steps;
		else out << "n/a";
		out << " battery_used=" << robot.battery_used << '\n';
	}
}

void SimulationController::printStatus() const {
	// DONE (provided)
	cout << "Simulation time: " << currentTime << '\n';
	cout << "Locations: " << environment.get_location_count()
		<< "  Connections: " << environment.get_connection_count() << '\n';
	for (int i = 0; i < robots.size(); ++i) {
		cout << "Robot " << robots[i].id << " " << robot_state_name(robots[i].state)
			<< " at " << robots[i].current_location_id
				<< " battery " << robots[i].battery_level << "/" << robots[i].max_battery << '\n';
	}
	for (int i = 0; i < requests.size(); ++i) {
		cout << "Request " << requests[i].id << " " << request_state_name(requests[i].state)
			<< " " << category_name(requests[i].category)
				<< " urgency " << requests[i].urgency << '\n';
	}
}

void SimulationController::printFinalStatistics() const {
	// DONE (provided)
	writeStatistics(cout);
}

bool SimulationController::writeOutputs(const string& logPath,
	const string& statsPath) const {
	// DONE (provided)
	bool logOk = logger.save_to_file(logPath);
	ofstream out(statsPath.c_str());
	if (!out.is_open()) return false;
	writeStatistics(out);
	out.close();
	return logOk && !out.fail();
}
