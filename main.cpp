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

    int num_proc = 32;
    int num_vertex_local = 1024;

    // vector of vectors of size 2 (double)
    std::string csv_filename = "strong_scaling/strong_scale.csv";
    // std::vector<std::string> csv_fields {"t_mpi", "t_mpi_mpi", "t_mpi_local", "num_proc", "num_vertex_per_proc"};

    AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, num_vertex_local);

    if (rank == 0) {
        // CsvUtil::add_csv_row(csv_filename, csv_fields);

        std::vector<std::string> row {std::to_string(result.t_total), std::to_string(result.t_mpi_total), std::to_string(result.t_local_total), std::to_string(num_proc), std::to_string(num_vertex_local)};

        CsvUtil::add_csv_row(csv_filename, row);

        // open a file and print result.logs
        std::ostringstream stringStream;
        stringStream << "strong_scaling/strong_scale_t";
        stringStream << std::to_string(num_proc);
        stringStream << "_";
        stringStream << std::to_string(num_vertex_local);
        stringStream << "_logs.txt";
        std::string scaling_detail_filename = stringStream.str();
        
        std::ofstream scaling_detail_file(scaling_detail_filename);

        for (const LogDist& log : result.logs) {
            scaling_detail_file << log.k << ", " << log.t_total << ", " << log.t_comm0 << ", " << log.t_comm1 << ", " << log.t_comm2 << ", " << log.t_comm3 << ", " << log.t_comm4 << std::endl;
        }

        scaling_detail_file.close();
    }

    return 0;
}
