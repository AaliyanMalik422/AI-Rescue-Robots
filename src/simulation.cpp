#include "simulation.hpp"
#include "pathfinder.hpp"
#include "names.hpp"
#include <fstream>
#include <iostream>

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

bool SimulationController::step() {
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
	if (!initialized) {
		cerr << "Cannot run: scenario was not loaded.\n";
		return;
	}
	while (step()) {
	}
}

void SimulationController::applyDueEvents() {
	Event next;

	while (eventQueue.peek(next)) {

		if (next.timestamp > currentTime) {
			break;
		}

		if (!eventQueue.pop(next)) {
			break;
		}

		bool applied = applyEvent(next);

		string detail = event_type_name(next.type);

		if (next.type == EVENT_SET_URGENCY) {
			detail += ";urgency=";
			detail += int_to_string(next.value);
		}

		logger.log(
			currentTime,
			applied ? "EVENT" : "REJECTED",
			"target=" + int_to_string(next.target_id),
			detail
		);
	}
}

bool SimulationController::applyEvent(const Event& event) {

	if (event.type == EVENT_BLOCK) {
		return environment.set_connection_blocked(event.target_id, true);
	}

	if (event.type == EVENT_UNBLOCK) {
		return environment.set_connection_blocked(event.target_id, false);
	}

	if (event.type == EVENT_STATION_DOWN) {
		return environment.set_station_available(event.target_id, false);
	}

	if (event.type == EVENT_STATION_UP) {
		return environment.set_station_available(event.target_id, true);
	}

	if (event.type == EVENT_SET_URGENCY) {

		int index = findRequestIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		return requests[index].set_urgency(event.value);
	}

	if (event.type == EVENT_ROBOT_OFFLINE) {

		int index = findRobotIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		Robot& robot = robots[index];

		if (robot.state == ROBOT_OFFLINE) {
			return false;
		}

		int requestId = robot.assigned_request_id;
		int requestIndex = -1;
		int oldRequestState = -1;

		if (requestId != -1) {

			requestIndex = findRequestIndex(requestId);

			if (requestIndex == -1) {
				return false;
			}

			RescueRequest& request = requests[requestIndex];

			if (request.assigned_robot_id != robot.id) {
				return false;
			}

			if (request.state != REQUEST_ASSIGNED &&
				request.state != REQUEST_IN_PROGRESS) {
				return false;
			}

			oldRequestState = request.state;
		}

		int oldRobotState = robot.state;

		// go_offline() preserves physical progress if the robot is mid-road.
		int releasedRequestId = robot.go_offline();

		if (releasedRequestId != requestId ||
			robot.state != ROBOT_OFFLINE) {
			return false;
		}

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

		logger.log(
			currentTime,
			"ROBOT_STATE",
			"robot=" + int_to_string(robot.id),
			string(robot_state_name(oldRobotState)) +
			"->OFFLINE;ROBOT_OFFLINE"
		);

		return true;
	}

	if (event.type == EVENT_ROBOT_ONLINE) {

		int index = findRobotIndex(event.target_id);

		if (index == -1) {
			return false;
		}

		Robot& robot = robots[index];

		if (robot.state != ROBOT_OFFLINE) {
			return false;
		}

		if (!robot.go_online()) {
			return false;
		}

		logger.log(
			currentTime,
			"ROBOT_STATE",
			"robot=" + int_to_string(robot.id),
			"OFFLINE->AVAILABLE;ROBOT_ONLINE"
		);

		if (robot.is_on_road()) {
			return changeRobotState(
				robot,
				ROBOT_UNAVAILABLE,
				"online;finish current road"
			);
		}

		if (environment.is_operational_station(robot.current_location_id) &&
			robot.battery_level < robot.max_battery) {

			return changeRobotState(
				robot,
				ROBOT_RECHARGING,
				"online at charging station"
			);
		}

		decideAfterTask(robot);

		return true;
	}

	return false;
}

