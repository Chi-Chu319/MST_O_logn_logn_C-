#include "algo/algo.h"

namespace Tester {
    AlgoMPIResult algo_mpi_test(boost::mpi::communicator world, int rank, int size, int max_weight, int num_vertices_local) {
        GraphLocal graph_local = GraphUtil::generate_distributed_clique_graph(world, rank, size, max_weight, num_vertices_local);

        // mpi wall time
        auto result = MSTSolver::algo_mpi(world, graph_local, rank, size);

        // delete graph_local
        graph_local.clear()

        return result;
    }
}