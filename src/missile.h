//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//

// missile.h
#ifndef MISSILE_H
#define MISSILE_H

typedef struct {
    int id;
    float x, y, z;
    float vx, vy, vz;
    int active;
} Missile;

void update_position(Missile *m, float dt);

#endif

