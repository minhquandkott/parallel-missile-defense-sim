#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "missile.h"
#include "radar.h"
#include "intercept.h"

#define NUM_MISSILES 100
#define WIDTH 100.0
#define HEIGHT 100.0
#define DT 0.1
#define STEPS 500

int main() {
    int rank, size;
    int intercept_count = 0;
    init_mpi(&rank, &size);

    Missile missiles[NUM_MISSILES];
    if (rank == 0) {
        for (int i = 0; i < NUM_MISSILES; i++) {
            missiles[i].id = i;
            missiles[i].x = rand() % (int)WIDTH;
            missiles[i].y = HEIGHT;
            missiles[i].vx = 0;
            missiles[i].vy = -1.0 * (rand() % 3 + 1); // fall down
            missiles[i].active = 1;
        }
    }

    MPI_Bcast(missiles, sizeof(Missile) * NUM_MISSILES, MPI_BYTE, 0, MPI_COMM_WORLD);

    for (int t = 0; t < STEPS; t++) {
        for (int i = 0; i < NUM_MISSILES; i++) {
            if (missiles[i].active) update_position(&missiles[i], DT);
        }

        // filter and intercept
        Missile local[NUM_MISSILES];
        int local_count = 0;
        for (int i = 0; i < NUM_MISSILES; i++) {
            if (missiles[i].active &&
                is_missile_in_zone(missiles[i].x, rank, size, WIDTH)) {
                local[local_count++] = missiles[i];
                }
        }

        process_interception(local, local_count, rank, 0.0, &intercept_count);

        MPI_Barrier(MPI_COMM_WORLD);
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
            fprintf(f, "%d,%d\n", i, all_counts[i]);  // zone_id, count
        }

        fclose(f);
        free(all_counts);
    }


    MPI_Finalize();
    return 0;
}