void SimulationController::releaseDueRequests() {
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

void SimulationController::recheckRoutes() {
	DynamicArray<long long> stationCosts;
	bool stationCostsComputed = false;

	for (int i = 0; i < robots.size(); ++i) {
		Robot& robot = robots[i];

		// Offline robots and robots crossing roads are checked later.
		if (robot.state == ROBOT_OFFLINE || robot.is_on_road()) {
			continue;
		}

		if (robot.state == ROBOT_TRAVELLING) {

			if (!route_is_open(
				environment,
				robot.planned_route,
				robot.next_route_index - 1
			)) {
				PathResult replacement = searchRoute(
					robot.current_location_id,
					robot.mission_target_id
				);

				if (!replacement.exists) {
					interruptTask(robot, "no route");
					continue;
				}

				if (!robot.set_route(
					replacement.route,
					replacement.connection_ids
				)) {
					interruptTask(robot, "reroute failed");
					continue;
				}

				logger.log(
					currentTime,
					"REROUTE",
					"robot=" + int_to_string(robot.id),
					"target=" + int_to_string(robot.mission_target_id) +
					";cost=" + int_to_string(replacement.total_cost)
				);
			}
		}

		else if (robot.state == ROBOT_RETURNING) {

			bool stationAvailable =
				environment.is_operational_station(robot.mission_target_id);

			bool routeOpen = route_is_open(
				environment,
				robot.planned_route,
				robot.next_route_index - 1
			);

			if (stationAvailable && routeOpen) {
				continue;
			}

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

		else if (robot.state == ROBOT_RECHARGING) {

			if (environment.is_operational_station(
				robot.current_location_id
			)) {
				continue;
			}

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

		else if (robot.state != ROBOT_PERFORMING) {
			continue;
		}

		int requestIndex = findRequestIndex(robot.assigned_request_id);

		if (requestIndex == -1) {
			interruptTask(robot, "missing assigned request");
			continue;
		}

		if (!stationCostsComputed) {
			statistics.edgesRelaxed +=
				compute_station_costs(environment, stationCosts);

			stationCostsComputed = true;
		}

		RescueRequest& request = requests[requestIndex];

		int requestLocationIndex =
			environment.get_location_index(request.location_id);

		if (requestLocationIndex < 0 ||
			requestLocationIndex >= stationCosts.size()) {

			interruptTask(robot, "battery reserve unavailable");
			continue;
		}

		long long remainingCost = 0;

		if (robot.state == ROBOT_TRAVELLING) {

			int firstConnectionIndex = robot.next_route_index - 1;

			if (firstConnectionIndex < 0 ||
				firstConnectionIndex > robot.planned_connections.size()) {

				interruptTask(robot, "invalid remaining route");
				continue;
			}

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

		long long requiredBattery = mission_battery_required(
			remainingCost,
			request.remaining_work,
			stationCosts[requestLocationIndex],
			settings.battery_per_cost_unit,
			settings.battery_per_work_step
		);

		if (requiredBattery == -1 ||
			robot.battery_level < requiredBattery) {

			interruptTask(robot, "battery reserve");
			continue;
		}
	}
}

void SimulationController::advanceRobots() {
	for (int i = 0; i < robots.size(); ++i) {
		Robot& robot = robots[i];
		if (robot.state == ROBOT_OFFLINE) continue;
		robot.online_steps += 1;
		if (robot.state == ROBOT_TRAVELLING || robot.state == ROBOT_PERFORMING) robot.busy_steps += 1;
		updateRobot(robot);
	}
}

void SimulationController::updateRobot(Robot& robot) {

	if (robot.state == ROBOT_TRAVELLING) {

		if (!travel(robot)) {
			return;
		}

		int requestIndex = findRequestIndex(robot.assigned_request_id);

		if (requestIndex == -1) {
			interruptTask(robot, "missing assigned request");
			return;
		}

		RescueRequest& request = requests[requestIndex];

		// Start rescue work after reaching the destination.
		if (!request.start_work()) {
			interruptTask(robot, "could not start request");
			return;
		}

		logger.log(
			currentTime,
			"REQUEST_STATE",
			"request=" + int_to_string(request.id),
			"ASSIGNED->IN_PROGRESS;robot=" +
				int_to_string(robot.id)
		);

		if (!changeRobotState(
			robot,
			ROBOT_PERFORMING,
			"arrived at rescue location"
		)) {
			interruptTask(robot, "could not enter performing state");
			return;
		}

		logger.log(
			currentTime,
			"ARRIVE",
			"robot=" + int_to_string(robot.id) +
				";request=" + int_to_string(request.id),
			"location=" + int_to_string(robot.current_location_id)
		);
	}

	else if (robot.state == ROBOT_PERFORMING) {

		int requestIndex = findRequestIndex(robot.assigned_request_id);

		if (requestIndex == -1) {
			interruptTask(robot, "missing assigned request");
			return;
		}

		RescueRequest& request = requests[requestIndex];

		if (!robot.consume_battery(settings.battery_per_work_step)) {
			interruptTask(robot, "battery");
			return;
		}

		if (!request.perform_work(1)) {
			interruptTask(robot, "could not perform work");
			return;
		}

		logger.log(
			currentTime,
			"WORK",
			"robot=" + int_to_string(robot.id) +
				";request=" + int_to_string(request.id),
			"remaining=" + int_to_string(request.remaining_work) +
				";battery=" + int_to_string(robot.battery_level)
		);

		if (request.remaining_work == 0) {

			if (!request.complete(currentTime)) {
				interruptTask(robot, "could not complete request");
				return;
			}

			statistics.completedRequests++;

			logger.log(
				currentTime,
				"REQUEST_STATE",
				"request=" + int_to_string(request.id),
				"IN_PROGRESS->COMPLETED"
			);

			logger.log(
				currentTime,
				"COMPLETE",
				"robot=" + int_to_string(robot.id) +
					";request=" + int_to_string(request.id),
				"location=" + int_to_string(robot.current_location_id)
			);

			robot.release_request();
			decideAfterTask(robot);
		}
	}

	else if (robot.state == ROBOT_RETURNING) {

		if (travel(robot)) {

			if (changeRobotState(
				robot,
				ROBOT_RECHARGING,
				"arrived at charging station"
			)) {
				logger.log(
					currentTime,
					"ARRIVE",
					"robot=" + int_to_string(robot.id),
					"station=" + int_to_string(robot.current_location_id)
				);
			}
		}
	}

	else if (robot.state == ROBOT_RECHARGING) {

		int previousBattery = robot.battery_level;

		if (!robot.charge(settings.recharge_per_step)) {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(robot.id),
				"charging failed"
			);
			return;
		}

		int chargedAmount = robot.battery_level - previousBattery;

		logger.log(
			currentTime,
			"CHARGE",
			"robot=" + int_to_string(robot.id),
			"added=" + int_to_string(chargedAmount) +
				";battery=" + int_to_string(robot.battery_level) +
				"/" + int_to_string(robot.max_battery)
		);

		if (robot.battery_level == robot.max_battery) {
			changeRobotState(
				robot,
				ROBOT_AVAILABLE,
				"battery fully charged"
			);
		}
	}

	else if (robot.state == ROBOT_UNAVAILABLE) {

		if (robot.is_on_road()) {
			if (travel(robot)) {
				decideAfterTask(robot);
			}
		}
		else {
			decideAfterTask(robot);
		}
	}

}

bool SimulationController::travel(Robot& robot) {
	int budget = settings.speed;
	long long movedDistance = 0;
	bool moved = false;

	while (budget > 0) {
		int connectionId = -1;
		int targetLocationId = -1;
		bool startingRoad = false;

		if (!robot.is_on_road()) {
			if (robot.planned_route.empty() ||
				robot.next_route_index >= robot.planned_route.size()) {
				break;
			}

			int connectionIndex = robot.next_route_index - 1;

			if (connectionIndex < 0 ||
				connectionIndex >= robot.planned_connections.size()) {
				logger.log(
					currentTime,
					"REJECTED",
					"robot=" + int_to_string(robot.id),
					"invalid route connection index"
				);
				break;
			}

			connectionId = robot.planned_connections[connectionIndex];
			targetLocationId = robot.planned_route[robot.next_route_index];

			const Connection* nextConnection =
				environment.get_connection(connectionId);

			if (nextConnection == 0) {
				logger.log(
					currentTime,
					"REJECTED",
					"robot=" + int_to_string(robot.id),
					"planned connection not found"
				);
				break;
			}

			if (nextConnection->is_blocked) {
				break;
			}

			startingRoad = true;
		}
		else {
			// Continue the road crossing from the previous step.
			connectionId = robot.current_connection_id;
			targetLocationId = robot.road_target_id;
		}

		const Connection* connection =
			environment.get_connection(connectionId);

		if (connection == 0 || targetLocationId == -1) {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(robot.id),
				"current road information invalid"
			);
			break;
		}

		if (connection->is_blocked) {
			break;
		}

		int distanceCovered = startingRoad
			? 0
			: robot.distance_on_current_connection;

		int remainingDistance =
			connection->distance - distanceCovered;

		if (remainingDistance <= 0) {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(robot.id),
				"invalid road progress"
			);
			break;
		}

		int units = (budget < remainingDistance)
			? budget
			: remainingDistance;

		int batteryCost =
			units * settings.battery_per_cost_unit;

		if (!robot.consume_battery(batteryCost)) {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(robot.id),
				"no battery to move"
			);
			break;
		}

		if (startingRoad) {
			robot.current_connection_id = connectionId;
			robot.distance_on_current_connection = 0;
			robot.road_target_id = targetLocationId;
		}

		robot.distance_on_current_connection += units;
		budget -= units;

		statistics.totalMovementCost += units;
		movedDistance += units;
		moved = true;

		if (robot.distance_on_current_connection ==
			connection->distance) {

			robot.current_location_id = robot.road_target_id;

			robot.current_connection_id = -1;
			robot.distance_on_current_connection = 0;
			robot.road_target_id = -1;

			if (!robot.planned_route.empty() &&
				robot.next_route_index < robot.planned_route.size()) {
				robot.next_route_index++;
			}
		}
	}

	if (moved) {
		string detail =
			"distance=" + int_to_string(movedDistance) +
			";battery=" + int_to_string(robot.battery_level);

		if (robot.is_on_road()) {
			detail +=
				";connection=" + int_to_string(robot.current_connection_id) +
				";progress=" +
				int_to_string(robot.distance_on_current_connection) +
				";target=" + int_to_string(robot.road_target_id);
		}
		else {
			detail +=
				";location=" + int_to_string(robot.current_location_id);
		}

		logger.log(
			currentTime,
			"MOVE",
			"robot=" + int_to_string(robot.id),
			detail
		);
	}

	return !robot.is_on_road() &&
		(robot.planned_route.empty() ||
			robot.next_route_index >= robot.planned_route.size());
}

