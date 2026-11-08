#include <iostream>
#include "../include/robot.hpp"
#include "../include/rescue_request.hpp"
#include "../include/event.hpp"

int run_entities_tests() {
    int fails = 0;

    // Robot tests
    Robot r(1, 10, RobotCapability::MEDICAL, 100);
    if (!r.check_capability(RequestCategory::MEDICAL)) fails++;
    if (r.check_capability(RequestCategory::FIRE)) fails++;

    // Battery validation
    if (r.consume_battery(-10)) fails++; // Negative consumption rejected
    if (r.consume_battery(0)) fails++;   // 0 consumption rejected
    if (!r.consume_battery(30) || r.battery_level != 70) fails++;
    if (r.consume_battery(80)) fails++;  // Insufficient battery rejected

    if (r.charge(-20)) fails++;          // Negative charging rejected
    if (!r.charge(50) || r.battery_level != 100) fails++; // Clamped to max_battery
    if (!r.charge(2147483647) || r.battery_level != 100) fails++; // No integer overflow

    // Assignment is allowed only for an idle, online robot and a valid request ID.
    if (r.assign_request(-1)) fails++;
    if (!r.assign_request(500)) fails++;
    if (r.assign_request(501)) fails++;  // Already MOVING / assigned
    r.unassign();
    if (!r.set_state(RobotState::CHARGING)) fails++;
    if (r.assign_request(503)) fails++;  // Charging robot cannot be assigned
    if (!r.set_state(RobotState::IDLE)) fails++;
    if (!r.set_state(RobotState::MOVING)) fails++;
    if (r.assign_request(504)) fails++;  // Moving robot cannot be assigned
    if (!r.set_state(RobotState::IDLE)) fails++;

    // Offline state behavior and explicit return-online transition.
    if (!r.set_state(RobotState::OFFLINE)) fails++;
    if (r.assign_request(502)) fails++;  // Cannot assign offline robot
    if (r.set_state(RobotState::MOVING)) fails++; // Must return to IDLE first
    if (!r.set_state(RobotState::IDLE)) fails++;

    // RescueRequest tests
    RescueRequest req(100, 20, RequestCategory::MEDICAL, 4, 0, 10);
    if (req.state != RequestState::PENDING) fails++;

    // Invalid urgency / duration validation
    RescueRequest bad_req1(101, 20, RequestCategory::MEDICAL, 99, 0, 10); // Urgency > 5
    if (bad_req1.urgency != 1) fails++; // Fallback or rejected

    RescueRequest bad_req2(102, 20, RequestCategory::MEDICAL, 3, 0, -5); // Duration <= 0
    if (bad_req2.state != RequestState::FAILED) fails++;
    RescueRequest bad_req3(-1, 20, RequestCategory::MEDICAL, 3, 0, 5);
    if (bad_req3.state != RequestState::FAILED) fails++;

    // Request lifecycle
    if (req.assign(-1)) fails++;
    if (!req.assign(1)) fails++;
    if (req.state != RequestState::ASSIGNED) fails++;
    if (req.complete()) fails++; // Cannot complete before work starts
    if (!req.start_work()) fails++;

    if (req.perform_work(-5)) fails++; // Negative work units rejected
    if (!req.perform_work(4)) fails++;
    if (req.state == RequestState::COMPLETED) fails++; // Remaining 6 work units
    if (!req.perform_work(6)) fails++;
    if (req.state != RequestState::IN_PROGRESS || req.remaining_work != 0) fails++;
    if (!req.complete()) fails++; // Completion only after all work is done
    if (req.state != RequestState::COMPLETED) fails++;

    if (req.complete()) fails++; // Prevent repeated completion
    if (req.fail()) fails++;     // Prevent failing a completed request

    // Failed request behavior
    RescueRequest req_fail(200, 20, RequestCategory::FIRE, 3, 0, 5);
    req_fail.fail();
    if (req_fail.state != RequestState::FAILED) fails++;
    if (req_fail.assign(1)) fails++; // Failed state is terminal

    if (fails) std::cerr << "[FAIL] Entities tests (" << fails << " failures)." << std::endl;
    else std::cout << "[PASS] Entities tests." << std::endl;

    return fails > 0 ? 1 : 0;
}
