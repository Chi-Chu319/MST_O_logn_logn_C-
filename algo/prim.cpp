#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "algo/algo.h"

std::vector<int> MSTSolver::prim(GraphLocal graph_local) {
    std::vector<std::vector<double>> graph = graph_local.get_vertices();
    int n = graph.size();
    std::vector<int> parent(n, -1);
    std::vector<double> key(n, std::numeric_limits<double>::max());
    std::vector<bool> mstSet(n, false);

    auto start = std::chrono::high_resolution_clock::now();
    key[0] = 0;
    for (int count = 0; count < n - 1; count++) {
        int u = -1;
        double min_key = std::numeric_limits<double>::max();
        for (int i = 0; i < n; i++) {
            if (!mstSet[i] && key[i] < min_key) {
                u = i;
                min_key = key[i];
            }
        }
        mstSet[u] = true;
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !mstSet[v] && graph[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph[u][v];
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t_total = end - start;

    return parent;
}