#include <mpi.h>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "algo.h"
#include <vector>
#include <set>
#include <boost/serialization/vector.hpp>
#include <map>

namespace MSTSolver {
    AlgoMPIResult algo_mpi(boost::mpi::communicator world, GraphLocal& graph_local, int rank, int size) {
        double t_total_start = MPI_Wtime();
        
        int num_vertex_local = graph_local.get_num_vertices_local();

        int vertex_local_start = rank * num_vertex_local;
        int num_vertex = num_vertex_local * size;

        // phase
        int k = 0;
        std::vector<LogDist> logs;

        std::vector<std::vector<int>> clusters_local(num_vertex_local);

        int* cluster_finder_id = new int[num_vertex];

        for (int i = 0; i < num_vertex; ++i) {
            cluster_finder_id[i] = i;
        }

        std::vector<ClusterEdge> mst_edges = std::vector<ClusterEdge>();
        QuickUnion cluster_finder(cluster_finder_id, num_vertex);
        int num_cluster = num_vertex;


        while (true)
        {
            double t_rank0 = 0;
            double t_comm0 = 0;
            double t_comm1 = 0;
            double t_comm2 = 0;
            double t_comm3 = 0;
            double t_comm4 = 0;
            // mpi get time
            double t_start_all = MPI_Wtime();

            std::vector<ClusterEdge> guardian_cluster_edges;
            std::vector<std::vector<ClusterEdge>> gathered_edges(size);

            if (k == 0) {
                for (int vertex_local = 0; vertex_local < num_vertex_local; ++vertex_local) {
                    int vertex_to = vertex_local + vertex_local_start;
                    int vertex_from = 0;
                    double weight = graph_local.get_vertices()[vertex_local][vertex_from];
                    for (int i = 0; i < num_vertex; ++i) {
                        if (graph_local.get_vertices()[vertex_local][i] < weight) {
                            vertex_from = i;
                            weight = graph_local.get_vertices()[vertex_local][i];
                        }
                    }
                    guardian_cluster_edges.push_back(ClusterEdge(vertex_from, vertex_to, weight));
                }

                // comm0
                double t_start_comm0 = MPI_Wtime();
                boost::mpi::gather(world, guardian_cluster_edges, gathered_edges, 0);
                double t_end_comm0 = MPI_Wtime();

                t_comm0 = t_end_comm0 - t_start_comm0;
            }
            // When k = 0 the cluster local is not populated and the following will throw an error
            else {
                // step 1
                std::vector<std::vector<ClusterEdge>> sendbuf_to_clusters = GraphUtil::get_min_weight_to_cluster_edges(graph_local, cluster_finder);

                std::vector<std::vector<ClusterEdge>> recvbuf_to_clusters(size);

                // comm1
                double t_start_comm1 = MPI_Wtime();
                boost::mpi::all_to_all<std::vector<ClusterEdge>>(world, sendbuf_to_clusters, recvbuf_to_clusters);
                double t_end_comm1 = MPI_Wtime();
                t_comm1 = t_end_comm1 - t_start_comm1;

                // step 2
                std::vector<std::vector<ClusterEdge>> clusters_edges(num_vertex_local);

                for (int i = 0; i < recvbuf_to_clusters.size(); ++i) {
                    std::vector<ClusterEdge> edges = recvbuf_to_clusters[i];

                    for (int j = 0; j < edges.size(); ++j) {
                        ClusterEdge edge = edges[j];

                        clusters_edges[cluster_finder.get_cluster_leader(edge.to_v) - vertex_local_start].push_back(edge);
                    }
                }

                std::vector<std::vector<ClusterEdge>> sendbuf_from_clusters(size);
                std::vector<std::vector<ClusterEdge>> recvbuf_from_clusters(size);

                for (int i = 0; i < clusters_edges.size(); ++i) {
                    std::vector<ClusterEdge> cluster_edges = clusters_edges[i];

                    if (cluster_edges.size() == 0) {
                        continue;
                    }

                    std::vector<ClusterEdge> min_weight_from_cluster_edges = GraphUtil::get_min_weight_from_cluster_edges(cluster_edges, cluster_finder);
                    

                    std::sort(min_weight_from_cluster_edges.begin(), min_weight_from_cluster_edges.end(), [](ClusterEdge a, ClusterEdge b) {
                        return a.weight < b.weight;
                    });

                    int mu = std::min(clusters_local[i].size(), min_weight_from_cluster_edges.size());

                    min_weight_from_cluster_edges.resize(mu);

                    for (int j = 0; j < mu; ++j) {

                        int cluster_vertex = clusters_local[i][j];
                        ClusterEdge edge = min_weight_from_cluster_edges[j];

                        sendbuf_from_clusters[graph_local.get_vertex_machine(cluster_vertex)].push_back(edge);
                    }
                }

                // comm2
                double t_start_comm2 = MPI_Wtime();
                boost::mpi::all_to_all<std::vector<ClusterEdge>>(world, sendbuf_from_clusters, recvbuf_from_clusters);
                double t_end_comm2 = MPI_Wtime();
                t_comm2 = t_end_comm2 - t_start_comm2;

                std::vector<ClusterEdge> guardian_cluster_edges;

                // step 3
                for (int i = 0; i < recvbuf_from_clusters.size(); ++i) {
                    std::vector<ClusterEdge> edges = recvbuf_from_clusters[i];

                    for (int j = 0; j < edges.size(); ++j) {
                        ClusterEdge edge = edges[j];
                        guardian_cluster_edges.push_back(edge);
                    }
                }

                // // comm3
                double t_start_comm3 = MPI_Wtime();
                boost::mpi::gather(world, guardian_cluster_edges, gathered_edges, 0);
                double t_end_comm3 = MPI_Wtime();
                t_comm3 = t_end_comm3 - t_start_comm3;
            }

            double t_start_rank0 = MPI_Wtime();
            // step 4
            if (rank == 0) {
                // print cluster_finder_id
                std::vector<ClusterEdge> edges_to_add = guardian_cluster_edges;
                for (int i = 0; i < gathered_edges.size(); ++i) {
                    std::vector<ClusterEdge> edges = gathered_edges[i];

                    for (int j = 0; j < edges.size(); ++j) {
                        ClusterEdge edge = edges[j];
                        edges_to_add.push_back(edge);
                    }
                }

                std::sort(edges_to_add.begin(), edges_to_add.end(), [](ClusterEdge a, ClusterEdge b) {
                    return a.weight < b.weight;
                });

                std::vector<bool> heaviest_edges(edges_to_add.size());
                std::fill(heaviest_edges.begin(), heaviest_edges.end(), false);
                std::map<int, bool> encountered_clusters;

                for (int i = edges_to_add.size() - 1; i >= 0; --i) {
                    ClusterEdge edge = edges_to_add[i];
                    int to_cluster = cluster_finder.get_cluster_leader(edge.to_v);

                    if (encountered_clusters.find(to_cluster) == encountered_clusters.end()) {
                        encountered_clusters[to_cluster] = true;
                        heaviest_edges[i] = true;
                    }
                }

                for (int i = 0; i < edges_to_add.size(); ++i) {
                    ClusterEdge edge = edges_to_add[i];
                    int from_cluster = cluster_finder.get_cluster_leader(edge.from_v);
                    int to_cluster = cluster_finder.get_cluster_leader(edge.to_v);
                    bool from_cluster_finished = cluster_finder.is_finished(from_cluster);
                    bool to_cluster_finished = cluster_finder.is_finished(to_cluster);

                    if (to_cluster_finished && from_cluster_finished) {
                        continue;
                    }

                    bool merged = cluster_finder.safe_union(from_cluster, to_cluster);
                    if (merged) {
                        mst_edges.push_back(edge);
                        if (heaviest_edges[i] || (from_cluster_finished || to_cluster_finished)) {
                            cluster_finder.set_finished(from_cluster);
                            cluster_finder.set_finished(to_cluster);
                        }
                    } else {
                        if (heaviest_edges[i]) {
                            cluster_finder.set_finished(to_cluster);
                        }
                    }
                }

                cluster_finder.flatten();
            }
            double t_end_rank0 = MPI_Wtime();
            t_rank0 = t_end_rank0 - t_start_rank0;


            cluster_finder_id = cluster_finder.get_id();
            // get num_cluster which is the number of unique elements in cluster_finder_id
            std::set<int> unique_cluster_finder_id(cluster_finder_id, cluster_finder_id + num_vertex);
            num_cluster = unique_cluster_finder_id.size();

            // broad cast num_cluster and cluster_finder_id to all processes
            // comm4
            double t_start_comm4 = MPI_Wtime();
            boost::mpi::broadcast(world, num_cluster, 0);
            boost::mpi::broadcast(world, cluster_finder_id, num_vertex, 0);
            double t_end_comm4 = MPI_Wtime();
            t_comm4 = t_end_comm4 - t_start_comm4;

            if (rank != 0) {
                cluster_finder.set_id(cluster_finder_id);
            }
            cluster_finder.reset_finished();

            for (int vertex_local = 0; vertex_local < num_vertex_local; ++vertex_local) {
                clusters_local[vertex_local].clear();
            }

            for (int vertex = 0; vertex < num_vertex; ++vertex) {
                int cluster_leader = cluster_finder.get_cluster_leader(vertex);
                if (graph_local.get_vertex_machine(cluster_leader) == rank) {
                    clusters_local[cluster_leader - vertex_local_start].push_back(vertex);
                }
            }

            double t_end_all = MPI_Wtime();

            LogDist log;
            log.k = k;
            log.t_total = t_end_all - t_start_all;
            log.t_mpi = t_comm0 + t_comm1 + t_comm2 + t_comm3 + t_comm4;
            log.t_comm0 = t_comm0;
            log.t_comm1 = t_comm1;
            log.t_comm2 = t_comm2;
            log.t_comm3 = t_comm3;
            log.t_comm4 = t_comm4;
            log.t_rank0 = t_rank0;

            logs.push_back(log);

            k++;

            if (k >= 10) {
                throw std::runtime_error("k >= 10");
            }

            if (num_cluster == 1) {
                break;
            }
        }

        double t_total_end = MPI_Wtime();

        AlgoMPIResult result;
        result.logs = logs;
        result.mst_edges = mst_edges;
        result.k = k;

        double t_total = t_total_end - t_total_start;

        double t_mpi = 0;
        double t_rank0 = 0;

        for (auto log : result.logs) {
            t_mpi += log.t_mpi;
            t_rank0 += log.t_rank0;
        }

        result.t_total = t_total;
        result.t_mpi = t_mpi;
        result.t_rank0 = t_rank0;

        return result;
    }
}