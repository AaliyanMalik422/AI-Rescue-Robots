#pragma once

#include "types.hpp"
#include "collections.hpp"

struct NeighborNode {
    int target_location_id;
    int connection_id;

    NeighborNode() : target_location_id(-1), connection_id(-1) {}
    NeighborNode(int target, int connection)
        : target_location_id(target), connection_id(connection) {}
};

struct AdjacencyList {
    int location_id;
    LinkedList<NeighborNode> neighbors;

    AdjacencyList() : location_id(-1) {}
};

class Environment {
private:
    DynamicArray<Location> locations;
    DynamicArray<Connection> connections;
    DynamicArray<AdjacencyList> graph;

    int get_graph_index(int location_id) const;

public:
    Environment();

    bool add_location(const Location& location);
    bool add_connection(const Connection& connection);

    Location* get_location(int id);
    const Location* get_location(int id) const;
    Location* get_location_at(int index);
    const Location* get_location_at(int index) const;
    int get_location_count() const;

    Connection* get_connection(int id);
    const Connection* get_connection(int id) const;
    Connection* get_connection_at(int index);
    const Connection* get_connection_at(int index) const;
    int get_connection_count() const;
    Connection* get_connection_between(int from_id, int to_id);
    const Connection* get_connection_between(int from_id, int to_id) const;

    const LinkedList<NeighborNode>* get_neighbors(int location_id) const;

    bool set_connection_blocked(int connection_id, bool blocked);
    bool set_station_available(int location_id, bool available);
    bool has_operational_station(int location_id) const;
    bool is_charging_station(int location_id) const;

    void clear();
};
