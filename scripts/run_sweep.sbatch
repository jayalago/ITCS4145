#!/bin/bash
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --ntasks=1

module load gcc
make sweep

mkdir -p data
./sweep_bandwidth > data/sweep.csv