#include "kernels.h"
#include "random.h"
#include "types.h"
#include <SDL2/SDL_surface.h>
#include <time.h>

//#include <cblas.h>
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
static inline vector
add_vectors (vector a, vector b)
{
  vector res;
  res.x = a.x + b.x;
  res.y = a.y + b.y;
  return res; 
}








/*
  Multiply vector a with b value
  input :
    - f64
    - vector
  output : vector
*/
static inline vector
scale_vector (f64 b, vector a)
{
  vector res;
  res.x = a.x * b;
  res.y = a.y * b;
  return res;
}

/*
  Substitute vector b from the vector a
  input :
    - vector
    - vector
  output : vector
  */
static inline vector
sub_vectors (vector a, vector b)
{
  vector res; 
  res.x = a.x - b.x;
  res.y = a.y - b.y;
  return res;
}

/*
  Eclidian distance of vector
  input :
    - vector
  output : f64
*/
static inline f64
mod (vector a)
{
  return sqrt((a.x)*(a.x)+(a.y)*(a.y));
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
  const f64 COLLISION_THRESHOLD = 10.0; // Rayon de 2 particules
  const f64 THRESHOLD_SQ = COLLISION_THRESHOLD * COLLISION_THRESHOLD;
  
  // Ne tester que i < j pour éviter les doubles échanges
  for(int i = 0; i < nbodies; i++){
    for(int j = i + 1; j < nbodies; j++){  // ← j commence à i+1 !
      f64 dx = positions[i].x - positions[j].x;
      f64 dy = positions[i].y - positions[j].y;
      f64 dist_sq = dx*dx + dy*dy;
      
      if(dist_sq < THRESHOLD_SQ){  // Collision détectée
        // Échange simple des vitesses
        vector temp = velocities[i];
        velocities[i] = velocities[j];
        velocities[j] = temp;
      }
    }
  }
}

//
void
compute_accelerations (i32 nbodies, vector *accelerations, f64 *masses,
                       vector *positions)
{
  int i,j;
  for(i = 0; i < nbodies; i++){
    accelerations[i].x = 0;
    accelerations[i].y = 0;
    f64 cst = GRAVITY * masses[i]; //constant sur i
    for(j = 0; j < nbodies; j++){
      vector direction = sub_vectors(positions[j], positions[i]);
      f64 dist = sqrt(direction.x * direction.x + direction.y * direction.y);
      f64 denom = dist*sqrt(dist) +1e7;
      accelerations[i] =add_vectors(accelerations[i],scale_vector((cst/denom),direction));
    }
  }
}

//
void
compute_velocities (i32 nbodies, vector *velocities, vector *accelerations)
{
  int i,j;
  for(i = 0; i < nbodies; i++){
    velocities[i] = add_vectors( velocities[i], accelerations[i]);
  }
}

/*
  This function allows particules which quit the box to reenter from the
  opposite side.
*/
vector
check_position (vector a)
{
  vector res = a;
  
  // Vérification X
  if (res.x < 0) {
      res.x += DIM_WINDOW;
  } else if (res.x >= DIM_WINDOW) {
      res.x -= DIM_WINDOW;
  }

  // Vérification Y
  if (res.y < 0) {
      res.y += DIM_WINDOW;
  } else if (res.y >= DIM_WINDOW) {
      res.y -= DIM_WINDOW;
  }
  
  return res;
}

//
void
compute_positions (i32 nbodies, vector *positions, vector *velocities,
                   vector *accelerations)
{
  int i, j;
  for(i = 0; i < nbodies; i++){
    positions[i] = add_vectors( positions[i],add_vectors( velocities[i],scale_vector(1.0/2.0, accelerations[i])));
  }
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
