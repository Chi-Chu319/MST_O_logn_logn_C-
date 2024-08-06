#!/bin/bash
#SBATCH --job-name=mpiTest
#SBATCH --account=project_2009665
#SBATCH --time=02:00:00
#SBATCH --nodes=16
#SBATCH --ntasks-per-node=32
#SBATCH --mem-per-cpu=4G
#SBATCH --output=mst_result.out
#SBATCH --partition=large
module load boost/1.79.0-mpi

time srun ./main
