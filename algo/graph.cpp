#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <cmath>
#include "algo.h"

class Graph {
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

    double random_weight() {
        return dist(rng);
    }

public:
    Graph(int comm_size, int num_vertices_local, int expected_degree, int max_weight, bool is_clique)
    : comm_size(comm_size), num_vertices_local(num_vertices_local), expected_degree(expected_degree),
      max_weight(max_weight), is_clique(is_clique), dist(0.0, max_weight) {
        num_vertices = comm_size * num_vertices_local;
        vertices.resize(num_vertices, std::vector<double>(num_vertices, 0.0));
        std::random_device rd;
    }

    Graph generate() {
        double p = static_cast<double>(expected_degree) / (num_vertices - 1);

        for (int i = 0; i < num_vertices; ++i) {
            for (int j = i + 1; j < num_vertices; ++j) {
                if (dist(rng) < p || is_clique) {
                    double weight = random_weight();
                    vertices[i][j] = weight;
                    vertices[j][i] = weight;
                }
            }
        }

        for (int i = 0; i < num_vertices; ++i) {
            vertices[i][i] = std::numeric_limits<double>::max();
        }

        return *this;
    }

    std::vector<GraphLocal> split() {
        std::vector<GraphLocal> result;

        for (int i = 0; i < comm_size; ++i) {
            GraphLocal graphLocal(comm_size, i, num_vertices_local, max_weight);
            // Extract the sub-matrix for this local graph
            std::vector<std::vector<double>> sub_vertices(num_vertices_local, std::vector<double>(num_vertices, 0.0));
            for (int j = 0; j < num_vertices_local; ++j) {
                std::copy(vertices[i * num_vertices_local + j].begin(), vertices[i * num_vertices_local + j].end(), sub_vertices[j].begin());
            }
            graphLocal.set_vertices(sub_vertices);
            result.push_back(graphLocal);
        }

        return result;
    }
};
