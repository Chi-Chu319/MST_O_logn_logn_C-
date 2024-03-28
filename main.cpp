#include <mpi.h>
#include "algo/algo.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <chrono>
#include <float.h>
#include <string>

int main(int argc, char *argv[]) {
    boost::mpi::environment env;
    boost::mpi::communicator world;

    int rank = world.rank();
    int size = world.size();

    int k = 0;
    int i = 2;
    int k_max = 11;
    int num_vertex_local;

    // vector of vectors of size 2 (double)
    std::string csv_filename = "dist_n1_t4.csv";
    std::vector<std::string> csv_fields {"num_vertex_local", "k", "t_mpi_local", "t_mpi_mpi", "t_mpi"};
    std::vector<std::vector<std::string>> csv_data(k_max + 1);

    while (k <= k_max) {
        std::vector<GraphLocal> sendbuf;

        num_vertex_local = i;
        
        GraphLocal graph_local = GraphUtil::generate_distributed_clique_graph(world, rank, size, 10, num_vertex_local);

        // mpi wall time
        double mpi_start = MPI_Wtime();
        auto result = MSTSolver::algo_mpi(world, graph_local, rank, size);
        double mpi_end = MPI_Wtime();

        if (rank == 0) {
            // time the algo

            double t_mpi_mpi = 0;

            for (auto log : result.logs) {
                t_mpi_mpi += log.t_mpi;
            }

            double t_mpi_local = (mpi_end - mpi_start) - t_mpi_mpi;

            csv_data[k] = {std::to_string(num_vertex_local * size),  std::to_string(result.k), std::to_string(t_mpi_local), std::to_string(t_mpi_mpi), std::to_string(mpi_end - mpi_start)};
        }


        i *= 2;
        k += 1;
    }

    if (rank == 0) {
        CsvUtil::save_csv(csv_filename, csv_fields, csv_data);
    }

    return 0;
}
