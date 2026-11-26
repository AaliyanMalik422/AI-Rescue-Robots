#include "dispatch_rules.hpp"

bool request_has_priority(const RescueRequest& first, const RescueRequest& second) {

    if (first.urgency != second.urgency)
        return first.urgency > second.urgency;

    if (first.report_time != second.report_time)
        return first.report_time < second.report_time;

    return first.id < second.id;
}

void sort_request_indices(const DynamicArray<RescueRequest>& requests,
                          DynamicArray<int>& indices) {

    int n = indices.size();

    if (n < 2)
        return;

    DynamicArray<int> tmp;

    for (int i = 0; i < n; ++i)
        tmp.push_back(0);

    for (int wid = 1; wid < n; wid *= 2) {

        for (int lo = 0; lo < n; lo += 2 * wid) {

            int mid = lo + wid;
            int hi = lo + 2 * wid;

            if (mid > n)
                mid = n;
            if (hi > n)
                hi = n;

            int i = lo;
            int j = mid;
            int k = lo;

            while (i < mid && j < hi) {

                if (request_has_priority(requests[indices[j]], requests[indices[i]])) {
                    tmp[k] = indices[j];
                    j++;
                }
                else {
                    tmp[k] = indices[i];
                    i++;
                }
                k++;
            }

            while (i < mid) {
                tmp[k] = indices[i];
                i++;
                k++;
            }

            while (j < hi) {
                tmp[k] = indices[j];
                j++;
                k++;
            }
        }

        for (int i = 0; i < n; ++i)
            indices[i] = tmp[i];
    }
}

long long mission_battery_required(long long outbound_cost, int duration_steps,
                                   long long station_cost_after_task,
                                   int battery_per_cost_unit, int battery_per_work_step) {

    if (station_cost_after_task >= UNREACHABLE_COST)
        return -1;

    return outbound_cost * (long long)battery_per_cost_unit
         + (long long)duration_steps * (long long)battery_per_work_step
         + station_cost_after_task * (long long)battery_per_cost_unit;
}

bool battery_is_low(const Robot& robot, int low_percent) {
    return (long long)robot.battery_level * 100 <= (long long)low_percent * robot.max_battery;
}
