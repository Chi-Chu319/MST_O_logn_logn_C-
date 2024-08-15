#include <vector>
#include "../algo.h"
#include <map>
#include <boost/serialization/vector.hpp>

namespace GraphUtil {
    Graph generate_clique_graph(int size, int max_weight, int num_vertices_local) {
        Graph graph = Graph(
            size,
            num_vertices_local,
            -1,
            max_weight,
            true
        );

        graph.generate();

        return graph;
    }

    GraphLocal generate_distributed_clique_graph(boost::mpi::communicator world, int rank, int size, int max_weight, int num_vertices_local) {
        GraphLocal graph_local = GraphLocal(size, rank, num_vertices_local, max_weight);
        std::vector<std::vector<double>> sendbuf = graph_local.generate();
        std::vector<std::vector<double>> recvbuf;

        boost::mpi::all_to_all<std::vector<double>>(world, sendbuf, recvbuf);

        graph_local.fill(recvbuf);

        return graph_local;
    }

    std::vector<std::vector<ClusterEdge>> get_min_weight_to_cluster_edges(GraphLocal& graph_local, QuickUnion& cluster_finder) {
        int vertex_local_start = graph_local.get_vertex_local_start();
        int comm_size = graph_local.get_comm_size();
        int num_vertices_local = graph_local.get_num_vertices_local();
        int num_vertices = graph_local.get_num_vertices();

        std::vector<std::vector<double>> vertices = graph_local.get_vertices();

        std::vector<std::vector<ClusterEdge>> sendbuf = std::vector<std::vector<ClusterEdge>>(comm_size);

        std::vector<ClusterEdge> min_cluster_edges;

        for (int vertex_local = 0; vertex_local < num_vertices_local; ++vertex_local) {
            int from_v = vertex_local + vertex_local_start;
            std::map<int, ClusterEdge> cluster_edges;

            for (int to_v = 0; to_v < num_vertices; ++to_v) {
                double weight = vertices[vertex_local][to_v];
                int from_cluster = cluster_finder.get_cluster_leader(from_v);
                int to_cluster = cluster_finder.get_cluster_leader(to_v);
                if (from_cluster != to_cluster) {
                    if (cluster_edges.find(to_cluster) == cluster_edges.end()) {
                        cluster_edges[to_cluster] = ClusterEdge(from_v, to_v, weight);
                    } else if (weight < cluster_edges[to_cluster].weight) {
                        cluster_edges[to_cluster] = ClusterEdge(from_v, to_v, weight);
                    }
                }
            }

            for(std::map<int,ClusterEdge>::iterator it = cluster_edges.begin(); it != cluster_edges.end(); ++it) {
                min_cluster_edges.push_back(it->second);
            }

        }

        for (int i = 0; i < min_cluster_edges.size(); ++i) {
            ClusterEdge edge = min_cluster_edges[i];

            sendbuf[graph_local.get_vertex_machine(cluster_finder.get_cluster_leader(edge.to_v))].push_back(edge);
        }

        return sendbuf;
    }

    std::vector<ClusterEdge> get_min_weight_from_cluster_edges(std::vector<ClusterEdge>& cluster_edges, QuickUnion& cluster_finder) {
        std::map<int, ClusterEdge> min_cluster_edges;
        std::vector<ClusterEdge> result;

        for (int i = 0; i < cluster_edges.size(); ++i) {
            ClusterEdge edge = cluster_edges[i];
            int from_cluster = cluster_finder.get_cluster_leader(edge.from_v);

            if (min_cluster_edges.count(from_cluster) == 0) {
                min_cluster_edges[from_cluster] = edge;
            } else if (edge.weight < min_cluster_edges[from_cluster].weight) {
                min_cluster_edges[from_cluster] = edge;
            }
        }

        for(std::map<int,ClusterEdge>::iterator it = min_cluster_edges.begin(); it != min_cluster_edges.end(); ++it) {
            result.push_back(it->second);
        }

        return result;
    }
}