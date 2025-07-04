//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// intercept.c
#include <stdio.h>
#include <omp.h>
#include "intercept.h"

void process_interception(Missile *missiles, int count, int rank, float radar_y, int *intercept_count) {
#pragma omp parallel for
    for (int i = 0; i < count; i++) {
        if (!missiles[i].active) continue;

        float time_to_intercept = (missiles[i].y - radar_y) / (-missiles[i].vy);
        if (time_to_intercept > 0 && time_to_intercept < 5.0) {
            missiles[i].active = 0;

#pragma omp critical
            {
                printf("Missile #%d intercepted at (%.1f, %.1f) by Process #%d\n",
                       missiles[i].id, missiles[i].x, missiles[i].y, rank);
                (*intercept_count)++;
            }
        }
    }
}


