#!/bin/bash
#SBATCH --job-name=mpiTest
#SBATCH --account=project_2009665
#SBATCH --time=02:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --mem-per-cpu=8G
#SBATCH --output=mst_result.out
#SBATCH --partition=large
mpicc -o main.cpp main -lm  # Compile your program

time srun main 128 1000 10
