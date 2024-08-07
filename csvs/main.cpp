#include <mpi.h>
#include "algo/algo.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <chrono>
#include <float.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char *argv[]) {
    boost::mpi::environment env;
    boost::mpi::communicator world;

    int rank = world.rank();
    int size = world.size();

    int initial_num_vertices_local = 1024;
    int initial_num_threads = 2;

    int num_threads = size;
    int num_vertices_local = initial_num_vertices_local / sqrt(num_threads / initial_num_threads);

    AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, num_vertices_local);

    std::string csv_filename = "csvs/weak_scale_1024.csv";
    std::vector<std::string> row {std::to_string(result.t_total), std::to_string(result.t_mpi), std::to_string(result.t_rank0), std::to_string(num_threads), std::to_string(num_vertex_local)  };

    if (rank == 0) {
        CsvUtil::add_csv_row(csv_filename, row);
    }

    // int k = 0;
    // int i = 2;
    // int k_max = 12;

    // std::vector<std::string> csv_fields {"num_vertices", "t_rank0", "t_mpi", "t_total"};
    // if (rank == 0) {
    //     CsvUtil::add_csv_row(csv_filename, csv_fields);
    // }

    // while (k <= k_max) {
    //     AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, i);
        
    //     if (rank == 0) {
    //         std::vector<std::string> row {std::to_string(i * size), std::to_string(result.t_rank0), std::to_string(result.t_mpi), std::to_string(result.t_total) };
    //         CsvUtil::add_csv_row(csv_filename, row);
    //     }
        
    //     k += 1;
    //     i *= 2;
    // }

    return 0;
}
