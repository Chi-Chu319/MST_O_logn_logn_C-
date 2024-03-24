#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include "algo/algo.h"

double MSTSolver::prim(Graph graph) {
    std::vector<std::vector<double>> vertices = graph.get_vertices();
    int n = vertices.size();
    std::vector<int> parent(n, -1);
    std::vector<double> key(n, std::numeric_limits<double>::max());
    std::vector<bool> mstSet(n, false);

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
            if (!mstSet[v] && vertices[u][v] < key[v]) {
                parent[v] = u;
                key[v] = vertices[u][v];
            }
        }
    }

    double sum_weight = 0;
    for (int i = 1; i < n; i++) {
        sum_weight += vertices[i][parent[i]];
    }

    return sum_weight;
}