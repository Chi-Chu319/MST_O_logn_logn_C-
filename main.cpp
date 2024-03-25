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
    int k_max = 10;
    int num_vertex_local;

    // vector of vectors of size 2 (double)
    std::string csv_filename = "seq_vs_dist_n1_t8.csv";
    std::vector<std::string> csv_fields {"num_vertex_local", "k", "t_mpi_local", "t_mpi_mpi", "t_mpi", "t_prim"};
    std::vector<std::vector<std::string>> csv_data(k_max + 1);

    while (k <= k_max) {
        std::vector<GraphLocal> sendbuf;

        num_vertex_local = i;
        
        double sum_weight_prim = 0;
        std::chrono::duration<double> t_total_prim;
        if (rank == 0) {
            Graph graph = GraphUtil::generate_clique_graph(size, 10, num_vertex_local);
            auto start_prim = std::chrono::high_resolution_clock::now();
            sum_weight_prim = MSTSolver::prim(graph);
            auto end_prim = std::chrono::high_resolution_clock::now();
            t_total_prim = end_prim - start_prim;

            sendbuf = graph.split();
        }

        GraphLocal graph_local;

        boost::mpi::scatter<GraphLocal>(world, sendbuf, graph_local, 0);

        // mpi wall time
        double mpi_start = MPI_Wtime();
        auto result = MSTSolver::algo_mpi(world, graph_local, rank, size);
        double mpi_end = MPI_Wtime();

        // loop through result.mst_edges and sum weight
        double sum_weight_mpi = 0;
        for (auto edge : result.mst_edges) {
            sum_weight_mpi += edge.weight;
        }

        if (rank == 0) {
            // time the algo
            std::cout << "Graph size: " << num_vertex_local * size << std::endl;
            std::cout << "K: " << result.k << std::endl;
            std::cout << "MPI Wall Time: " << mpi_end - mpi_start << std::endl;
            std::cout << "t_total_prim time: " << std::chrono::duration<double>(t_total_prim).count() << std::endl;

            double t_mpi_mpi = 0;

            for (auto log : result.logs) {
                t_mpi_mpi += log.t_mpi;
            }

            double t_mpi_local = (mpi_end - mpi_start) - t_mpi_mpi;

            if (fabs(sum_weight_mpi - sum_weight_prim) < DBL_EPSILON) {
                std::cout << "Error: sum_weight_mpi != sum_weight_prim" << std::endl;
                std::cout << "sum_weight_mpi: " << sum_weight_mpi << std::endl;
                std::cout << "sum_weight_prim: " << sum_weight_prim << std::endl;
            }
            csv_data[k] = {std::to_string(num_vertex_local * size),  std::to_string(k), std::to_string(t_mpi_local), std::to_string(t_mpi_mpi), std::to_string(mpi_end - mpi_start), std::to_string(std::chrono::duration<double>(t_total_prim).count())};
        }


        i *= 2;
        k += 1;
    }

    if (rank == 0) {
        CsvUtil::save_csv(csv_filename, csv_fields, csv_data);
    }

    return 0;
}
