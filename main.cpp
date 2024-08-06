#include <mpi.h>
#include "algo/algo.h"
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <chrono>
#include <float.h>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    boost::mpi::environment env;
    boost::mpi::communicator world;

    int rank = world.rank();
    int size = world.size();

    int num_vertices = 16384;
    int num_proc = size;
    int num_vertex_local = num_vertices / num_proc;
    
    AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, num_vertex_local);

    std::string csv_filename = "csvs/strong_scale_16384.csv";
    std::vector<std::string> row {std::to_string(result.t_total), std::to_string(result.t_mpi), std::to_string(result.t_rank0), std::to_string(num_proc), std::to_string(num_vertex_local)  };

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
