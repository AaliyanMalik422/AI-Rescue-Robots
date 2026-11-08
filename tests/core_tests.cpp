#include <iostream>
#include "collections.hpp"
#include "environment.hpp"
#include "min_heap.hpp"
#include "pathfinder.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"

int failures = 0;

void check(bool condition, const char* test_name) {
    if (condition) {
        std::cout << "[PASS] " << test_name << '\n';
    } else {
        std::cout << "[FAIL] " << test_name << '\n';
        ++failures;
    }
}

void test_dynamic_array() {
    DynamicArray<int> values;
    values.push_back(10);
    values.push_back(20);
    values.push_back(30);
    values.push_back(40);

    // This tests the aliasing case: appending an element while the array is full
    // forces a resize, but the source reference must remain valid.
    bool alias_append = values.push_back(values[0]);
    check(alias_append && values.size() == 5 && values[4] == 10,
          "DynamicArray append and self-alias during resize");

    check(values.remove_at(1) && values.size() == 4 && values[1] == 30,
          "DynamicArray remove_at");
    check(values.pop_back() && values.size() == 3,
          "DynamicArray pop_back");

    DynamicArray<int> copied(values);
    check(copied.size() == values.size() && copied[0] == values[0],
          "DynamicArray copy constructor");

    copied[0] = 99;
    check(values[0] == 10 && copied[0] == 99,
          "DynamicArray copies do not share storage");

    DynamicArray<int> assigned;
    assigned = values;
    check(assigned.size() == values.size() && assigned[2] == values[2],
          "DynamicArray copy assignment");

    assigned.clear();
    check(assigned.empty() && assigned.capacity() >= 4,
          "DynamicArray clear keeps reusable capacity");
}

void test_linked_list() {
    LinkedList<int> values;
    values.push_back(5);
    values.push_back(8);
    values.push_front(2);

    const ListNode<int>* first = values.get_head();
    check(values.size() == 3 && first != 0 && first->data == 2 &&
          first->next != 0 && first->next->data == 5,
          "LinkedList insertion and traversal");

    LinkedList<int> copied(values);
    check(copied.size() == 3 && copied.get_head()->data == 2,
          "LinkedList copy constructor");

    values.pop_front();
    check(values.size() == 2 && values.get_head()->data == 5 &&
          copied.size() == 3,
          "LinkedList independent copy and pop_front");

    values.pop_back();
    check(values.size() == 1 && values.get_head()->data == 5,
          "LinkedList pop_back");

    LinkedList<int> assigned;
    assigned = copied;
    check(assigned.size() == 3 && assigned.get_head()->data == 2,
          "LinkedList copy assignment");
    assigned.clear();
    check(assigned.empty(), "LinkedList clear");
}

void test_environment_and_pathfinder() {
    Environment environment;

    Location one(1, "One", LOCATION_JUNCTION);
    Location two(2, "Two", LOCATION_BUILDING);
    Location three(3, "Three", LOCATION_CHARGING);
    Location four(4, "Four", LOCATION_HOSPITAL);

    check(environment.add_location(one), "Environment add first location");
    check(environment.add_location(two), "Environment add second location");
    check(environment.add_location(three), "Environment add third location");
    check(environment.add_location(four), "Environment add fourth location");
    check(!environment.add_location(one), "Environment rejects duplicate location ID");

    // Force the top-level arrays to grow beyond their initial capacity.
    for (int id = 5; id <= 9; ++id) {
        Location extra(id, "Extra", LOCATION_JUNCTION);
        check(environment.add_location(extra), "Environment handles dynamic-array growth");
    }

    check(environment.add_connection(Connection(1, 1, 2, 5, false)),
          "Environment add connection");
    check(environment.add_connection(Connection(2, 1, 3, 2, false)),
          "Environment add second connection");
    check(environment.add_connection(Connection(3, 3, 2, 1, false)),
          "Environment add third connection");
    check(environment.add_connection(Connection(4, 2, 4, 3, false)),
          "Environment add fourth connection");
    check(!environment.add_connection(Connection(5, 1, 99, 2, false)),
          "Environment rejects connection with missing endpoint");

    const LinkedList<NeighborNode>* neighbors = environment.get_neighbors(1);
    check(neighbors != 0 && neighbors->size() == 2,
          "Environment stores undirected adjacency lists");

    PathResult shortest = Pathfinder::find_shortest_path(environment, 1, 4);
    check(shortest.exists && shortest.total_cost == 6 && shortest.route.size() == 4 &&
          shortest.route[0] == 1 && shortest.route[1] == 3 &&
          shortest.route[2] == 2 && shortest.route[3] == 4,
          "Dijkstra finds the lowest-cost route");

    environment.set_connection_blocked(3, true);
    PathResult rerouted = Pathfinder::find_shortest_path(environment, 1, 4);
    check(rerouted.exists && rerouted.total_cost == 8,
          "Dijkstra ignores a blocked connection");

    environment.set_connection_blocked(1, true);
    environment.set_connection_blocked(2, true);
    environment.set_connection_blocked(4, true);
    PathResult unreachable = Pathfinder::find_shortest_path(environment, 1, 4);
    check(!unreachable.exists, "Dijkstra reports unreachable destination");

    check(environment.has_operational_station(3), "Charging station initially operational");
    check(environment.set_station_available(3, false) &&
          !environment.has_operational_station(3),
          "Environment updates charging station availability");
    check(!environment.set_station_available(2, false),
          "Environment rejects station update at non-charging location");
}

