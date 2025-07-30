#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "missile.h"
#include "radar.h"
#include "intercept.h"

#define RADIUS 100.0
#define DT 0.1

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Set default values
    int num_missiles = 200;
    int steps = 300;

    //Parse command-line arguments (argc/argv)
    if (argc >= 2) num_missiles = atoi(argv[1]);
    if (argc >= 3) steps = atoi(argv[2]);
    if (rank == 0) {
        printf("Launching simulation: %d missiles, %d steps, %d processes\n", num_missiles, steps, size);
    }

    //Allocate missile array dynamically
    Missile *missiles = malloc(sizeof(Missile) * num_missiles);

    //Initialize missiles (on rank 0)
    if (rank == 0) {
        for (int i = 0; i < num_missiles; i++) {
            missiles[i].id = i;

            float theta = ((float)rand() / RAND_MAX) * M_PI;
            float phi = ((float)rand() / RAND_MAX) * 2 * M_PI;
            float r = RADIUS + 10.0;

            missiles[i].x = r * sin(theta) * cos(phi);
            missiles[i].y = r * sin(theta) * sin(phi);
            missiles[i].z = r * cos(theta);

            float norm = sqrt(missiles[i].x * missiles[i].x +
                              missiles[i].y * missiles[i].y +
                              missiles[i].z * missiles[i].z);

            missiles[i].vx = -missiles[i].x / norm;
            missiles[i].vy = -missiles[i].y / norm;
            missiles[i].vz = -missiles[i].z / norm;

            missiles[i].active = 1;
        }
    }

    //Broadcast missiles to all processes
    MPI_Bcast(missiles, sizeof(Missile) * num_missiles, MPI_BYTE, 0, MPI_COMM_WORLD);

    float radar_x, radar_y, radar_z;
    float theta_min, theta_max, phi_min, phi_max;
    get_radar_coordinates(rank, size, RADIUS, &radar_x, &radar_y, &radar_z,
                          &theta_min, &theta_max, &phi_min, &phi_max);

    int intercept_count = 0;
    double start_time = MPI_Wtime();

    for (int t = 0; t < steps; t++) {
        for (int i = 0; i < num_missiles; i++) {
            if (missiles[i].active)
                update_position(&missiles[i], DT);
        }

        Missile *local = malloc(sizeof(Missile) * num_missiles);
        int local_count = 0;

        for (int i = 0; i < num_missiles; i++) {
            if (missiles[i].active &&
                is_missile_in_zone(missiles[i].x, missiles[i].y, missiles[i].z,
                                   theta_min, theta_max, phi_min, phi_max)) {
                local[local_count++] = missiles[i];
                missiles[i].active = 0;  // lock it from other zones
            }
        }

        process_interception(local, local_count, rank, radar_x, radar_y, radar_z, &intercept_count);
        free(local);

        MPI_Barrier(MPI_COMM_WORLD); // sync all ranks
    }

    double end_time = MPI_Wtime();
    if (rank == 0) {
        printf("Total runtime: %.2f seconds\n", end_time - start_time);
    }

    int *all_counts = NULL;
    if (rank == 0) {
        all_counts = malloc(sizeof(int) * size);
    }

    MPI_Gather(&intercept_count, 1, MPI_INT, all_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n--- Interception Summary ---\n");
        FILE *f = fopen("../report/intercept_counts.csv", "w");
        for (int i = 0; i < size; i++) {
            printf("Zone %d intercepted %d missiles\n", i, all_counts[i]);
            fprintf(f, "%d,%d\n", i, all_counts[i]);
        }
        fclose(f);
        free(all_counts);
    }

    free(missiles);
    MPI_Finalize();
    return 0;
}