void SimulationController::decideAfterTask(Robot& robot) {
	if (robot.is_on_road()) {
		changeRobotState(
			robot,
			ROBOT_UNAVAILABLE,
			"still on road"
		);
		return;
	}

	bool needCharge = battery_is_low(robot, 30) ||
		batteryBlocksAllPending(robot);

	if (!needCharge) {
		changeRobotState(
			robot,
			ROBOT_AVAILABLE,
			"ready for another task"
		);
		return;
	}

	PathResult stationRoute = searchNearestStation(robot.current_location_id);

	if (!stationRoute.exists) {
		changeRobotState(
			robot,
			ROBOT_UNAVAILABLE,
			"no reachable charging station"
		);
		return;
	}

	if (!robot.set_route(stationRoute.route, stationRoute.connection_ids)) {
		changeRobotState(
			robot,
			ROBOT_UNAVAILABLE,
			"could not set charging route"
		);
		return;
	}

	robot.return_station_id =
		stationRoute.route[stationRoute.route.size() - 1];

	changeRobotState(
		robot,
		ROBOT_RETURNING,
		"returning to charging station"
	);
}

void SimulationController::interruptTask(Robot& robot, const string& reason) {
	int requestId = robot.assigned_request_id;
	int requestIndex = -1;
	int oldRequestState = -1;

	if (requestId != -1) {
		requestIndex = findRequestIndex(requestId);

		if (requestIndex != -1) {
			RescueRequest& request = requests[requestIndex];

			if (request.assigned_robot_id == robot.id &&
				(request.state == REQUEST_ASSIGNED ||
					request.state == REQUEST_IN_PROGRESS)) {

				oldRequestState = request.state;

				if (!request.return_to_pending()) {
					logger.log(
						currentTime,
						"REJECTED",
						"robot=" + int_to_string(robot.id),
						"could not release request;reason=" + reason
					);
					return;
				}

				statistics.reassignments++;

				logger.log(
					currentTime,
					"REQUEST_STATE",
					"request=" + int_to_string(requestId),
					string(request_state_name(oldRequestState)) +
					"->PENDING;INTERRUPTED"
				);
			}
			else {
				logger.log(
					currentTime,
					"REJECTED",
					"robot=" + int_to_string(robot.id),
					"invalid request assignment;reason=" + reason
				);
			}
		}
		else {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(robot.id),
				"assigned request not found;reason=" + reason
			);
		}
	}

	string ids = "robot=" + int_to_string(robot.id);

	if (requestId != -1) {
		ids += ";request=" + int_to_string(requestId);
	}

	logger.log(currentTime, "INTERRUPT", ids, reason);

	robot.release_request();

	decideAfterTask(robot);
}

