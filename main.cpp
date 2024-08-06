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

    int num_proc = 8;
    int num_vertex_local = 1024;
    
    AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, 1024);

    std::string csv_filename = "n8_t1_composition.csv";

    int k = 0;
    int i = 2;
    int k_max = 12;

    std::vector<std::string> csv_fields {"num_vertices", "t_rank0", "t_mpi", "t_total"};
    if (rank == 0) {
        CsvUtil::add_csv_row(csv_filename, csv_fields);
    }

    while (k <= k_max) {
        AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, i);
        
        if (rank == 0) {
            std::vector<std::string> row {std::to_string(i * size), std::to_string(result.t_rank0), std::to_string(result.t_mpi), std::to_string(result.t_total) };
            CsvUtil::add_csv_row(csv_filename, row);
        }
        
        k += 1;
        i *= 2;
    }

    return 0;
}
