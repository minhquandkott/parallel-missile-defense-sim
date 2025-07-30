//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// radar.c
#include <math.h>
#include "radar.h"

#include <stdio.h>

void get_radar_coordinates(int rank, int size, float r,
                           float *radar_x, float *radar_y, float *radar_z,
                           float *theta_min, float *theta_max,
                           float *phi_min, float *phi_max) {
    int sqrt_P = (int)(sqrt(size) + 0.5);  // Làm tròn
    if (sqrt_P * sqrt_P != size && rank == 0) {
        printf("⚠️ Warning: Number of processes should be a perfect square for balanced zone splitting.\n");
    }

    int theta_idx = rank / sqrt_P;
    int phi_idx = rank % sqrt_P;

    *theta_min = M_PI * theta_idx / sqrt_P;
    *theta_max = M_PI * (theta_idx + 1) / sqrt_P;
    *phi_min = 2.0 * M_PI * phi_idx / sqrt_P;
    *phi_max = 2.0 * M_PI * (phi_idx + 1) / sqrt_P;

    float theta_center = (*theta_min + *theta_max) / 2.0;
    float phi_center = (*phi_min + *phi_max) / 2.0;

    *radar_x = r * sin(theta_center) * cos(phi_center);
    *radar_y = r * sin(theta_center) * sin(phi_center);
    *radar_z = r * cos(theta_center);
}

int is_missile_in_zone(float x, float y, float z,
                       float theta_min, float theta_max,
                       float phi_min, float phi_max) {
    float r = sqrt(x*x + y*y + z*z);
    float theta = acos(z / r);
    float phi = atan2(y, x);
    if (phi < 0) phi += 2 * M_PI;

    return (theta >= theta_min && theta < theta_max &&
            phi >= phi_min && phi < phi_max);
}
