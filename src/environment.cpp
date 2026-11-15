#include "environment.hpp"

// MEMBER 1: implement add_location, add_connection, get_connection_between and is_cut_off.
// Lookups, getters and block/station setters are provided. See include/environment.hpp.
// Index convention: locations[i], graph[i] and the lookup entry for locations[i].id all
// describe the SAME location i. Connections work the same way with connections[k].

Environment::Environment() {}

bool Environment::add_location(const Location& location) {
    // TODO (Member 1)
    // PSEUDOCODE:
    //   if location.id < 0: return false
    //   if get_location_index(location.id) != -1: return false          (duplicate)
    //   index = locations.size()
    //   if not locations.push_back(location): return false
    //   if not graph.push_back(empty LinkedList<NeighborNode>): locations.pop_back(); return false
    //   if not location_lookup.insert(location.id, index):  undo both push_backs; return false
    //   return true
    (void)location;
    return false;
}

bool Environment::add_connection(const Connection& connection) {
    // TODO (Member 1)
    // PSEUDOCODE:
    //   if connection.id < 0 or connection.distance <= 0: return false
    //   if connection.endpoint1_id == connection.endpoint2_id: return false     (no self-loops)
    //   if get_connection_index(connection.id) != -1: return false               (duplicate)
    //   a = get_location_index(endpoint1_id); b = get_location_index(endpoint2_id)
    //   if a == -1 or b == -1: return false                                      (unknown endpoint)
    //   k = connections.size()
    //   push the connection; insert (id -> k) in connection_lookup
    //   graph[a].push_back(NeighborNode(b, k));  graph[b].push_back(NeighborNode(a, k))
    //   return true
    (void)connection;
    return false;
}

int Environment::get_location_index(int id) const {
    // DONE (provided)
    int index = -1;
    if (location_lookup.find(id, index)) return index;
    return -1;
}

int Environment::get_connection_index(int id) const {
    // DONE (provided)
    int index = -1;
    if (connection_lookup.find(id, index)) return index;
    return -1;
}

// DONE (provided): lookups by ID and by array index.
Location* Environment::get_location(int id) {
    int i = get_location_index(id);
    if (i < 0) return 0;
    return &locations[i];
}
const Location* Environment::get_location(int id) const {
    int i = get_location_index(id);
    if (i < 0) return 0;
    return &locations[i];
}
Location* Environment::get_location_at(int index) { return locations.get(index); }
const Location* Environment::get_location_at(int index) const { return locations.get(index); }
Connection* Environment::get_connection(int id) {
    int i = get_connection_index(id);
    if (i < 0) return 0;
    return &connections[i];
}
const Connection* Environment::get_connection(int id) const {
    int i = get_connection_index(id);
    if (i < 0) return 0;
    return &connections[i];
}
Connection* Environment::get_connection_at(int index) { return connections.get(index); }
const Connection* Environment::get_connection_at(int index) const { return connections.get(index); }

const LinkedList<NeighborNode>* Environment::get_neighbors_at(int index) const {
    // DONE (provided)
    if (index < 0 || index >= graph.size()) return 0;
    return &graph[index];
}

const Connection* Environment::get_connection_between(int from_id, int to_id) const {
    // TODO (Member 1)
    // PSEUDOCODE: a = get_location_index(from_id); if a == -1 return 0
    //   walk the neighbour list of a; for each node whose target location has ID to_id:
    //       return &connections[node.connection_index]
    //   return 0
    (void)from_id; (void)to_id;
    return 0;
}

bool Environment::set_connection_blocked(int connection_id, bool blocked) {
    // DONE (provided): one shared record, so both directions always agree.
    Connection* connection = get_connection(connection_id);
    if (connection == 0) return false;
    connection->is_blocked = blocked;
    return true;
}

bool Environment::set_station_available(int location_id, bool available) {
    // DONE (provided)
    Location* location = get_location(location_id);
    if (location == 0 || location->type != LOCATION_CHARGING) return false;
    location->station_available = available;
    return true;
}

bool Environment::is_operational_station(int location_id) const {
    // DONE (provided)
    const Location* location = get_location(location_id);
    return location != 0 && location->type == LOCATION_CHARGING && location->station_available;
}

bool Environment::is_cut_off(int location_id) const {
    // TODO (Member 1)
    // PSEUDOCODE: i = get_location_index(location_id); if i == -1: return false
    //   for each neighbour node of graph[i]: if connections[node.connection_index] is NOT blocked: return false
    //   return true      (a location with no connections at all is also cut off)
    (void)location_id;
    return false;
}

void Environment::clear() {
    // DONE (provided)
    locations.clear();
    connections.clear();
    graph.clear();
    location_lookup.clear();
    connection_lookup.clear();
}
