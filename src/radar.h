//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//

// radar.h
#ifndef RADAR_H
#define RADAR_H

void get_radar_coordinates(int rank, int size, float r,
                           float *radar_x, float *radar_y, float *radar_z,
                           float *theta_min, float *theta_max,
                           float *phi_min, float *phi_max);

int is_missile_in_zone(float x, float y, float z,
                       float theta_min, float theta_max,
                       float phi_min, float phi_max);

#endif


