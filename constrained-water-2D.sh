#!/bin/sh
#SBATCH --mem-per-cpu=1000
#SBATCH --mail-type ALL
#SBATCH --mail-user jrbgallagher@gmail.com

set -ev

hostname
date

time nice -19 paper/figs/constrained-water-2D.mkdat

date