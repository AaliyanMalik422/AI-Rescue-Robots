#include "environment.hpp"

Environment::Environment() {}

bool Environment::add_location(const Location& location) {
    if (location.id < 0 || location.id > MAX_ID) return false;
    if (get_location_index(location.id) != -1) return false;

    int index = locations.size();
    if (!locations.push_back(location)) return false;

    LinkedList<NeighborNode> empty_list;
    if (!graph.push_back(empty_list)) {
        locations.pop_back();
        return false;
    }

    if (!location_lookup.insert(location.id, index)) {
        graph.pop_back();
        locations.pop_back();
        return false;
    }
    return true;
}

bool Environment::add_connection(const Connection& connection) {
    if (connection.id < 0 || connection.id > MAX_ID || connection.distance <= 0)
        return false;
    if (connection.endpoint1_id == connection.endpoint2_id) return false;
    if (get_connection_index(connection.id) != -1) return false;

    int a = get_location_index(connection.endpoint1_id);
    int b = get_location_index(connection.endpoint2_id);
    if (a == -1 || b == -1) return false;

    int index = connections.size();
    if (!connections.push_back(connection)) return false;

    if (!connection_lookup.insert(connection.id, index)) {
        connections.pop_back();
        return false;
    }

    NeighborNode ab(b, index);
    NeighborNode ba(a, index);
    if (!graph[a].push_back(ab)) {
        connection_lookup.clear();
        for (int i = 0; i < connections.size(); ++i)
            connection_lookup.insert(connections[i].id, i);
        connections.pop_back();
        return false;
    }
    if (!graph[b].push_back(ba)) {
        graph[a].pop_back();
        connection_lookup.clear();
        for (int i = 0; i < connections.size(); ++i)
            connection_lookup.insert(connections[i].id, i);
        connections.pop_back();
        return false;
    }
    return true;
}

int Environment::get_location_index(int id) const {
    int index = -1;
    if (location_lookup.find(id, index)) return index;
    return -1;
}

int Environment::get_connection_index(int id) const {
    int index = -1;
    if (connection_lookup.find(id, index)) return index;
    return -1;
}

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
    if (index < 0 || index >= graph.size()) return 0;
    return &graph[index];
}

const Connection* Environment::get_connection_between(int from_id, int to_id) const {
    int from_index = get_location_index(from_id);
    if (from_index == -1 || get_location_index(to_id) == -1) return 0;

    const ListNode<NeighborNode>* node = graph[from_index].get_head();
    while (node != 0) {
        const NeighborNode& neighbour = node->data;
        const Location* target = locations.get(neighbour.target_location_index);
        if (target != 0 && target->id == to_id)
            return connections.get(neighbour.connection_index);
        node = node->next;
    }
    return 0;
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

bool Environment::is_operational_station(int location_id) const {
    const Location* location = get_location(location_id);
    return location != 0 && location->type == LOCATION_CHARGING && location->station_available;
}

bool Environment::is_cut_off(int location_id) const {
    int index = get_location_index(location_id);
    if (index == -1) return false;

    const ListNode<NeighborNode>* node = graph[index].get_head();
    while (node != 0) {
        const Connection* connection = connections.get(node->data.connection_index);
        if (connection != 0 && !connection->is_blocked) return false;
        node = node->next;
    }
    return true;
}

void Environment::clear() {
    locations.clear();
    connections.clear();
    graph.clear();
    location_lookup.clear();
    connection_lookup.clear();
}
