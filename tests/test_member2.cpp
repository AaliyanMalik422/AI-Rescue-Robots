#include "test_harness.hpp"
#include "collections.hpp"
#include "min_heap.hpp"
#include "event_queue.hpp"
#include "robot.hpp"
#include "rescue_request.hpp"
#include "dispatch_rules.hpp"
#include "logger.hpp"

// MEMBER 2 starter tests. They FAIL until you implement your modules.
// Add more: large arrays, copy independence, empty-structure edge cases,
// every illegal robot/request transition, memory behaviour (run with valgrind
// or -fsanitize=address if available).

static void test_dynamic_array() {
    DynamicArray<int> a;
    for (int i = 0; i < 100; ++i) a.push_back(i);
    REQUIRE(a.size() == 100);
    CHECK(a[0] == 0 && a[99] == 99);
    CHECK(a.push_back(a[0]));                       // aliasing across a resize
    REQUIRE(a.size() == 101);
    CHECK(a[100] == 0);
    CHECK(a.insert_at(0, -5));
    REQUIRE(a.size() == 102);
    CHECK(a[0] == -5 && a[1] == 0);
    CHECK(a.remove_at(0));
    REQUIRE(a.size() == 101);
    CHECK(a[0] == 0);
    CHECK(!a.remove_at(500));
    DynamicArray<int> copy(a);
    REQUIRE(copy.size() == a.size());
    copy[0] = 42;
    CHECK(a[0] == 0);                               // deep copy
    DynamicArray<int> assigned;
    assigned = a;
    CHECK(assigned.size() == a.size());
    CHECK(a.get(0) != 0 && a.get(-1) == 0 && a.get(a.size()) == 0);
    a.clear();
    CHECK(a.empty());
    CHECK(!a.pop_back());
}

static void test_linked_list() {
    LinkedList<int> list;
    list.push_back(2);
    list.push_back(3);
    list.push_front(1);
    REQUIRE(list.size() == 3 && list.get_head() != 0);
    CHECK(list.get_head()->data == 1);
    CHECK(list.find(3) != 0 && list.find(9) == 0);
    CHECK(list.pop_back() && list.size() == 2);
    LinkedList<int> copy(list);
    CHECK(list.pop_front());
    CHECK(copy.size() == 2);                        // deep copy
    list.clear();
    CHECK(list.empty() && !list.pop_front());
}

static void test_min_heap() {
    MinHeap heap;
    heap.reset(10);
    CHECK(heap.insert_or_decrease(3, 50));
    CHECK(heap.insert_or_decrease(1, 20));
    CHECK(heap.insert_or_decrease(5, 20));          // tie with item 1 -> lower item first
    CHECK(heap.insert_or_decrease(3, 5));           // decrease-key
    CHECK(heap.insert_or_decrease(1, 99));          // higher cost ignored
    CHECK(heap.size() == 3);
    PathNode node;
    CHECK(heap.extract_min(node) && node.item == 3 && node.cost == 5);
    CHECK(heap.extract_min(node) && node.item == 1 && node.cost == 20);
    CHECK(heap.extract_min(node) && node.item == 5);
    CHECK(!heap.extract_min(node) && heap.empty());
    CHECK(!heap.insert_or_decrease(10, 1));         // out of range
}

static void test_event_queue() {
    EventQueue queue;
    queue.push(Event(EVENT_BLOCK, 20, 1, 0, 0));
    queue.push(Event(EVENT_UNBLOCK, 5, 2, 0, 1));
    queue.push(Event(EVENT_BLOCK, 5, 3, 0, 0));
    Event e;
    CHECK(queue.pop(e) && e.timestamp == 5 && e.original_order == 0);
    CHECK(queue.pop(e) && e.timestamp == 5 && e.original_order == 1);
    CHECK(queue.peek(e) && e.timestamp == 20);
    CHECK(queue.pop(e) && queue.empty());
}

