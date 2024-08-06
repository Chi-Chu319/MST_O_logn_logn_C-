#!/bin/bash
#SBATCH --job-name=mpiTest
#SBATCH --account=project_2009665
#SBATCH --time=02:00:00
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=1
#SBATCH --mem-per-cpu=16G
#SBATCH --output=mst_result.out
#SBATCH --partition=large
module load boost/1.79.0-mpi

time srun ./main
