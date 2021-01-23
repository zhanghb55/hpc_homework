#!/bin/bash
#SBATCH -J zhb -p work             # 任务名
#SBATCH -N 8                     # 申请 4 个节点
#SBATCH --ntasks-per-node=1     # 每个节点开 1 个进程
#SBATCH --cpus-per-task=1        # 每个进程占用 20 个 core
module load intel/18.0.1         # 添加 intelcompiler/18.0.0 模块
module load IMPI/2018.1.163-icc-18.0.1
export I_MPI_FAVRICS=shm:dapl
export OMP_NUM_THREADS=20         # 设置全局 OpenMP 线程为 20
#mpirun ./a.out
valgrind --tool=massif --stacks=yes mpirun ./a.out
