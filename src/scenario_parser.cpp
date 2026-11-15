#include "scenario_parser.hpp"
#include "line_reader.hpp"  // provided: read_content_line, split_tokens
#include "names.hpp"       // provided: parse_location_type, parse_capability, parse_category,
                           //           parse_event_type, parse_int
#include <fstream>
#include <sstream>

// MEMBER 1: Appendix A parser + validation (design report section 5).
// Rules for every error: stop at the FIRST problem, set error_message to
// "<SECTION> record <n>: <what is wrong>", return false. Never crash on bad input.
//
// Already provided: read_content_line and split_tokens (line_reader.hpp), parse_int and the
// word-to-constant functions (names.hpp).
//
// Helper functions you will write in this file (declare them above parse_scenario):
//
//   bool read_section_header(file, "LOCATIONS", count, error_message)
//     PSEUDOCODE: read_content_line; split into tokens; require exactly 2 tokens,
//                 tokens[0] == expected keyword, tokens[1] parses as an int >= 0 -> count
//
//   parse_locations / parse_connections / parse_robots / parse_requests / parse_events:
//     PSEUDOCODE (same shape for all five):
//       read the header -> count
//       repeat count times:
//           if read_content_line fails: error "fewer records than declared"
//           split into tokens; check the token count and every field (list below)
//           build the object and store it (environment.add_location, robots.push_back, ...)
//       afterwards the NEXT content line must be a header or END; if it still looks like
//       a record of this section, the count was too small: error
//
// Field checks (reject with a message naming section and record):
//   PARAM      each of max_steps, speed, battery_per_cost_unit, battery_per_work_step,
//              recharge_per_step present exactly once; max_steps > 0, speed > 0, others >= 0;
//              unknown key or duplicate key -> error
//   LOCATIONS  <id> <name> <type>: id 1..MAX_ID and unique; type via parse_location_type != 0
//   CONNECTIONS <id> <a> <b> <cost> <OPEN|BLOCKED>: id unique, a and b exist and differ,
//              cost > 0, status word is exactly OPEN or BLOCKED
//   ROBOTS     <id> <start> <capability> <max> <initial>: id unique, start exists,
//              capability != 0, max > 0, 0 <= initial <= max
//   REQUESTS   <id> <time> <location> <category> <urgency> <duration>: id unique,
//              location exists, category != 0, urgency 1..5, duration > 0,
//              0 <= time < max_steps
//   EVENTS     <time> <TYPE> <target> [<value>]: 0 <= time < max_steps;
//              BLOCK/UNBLOCK target = existing connection id; STATION_* target = existing
//              location of type CHARGING; SET_URGENCY needs a 4th token (urgency 1..5) and an
//              existing request id; ROBOT_* target = existing robot id; any other token count = error
//              Each event gets original_order = its position in the file (0, 1, 2, ...).
//
// Whole-file rules:
//   first content line: "SCENARIO <name>" -> settings.scenario_name
//   then PARAM lines until the first section header (PARAM has no count)
//   sections in the fixed order LOCATIONS, CONNECTIONS, ROBOTS, REQUESTS, EVENTS
//   last content line must be END, and nothing but comments/blank lines may follow
//   the file must open; otherwise error "cannot open file"
// The provided scenario_provided.txt must load unchanged:
//   56 locations, 117 connections (7 BLOCKED), 12 robots, 60 requests, 20 events.
// A BLOCKED connection is stored with is_blocked = true; robots start with
// state AVAILABLE and request state UNREPORTED (the constructors already do this).

bool parse_scenario(const std::string& file_path,
                    Environment& environment,
                    DynamicArray<Robot>& robots,
                    DynamicArray<RescueRequest>& requests,
                    DynamicArray<Event>& events,
                    SimulationSettings& settings,
                    std::string& error_message) {
    // TODO (Member 1): follow the pseudocode and the field checks in the comment above.
    (void)file_path; (void)environment; (void)robots;
    (void)requests; (void)events; (void)settings;
    error_message = "parse_scenario is not implemented yet";
    return false;
}
