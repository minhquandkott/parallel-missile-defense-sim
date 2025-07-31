// ===== main.c =====
#include <stdio.h>
#include <stdlib.h>
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

    FILE *logfile = fopen("intercept_log.txt", "a");
    if (!logfile) {
        if (rank == 0) fprintf(stderr, "Cannot open log file!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int num_missiles = 200;
    int steps = 300;
    if (argc >= 2) num_missiles = atoi(argv[1]);
    if (argc >= 3) steps = atoi(argv[2]);

    if (rank == 0) {
        fprintf(logfile, "Launching simulation: %d missiles, %d steps, %d processes\n", num_missiles, steps, size);
        fflush(logfile);
    }

    Missile *missiles = malloc(sizeof(Missile) * num_missiles);
    if (!missiles) {
        fprintf(stderr, "Process %d: Failed to allocate missiles\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        initialize_missiles(missiles, num_missiles, RADIUS);
    }

    MPI_Bcast(missiles, sizeof(Missile) * num_missiles, MPI_BYTE, 0, MPI_COMM_WORLD);

    float radar_x, radar_y, radar_z;
    float theta_min, theta_max, phi_min, phi_max;
    get_radar_coordinates(rank, size, RADIUS,
                          &radar_x, &radar_y, &radar_z,
                          &theta_min, &theta_max,
                          &phi_min, &phi_max);

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
                missiles[i].active = 0;
            }
        }

        process_interception(local, local_count, rank, radar_x, radar_y, radar_z, &intercept_count, logfile);
        free(local);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    double end_time = MPI_Wtime();
    if (rank == 0) {
        fprintf(logfile, "Total runtime: %.2f seconds\n", end_time - start_time);
        fflush(logfile);
    }

    int *all_counts = (rank == 0) ? malloc(sizeof(int) * size) : NULL;
    MPI_Gather(&intercept_count, 1, MPI_INT, all_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        fprintf(logfile, "\n--- Interception Summary ---\n");
        for (int i = 0; i < size; i++) {
            fprintf(logfile, "Zone %d intercepted %d missiles\n", i, all_counts[i]);
        }
        fflush(logfile);
        free(all_counts);
    }

    free(missiles);
    fclose(logfile);
    MPI_Finalize();
    return 0;
}