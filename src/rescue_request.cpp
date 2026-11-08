#include "rescue_request.hpp"

namespace {
bool valid_request_category(int category) {
    return category == CATEGORY_MEDICAL || category == CATEGORY_FIRE ||
           category == CATEGORY_SUPPLY || category == CATEGORY_SEARCH;
}
}

RescueRequest::RescueRequest()
    : id(-1), location_id(-1), category(CATEGORY_SEARCH), urgency(1), report_time(0),
      duration_steps(1), remaining_work(1), state(REQUEST_PENDING), assigned_robot_id(-1) {}

RescueRequest::RescueRequest(int request_id, int location, int request_category,
                             int request_urgency, int time_reported, int work_duration)
    : id(request_id), location_id(location), category(request_category), urgency(request_urgency),
      report_time(time_reported), duration_steps(work_duration), remaining_work(work_duration),
      state(REQUEST_PENDING), assigned_robot_id(-1) {
    if (id < 0 || location_id < 0 || !valid_request_category(category) ||
        urgency < 1 || urgency > 5 || report_time < 0 || duration_steps <= 0) {
        state = REQUEST_FAILED;
        if (duration_steps <= 0) {
            duration_steps = 1;
            remaining_work = 1;
        }
    }
}

bool RescueRequest::assign(int robot_id) {
    if (robot_id < 0 || state != REQUEST_PENDING) return false;
    assigned_robot_id = robot_id;
    state = REQUEST_ASSIGNED;
    return true;
}

bool RescueRequest::start_work() {
    if (state != REQUEST_ASSIGNED) return false;
    state = REQUEST_IN_PROGRESS;
    return true;
}

bool RescueRequest::perform_work(int time_units) {
    if (time_units <= 0 || state != REQUEST_IN_PROGRESS) return false;

    if (time_units >= remaining_work) remaining_work = 0;
    else remaining_work -= time_units;
    return true;
}

bool RescueRequest::complete() {
    if (state != REQUEST_IN_PROGRESS || remaining_work != 0) return false;
    state = REQUEST_COMPLETED;
    return true;
}

bool RescueRequest::fail() {
    if (state == REQUEST_COMPLETED || state == REQUEST_FAILED) return false;
    state = REQUEST_FAILED;
    assigned_robot_id = -1;
    return true;
}

bool RescueRequest::return_to_pending() {
    if (state != REQUEST_ASSIGNED && state != REQUEST_IN_PROGRESS) return false;
    state = REQUEST_PENDING;
    assigned_robot_id = -1;
    return true;
}
