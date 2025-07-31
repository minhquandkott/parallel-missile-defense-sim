//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// ===== missile.c =====
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include "missile.h"

void update_position(Missile *m, float dt) {
    m->x += m->vx * dt;
    m->y += m->vy * dt;
    m->z += m->vz * dt;
}

void initialize_missiles(Missile *missiles, int num, float radius) {
    for (int i = 0; i < num; i++) {
        missiles[i].id = i;
        float theta = ((float)rand() / RAND_MAX) * M_PI;
        float phi = ((float)rand() / RAND_MAX) * 2 * M_PI;
        float r = radius + 10.0f;

        missiles[i].x = r * sinf(theta) * cosf(phi);
        missiles[i].y = r * sinf(theta) * sinf(phi);
        missiles[i].z = r * cosf(theta);

        float norm = sqrtf(missiles[i].x * missiles[i].x +
                           missiles[i].y * missiles[i].y +
                           missiles[i].z * missiles[i].z);
        missiles[i].vx = -missiles[i].x / norm;
        missiles[i].vy = -missiles[i].y / norm;
        missiles[i].vz = -missiles[i].z / norm;

        missiles[i].active = 1;
    }
}