PathResult SimulationController::searchRoute(int fromId, int toId) {
	PathResult result = find_shortest_path(environment, fromId, toId);
	statistics.routeSearches += 1;
	statistics.edgesRelaxed += result.edges_relaxed;
	return result;
}

PathResult SimulationController::searchNearestStation(int fromId) {
	PathResult result = find_nearest_station(environment, fromId);
	statistics.routeSearches += 1;
	statistics.edgesRelaxed += result.edges_relaxed;
	return result;
}

bool SimulationController::changeRobotState(Robot& robot, int newState,
	const string& reason) {
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
	int compatible = 0;
	int feasible = 0;

	for (int i = 0; i < requests.size(); ++i) {
		const RescueRequest& request = requests[i];

		if (request.state == REQUEST_PENDING &&
			robot.check_capability(request.category)) {
			compatible++;
		}
	}

	if (compatible == 0) {
		return false;
	}

	DynamicArray<long long> stationCosts;
	statistics.edgesRelaxed +=
		compute_station_costs(environment, stationCosts);

	for (int i = 0; i < requests.size(); ++i) {
		const RescueRequest& request = requests[i];

		if (request.state != REQUEST_PENDING ||
			!robot.check_capability(request.category)) {
			continue;
		}

		int locationIndex =
			environment.get_location_index(request.location_id);

		if (locationIndex < 0 || locationIndex >= stationCosts.size()) {
			continue;
		}

		PathResult route = searchRoute(
			robot.current_location_id,
			request.location_id
		);

		if (!route.exists) {
			continue;
		}

		long long requiredBattery = mission_battery_required(
			route.total_cost,
			request.duration_steps,
			stationCosts[locationIndex],
			settings.battery_per_cost_unit,
			settings.battery_per_work_step
		);

		if (requiredBattery >= 0 &&
			requiredBattery <= robot.battery_level) {
			feasible++;
		}
	}

	return compatible > 0 && feasible == 0;
}

