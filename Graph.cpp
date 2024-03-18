#include "Graph.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <limits>
#include <iostream>
#include <utility>
#include <set>

using namespace std;

Graph::Graph(const char* const & edgelist_csv_fn) {
    ifstream fileStream(edgelist_csv_fn);
    string currentLine;
    while (getline(fileStream, currentLine)) {
        istringstream lineStream(currentLine);
        string node1, node2;
        double edgeWeight;
        getline(lineStream, node1, ',');
        getline(lineStream, node2, ',');
        lineStream >> edgeWeight;
        adjacency_map[node1][node2] = edgeWeight;
        adjacency_map[node2][node1] = edgeWeight;
    }
}
unsigned int Graph::num_nodes() {
    return adjacency_map.size();
}
vector<string> Graph::nodes() {
    vector<string> nodeLabels;
    for (const auto& pair : adjacency_map) {
        nodeLabels.push_back(pair.first);
    }
    return nodeLabels;
}
unsigned int Graph::num_edges() {
    unsigned int edgeCount = 0;
    for (const auto& pair : adjacency_map) {
        edgeCount += pair.second.size();
    }
    return edgeCount / 2; 
}
double Graph::edge_weight(string const & u_label, string const & v_label) {
    auto mapIterator = adjacency_map.find(u_label);
    if (mapIterator != adjacency_map.end()) {
        auto innerMapIterator = mapIterator->second.find(v_label);
        if (innerMapIterator != mapIterator->second.end()) {
            return innerMapIterator->second;
        }
    }
    return -1; 
}
unsigned int Graph::num_neighbors(string const & node_label) {
    auto mapIterator = adjacency_map.find(node_label);
    return mapIterator == adjacency_map.end() ? 0 : mapIterator->second.size();
}
vector<string> Graph::neighbors(string const & node_label) {
    vector<string> neighborLabels;
    auto mapIterator = adjacency_map.find(node_label);
    if (mapIterator != adjacency_map.end()) {
        for (const auto& pair : mapIterator->second) {
            neighborLabels.push_back(pair.first);
        }
    }
    return neighborLabels;
}
vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    if (start_label == end_label) {
        return {start_label};
    }
    map<string, string> predecessors;
    queue<string> searchQueue;
    map<string, bool> visitedNodes;
    searchQueue.push(start_label);
    visitedNodes[start_label] = true;
    while (!searchQueue.empty()) {
        string currentNode = searchQueue.front();
        searchQueue.pop();
        for (const auto& neighbor : adjacency_map[currentNode]) {
            if (!visitedNodes[neighbor.first]) {
                visitedNodes[neighbor.first] = true;
                predecessors[neighbor.first] = currentNode;
                searchQueue.push(neighbor.first);
                if (neighbor.first == end_label) {
                    vector<string> path;
                    for (string at = end_label; at != start_label; at = predecessors[at]) {
                        path.push_back(at);
                    }
                    path.push_back(start_label);
                    reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }
    return {}; 
}
vector<tuple<string, string, double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    map<string, double> shortestDistances;
    map<string, string> previousNodes;
    auto comparator = [](const pair<double, string>& left, const pair<double, string>& right) {
        return left.first > right.first;
    };
    priority_queue<pair<double, string>, vector<pair<double, string>>, decltype(comparator)> priorityQueue(comparator);

    for (const auto& node : adjacency_map) {
        shortestDistances[node.first] = numeric_limits<double>::infinity();
    }
    shortestDistances[start_label] = 0.0;
    priorityQueue.push(make_pair(0.0, start_label));
    while (!priorityQueue.empty()) {
        string currentNode = priorityQueue.top().second;
        priorityQueue.pop();
        if (currentNode == end_label) break;
        for (const auto& neighbor : adjacency_map[currentNode]) {
            double alt = shortestDistances[currentNode] + neighbor.second;
            if (alt < shortestDistances[neighbor.first]) {
                shortestDistances[neighbor.first] = alt;
                previousNodes[neighbor.first] = currentNode;
                priorityQueue.push(make_pair(alt, neighbor.first));
            }
        }
    }

    vector<tuple<string, string, double>> path;
    for (string at = end_label; at != start_label; at = previousNodes[at]) {
        if (previousNodes.find(at) == previousNodes.end()) return {};
        path.emplace_back(previousNodes[at], at, adjacency_map[previousNodes[at]][at]);
    }
    reverse(path.begin(), path.end());
    return path;
}
vector<vector<string>> Graph::connected_components(double const & threshold) {
    vector<vector<string>> componentsList;
    set<string> visitedNodes;
    for (const auto& node : adjacency_map) {
        if (visitedNodes.find(node.first) == visitedNodes.end()) {
            vector<string> currentComponent;
            queue<string> searchQueue;
            searchQueue.push(node.first);
            visitedNodes.insert(node.first);
            while (!searchQueue.empty()) {
                string currentNode = searchQueue.front();
                searchQueue.pop();
                currentComponent.push_back(currentNode);
                for (const auto& neighbor : adjacency_map[currentNode]) {
                    if (visitedNodes.find(neighbor.first) == visitedNodes.end() && neighbor.second <= threshold) {
                        searchQueue.push(neighbor.first);
                        visitedNodes.insert(neighbor.first);
                    }
                }
            }
            componentsList.push_back(currentComponent);
        }
    }
    return componentsList;
}
double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label) {
    if (start_label == end_label) {
        return 0.0;
    }
    vector<tuple<double, string, string>> sortedEdges;
    for (auto& src : adjacency_map) {
        for (auto& dest : src.second) {
            sortedEdges.emplace_back(dest.second, src.first, dest.first);
        }
    }
    sort(sortedEdges.begin(), sortedEdges.end());
    map<string, string> rootParent;
    for (const auto& node : adjacency_map) {
        rootParent[node.first] = node.first;
    }
    auto findRoot = [&rootParent](string node) {
        string root = node;
        while (root != rootParent[root]) {
            root = rootParent[root];
        }
        return root;
    };
    for (const auto& edge : sortedEdges) {
        double currentWeight = get<0>(edge);
        string u = get<1>(edge);
        string v = get<2>(edge);
        string rootNodeU = findRoot(u);
        string rootNodeV = findRoot(v);
        if (rootNodeU != rootNodeV) {
            rootParent[rootNodeU] = rootNodeV;
            if (findRoot(start_label) == findRoot(end_label)) {
                return currentWeight;
            }
        }
    }
    return -1.0; 
}

