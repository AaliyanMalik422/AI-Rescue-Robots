#ifndef SIMULATION_HPP
#define SIMULATION_HPP

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

using namespace std;

// =====================================================================
// MEMBER 3 - Simulation controller and integration.
//
// ONE STEP, for currentTime = 0 .. max_steps-1, in this order (design report 4.3):
//   1. applyDueEvents      events with timestamp == t, in file order
//   2. releaseDueRequests  UNREPORTED -> PENDING when report_time <= t
//   3. recheckRoutes        reroute / interrupt robots affected by events
//   4. advanceRobots        move, work, complete, recharge (robots active at this point)
//   5. assignPendingRequests   new assignments start moving on the NEXT step
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
//   * conflictsPrevented = number of requests for which MORE THAN ONE robot was feasible
//     (only one is chosen).
// =====================================================================

struct SimulationStatistics {
	int completedRequests;
	int failedRequests;
	int unresolvedAtEnd;
	int assignments;
	int reassignments;
	int conflictsPrevented;
	long long totalMovementCost;
	long long totalBatteryUsed;
	long long totalResponseTime;
	int maximumResponseTime;
	long long routeSearches;     // work counters for the performance analysis
	long long edgesRelaxed;

	SimulationStatistics()
		: completedRequests(0), failedRequests(0), unresolvedAtEnd(0), assignments(0),
			reassignments(0), conflictsPrevented(0), totalMovementCost(0),
				totalBatteryUsed(0), totalResponseTime(0), maximumResponseTime(0),
					routeSearches(0), edgesRelaxed(0) {}
};

class SimulationController {
private:
	Environment environment;
	DynamicArray<Robot> robots;
	DynamicArray<RescueRequest> requests;
	EventQueue eventQueue;
	SimulationSettings settings;
	SimulationStatistics statistics;
	Logger logger;

	int currentTime;
	bool initialized;
	bool finished;

	int findRobotIndex(int robotId) const;
	int findRequestIndex(int requestId) const;

	// --- the five pipeline stages (applyDueEvents, applyEvent, recheckRoutes and
	//     assignPendingRequests are TODO Member 3; releaseDueRequests and advanceRobots are provided) ---
	void applyDueEvents();
	bool applyEvent(const Event& event);
	void releaseDueRequests();
	void recheckRoutes();
	void advanceRobots();
	void assignPendingRequests();

	// --- the movement and dispatch logic (TODO, Member 3; searchRoute, searchNearestStation
	//     and changeRobotState are provided) ---
	void updateRobot(Robot& robot);
	// Moves up to `speed` units along robot.planned_route / the road it is on.
	// Returns true if the robot is now at the END of its route (not on a road).
	bool travel(Robot& robot);
	// Robot is at a location with no task: choose AVAILABLE, or RETURNING to a station,
	// or UNAVAILABLE (still on a road, or no station reachable).
	void decideAfterTask(Robot& robot);
	// Request goes back to PENDING (work reset), robot released, then decideAfterTask.
	void interruptTask(Robot& robot, const string& reason);
	// Wrappers around Member 1's searches that also count work and log nothing.
	PathResult searchRoute(int fromId, int toId);
	PathResult searchNearestStation(int fromId);
	// set_state that logs the change, or logs REJECTED when the transition is illegal.
	bool changeRobotState(Robot& robot, int newState, const string& reason);
	// True if some PENDING request is compatible with the robot but the robot is feasible for none.
	bool batteryBlocksAllPending(const Robot& robot);

	void failUnresolvedRequests();
	void computeFinalStatistics();
	void writeStatistics(ostream& out) const;

public:
	SimulationController();

	// Loads and validates the scenario, fills eventQueue. (Already wired to Member 1's parser.)
	bool loadScenario(const string& filePath);

	// DONE (provided): run exactly one time step. False if not loaded or already finished.
	bool step();

	// DONE (provided): call step() until finished.
	void run();

	// DONE (provided): print environment summary, robots, and requests grouped by state.
	void printStatus() const;
	void printFinalStatistics() const;

	// DONE (provided): save the log and a statistics file. False on I/O failure.
	bool writeOutputs(const string& logPath, const string& statsPath) const;

	bool isFinished() const { return finished; }
	int getCurrentTime() const { return currentTime; }
	const SimulationStatistics& getStatistics() const { return statistics; }
	const DynamicArray<RescueRequest>& getRequests() const { return requests; }
	const DynamicArray<Robot>& getRobots() const { return robots; }
	const Logger& getLogger() const { return logger; }
};

#endif
