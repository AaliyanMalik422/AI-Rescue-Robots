#pragma once
#include "types.hpp"
#include "collections.hpp"

class Environment {
public:
    struct NeighborNode {
        int target_location_id;
        int connection_id;
    };

private:
    struct AdjacencyList {
        int location_id;
        DynamicArray<NeighborNode> neighbors;
    };

    DynamicArray<Location> locations;
    DynamicArray<Connection> connections;
    DynamicArray<AdjacencyList> graph;

    int get_graph_index(int location_id) const;

public:
    Environment();
    ~Environment();

    bool add_location(int id, const std::string& name, bool has_station, bool is_station_available);
    bool add_connection(int id, int from_id, int to_id, int distance, bool is_blocked);

    Location* get_location(int id);
    const Location* get_location(int id) const;

    Connection* get_connection(int id);
    const Connection* get_connection(int id) const;

    DynamicArray<NeighborNode> get_neighbors(int location_id) const;

    bool set_connection_blocked(int id, bool blocked);
    bool is_connection_blocked(int id) const;

    bool set_station_available(int location_id, bool available);
    bool has_operational_station(int location_id) const;

    void clear();
};
