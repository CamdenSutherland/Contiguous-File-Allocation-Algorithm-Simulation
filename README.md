# Contiguous-File-Allocation-Algorithm-Simulation
Simple program to simulate the contiguous allocation algorithms   

## Usage:
simulate <FF|WF|BF> -NMAX <number> -TMAX <number> -JMAX <number> -PALLOC <number> -PDEALLOC <number>
  
## Options:
Algorithm <FF|WF|BF>: FF: First Fit | WF: Worst Fit | BF: Best Fit

NMAX - Maximum number of storage blocks

TMAX - Maximum number of time units to simulate over

JMAX - Maximum number of contiguous storage blocks a job can request

PALLOC - Probability of a new job for each time unit

PEDALLOC - Probability of an allocated job is deallocated for each time unit

## Examble:
e.g. To simulate first fit on a virtual machine with 1024 blocks of storage type the below to run:
simulate FF -NMAX 1024 -TMAX 10000 -JMAX 20 -PALLOC 0.45 -PDEALLOC 0.35
