//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// ===== intercept.c =====
#include <math.h>
#include <stdio.h>
#include <omp.h>
#include "intercept.h"

void process_interception(Missile *missiles, int count, int rank,
                          float xr, float yr, float zr, int *intercept_count, FILE *log) {
    int local_count = 0;
#pragma omp parallel for reduction(+:local_count)
    for (int i = 0; i < count; i++) {
        if (!missiles[i].active) continue;

        float dx = missiles[i].x - xr;
        float dy = missiles[i].y - yr;
        float dz = missiles[i].z - zr;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);

        if (dist < 50.0f) {
            missiles[i].active = 0;
            local_count++;
#pragma omp critical
            fprintf(log, "Missile #%d intercepted at (%.1f, %.1f, %.1f) by Process #%d\n",
                    missiles[i].id, missiles[i].x, missiles[i].y, missiles[i].z, rank);
        }
    }
    *intercept_count += local_count;
}