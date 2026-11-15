#include "rescue_request.hpp"

// MEMBER 2: request lifecycle. Constructors only store values (state starts
// UNREPORTED); validating scenario data is the parser's job (Member 1).

RescueRequest::RescueRequest()
    : id(-1), location_id(-1), category(CATEGORY_SEARCH), urgency(1), report_time(0),
      duration_steps(1), remaining_work(1), state(REQUEST_UNREPORTED), assigned_robot_id(-1),
      completion_time(-1), fail_reason(FAIL_NONE), first_assigned_time(-1), times_reassigned(0) {}

RescueRequest::RescueRequest(int request_id, int location, int request_category,
                             int request_urgency, int time_reported, int work_duration)
    : id(request_id), location_id(location), category(request_category), urgency(request_urgency),
      report_time(time_reported), duration_steps(work_duration), remaining_work(work_duration),
      state(REQUEST_UNREPORTED), assigned_robot_id(-1), completion_time(-1),
      fail_reason(FAIL_NONE), first_assigned_time(-1), times_reassigned(0) {}

bool RescueRequest::report() {
    // DONE (provided)
    if (state != REQUEST_UNREPORTED) return false;
    state = REQUEST_PENDING;
    return true;
}

bool RescueRequest::assign(int robot_id, int time) {
    // TODO (Member 2)
    // PSEUDOCODE: if robot_id < 0 or state != PENDING: return false
    //   assigned_robot_id = robot_id; state = ASSIGNED
    //   if first_assigned_time == -1: first_assigned_time = time
    //   return true
    (void)robot_id; (void)time;
    return false;
}

bool RescueRequest::start_work() {
    // DONE (provided)
    if (state != REQUEST_ASSIGNED) return false;
    state = REQUEST_IN_PROGRESS;
    return true;
}

bool RescueRequest::perform_work(int time_units) {
    // TODO (Member 2)
    // PSEUDOCODE: if time_units <= 0 or state != IN_PROGRESS: return false
    //   remaining_work -= time_units; if remaining_work < 0: remaining_work = 0; return true
    (void)time_units;
    return false;
}

bool RescueRequest::complete(int time) {
    // TODO (Member 2)
    // PSEUDOCODE: if state != IN_PROGRESS or remaining_work != 0: return false
    //   state = COMPLETED; completion_time = time; return true
    (void)time;
    return false;
}

bool RescueRequest::fail(int reason) {
    // TODO (Member 2)
    // PSEUDOCODE: if is_terminal(): return false
    //   state = FAILED; fail_reason = reason; assigned_robot_id = -1; return true
    (void)reason;
    return false;
}

bool RescueRequest::return_to_pending() {
    // TODO (Member 2)
    // PSEUDOCODE: if state != ASSIGNED and state != IN_PROGRESS: return false
    //   state = PENDING; assigned_robot_id = -1; remaining_work = duration_steps
    //   times_reassigned += 1; return true
    return false;
}

bool RescueRequest::set_urgency(int new_urgency) {
    // DONE (provided)
    if (new_urgency < 1 || new_urgency > 5 || is_terminal()) return false;
    urgency = new_urgency;
    return true;
}

bool RescueRequest::is_terminal() const {
    // DONE (provided)
    return state == REQUEST_COMPLETED || state == REQUEST_FAILED;
}
