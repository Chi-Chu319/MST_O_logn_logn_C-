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

    std::string csv_filename = "csvs/n1_t8_16384_details_original.csv";

    AlgoMPIResult result = Tester::algo_mpi_test(world, rank, size, 10, 2048);

    if (rank == 0) {
        std::vector<std::string> csv_fields {"round", "t_total", "t_mpi", "t_rank0", "comm0", "comm1", "comm2", "comm3", "comm4"};
        CsvUtil::add_csv_row(csv_filename, csv_fields);

        for (int i = 0; i < result.logs.size(); i++) {
            LogDist log = result.logs[i];
            std::vector<std::string> csv_row {std::to_string(log.k), std::to_string(log.t_total), std::to_string(log.t_mpi), std::to_string(log.t_rank0), std::to_string(log.t_comm0), std::to_string(log.t_comm1), std::to_string(log.t_comm2), std::to_string(log.t_comm3), std::to_string(log.t_comm4)};

            CsvUtil::add_csv_row(csv_filename, csv_row);
        }    
    }

    return 0;
}
