#pragma once

#include "types.hpp"

// =====================================================================
// MEMBER 2 - Rescue request lifecycle.
//
//   UNREPORTED -> PENDING -> ASSIGNED -> IN_PROGRESS -> COMPLETED
//                    ^____________|______________|
//                         (return_to_pending)
//   any non-terminal state -> FAILED
//
// Invariant: a request has at most one assigned robot; COMPLETED/FAILED
// are terminal.
// =====================================================================
class RescueRequest {
public:
    int id;
    int location_id;
    int category;
    int urgency;
    int report_time;
    int duration_steps;
    int remaining_work;
    int state;
    int assigned_robot_id;     // -1 when none
    int completion_time;       // -1 until completed
    int fail_reason;           // FAIL_NONE unless FAILED
    int first_assigned_time;   // -1 until first assigned (response-time data)
    int times_reassigned;

    RescueRequest();
    RescueRequest(int request_id, int location, int request_category,
                  int request_urgency, int time_reported, int work_duration);

    // Each method returns true only for a legal transition and leaves the request untouched
    // otherwise. report, start_work, set_urgency and is_terminal are provided; the rest are
    // TODO (Member 2).
    bool report();                        // UNREPORTED -> PENDING
    bool assign(int robot_id, int time);  // PENDING -> ASSIGNED (sets robot, first_assigned_time)
    bool start_work();                    // ASSIGNED -> IN_PROGRESS
    bool perform_work(int time_units);    // IN_PROGRESS: reduce remaining_work, never below 0
    bool complete(int time);              // IN_PROGRESS with remaining_work == 0 -> COMPLETED
    bool fail(int reason);                // any non-terminal -> FAILED
    bool return_to_pending();             // ASSIGNED/IN_PROGRESS -> PENDING, reset remaining_work
                                          // to duration_steps, clear robot, ++times_reassigned
    bool set_urgency(int new_urgency);    // 1..5, only while not terminal
    bool is_terminal() const;             // COMPLETED or FAILED
};
