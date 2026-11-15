#pragma once

#include "types.hpp"
#include "collections.hpp"
#include "id_index_map.hpp"

// =====================================================================
// MEMBER 1 - Environment: undirected weighted graph (adjacency list).
//
// Design report 3.1:
//  * locations and connections live in arrays; their ARRAY INDICES are stable;
//  * graph[i] is the neighbour list of locations[i];
//  * each road adds one NeighborNode at EACH endpoint, both storing the same
//    connection index, so a block/unblock updates one central record;
//  * two IdIndexMaps (direct-address arrays) map external IDs to array indices.
// =====================================================================

struct NeighborNode {
    int target_location_index;   // index into locations
    int connection_index;        // index into connections

    NeighborNode() : target_location_index(-1), connection_index(-1) {}
    NeighborNode(int target, int connection)
        : target_location_index(target), connection_index(connection) {}
    bool operator==(const NeighborNode& other) const {
        return target_location_index == other.target_location_index &&
               connection_index == other.connection_index;
    }
};

class Environment {
private:
    DynamicArray<Location> locations;
    DynamicArray<Connection> connections;
    DynamicArray<LinkedList<NeighborNode> > graph;   // graph[i] = neighbours of locations[i]
    IdIndexMap location_lookup;                      // location ID -> index
    IdIndexMap connection_lookup;                    // connection ID -> index

public:
    Environment();

    // TODO (Member 1): reject duplicate / negative IDs. Add the empty neighbour
    // list and the lookup entry. Keep all three structures consistent on failure.
    bool add_location(const Location& location);

    // TODO (Member 1): reject duplicate ID, unknown endpoints, self-loops,
    // distance <= 0. Add ONE NeighborNode at each endpoint (same connection_index).
    bool add_connection(const Connection& connection);

    // DONE (provided): ID -> index via the IdIndexMap; -1 if unknown.
    int get_location_index(int id) const;
    int get_connection_index(int id) const;

    // DONE (provided): lookups by ID (0 if unknown) and by index (0 if out of range).
    Location* get_location(int id);
    const Location* get_location(int id) const;
    Location* get_location_at(int index);
    const Location* get_location_at(int index) const;
    Connection* get_connection(int id);
    const Connection* get_connection(int id) const;
    Connection* get_connection_at(int index);
    const Connection* get_connection_at(int index) const;

    int get_location_count() const { return locations.size(); }
    int get_connection_count() const { return connections.size(); }

    // DONE (provided): neighbour list for the location at `index`, or 0.
    const LinkedList<NeighborNode>* get_neighbors_at(int index) const;

    // TODO (Member 1): first connection joining the two location IDs, or 0.
    const Connection* get_connection_between(int from_id, int to_id) const;

    // DONE (provided): change road state. False if the ID is unknown.
    bool set_connection_blocked(int connection_id, bool blocked);

    // DONE (provided): only CHARGING locations can go up/down.
    bool set_station_available(int location_id, bool available);

    // DONE (provided): CHARGING and currently available.
    bool is_operational_station(int location_id) const;

    // TODO (Member 1): true if every connection of this location is blocked
    // (the "cut off" location in the provided scenario).
    bool is_cut_off(int location_id) const;

    // DONE (provided): empty everything, including the lookup maps.
    void clear();
};
