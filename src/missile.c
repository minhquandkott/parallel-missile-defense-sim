//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//
// missile.c
#include "missile.h"

void update_position(Missile *m, float dt) {
    m->x += m->vx * dt;
    m->y += m->vy * dt;
}
