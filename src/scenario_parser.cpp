#include "scenario_parser.hpp"
#include "line_reader.hpp"
#include "names.hpp"
#include <fstream>
#include <sstream>

static bool next_line(std::ifstream& file, std::string& line) {
    return read_content_line(file, line);
}

static bool get_header(std::ifstream& file, const char* expected, int& count,
                       std::string& error, int section_number) {
    std::string line, tokens[4];
    if (!next_line(file, line)) {
        error = std::string(expected) + " record 1: missing section header";
        return false;
    }
    int n = split_tokens(line, tokens, 4);
    if (n != 2 || tokens[0] != expected || !parse_int(tokens[1], count) || count < 0) {
        error = std::string(expected) + " record 1: invalid section header";
        return false;
    }
    (void)section_number;
    return true;
}

static bool duplicate_robot(const DynamicArray<Robot>& robots, int id) {
    for (int i = 0; i < robots.size(); ++i) if (robots[i].id == id) return true;
    return false;
}
static bool duplicate_request(const DynamicArray<RescueRequest>& requests, int id) {
    for (int i = 0; i < requests.size(); ++i) if (requests[i].id == id) return true;
    return false;
}
static bool has_robot(const DynamicArray<Robot>& robots, int id) {
    for (int i = 0; i < robots.size(); ++i) if (robots[i].id == id) return true;
    return false;
}
static bool has_request(const DynamicArray<RescueRequest>& requests, int id) {
    for (int i = 0; i < requests.size(); ++i) if (requests[i].id == id) return true;
    return false;
}

