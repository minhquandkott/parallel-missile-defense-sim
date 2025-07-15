#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "missile.h"
#include "radar.h"
#include "intercept.h"

#define NUM_MISSILES 200
#define RADIUS 100.0
#define DT 0.1
#define STEPS 300

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Missile missiles[NUM_MISSILES];

    if (rank == 0) {
        for (int i = 0; i < NUM_MISSILES; i++) {
            missiles[i].id = i;
            // Spawn random outside sphere
            float theta = ((float)rand() / RAND_MAX) * M_PI;
            float phi = ((float)rand() / RAND_MAX) * 2 * M_PI;
            float r = RADIUS + 50.0; // start outside

            missiles[i].x = r * sin(theta) * cos(phi);
            missiles[i].y = r * sin(theta) * sin(phi);
            missiles[i].z = r * cos(theta);

            // Velocity vector towards center (roughly)
            missiles[i].vx = -missiles[i].x / sqrt(missiles[i].x*missiles[i].x + missiles[i].y*missiles[i].y + missiles[i].z*missiles[i].z);
            missiles[i].vy = -missiles[i].y / sqrt(missiles[i].x*missiles[i].x + missiles[i].y*missiles[i].y + missiles[i].z*missiles[i].z);
            missiles[i].vz = -missiles[i].z / sqrt(missiles[i].x*missiles[i].x + missiles[i].y*missiles[i].y + missiles[i].z*missiles[i].z);

            missiles[i].active = 1;
        }
    }

    MPI_Bcast(missiles, sizeof(Missile) * NUM_MISSILES, MPI_BYTE, 0, MPI_COMM_WORLD);

    float radar_x, radar_y, radar_z;
    float theta_min, theta_max, phi_min, phi_max;
    get_radar_coordinates(rank, size, RADIUS, &radar_x, &radar_y, &radar_z,
                          &theta_min, &theta_max, &phi_min, &phi_max);

    int intercept_count = 0;

    double start_time = MPI_Wtime();

    for (int t = 0; t < STEPS; t++) {
        for (int i = 0; i < NUM_MISSILES; i++) {
            if (missiles[i].active) update_position(&missiles[i], DT);
        }

        Missile local[NUM_MISSILES];
        int local_count = 0;

        for (int i = 0; i < NUM_MISSILES; i++) {
            if (missiles[i].active &&
                is_missile_in_zone(missiles[i].x, missiles[i].y, missiles[i].z,
                                   theta_min, theta_max, phi_min, phi_max)) {
                // if (missiles[i].active) {
                local[local_count++] = missiles[i];
            }
        }

        process_interception(local, local_count, rank, radar_x, radar_y, radar_z, &intercept_count);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double end_time = MPI_Wtime();
    if (rank == 0)
        printf("Total runtime: %.2f seconds\n", end_time - start_time);

    int *all_counts = NULL;
    if (rank == 0) {
        all_counts = malloc(sizeof(int) * size);
    }

    MPI_Gather(&intercept_count, 1, MPI_INT, all_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n--- Interception Summary ---\n");
        FILE *f = fopen("intercept_counts.csv", "w");
        for (int i = 0; i < size; i++) {
            printf("Zone %d intercepted %d missiles\n", i, all_counts[i]);
            fprintf(f, "%d,%d\n", i, all_counts[i]);
        }
        fclose(f);
        free(all_counts);
    }

    MPI_Finalize();
    return 0;
}

