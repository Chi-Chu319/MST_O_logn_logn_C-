#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <random>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>

class GraphLocal {
public:
    GraphLocal();
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

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & rank;
        ar & comm_size;
        ar & num_vertices_local;
        ar & max_weight;
        ar & num_vertices;
        ar & vertex_local_start;
        ar & vertices;
    }

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
    Graph();
    Graph(int comm_size, int num_vertices_local, int expected_degree, int max_weight, bool is_clique);
    Graph generate();
    std::vector<GraphLocal> split();
    const std::vector<std::vector<double>>& get_vertices() const;
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
    double weight;
    ClusterEdge(int from_v, int to_v, double weight) : from_v(from_v), to_v(to_v), weight(weight) {};
    ClusterEdge() {};
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & from_v;
        ar & to_v;
        ar & weight;
    }
};

struct LogDist {
    int k;
    double t_total;
    double t_mpi;
    double t_comm0;
    double t_comm1;
    double t_comm2;
    double t_comm3;
    double t_comm4;
    double t_rank0;
};

struct AlgoMPIResult {
    std::vector<LogDist> logs;
    std::vector<ClusterEdge> mst_edges;
    int k;
    double t_total;
    double t_mpi;
    double t_rank0;
};

class QuickUnion {
public:
    QuickUnion(int* id, int n);
    ~QuickUnion();
    int* get_id();
    void set_id(int* newId);
    int get_cluster_leader(int i);
    void set_finished(int i);
    void reset_finished();
    int root(int i);
    void flatten();
    bool is_finished(int i);
    bool safe_union(int p, int q);
private:
    int* id;
    int* sz;
    bool* finished;
    int n;
};

namespace GraphUtil {
    std::vector<std::vector<ClusterEdge>> get_min_weight_to_cluster_edges(GraphLocal& graph_local, QuickUnion& cluster_finder);
    std::vector<ClusterEdge> get_min_weight_from_cluster_edges(std::vector<ClusterEdge>& cluster_edges, QuickUnion& cluster_finder);
    Graph generate_clique_graph(int size, int max_weight, int num_vertex_local);
    GraphLocal generate_distributed_clique_graph(boost::mpi::communicator world, int rank, int size, int max_weight, int num_vertex_local);
}

namespace MSTSolver {
    AlgoMPIResult algo_mpi(boost::mpi::communicator world, GraphLocal& graph_local, int rank, int size);
    double prim(Graph graph);
    double kruskal(Graph graph);
}

namespace CsvUtil {
    void save_csv(const std::string& filename, const std::vector<std::string> fields, const std::vector<std::vector<std::string>>& data);
}

namespace CsvUtil {
    void save_csv(const std::string& filename, const std::vector<std::string> fields, const std::vector<std::vector<std::string>>& data);
    void add_csv_row(std::string filename, std::vector<std::string> row);
}

namespace Tester {
    AlgoMPIResult algo_mpi_test(boost::mpi::communicator world, int rank, int size, int max_weight, int num_vertex_local);
}

#endif // ALGO_H