void test_min_heap() {
    MinHeap heap;
    heap.insert(PathNode(8, 10));
    heap.insert(PathNode(5, 4));
    heap.insert(PathNode(3, 4));
    heap.insert(PathNode(1, 20));

    PathNode result;
    bool first = heap.extract_min(result) && result.location_id == 3 && result.cost == 4;
    bool second = heap.extract_min(result) && result.location_id == 5 && result.cost == 4;
    bool third = heap.extract_min(result) && result.location_id == 8 && result.cost == 10;
    bool fourth = heap.extract_min(result) && result.location_id == 1 && result.cost == 20;
    check(first && second && third && fourth && heap.empty(),
          "MinHeap order and deterministic tie-breaking");
    check(!heap.extract_min(result), "MinHeap safely handles empty extraction");
}

void test_entities() {
    Robot robot(7, 2, CAPABILITY_MEDICAL, 100, 80);
    check(robot.check_capability(CATEGORY_MEDICAL) &&
          !robot.check_capability(CATEGORY_FIRE),
          "Robot capability matching");
    check(robot.consume_battery(20) && robot.battery_level == 60,
          "Robot battery consumption");
    check(!robot.consume_battery(61), "Robot rejects battery over-consumption");
    check(robot.charge(70) && robot.battery_level == 100,
          "Robot charging respects maximum battery");
    check(robot.assign_request(12) && robot.state == ROBOT_MOVING,
          "Robot accepts valid request assignment");
    check(!robot.assign_request(13), "Robot rejects second simultaneous assignment");
    robot.release_request();
    check(robot.state == ROBOT_IDLE && robot.assigned_request_id == -1,
          "Robot releases an assignment");
    check(robot.assign_request(14), "Robot accepts assignment before offline event");
    check(robot.set_state(ROBOT_OFFLINE) && robot.assigned_request_id == -1 &&
          !robot.set_state(ROBOT_MOVING) && robot.set_state(ROBOT_IDLE),
          "Robot releases assignment and handles offline/online transitions");

    RescueRequest request(12, 9, CATEGORY_MEDICAL, 5, 0, 2);
    check(request.assign(7) && request.start_work(),
          "Rescue request assignment and start");
    check(request.perform_work(1) && request.remaining_work == 1,
          "Rescue request tracks partial work");
    check(request.perform_work(5) && request.remaining_work == 0 && request.complete(),
          "Rescue request completion");
    check(!request.fail(), "Rescue request rejects failure after completion");

    RescueRequest reassigned(13, 8, CATEGORY_SEARCH, 3, 1, 4);
    check(reassigned.assign(7) && reassigned.return_to_pending() &&
          reassigned.state == REQUEST_PENDING && reassigned.assigned_robot_id == -1,
          "Rescue request returns to pending for reassignment");
}

int main() {
    std::cout << "Running AI Rescue Robots foundation tests...\n";
    test_dynamic_array();
    test_linked_list();
    test_environment_and_pathfinder();
    test_min_heap();
    test_entities();

    if (failures != 0) {
        std::cout << "\n" << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout << "\nAll foundation tests passed.\n";
    return 0;
}
