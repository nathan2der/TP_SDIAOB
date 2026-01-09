#include "kernels.h"
#include "random.h"
#include "types.h"
#include <SDL2/SDL_surface.h>
#include <time.h>

#include <cblas.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void
unimplemented ()
{
  puts ("unimplemented functions called");
}

/*
  Add vector a with vector b
  input :
    - vector
    - vector
  output : vector
*/
vector
add_vectors (vector a, vector b)
{
  unimplemented ();
}

/*
  Multiply vector a with b value
  input :
    - f64
    - vector
  output : vector
*/
vector
scale_vector (f64 b, vector a)
{
  unimplemented ();
}

/*
  Substitute vector b from the vector a
  input :
    - vector
    - vector
  output : vector
  */
vector
sub_vectors (vector a, vector b)
{
  unimplemented ();
}

/*
  Eclidian distance of vector
  input :
    - vector
  output : f64
*/
f64
mod (vector a)
{
  unimplemented ();
}

/*
  Initialize the system by setting random values
  input :
    - i32
    - f64 *
    - vector *
    - vector *
  output : void
*/
void
init_system (i32 nbodies, f64 *masses, vector *positions, vector *velocities)
{
  //
  srand (time (NULL));

  //
  for (i32 i = 0; i < nbodies; i++)
    {
      masses[i] = 5;

      positions[i].x = randxy (10, DIM_WINDOW);
      positions[i].y = randxy (10, DIM_WINDOW);

      velocities[i].x = randreal ();
      velocities[i].y = randreal ();
    }
}

/*
  This function interverts the velocity of 2 particles if they are in collision
*/
void
resolve_collisions (i32 nbodies, vector *positions, vector *velocities)
{
  unimplemented ();
}

//
void
compute_accelerations (i32 nbodies, vector *accelerations, f64 *masses,
                       vector *positions)
{
  unimplemented ();
}

//
void
compute_velocities (i32 nbodies, vector *velocities, vector *accelerations)
{
  unimplemented ();
}

/*
  This function allows particules which quit the box to reenter from the
  opposite side.
*/
vector
check_position (vector a)
{
  unimplemented ();
}

//
void
compute_positions (i32 nbodies, vector *positions, vector *velocities,
                   vector *accelerations)
{
  unimplemented ();
}

/*
  Simulate is the main simulation function which calls all steps
  input :
    - i32
    - f64 *
    - vector *
    - vector *
    - vector *
  output : void
*/
void
simulate (i32 nbodies, f64 *masses, vector *positions, vector *velocities,
          vector *accelerations)
{
  compute_accelerations (nbodies, accelerations, masses, positions);
  compute_positions (nbodies, positions, velocities, accelerations);
  compute_velocities (nbodies, velocities, accelerations);
  resolve_collisions (nbodies, positions, velocities);
}

/*
  Free all allocated memory
*/
void
free_memory (i32 nbodies, f64 *masses, vector *positions, vector *velocities,
             vector *accelerations)
{
  free (positions);
  free (velocities);
  free (accelerations);
  free (masses);
}
