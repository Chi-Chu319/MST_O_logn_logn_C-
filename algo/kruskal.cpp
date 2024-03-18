// kruskal's algorithm for MST

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "algo/algo.h"

AlgoMPIResult MSTSolver::kruskal(GraphLocal graph_local, int rank, int size) {
    std::vector<std::vector<double>> graph = graph_local.get_vertices();
    int n = graph.size();
    std::vector<Edge> edges;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            edges.push_back(Edge(i, j, graph[i][j]));
        }
    }
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.weight < b.weight;
    });

    QuickUnion qu(new int[n], n);
    std::vector<Edge> mst;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < edges.size(); i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int set_u = qu.get_cluster_leader(u);
        int set_v = qu.get_cluster_leader(v);
        if (set_u != set_v) {
            mst.push_back(edges[i]);
            qu.set_finished(u);
            qu.set_finished(v);
            qu.set_id(set_u, set_v);
        }
    }
    qu.flatten();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t_total = end - start;

    AlgoMPILog log;
    log.t_total = t_total.count();
    log.t_mpi = 0;
    logs.push_back(log);

    AlgoMPIResult result;
    result.mst_edges = mst;
    result.logs = logs;

    return result;
}