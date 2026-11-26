#include "rescue_request.hpp"

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

    if (state != REQUEST_UNREPORTED)
        return false;

    state = REQUEST_PENDING;
    return true;
}

bool RescueRequest::assign(int robot_id, int time) {

    if (robot_id < 0 || state != REQUEST_PENDING)
        return false;

    assigned_robot_id = robot_id;
    state = REQUEST_ASSIGNED;

    if (first_assigned_time == -1)
        first_assigned_time = time;
    return true;
}

bool RescueRequest::start_work() {

    if (state != REQUEST_ASSIGNED)
        return false;

    state = REQUEST_IN_PROGRESS;
    return true;
}

bool RescueRequest::perform_work(int time_units) {

    if (time_units <= 0 || state != REQUEST_IN_PROGRESS)
        return false;

    remaining_work -= time_units;

    if (remaining_work < 0)
        remaining_work = 0;
    return true;
}

bool RescueRequest::complete(int time) {

    if (state != REQUEST_IN_PROGRESS || remaining_work != 0)
        return false;

    state = REQUEST_COMPLETED;
    completion_time = time;
    return true;
}

bool RescueRequest::fail(int reason) {

    if (is_terminal())
        return false;

    state = REQUEST_FAILED;
    fail_reason = reason;
    assigned_robot_id = -1;
    return true;
}

bool RescueRequest::return_to_pending() {

    if (state != REQUEST_ASSIGNED && state != REQUEST_IN_PROGRESS)
        return false;

    state = REQUEST_PENDING;
    assigned_robot_id = -1;
    remaining_work = duration_steps;
    times_reassigned++;
    return true;
}

bool RescueRequest::set_urgency(int new_urgency) {

    if (new_urgency < 1 || new_urgency > 5 || is_terminal())
        return false;

    urgency = new_urgency;
    return true;
}

bool RescueRequest::is_terminal() const {
    return state == REQUEST_COMPLETED || state == REQUEST_FAILED;
}
