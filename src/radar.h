//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//

// radar.h
#ifndef RADAR_H
#define RADAR_H

void init_mpi(int *rank, int *size);
int is_missile_in_zone(float x, int rank, int size, float width);

#endif

