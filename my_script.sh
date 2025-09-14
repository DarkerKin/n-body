#!/bin/bash
#SBATCH --job-name=particle_sim
#SBATCH --partition=Centaurus
#SBATCH --time=20:00:00
#SBATCH --output=slurm-%j.out
#SBATCH --error=slurm-%j.err
#SBATCH --mem=20G

#load anaconda
module load anaconda3/2023.09

# create or activate conda environment
if ! conda env list | grep -q "myenv"; then
    echo "Creating conda environment..."
    conda create -y -n myenv python=3.12
    source activate myenv   
    pip install --upgrade pip
    pip install -r requirements.txt
else
    echo "Using existing conda environment..."
    source activate myenv
fi


# Run the tests
make test-1
make test-2
make test-3