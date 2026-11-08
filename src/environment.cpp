#include "environment.hpp"

namespace {
bool valid_location_type(int type) {
    return type == LOCATION_JUNCTION || type == LOCATION_BUILDING ||
           type == LOCATION_HOSPITAL || type == LOCATION_DEPOT ||
           type == LOCATION_CHARGING || type == LOCATION_COMMAND;
}
}

Environment::Environment() {}

int Environment::get_graph_index(int location_id) const {
    for (int i = 0; i < graph.size(); ++i) {
        if (graph[i].location_id == location_id) return i;
    }
    return -1;
}

bool Environment::add_location(const Location& location) {
    if (location.id < 0 || !valid_location_type(location.type)) return false;
    if (get_location(location.id) != 0) return false;

    AdjacencyList adjacency;
    adjacency.location_id = location.id;

    if (!locations.push_back(location)) return false;
    if (!graph.push_back(adjacency)) {
        locations.pop_back();
        return false;
    }
    return true;
}

bool Environment::add_connection(const Connection& connection) {
    if (connection.id < 0 || connection.distance <= 0) return false;
    if (connection.endpoint1_id < 0 || connection.endpoint2_id < 0) return false;
    if (connection.endpoint1_id == connection.endpoint2_id) return false;
    if (get_connection(connection.id) != 0) return false;

    int first_index = get_graph_index(connection.endpoint1_id);
    int second_index = get_graph_index(connection.endpoint2_id);
    if (first_index < 0 || second_index < 0) return false;

    if (!connections.push_back(connection)) return false;

    // Appendix A connections are undirected, so store both directions.
    NeighborNode first_neighbor(connection.endpoint2_id, connection.id);
    NeighborNode second_neighbor(connection.endpoint1_id, connection.id);
    graph[first_index].neighbors.push_back(first_neighbor);
    graph[second_index].neighbors.push_back(second_neighbor);
    return true;
}

Location* Environment::get_location(int id) {
    for (int i = 0; i < locations.size(); ++i) {
        if (locations[i].id == id) return &locations[i];
    }
    return 0;
}

const Location* Environment::get_location(int id) const {
    for (int i = 0; i < locations.size(); ++i) {
        if (locations[i].id == id) return &locations[i];
    }
    return 0;
}

Location* Environment::get_location_at(int index) { return locations.get(index); }
const Location* Environment::get_location_at(int index) const { return locations.get(index); }
int Environment::get_location_count() const { return locations.size(); }

Connection* Environment::get_connection(int id) {
    for (int i = 0; i < connections.size(); ++i) {
        if (connections[i].id == id) return &connections[i];
    }
    return 0;
}

const Connection* Environment::get_connection(int id) const {
    for (int i = 0; i < connections.size(); ++i) {
        if (connections[i].id == id) return &connections[i];
    }
    return 0;
}

Connection* Environment::get_connection_at(int index) { return connections.get(index); }
const Connection* Environment::get_connection_at(int index) const { return connections.get(index); }
int Environment::get_connection_count() const { return connections.size(); }

Connection* Environment::get_connection_between(int from_id, int to_id) {
    for (int i = 0; i < connections.size(); ++i) {
        const Connection& connection = connections[i];
        if ((connection.endpoint1_id == from_id && connection.endpoint2_id == to_id) ||
            (connection.endpoint1_id == to_id && connection.endpoint2_id == from_id)) {
            return &connections[i];
        }
    }
    return 0;
}

const Connection* Environment::get_connection_between(int from_id, int to_id) const {
    for (int i = 0; i < connections.size(); ++i) {
        const Connection& connection = connections[i];
        if ((connection.endpoint1_id == from_id && connection.endpoint2_id == to_id) ||
            (connection.endpoint1_id == to_id && connection.endpoint2_id == from_id)) {
            return &connections[i];
        }
    }
    return 0;
}

const LinkedList<NeighborNode>* Environment::get_neighbors(int location_id) const {
    int index = get_graph_index(location_id);
    if (index < 0) return 0;
    return &graph[index].neighbors;
}

bool Environment::set_connection_blocked(int connection_id, bool blocked) {
    Connection* connection = get_connection(connection_id);
    if (connection == 0) return false;
    connection->is_blocked = blocked;
    return true;
}

bool Environment::set_station_available(int location_id, bool available) {
    Location* location = get_location(location_id);
    if (location == 0 || location->type != LOCATION_CHARGING) return false;
    location->station_available = available;
    return true;
}

bool Environment::has_operational_station(int location_id) const {
    const Location* location = get_location(location_id);
    return location != 0 && location->type == LOCATION_CHARGING && location->station_available;
}

bool Environment::is_charging_station(int location_id) const {
    const Location* location = get_location(location_id);
    return location != 0 && location->type == LOCATION_CHARGING;
}

void Environment::clear() {
    locations.clear();
    connections.clear();
    graph.clear();
}
