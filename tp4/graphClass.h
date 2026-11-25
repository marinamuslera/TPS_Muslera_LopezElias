#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

template <typename VData = std::string, typename EData = std::string>
class Graph {
private:
    struct Vertex {
        std::optional<VData> data;
        std::unordered_map<std::string, std::optional<EData>> neighbors;
        std::vector<std::string> neighborOrder;  // orden de inserción
    };

    std::unordered_map<std::string, Vertex> _graph;
    std::vector<std::string> _order;  // orden de inserción de vértices

    template <typename T>
    static void printOptional(std::ostream& os, const std::optional<T>& v) {
        if (v) os << *v;
        else os << "None";
    }

public:
    Graph() = default;

    /**
     * Adds a vertex to the graph
     * @param vertex the vertex name
     * @param data data associated with the vertex
     */
    void add_vertex(const std::string& vertex, std::optional<VData> data = std::nullopt) {
        if (_graph.find(vertex) == _graph.end()) {
            _graph.emplace(vertex, Vertex{std::move(data), {}, {}});
            _order.push_back(vertex);
        }
    }

    /**
     * Adds an edge to the graph
     * @param vertex1 vertex1 key
     * @param vertex2 vertex2 key
     * @param data the data associated with the edge
     */
    void add_edge(const std::string& vertex1, const std::string& vertex2,
                  std::optional<EData> data = std::nullopt) {
        auto it = _graph.find(vertex1);
        if (it == _graph.end() || _graph.find(vertex2) == _graph.end())
            throw std::invalid_argument("The vertexes do not exist");
        auto& v = it->second;
        if (v.neighbors.find(vertex2) == v.neighbors.end())
            v.neighborOrder.push_back(vertex2);
        v.neighbors[vertex2] = std::move(data);
    }

    /**
     * Get the list of all vertexes
     * @return the vertexes, in insertion order
     */
    std::vector<std::string> get_vertices() const {
        return _order;
    }

    /**
     * Get the list of vertex neighbors
     * @param vertex the vertex to query
     * @return the list of neighbor vertexes
     */
    std::vector<std::string> get_neighbors(const std::string& vertex) const {
        auto it = _graph.find(vertex);
        if (it != _graph.end()) return it->second.neighborOrder;
        return {};
    }

    /**
     * Gets vertex associated data
     * @param vertex the vertex name
     * @return the vertex data (std::nullopt if it doesn't exist or has no data)
     */
    std::optional<VData> get_vertex_data(const std::string& vertex) const {
        auto it = _graph.find(vertex);
        if (it != _graph.end()) return it->second.data;
        return std::nullopt;
    }

    /**
     * Gets the vertexes edge data
     * @param vertex1 the vertex1 name
     * @param vertex2 the vertex2 name
     * @return vertexes edge data
     */
    std::optional<EData> get_edge_data(const std::string& vertex1, const std::string& vertex2) const {
        if (edge_exists(vertex1, vertex2))
            return _graph.at(vertex1).neighbors.at(vertex2);
        throw std::invalid_argument("The edge does not exist");
    }

    /**
     * Prints the graph
     */
    void print_graph(std::ostream& os = std::cout) const {
        for (const auto& name : _order) {
            const auto& v = _graph.at(name);
            os << "Vertex: " << name << "\n";
            os << "Data: ";
            printOptional(os, v.data);
            os << "\n";
            os << "Neighbors: {";
            bool first = true;
            for (const auto& n : v.neighborOrder) {
                if (!first) os << ", ";
                first = false;
                os << "'" << n << "': ";
                printOptional(os, v.neighbors.at(n));
            }
            os << "}\n\n";
        }
    }

    /**
     * If contains a vertex
     * @param vertex the vertex name
     * @return boolean
     */
    bool vertex_exists(const std::string& vertex) const {
        return _graph.find(vertex) != _graph.end();
    }

    /**
     * If contains an edge
     * @param vertex1 the vertex1 name
     * @param vertex2 the vertex2 name
     * @return boolean
     */
    bool edge_exists(const std::string& vertex1, const std::string& vertex2) const {
        auto it = _graph.find(vertex1);
        return it != _graph.end() && it->second.neighbors.count(vertex2) > 0;
    }
};

#endif  // GRAPH_CLASS_H
