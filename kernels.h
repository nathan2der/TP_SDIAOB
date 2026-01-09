#ifndef KERNELS_H
#define KERNELS_H

#include "types.h"
#include <stdio.h>

void simulate (i32 nbodies, f64 *masses, vector *positions, vector *velocities,
               vector *accelerations);
void init_system (i32 nbodies, f64 *masses, vector *positions,
                  vector *velocities);
void free_memory (i32 nbodies, f64 *masses, vector *positions,
                  vector *velocities, vector *accelerations);

#endif
