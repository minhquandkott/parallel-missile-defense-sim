//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// radar.c
#include <mpi.h>
#include "radar.h"

void init_mpi(int *rank, int *size) {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

int is_missile_in_zone(float x, int rank, int size, float width) {
    float zone_width = width / size;
    return (x >= rank * zone_width) && (x < (rank + 1) * zone_width);
}
