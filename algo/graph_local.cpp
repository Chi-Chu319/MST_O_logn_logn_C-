#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <mpi.h>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include "algo/algo.h"

double GraphLocal::random_weight() {
    return dist(rng);
}

GraphLocal::GraphLocal() {
}

GraphLocal::GraphLocal(int comm_size, int rank, int num_vertices_local, int max_weight)
    : comm_size(comm_size), rank(rank), num_vertices_local(num_vertices_local), max_weight(max_weight),
        num_vertices(comm_size * num_vertices_local), vertex_local_start(rank * num_vertices_local),
        dist(0, max_weight) {
    std::random_device dev;
    this->rng = std::mt19937(dev());
    vertices.resize(num_vertices_local, std::vector<double>(num_vertices, 0));
}

int GraphLocal::get_num_vertices() const {
    return num_vertices;
}

void GraphLocal::set_vertices(const std::vector<std::vector<double>>& vertices) {
    this->vertices = vertices;
}

std::vector<std::vector<double>> GraphLocal::generate() {
    std::vector<std::vector<double>> sendbuf(comm_size);

    for (int vertex_local = 0; vertex_local < num_vertices_local; ++vertex_local) {
        int vertex_from = vertex_local + vertex_local_start;
        for (int vertex_to = 0; vertex_to < vertex_from; ++vertex_to) {
            double random_weight = this->random_weight();
            vertices[vertex_local][vertex_to] = random_weight;
            sendbuf[get_vertex_machine(vertex_to)].push_back(random_weight);
        }
    }

    return sendbuf;
}

void GraphLocal::fill(const std::vector<std::vector<double>>& recvbuf) {
    for (int from_rank = 0; from_rank < static_cast<int>(recvbuf.size()); ++from_rank) {
        int idx = 0;
        if (from_rank < rank) continue;
        for (int vertex_local = 0; vertex_local < num_vertices_local; ++vertex_local) {
            int vertex_from = vertex_local + num_vertices_local * from_rank;
            for (int vertex_to = vertex_local_start; vertex_to < std::min(vertex_from, vertex_local_start + num_vertices_local); ++vertex_to) {
                vertices[vertex_to - vertex_local_start][vertex_from] = recvbuf[from_rank][idx++];
            }
        }
    }

    for (int vertex_local = 0; vertex_local < num_vertices_local; ++vertex_local) {
        int vertex = vertex_local + vertex_local_start;
        vertices[vertex_local][vertex] = std::numeric_limits<double>::max();
    }
}

int GraphLocal::get_vertex_local_start() const {
    return vertex_local_start;
}

int GraphLocal::get_num_vertices_local() const {
    return num_vertices_local;
}

const std::vector<std::vector<double>>& GraphLocal::get_vertices() const {
    return vertices;
}

int GraphLocal::get_comm_size() const {
    return comm_size;
}

int GraphLocal::get_vertex_machine(int vertex) const {
    return vertex / num_vertices_local;
}

