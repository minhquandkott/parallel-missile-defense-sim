//
// Created by Huynh Tam Minh Quan on 20/06/2025.
//

// intercept.h
#ifndef INTERCEPT_H
#define INTERCEPT_H

#include "missile.h"

void process_interception(Missile *missiles, int count, int rank,
                          float xr, float yr, float zr, int *intercept_count);

#endif


