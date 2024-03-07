#include <mpi.h>
#include <boost/mpi.hpp>
#include "algo.h"
#include <boost/mpi/collectives.hpp>
#include <vector>

void algo_mpi(boost::mpi::communicator world, GraphLocal graph_local, int rank, int size) {
    int num_vertex_local = graph_local.get_num_vertices_local();

    int vertex_local_start = rank * num_vertex_local;
    int num_vertex = num_vertex_local * size;

    // phase
    int k = 0;

    std::vector<std::vector<int>> clusters_local(num_vertex);
    int* cluster_finder_id = new int[num_vertex];

    for (int i = 0; i < num_vertex; ++i) {
        cluster_finder_id[i] = i;
    }

    QuickUnion cluster_finder(cluster_finder_id, num_vertex);
    int num_cluster = num_vertex;

    while (true)
    {
        double mpi_time = 0.0;
        // mpi get time
        double s_start_all = MPI_Wtime();

        cluster_finder.set_id(cluster_finder_id);
        cluster_finder.reset_finished();
    }
}

