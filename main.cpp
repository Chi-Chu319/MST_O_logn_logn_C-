#include <mpi.h>
#include "algo/algo.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>

int main(int argc, char *argv[]) {
    boost::mpi::environment env;
    boost::mpi::communicator world;

    int rank = world.rank();
    int size = world.size();

    GraphLocal graph_local = GraphLocal(size, rank, 2, 10);

    std::vector<std::vector<double>> sendbuf = graph_local.generate();
    std::vector<std::vector<double>> recvbuf;

    boost::mpi::all_to_all<std::vector<double>>(world, sendbuf, recvbuf);

    graph_local.fill(recvbuf);

    AlgoMPIResult result = MSTSolver::algo_mpi(world, graph_local, rank, size);

    if (rank == 0) {
        double sum_weight = 0;
        double t_dist_all = 0;
        double t_dist_mpi = 0;
        for (int i = 0; i < result.logs.size(); i++) {
            t_dist_all += result.logs[i].t_total;
            t_dist_mpi += result.logs[i].t_mpi;
        }

        // Print sum weight loop through result.mst_edges to get edge.weight
        for (int i = 0; i < result.mst_edges.size(); i++) {
            sum_weight += result.mst_edges[i].weight;
        }

        std::cout << "Sum weight: " << sum_weight << std::endl;
        std::cout << "Total time: " << t_dist_all << std::endl;
        std::cout << "MPI time: " << t_dist_mpi << std::endl;
    }

    return 0;
}
