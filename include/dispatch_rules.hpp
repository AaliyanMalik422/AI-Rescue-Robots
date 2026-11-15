#pragma once

#include "collections.hpp"
#include "rescue_request.hpp"
#include "robot.hpp"

// =====================================================================
// MEMBER 2 - Pure decision rules used by Member 3's dispatcher.
// No simulation state is touched here: given values in, answer out. That
// keeps them easy to test on their own.
// =====================================================================

// TODO (Member 2): true if `first` must be served before `second`:
// higher urgency first, then earlier report_time, then lower request ID.
bool request_has_priority(const RescueRequest& first, const RescueRequest& second);

// TODO (Member 2): sort `indices` (indices into `requests`) with YOUR OWN sort
// (insertion or merge sort; no <algorithm>) using request_has_priority.
// Request records must not move. Equal-priority cannot occur (IDs are unique).
// Complexity target: O(Q log Q) (merge sort) - document what you chose.
void sort_request_indices(const DynamicArray<RescueRequest>& requests,
                          DynamicArray<int>& indices);

// TODO (Member 2): battery needed for a whole mission (design report 4.2):
//   outbound_cost * per_cost_unit + duration * per_work_step + station_cost * per_cost_unit
// Return -1 if station_cost == UNREACHABLE_COST (no station reachable after the task).
// Must not overflow: use long long.
long long mission_battery_required(long long outbound_cost, int duration_steps,
                                   long long station_cost_after_task,
                                   int battery_per_cost_unit, int battery_per_work_step);

// DONE (provided): true if battery_level * 100 <= low_percent * max_battery
// (design report: low-battery threshold is 30 percent).
bool battery_is_low(const Robot& robot, int low_percent);
