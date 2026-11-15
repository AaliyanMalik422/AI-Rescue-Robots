#include "dispatch_rules.hpp"

// MEMBER 2: implement the rules declared in dispatch_rules.hpp.

bool request_has_priority(const RescueRequest& first, const RescueRequest& second) {
    // TODO (Member 2)
    // PSEUDOCODE: if first.urgency != second.urgency: return first.urgency > second.urgency
    //             if first.report_time != second.report_time: return first.report_time < second.report_time
    //             return first.id < second.id
    (void)first; (void)second;
    return false;
}

void sort_request_indices(const DynamicArray<RescueRequest>& requests,
                          DynamicArray<int>& indices) {
    // TODO (Member 2)
    // PSEUDOCODE (insertion sort, O(n^2), simplest; merge sort for O(n log n)):
    //   for i from 1 to indices.size()-1:
    //       key = indices[i]; j = i - 1
    //       while j >= 0 and request_has_priority(requests[key], requests[indices[j]]):
    //           indices[j+1] = indices[j]; j = j - 1
    //       indices[j+1] = key
    // Merge sort alternative: split in halves, sort each, merge into a temporary
    // DynamicArray<int> taking the higher-priority front element each time.
    (void)requests; (void)indices;
}

long long mission_battery_required(long long outbound_cost, int duration_steps,
                                   long long station_cost_after_task,
                                   int battery_per_cost_unit, int battery_per_work_step) {
    // TODO (Member 2)
    // PSEUDOCODE: if station_cost_after_task >= UNREACHABLE_COST: return -1
    //   return outbound_cost * battery_per_cost_unit
    //        + duration_steps * battery_per_work_step
    //        + station_cost_after_task * battery_per_cost_unit      (all as long long)
    (void)outbound_cost; (void)duration_steps; (void)station_cost_after_task;
    (void)battery_per_cost_unit; (void)battery_per_work_step;
    return -1;
}

bool battery_is_low(const Robot& robot, int low_percent) {
    // DONE (provided)
    return (long long)robot.battery_level * 100 <= (long long)low_percent * robot.max_battery;
}
