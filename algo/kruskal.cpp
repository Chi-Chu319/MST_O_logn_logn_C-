// kruskal's algorithm for MST

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "algo/algo.h"

std::vector<ClusterEdge> MSTSolver::kruskal(GraphLocal graph_local) {
    std::vector<std::vector<double>> graph = graph_local.get_vertices();
    int n = graph.size();
    std::vector<ClusterEdge> edges;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            edges.push_back(ClusterEdge(i, j, graph[i][j]));
        }
    }
    std::sort(edges.begin(), edges.end(), [](const ClusterEdge& a, const ClusterEdge& b) {
        return a.weight < b.weight;
    });

    int* qu_id = new int[n];
    for (int i = 0; i < n; i++) {
        qu_id[i] = i;
    }

    QuickUnion qu(qu_id, n);
    std::vector<ClusterEdge> mst;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < edges.size(); i++) {
        int u = edges[i].from_v;
        int v = edges[i].to_v;

        bool merged = qu.safe_union(u, v);

        if (merged) {
            mst.push_back(edges[i]);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t_total = end - start;

    return mst;
}