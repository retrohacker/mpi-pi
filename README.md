mpi-pi
===

A monte carlo algorithm for approximating pi using MPI.

# Usage

### Compile

`mpicc monte.c -o monte -lm`

### Run

`mpiexec -n <processes> ./monte`
