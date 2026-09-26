#!/bin/bash
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --ntasks=1

module load gcc
make

mkdir -p data

./array_max > data/array_max.csv