void SimulationController::assignPendingRequests() {
	bool hasAvailableRobot = false;

	for (int i = 0; i < robots.size(); ++i) {
		if (robots[i].state == ROBOT_AVAILABLE) {
			hasAvailableRobot = true;
			break;
		}
	}

	if (!hasAvailableRobot) {
		return;
	}

	DynamicArray<int> requestIndices;

	for (int i = 0; i < requests.size(); ++i) {
		if (requests[i].state == REQUEST_PENDING) {
			if (!requestIndices.push_back(i)) {
				logger.log(
					currentTime,
					"REJECTED",
					"dispatcher",
					"could not store pending requests"
				);
				return;
			}
		}
	}

	if (requestIndices.empty()) {
		return;
	}

	sort_request_indices(requests, requestIndices);

	DynamicArray<long long> stationCosts;
	statistics.edgesRelaxed +=
		compute_station_costs(environment, stationCosts);

	for (int i = 0; i < requestIndices.size(); ++i) {
		int requestIndex = requestIndices[i];
		RescueRequest& request = requests[requestIndex];

		int locationIndex =
			environment.get_location_index(request.location_id);

		if (locationIndex < 0 || locationIndex >= stationCosts.size()) {
			continue;
		}

		int bestRobotIndex = -1;
		long long bestCost = 0;
		long long bestRequiredBattery = 0;
		int feasibleCount = 0;

		DynamicArray<int> bestRoute;
		DynamicArray<int> bestConnections;

		for (int j = 0; j < robots.size(); ++j) {
			Robot& robot = robots[j];

			if (robot.state != ROBOT_AVAILABLE ||
				robot.assigned_request_id != -1 ||
				robot.is_on_road()) {
				continue;
			}

			if (!robot.check_capability(request.category)) {
				continue;
			}

			PathResult route = searchRoute(
				robot.current_location_id,
				request.location_id
			);

			if (!route.exists) {
				continue;
			}

			long long requiredBattery = mission_battery_required(
				route.total_cost,
				request.duration_steps,
				stationCosts[locationIndex],
				settings.battery_per_cost_unit,
				settings.battery_per_work_step
			);

			if (requiredBattery < 0 ||
				requiredBattery > robot.battery_level) {
				continue;
			}

			feasibleCount++;

			if (bestRobotIndex == -1 ||
				route.total_cost < bestCost ||
				(route.total_cost == bestCost &&
					robot.id < robots[bestRobotIndex].id)) {

				bestRobotIndex = j;
				bestCost = route.total_cost;
				bestRequiredBattery = requiredBattery;
				bestRoute = route.route;
				bestConnections = route.connection_ids;
			}
		}

		if (bestRobotIndex == -1) {
			continue;
		}

		Robot& bestRobot = robots[bestRobotIndex];
		int oldRobotState = bestRobot.state;

		if (!bestRobot.set_route(bestRoute, bestConnections)) {
			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(bestRobot.id),
				"could not set assignment route"
			);
			continue;
		}

		if (!bestRobot.assign_request(request.id)) {
			bestRobot.release_request();

			logger.log(
				currentTime,
				"REJECTED",
				"robot=" + int_to_string(bestRobot.id),
				"robot assignment failed"
			);
			continue;
		}

		if (!request.assign(bestRobot.id, currentTime)) {
			bestRobot.release_request();

			changeRobotState(
				bestRobot,
				ROBOT_AVAILABLE,
				"request assignment failed"
			);

			logger.log(
				currentTime,
				"REJECTED",
				"request=" + int_to_string(request.id),
				"request assignment failed"
			);
			continue;
		}

		logger.log(
			currentTime,
			"ROBOT_STATE",
			"robot=" + int_to_string(bestRobot.id),
			string(robot_state_name(oldRobotState)) +
				"->TRAVELLING;ASSIGNED"
		);

		logger.log(
			currentTime,
			"REQUEST_STATE",
			"request=" + int_to_string(request.id),
			"PENDING->ASSIGNED;robot=" +
				int_to_string(bestRobot.id)
		);

		statistics.assignments++;

		if (feasibleCount > 1) {
			statistics.conflictsPrevented++;

			logger.log(
				currentTime,
				"CONFLICT",
				"request=" + int_to_string(request.id),
				"feasible_robots=" + int_to_string(feasibleCount) +
					";selected_robot=" + int_to_string(bestRobot.id)
			);
		}

		logger.log(
			currentTime,
			"ASSIGN",
			"robot=" + int_to_string(bestRobot.id) +
				";request=" + int_to_string(request.id),
			"cost=" + int_to_string(bestCost) +
				";battery=" + int_to_string(bestRobot.battery_level) +
				";required=" + int_to_string(bestRequiredBattery)
		);

		bool anotherRobotAvailable = false;

		for (int j = 0; j < robots.size(); ++j) {
			if (robots[j].state == ROBOT_AVAILABLE) {
				anotherRobotAvailable = true;
				break;
			}
		}

		if (!anotherRobotAvailable) {
			break;
		}
	}
}

void SimulationController::failUnresolvedRequests() {
	for (int i = 0; i < requests.size(); ++i) {
		if (requests[i].state != REQUEST_COMPLETED && requests[i].fail(FAIL_TIME_LIMIT)) {
			logger.log(currentTime, "REQUEST_STATE", "request=" + int_to_string(requests[i].id),
				"->FAILED;TIME_LIMIT");
		}
	}
}

void SimulationController::computeFinalStatistics() {
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
	writeStatistics(cout);
}

bool SimulationController::writeOutputs(const string& logPath,
	const string& statsPath) const {
	bool logOk = logger.save_to_file(logPath);
	ofstream out(statsPath.c_str());
	if (!out.is_open()) return false;
	writeStatistics(out);
	out.close();
	return logOk && !out.fail();
}