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
	long long routeSearches;
	long long edgesRelaxed;

	SimulationStatistics()
		: completedRequests(0), failedRequests(0), unresolvedAtEnd(0), assignments(0),
		reassignments(0), conflictsPrevented(0), totalMovementCost(0), totalBatteryUsed(0),
		totalResponseTime(0), maximumResponseTime(0), routeSearches(0), edgesRelaxed(0) {}
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

	void applyDueEvents();
	bool applyEvent(const Event& event);
	void releaseDueRequests();
	void recheckRoutes();
	void advanceRobots();
	void assignPendingRequests();

	void updateRobot(Robot& robot);
	bool travel(Robot& robot);
	void decideAfterTask(Robot& robot);
	void interruptTask(Robot& robot, const string& reason);
	PathResult searchRoute(int fromId, int toId);
	PathResult searchNearestStation(int fromId);
	bool changeRobotState(Robot& robot, int newState, const string& reason);
	bool batteryBlocksAllPending(const Robot& robot);

	void failUnresolvedRequests();
	void computeFinalStatistics();
	void writeStatistics(ostream& out) const;

public:
	SimulationController();

	bool loadScenario(const string& filePath);
	bool step();
	void run();
	void printStatus() const;
	void printFinalStatistics() const;
	bool writeOutputs(const string& logPath, const string& statsPath) const;

	bool isFinished() const { return finished; }
	int getCurrentTime() const { return currentTime; }
	const SimulationStatistics& getStatistics() const { return statistics; }
	const DynamicArray<RescueRequest>& getRequests() const { return requests; }
	const DynamicArray<Robot>& getRobots() const { return robots; }
	const Logger& getLogger() const { return logger; }
};

#endif