
#ifndef GRAPH_H
#define GRAPH_H
#include <string>
#include <tuple>
#include <vector>
#include <map>
using namespace std;


class Graph {
    private:
    map<string, map<string, double>> adjacency_map;
    public:
       
        Graph(const char* const & edgelist_csv_fn);

        
        unsigned int num_nodes();

       
        vector<string> nodes();

       
        unsigned int num_edges();

        
        double edge_weight(string const & u_label, string const & v_label);

       
        unsigned int num_neighbors(string const & node_label);

        
        vector<string> neighbors(string const & node_label);

       
        vector<string> shortest_path_unweighted(string const & start_label, string const & end_label);

       
        vector<tuple<string,string,double>> shortest_path_weighted(string const & start_label, string const & end_label);

        
        vector<vector<string>> connected_components(double const & threshold);

       
        double smallest_connecting_threshold(string const & start_label, string const & end_label);
};
#endif
