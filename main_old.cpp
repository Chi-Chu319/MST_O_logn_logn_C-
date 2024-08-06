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


    int k = 0;
    int i = 2;
    int k_max = 12;

    std::vector<std::string> csv_fields {"num_vertices", "k", "t_rank0", "t_mpi", "t_total"};
    std::vector<std::string> csv_fields {"round", "total time (second)", "comm0 time", "comm1 time", "comm2 time", "comm3 time", "comm4 time"};

    // std::vector<std::vector<std::string>> csv_data;
    // while (k <= k_max) {
    //     AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, i);
        
    //     if (rank == 0) {
    //         std::vector<std::string> row {std::to_string(i * size), std::to_string(k), std::to_string(result.t_local_total), std::to_string(result.t_mpi_total), std::to_string(result.t_total) };
    //         csv_data.push_back(row);
    //     }
        
    //     k += 1;
    //     i *= 2;
    // }
    
    // if (rank == 0) {
    //     std::string csv_filename = "dist_n1_t1_original.csv";
    //     CsvUtil::save_csv(csv_filename, csv_fields, csv_data);
    // }

    // // vector of vectors of size 2 (double)
    std::string csv_filename = "8_1024.csv";
    // // std::vector<std::string> csv_fields {"t_mpi", "t_mpi_mpi", "t_mpi_local", "num_proc", "num_vertex_per_proc"};

    if (rank == 0) {
        CsvUtil::add_csv_row(csv_filename, csv_fields);

        std::vector<std::string> row {std::to_string(result.t_total), std::to_string(result.t_mpi_total), std::to_string(result.t_local_total), std::to_string(num_proc), std::to_string(num_vertex_local)};

        CsvUtil::add_csv_row(csv_filename, row);

        std::ofstream scaling_detail_file(csv_filename);

        for (const LogDist& log : result.logs) {
            scaling_detail_file << log.k << ", " << log.t_total << ", " << log.t_comm0 << ", " << log.t_comm1 << ", " << log.t_comm2 << ", " << log.t_comm3 << ", " << log.t_comm4 << std::endl;
        }

        scaling_detail_file.close();
    }

    return 0;
}
