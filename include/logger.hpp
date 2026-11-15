#pragma once

#include <string>
#include "collections.hpp"

// =====================================================================
// MEMBER 2 - Chronological simulation record (CSV-style, design report 5).
//
// One line per event:  time,event,ids,detail
// e.g.                 42,ASSIGN,request=7;robot=3,route_cost=12;battery=80
// Member 3 decides WHAT to log and when; you implement the container and file output.
// Must be enough for an evaluator to reconstruct: robot/request state
// changes, assignments, completions, movement/route events, failures and
// rejected actions, environment changes and battery changes.
// =====================================================================
class Logger {
private:
    DynamicArray<std::string> entries;
    bool echo_to_console;

public:
    Logger();

    // DONE (provided): store "time,event,ids,detail" (and print it if echo is on).
    void log(int time, const std::string& event,
             const std::string& ids, const std::string& detail);

    // TODO (Member 2): header line first, then every entry. False if the file cannot be written.
    bool save_to_file(const std::string& filename) const;

    void set_echo(bool enabled) { echo_to_console = enabled; }
    int size() const { return entries.size(); }
    void clear() { entries.clear(); }
};
