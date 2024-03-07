#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <random>

class GraphLocal {
public:
    GraphLocal(int comm_size, int rank, int num_vertices_local, int max_weight);
    void set_vertices(const std::vector<std::vector<double>>& vertices);
    std::vector<std::vector<double>> generate();
    void fill(const std::vector<std::vector<double>>& recvbuf);
    int get_vertex_local_start() const;
    int get_num_vertices_local() const;
    const std::vector<std::vector<double>>& get_vertices() const;
    int get_comm_size() const;
    int get_num_vertices() const;
    int get_vertex_machine(int vertex) const;

private:
    int rank;
    int comm_size;
    int num_vertices_local;
    int max_weight;
    int num_vertices;
    int vertex_local_start;
    std::vector<std::vector<double>> vertices;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    double random_weight();
};

class Graph {
public:
    Graph(int comm_size, int num_vertices_local, int expected_degree, int max_weight, bool is_clique);
    Graph generate();
    std::vector<GraphLocal> split();
private:
    int comm_size;
    int num_vertices_local;
    int expected_degree;
    int max_weight;
    int num_vertices;
    std::vector<std::vector<double>> vertices;
    bool is_clique;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    double random_weight();
};

struct ClusterEdge {
    int from_v;
    int to_v;
    float weight;
    ClusterEdge(int from_v, int to_v, float weight) : from_v(from_v), to_v(to_v), weight(weight) {};
};

class QuickUnion {
public:
    QuickUnion(int* id, int n);
    ~QuickUnion();
    int* get_id();
    int get_cluster_leader(int i);
    void set_finished(int i);
    int root(int i);
    int flatten(int i);
    bool is_finished(int i);
private:
    int* id;
    int* sz;
    bool* finished;
    int n;
};

#endif // ALGO_H