bool parse_scenario(const std::string& file_path,
                    Environment& environment,
                    DynamicArray<Robot>& robots,
                    DynamicArray<RescueRequest>& requests,
                    DynamicArray<Event>& events,
                    SimulationSettings& settings,
                    std::string& error_message) {
    error_message = "";
    environment.clear();
    robots.clear();
    requests.clear();
    events.clear();
    settings = SimulationSettings();

    std::ifstream file(file_path.c_str());
    if (!file.is_open()) {
        error_message = "SCENARIO record 1: cannot open file";
        return false;
    }

    std::string line, tokens[8];
    if (!next_line(file, line)) {
        error_message = "SCENARIO record 1: missing scenario name";
        return false;
    }
    int n = split_tokens(line, tokens, 8);
    if (n != 2 || tokens[0] != "SCENARIO") {
        error_message = "SCENARIO record 1: expected SCENARIO <name>";
        return false;
    }
    settings.scenario_name = tokens[1];

    bool seen_max = false, seen_speed = false, seen_cost = false;
    bool seen_work = false, seen_recharge = false;
    bool have_next = next_line(file, line);
    while (have_next) {
        n = split_tokens(line, tokens, 8);
        if (n > 0 && tokens[0] == "PARAM") {
            int value = 0;
            if (n != 3 || !parse_int(tokens[2], value)) {
                error_message = "PARAM record: expected PARAM <key> <integer>";
                return false;
            }
            if (tokens[1] == "max_steps") {
                if (seen_max || value <= 0) { error_message = "PARAM record: duplicate or invalid max_steps"; return false; }
                settings.max_steps = value; seen_max = true;
            } else if (tokens[1] == "speed") {
                if (seen_speed || value <= 0) { error_message = "PARAM record: duplicate or invalid speed"; return false; }
                settings.speed = value; seen_speed = true;
            } else if (tokens[1] == "battery_per_cost_unit") {
                if (seen_cost || value < 0) { error_message = "PARAM record: duplicate or invalid battery_per_cost_unit"; return false; }
                settings.battery_per_cost_unit = value; seen_cost = true;
            } else if (tokens[1] == "battery_per_work_step") {
                if (seen_work || value < 0) { error_message = "PARAM record: duplicate or invalid battery_per_work_step"; return false; }
                settings.battery_per_work_step = value; seen_work = true;
            } else if (tokens[1] == "recharge_per_step") {
                if (seen_recharge || value < 0) { error_message = "PARAM record: duplicate or invalid recharge_per_step"; return false; }
                settings.recharge_per_step = value; seen_recharge = true;
            } else {
                error_message = "PARAM record: unknown parameter " + tokens[1];
                return false;
            }
            have_next = next_line(file, line);
        } else break;
    }
    if (!seen_max || !seen_speed || !seen_cost || !seen_work || !seen_recharge) {
        error_message = "PARAM record: one or more required parameters are missing";
        return false;
    }
    if (!have_next) { error_message = "LOCATIONS record 1: missing section"; return false; }

    int count = 0, value = 0;
    n = split_tokens(line, tokens, 8);
    if (n != 2 || tokens[0] != "LOCATIONS" || !parse_int(tokens[1], count) || count < 0) {
        error_message = "LOCATIONS record 1: invalid section header"; return false;
    }
    for (int i = 0; i < count; ++i) {
        if (!next_line(file, line)) { error_message = "LOCATIONS record " + int_to_string(i + 1) + ": fewer records than declared"; return false; }
        n = split_tokens(line, tokens, 8);
        int id = 0;
        if (n != 3 || !parse_int(tokens[0], id) || id < 1 || id > MAX_ID ||
            parse_location_type(tokens[2]) == 0) {
            error_message = "LOCATIONS record " + int_to_string(i + 1) + ": invalid ID, name, or location type"; return false;
        }
        if (!environment.add_location(Location(id, tokens[1], parse_location_type(tokens[2])))) {
            error_message = "LOCATIONS record " + int_to_string(i + 1) + ": duplicate or invalid location ID"; return false;
        }
    }

    if (!get_header(file, "CONNECTIONS", count, error_message, 0)) return false;
    for (int i = 0; i < count; ++i) {
        if (!next_line(file, line)) { error_message = "CONNECTIONS record " + int_to_string(i + 1) + ": fewer records than declared"; return false; }
        n = split_tokens(line, tokens, 8);
        int id, a, b, cost;
        if (n != 5 || !parse_int(tokens[0], id) || !parse_int(tokens[1], a) ||
            !parse_int(tokens[2], b) || !parse_int(tokens[3], cost) ||
            id < 1 || id > MAX_ID || cost <= 0 ||
            (tokens[4] != "OPEN" && tokens[4] != "BLOCKED")) {
            error_message = "CONNECTIONS record " + int_to_string(i + 1) + ": invalid connection fields"; return false;
        }
        if (!environment.add_connection(Connection(id, a, b, cost, tokens[4] == "BLOCKED"))) {
            error_message = "CONNECTIONS record " + int_to_string(i + 1) + ": duplicate ID, self-loop, or unknown endpoint"; return false;
        }
    }

    if (!get_header(file, "ROBOTS", count, error_message, 0)) return false;
    for (int i = 0; i < count; ++i) {
        if (!next_line(file, line)) { error_message = "ROBOTS record " + int_to_string(i + 1) + ": fewer records than declared"; return false; }
        n = split_tokens(line, tokens, 8);
        int id, start, maximum, initial;
        int capability = (n == 5) ? parse_capability(tokens[2]) : 0;
        if (n != 5 || !parse_int(tokens[0], id) || !parse_int(tokens[1], start) ||
            !parse_int(tokens[3], maximum) || !parse_int(tokens[4], initial) ||
            id < 1 || id > MAX_ID || environment.get_location(start) == 0 ||
            capability == 0 || maximum <= 0 || initial < 0 || initial > maximum) {
            error_message = "ROBOTS record " + int_to_string(i + 1) + ": invalid robot fields"; return false;
        }
        if (duplicate_robot(robots, id)) { error_message = "ROBOTS record " + int_to_string(i + 1) + ": duplicate robot ID"; return false; }
        robots.push_back(Robot(id, start, capability, maximum, initial));
    }

    if (!get_header(file, "REQUESTS", count, error_message, 0)) return false;
    for (int i = 0; i < count; ++i) {
        if (!next_line(file, line)) { error_message = "REQUESTS record " + int_to_string(i + 1) + ": fewer records than declared"; return false; }
        n = split_tokens(line, tokens, 8);
        int id, time, location, urgency, duration;
        int category = (n == 6) ? parse_category(tokens[3]) : 0;
        if (n != 6 || !parse_int(tokens[0], id) || !parse_int(tokens[1], time) ||
            !parse_int(tokens[2], location) || !parse_int(tokens[4], urgency) ||
            !parse_int(tokens[5], duration) || id < 1 || id > MAX_ID ||
            environment.get_location(location) == 0 || category == 0 ||
            urgency < 1 || urgency > 5 || duration <= 0 ||
            time < 0 || time >= settings.max_steps) {
            error_message = "REQUESTS record " + int_to_string(i + 1) + ": invalid request fields"; return false;
        }
        if (duplicate_request(requests, id)) { error_message = "REQUESTS record " + int_to_string(i + 1) + ": duplicate request ID"; return false; }
        requests.push_back(RescueRequest(id, location, category, urgency, time, duration));
    }

    if (!get_header(file, "EVENTS", count, error_message, 0)) return false;
    for (int i = 0; i < count; ++i) {
        if (!next_line(file, line)) { error_message = "EVENTS record " + int_to_string(i + 1) + ": fewer records than declared"; return false; }
        n = split_tokens(line, tokens, 8);
        int time, target, event_type;
        if (n < 3 || n > 4 || !parse_int(tokens[0], time) || !parse_int(tokens[2], target) ||
            time < 0 || time >= settings.max_steps) {
            error_message = "EVENTS record " + int_to_string(i + 1) + ": invalid event fields"; return false;
        }
        event_type = parse_event_type(tokens[1]);
        int event_value = 0;
        if (event_type == 0) { error_message = "EVENTS record " + int_to_string(i + 1) + ": unknown event type"; return false; }

        bool valid = false;
        if (event_type == EVENT_BLOCK || event_type == EVENT_UNBLOCK) {
            valid = n == 3 && environment.get_connection(target) != 0;
        } else if (event_type == EVENT_STATION_DOWN || event_type == EVENT_STATION_UP) {
            const Location* loc = environment.get_location(target);
            valid = n == 3 && loc != 0 && loc->type == LOCATION_CHARGING;
        } else if (event_type == EVENT_SET_URGENCY) {
            valid = n == 4 && parse_int(tokens[3], event_value) &&
                    event_value >= 1 && event_value <= 5 && has_request(requests, target);
        } else if (event_type == EVENT_ROBOT_OFFLINE || event_type == EVENT_ROBOT_ONLINE) {
            valid = n == 3 && has_robot(robots, target);
        }
        if (!valid) { error_message = "EVENTS record " + int_to_string(i + 1) + ": invalid target, value, or token count"; return false; }
        events.push_back(Event(event_type, time, target, event_value, i));
    }

    if (!next_line(file, line) || line != "END") {
        error_message = "END record 1: missing END marker or incorrect section count";
        return false;
    }
    if (next_line(file, line)) {
        error_message = "END record 1: unexpected content after END";
        return false;
    }
    return true;
}
