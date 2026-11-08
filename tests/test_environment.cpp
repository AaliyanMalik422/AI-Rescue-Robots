#include <iostream>
#include "../include/environment.hpp"

int run_environment_tests() {
    int fails = 0;
    Environment env;

    // Add locations
    if (!env.add_location(1, "Alpha", true, true)) fails++;
    if (!env.add_location(2, "Beta", false, false)) fails++;
    if (env.add_location(1, "Duplicate", false, false)) fails++; // Duplicate location ID rejected

    // Add connections
    if (!env.add_connection(101, 1, 2, 5, false)) fails++;
    if (env.add_connection(101, 1, 2, 5, false)) fails++; // Duplicate connection ID rejected
    if (env.add_connection(102, 1, 99, 5, false)) fails++; // Non-existent endpoint 99 rejected
    if (env.add_connection(103, 1, 2, -10, false)) fails++; // Negative distance rejected

    // Operational stations
    if (!env.has_operational_station(1)) fails++;
    if (!env.set_station_available(1, false)) fails++;
    if (env.has_operational_station(1)) fails++;
    if (env.set_station_available(2, true)) fails++; // Location 2 has no station

    // Blocked road operations
    if (env.is_connection_blocked(101)) fails++;
    if (!env.set_connection_blocked(101, true)) fails++;
    if (!env.is_connection_blocked(101)) fails++;

    if (fails) std::cerr << "[FAIL] Environment tests (" << fails << " failures)." << std::endl;
    else std::cout << "[PASS] Environment tests." << std::endl;

    return fails > 0 ? 1 : 0;
}