static void test_robot() {
    Robot r(1, 5, CAPABILITY_MEDICAL, 100, 100);
    CHECK(r.check_capability(CATEGORY_MEDICAL) && !r.check_capability(CATEGORY_FIRE));
    Robot general(2, 5, CAPABILITY_GENERAL, 50, 50);
    CHECK(general.check_capability(CATEGORY_SEARCH));
    CHECK(r.consume_battery(30) && r.battery_level == 70);
    CHECK(!r.consume_battery(71) && r.battery_level == 70);
    CHECK(r.charge(500) && r.battery_level == 100);
    CHECK(r.assign_request(9) && r.state == ROBOT_TRAVELLING);
    CHECK(!r.assign_request(10));                   // only one request at a time
    CHECK(!r.set_state(ROBOT_RECHARGING));          // illegal from TRAVELLING
    CHECK(r.set_state(ROBOT_PERFORMING));
    CHECK(r.go_offline() == 9 && r.state == ROBOT_OFFLINE && r.assigned_request_id == -1);
    CHECK(!r.set_state(ROBOT_TRAVELLING));          // offline robots cannot start travelling
    CHECK(r.go_online() && r.state == ROBOT_AVAILABLE);
}

static void test_request() {
    RescueRequest q(1, 10, CATEGORY_FIRE, 3, 4, 5);
    CHECK(q.state == REQUEST_UNREPORTED);
    CHECK(!q.assign(2, 4));                         // not pending yet
    CHECK(q.report() && q.state == REQUEST_PENDING);
    CHECK(q.assign(2, 6) && q.assigned_robot_id == 2);
    CHECK(!q.assign(3, 6));                         // only one robot at a time
    CHECK(q.start_work() && q.perform_work(2) && q.remaining_work == 3);
    CHECK(!q.complete(9));                          // work not finished
    CHECK(q.return_to_pending() && q.remaining_work == 5 && q.assigned_robot_id == -1);
    CHECK(q.assign(4, 10) && q.start_work() && q.perform_work(9) && q.remaining_work == 0);
    CHECK(q.complete(20) && q.state == REQUEST_COMPLETED && q.completion_time == 20);
    CHECK(!q.fail(FAIL_TIME_LIMIT));                // terminal
    CHECK(q.is_terminal());
    RescueRequest other(2, 10, CATEGORY_FIRE, 3, 0, 1);
    CHECK(other.set_urgency(5) && !other.set_urgency(6));
}

static void test_dispatch_rules() {
    DynamicArray<RescueRequest> requests;
    requests.push_back(RescueRequest(1, 5, CATEGORY_FIRE, 2, 10, 3));
    requests.push_back(RescueRequest(2, 5, CATEGORY_FIRE, 5, 20, 3));
    requests.push_back(RescueRequest(3, 5, CATEGORY_FIRE, 5, 10, 3));
    requests.push_back(RescueRequest(4, 5, CATEGORY_FIRE, 5, 10, 3));
    REQUIRE(requests.size() == 4);
    CHECK(request_has_priority(requests[1], requests[0]));     // urgency 5 beats 2
    CHECK(request_has_priority(requests[2], requests[1]));     // same urgency, earlier report
    CHECK(request_has_priority(requests[2], requests[3]));     // same time, lower ID
    DynamicArray<int> indices;
    for (int i = 0; i < 4; ++i) indices.push_back(i);
    sort_request_indices(requests, indices);
    REQUIRE(indices.size() == 4);
    CHECK(indices[0] == 2 && indices[1] == 3 && indices[2] == 1 && indices[3] == 0);
    CHECK(requests[0].id == 1);                                // records did not move
    CHECK(mission_battery_required(10, 4, 6, 1, 2) == 24);     // 10*1 + 4*2 + 6*1
    CHECK(mission_battery_required(10, 4, UNREACHABLE_COST, 1, 2) == -1);
    Robot robot(1, 1, CAPABILITY_FIRE, 100, 30);
    CHECK(battery_is_low(robot, 30));
    robot.battery_level = 31;
    CHECK(!battery_is_low(robot, 30));
}

static void test_logger() {
    Logger logger;
    logger.log(3, "ASSIGN", "request=1;robot=2", "cost=7");
    CHECK(logger.size() == 1);
    CHECK(logger.save_to_file("output/_test_log.txt"));
}

int main() {
    test_dynamic_array();
    test_linked_list();
    test_min_heap();
    test_event_queue();
    test_robot();
    test_request();
    test_dispatch_rules();
    test_logger();
    return finish_tests("Member 2");
}
