#!/bin/bash
#SBATCH --job-name=particle_sim
#SBATCH --partition=Centaurus
#SBATCH --time=20:00:00
#SBATCH --output=slurm-%j.out
#SBATCH --error=slurm-%j.err
#SBATCH --mem=20G

# Initialize conda
source ~/miniconda3/etc/profile.d/conda.sh  # adjust path if needed
conda activate myenv


# Run the tests
make test-1
make test-2
make test-3