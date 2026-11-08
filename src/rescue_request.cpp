#include "../include/rescue_request.hpp"

RescueRequest::RescueRequest() 
    : id(-1), location_id(-1), category(RequestCategory::SEARCH),
      urgency(1), report_time(0), duration(1), remaining_work(1),
      state(RequestState::PENDING), assigned_robot_id(-1) {}

RescueRequest::RescueRequest(int id_val, int loc_id, RequestCategory cat, int urg, int report_t, int dur)
    : id(id_val), location_id(loc_id), category(cat), 
      urgency((urg >= 1 && urg <= 5) ? urg : 1), 
      report_time(report_t >= 0 ? report_t : 0),
      duration(dur > 0 ? dur : 1), 
      remaining_work(dur > 0 ? dur : 1), 
      state((id_val >= 0 && loc_id >= 0 && urg >= 1 && urg <= 5 && dur > 0 && report_t >= 0)
                ? RequestState::PENDING : RequestState::FAILED), 
      assigned_robot_id(-1) {}

bool RescueRequest::assign(int robot_id) {
    if (robot_id < 0 || state != RequestState::PENDING) return false;
    assigned_robot_id = robot_id;
    state = RequestState::ASSIGNED;
    return true;
}

bool RescueRequest::start_work() {
    if (state != RequestState::ASSIGNED) return false;
    state = RequestState::IN_PROGRESS;
    return true;
}

bool RescueRequest::complete() {
    if (state != RequestState::IN_PROGRESS || remaining_work > 0) return false;
    state = RequestState::COMPLETED;
    remaining_work = 0;
    return true;
}

bool RescueRequest::fail() {
    if (state == RequestState::COMPLETED || state == RequestState::FAILED) return false;
    state = RequestState::FAILED;
    assigned_robot_id = -1;
    return true;
}

bool RescueRequest::perform_work(int time_units) {
    if (time_units <= 0) return false;
    if (state != RequestState::IN_PROGRESS) return false;

    if (time_units >= remaining_work) {
        remaining_work = 0;
    } else {
        remaining_work -= time_units;
    }
    // The controller explicitly calls complete() after the required work is done.
    return true;
}
