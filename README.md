# MST_O_logn_logn_C++


## Overview

This repository contains an implementation of the distributed Minimum Spanning Tree (MST) algorithm with O(log log n) complexity. The algorithm is based on the research paper "MST construction in O(log log n) communication rounds" presented in June 2003 (DOI:10.1145/777412.777428).

The implementation is written in C++ and utilizes MPI for communication between nodes in a distributed computing environment.

## Prerequisites

Before building and running the project, ensure that you have the following installed:
- An MPI implementation (e.g., MPICH, OpenMPI)
- Boost library (version 1.79.0 with MPI support)
- A C++ compiler supporting C++11 or later
- Debugging tools (Optional for debugging)

## How to Build

To build the project, you need to load the necessary module for the Boost library and then use `make` to compile the code. Follow these steps:

1. Load the Boost module with MPI support:
```
module load boost/1.79.0-mpi
```

2. Compile the project:
```
make
```

## How to Run

After building the project, you can run it using the provided batch script. This script submits a job to an MPI environment. To submit the job, execute the following command:
```
batch mst_job.sh
```

Ensure that `mst_job.sh` is properly configured for your computing environment and job scheduler (e.g., Slurm, PBS).

## How to Debug

For debugging, this project supports debugging with DDT. To start a debugging session, you will need to load the DDT module, allocate resources using `salloc`, and then start DDT in offline mode with `srun`. Here are the steps:

1. Load the DDT module:
```
module load ddt
```

2. Allocate resources and start a debugging session:
```
salloc --nodes=1 --ntasks-per-node=8 --time=00:30:00 --partition=small --account=xxx ddt --offline --output=output.html srun ./main
```

Replace `xxx` with your account information. The `--output` flag specifies the name of the file where the debugging session output will be saved.
