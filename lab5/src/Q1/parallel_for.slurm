#!/bin/bash
#SBATCH -J zhb -p work
#SBATCH -N 1                     
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36
module load intel/18.0.1
module load IMPI/2018.1.163-icc-18.0.1
module load opt/numactl/2.0.13 
sh run.sh
