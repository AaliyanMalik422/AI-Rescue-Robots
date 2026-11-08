#include "../include/environment.hpp"

Environment::Environment() {}
Environment::~Environment() {}

int Environment::get_graph_index(int location_id) const {
    for (size_t i = 0; i < graph.size(); ++i) {
        if (graph[i].location_id == location_id) return static_cast<int>(i);
    }
    return -1;
}

bool Environment::add_location(int id, const std::string& name, bool has_station, bool is_station_available) {
    if (id < 0) return false;
    if (get_location(id) != nullptr) return false; // Duplicate check

    Location loc{id, name, has_station, is_station_available};
    locations.push_back(loc);

    AdjacencyList adj;
    adj.location_id = id;
    graph.push_back(adj);
    return true;
}

bool Environment::add_connection(int id, int from_id, int to_id, int distance, bool is_blocked) {
    if (id < 0 || distance <= 0) return false;
    if (get_connection(id) != nullptr) return false; // Duplicate connection check
    if (get_location(from_id) == nullptr || get_location(to_id) == nullptr) return false; // Invalid endpoints

    Connection conn{id, from_id, to_id, distance, is_blocked};
    connections.push_back(conn);

    int idx_from = get_graph_index(from_id);
    int idx_to = get_graph_index(to_id);

    if (idx_from != -1) graph[idx_from].neighbors.push_back({to_id, id});
    if (idx_to != -1) graph[idx_to].neighbors.push_back({from_id, id});

    return true;
}

Location* Environment::get_location(int id) {
    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i].id == id) return &locations[i];
    }
    return nullptr;
}

const Location* Environment::get_location(int id) const {
    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i].id == id) return &locations[i];
    }
    return nullptr;
}

Connection* Environment::get_connection(int id) {
    for (size_t i = 0; i < connections.size(); ++i) {
        if (connections[i].id == id) return &connections[i];
    }
    return nullptr;
}

const Connection* Environment::get_connection(int id) const {
    for (size_t i = 0; i < connections.size(); ++i) {
        if (connections[i].id == id) return &connections[i];
    }
    return nullptr;
}

DynamicArray<Environment::NeighborNode> Environment::get_neighbors(int location_id) const {
    int idx = get_graph_index(location_id);
    if (idx != -1) return graph[idx].neighbors;
    return DynamicArray<Environment::NeighborNode>();
}

bool Environment::set_connection_blocked(int id, bool blocked) {
    Connection* conn = get_connection(id);
    if (conn) {
        conn->is_blocked = blocked;
        return true;
    }
    return false;
}

bool Environment::is_connection_blocked(int id) const {
    const Connection* conn = get_connection(id);
    if (conn) return conn->is_blocked;
    return false;
}

bool Environment::set_station_available(int location_id, bool available) {
    Location* loc = get_location(location_id);
    if (loc && loc->has_charging_station) {
        loc->is_station_available = available;
        return true;
    }
    return false;
}

bool Environment::has_operational_station(int location_id) const {
    const Location* loc = get_location(location_id);
    if (loc) {
        return loc->has_charging_station && loc->is_station_available;
    }
    return false;
}

void Environment::clear() {
    locations.clear();
    connections.clear();
    graph.clear();
}
