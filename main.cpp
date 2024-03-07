#include <mpi.h>
#include "algo/algo.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>

int main(int argc, char *argv[]) {
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    int rank = world.rank();
    int size = world.size();

    GraphLocal graph_local = GraphLocal(size, rank, 10, 10);

    std::vector<std::vector<double>> sendbuf = graph_local.generate();
    std::vector<std::vector<double>> recvbuf;

    boost::mpi::all_to_all<std::vector<double>>(world, sendbuf, recvbuf);

    graph_local.fill(recvbuf);
}
