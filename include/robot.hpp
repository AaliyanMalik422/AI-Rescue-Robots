#pragma once

#include "types.hpp"
#include "collections.hpp"

// =====================================================================
// MEMBER 2 - Robot entity: capability, battery and legal state changes.
// Member 3 drives movement/work; this class only guards the rules.
// =====================================================================
class Robot {
public:
    int id;
    int current_location_id;
    int capability;
    int state;
    int battery_level;
    int max_battery;
    int assigned_request_id;     // -1 when none

    // Route/movement bookkeeping filled and used by Member 3.
    DynamicArray<int> planned_route;        // location IDs, start..destination
    DynamicArray<int> planned_connections;  // connection ID of each hop (size = route size - 1)
    int next_route_index;                   // index in planned_route of the next stop
    // Mid-road state. A robot is "on a road" while it is between two locations.
    // This survives clear_route(), so an OFFLINE robot keeps its physical progress.
    int current_connection_id;              // road being crossed, or -1 when at a location
    int distance_on_current_connection;     // cost units already covered on that road
    int road_target_id;                     // location this road leads to, or -1
    int mission_target_id;
    int return_station_id;

    // Statistics inputs (design report 4.3 utilization).
    int busy_steps;       // steps spent TRAVELLING/PERFORMING
    int online_steps;     // steps not OFFLINE
    long long battery_used;

    Robot();
    Robot(int robot_id, int start_location, int robot_capability,
          int maximum_battery, int initial_battery);

    // DONE (provided): GENERAL serves any category; others only their own.
    // PSEUDOCODE: if capability == GENERAL: true
    //   MEDICAL<->CATEGORY_MEDICAL, FIRE<->FIRE, SUPPLY<->SUPPLY, SEARCH<->SEARCH: true; else false
    bool check_capability(int request_category) const;

    // DONE (provided): PSEUDOCODE: return amount >= 0 and battery_level >= amount
    bool can_afford(int amount) const;

    // DONE (provided): subtract; refuse if it would go below 0.
    // PSEUDOCODE: if amount < 0 or battery_level < amount: return false
    //   battery_level -= amount; battery_used += amount; return true
    bool consume_battery(int amount);

    // DONE (provided): add up to max_battery. Refuse amount <= 0.
    // PSEUDOCODE: if amount <= 0: return false; battery_level += amount;
    //   if battery_level > max_battery: battery_level = max_battery; return true
    bool charge(int amount);

    // TODO (Member 2): legal-transition table. Suggested:
    //   AVAILABLE   -> TRAVELLING, RECHARGING, RETURNING, UNAVAILABLE
    //   TRAVELLING  -> PERFORMING, RETURNING, AVAILABLE, UNAVAILABLE
    //   PERFORMING  -> RETURNING, AVAILABLE, UNAVAILABLE
    //   RETURNING   -> RECHARGING, AVAILABLE, UNAVAILABLE
    //   RECHARGING  -> AVAILABLE, RETURNING, UNAVAILABLE
    //   UNAVAILABLE -> AVAILABLE, RETURNING, RECHARGING
    //   any         -> OFFLINE;  OFFLINE -> AVAILABLE or UNAVAILABLE only.
    // Return false (state unchanged) for an illegal or unknown state.
    // PSEUDOCODE: if new_state not in 1..7: false; if new_state == state: true;
    //   if new_state == OFFLINE: state = OFFLINE; true;
    //   if state == OFFLINE: allowed only for AVAILABLE or UNAVAILABLE
    //   otherwise look new_state up in the table above; if allowed: state = new_state; true else false
    bool set_state(int new_state);

    // TODO (Member 2): only an AVAILABLE robot with no request may take one.
    // PSEUDOCODE: if request_id < 0 or state != ROBOT_AVAILABLE or assigned_request_id != -1: return false
    //   assigned_request_id = request_id; state = ROBOT_TRAVELLING; return true
    bool assign_request(int request_id);

    // DONE (provided): forget the request and clear the route; the caller (Member 3)
    // then chooses the new state with set_state.
    // PSEUDOCODE: assigned_request_id = -1; clear_route()
    void release_request();

    // TODO (Member 2): store a new route. Only legal while the robot is AT a location.
    // PSEUDOCODE:
    //   if route is empty, or connection_ids.size() != route.size() - 1, or is_on_road(): return false
    //   planned_route = route; planned_connections = connection_ids
    //   next_route_index = (route.size() > 1) ? 1 : route.size()      (index 0 is where we already are)
    //   mission_target_id = route[route.size() - 1]
    //   return true
    bool set_route(const DynamicArray<int>& route, const DynamicArray<int>& connection_ids);

    // DONE (provided): forget the planned route and targets, but NOT the mid-road fields.
    // PSEUDOCODE: planned_route.clear(); planned_connections.clear(); next_route_index = 0;
    //             mission_target_id = -1; return_station_id = -1
    void clear_route();

    // DONE (provided): true when current_connection_id != -1.
    bool is_on_road() const;

    // TODO (Member 2): ROBOT_OFFLINE event rules (design report 4.3).
    // PSEUDOCODE:
    //   old = assigned_request_id
    //   assigned_request_id = -1; clear_route()        (keeps battery, location and mid-road progress)
    //   state = ROBOT_OFFLINE                          (set directly: OFFLINE is legal from every state)
    //   return old                                     (Member 3 returns that request to PENDING)
    int go_offline();

    // DONE (provided): OFFLINE -> AVAILABLE (never resumes an old task).
    // PSEUDOCODE: if state != ROBOT_OFFLINE: return false;  state = ROBOT_AVAILABLE; return true
    // Member 3 then decides RECHARGING / UNAVAILABLE / keep crossing the road.
    bool go_online();
};
