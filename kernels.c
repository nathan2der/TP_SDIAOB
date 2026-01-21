#include "kernels.h"
#include "random.h"
#include "types.h"
#include <SDL2/SDL_surface.h>
#include <time.h>

#include <cblas.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <immintrin.h> // AVX intrinsics



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
  
  // RACE CONDITION : Plusieurs threads peuvent écrire simultanément dans velocities[j]
  // Solution : utiliser critical section ou atomic, mais perte de performance
  // Meilleure solution : garder séquentiel car O(n²) avec peu de collisions réelles
  for(int i = 0; i < nbodies; i++){
    for(int j = i + 1; j < nbodies; j++){
      f64 dx = positions[i].x - positions[j].x;
      f64 dy = positions[i].y - positions[j].y;
      f64 dist_sq = dx*dx + dy*dy;
      
      if(dist_sq < THRESHOLD_SQ){  // Collision détectée
        #pragma omp critical
        {
          // Échange atomique des vitesses
          vector temp = velocities[i];
          velocities[i] = velocities[j];
          velocities[j] = temp;
        }
      }
    }
  }
}


//

void
compute_accelerations (i32 nbodies, vector *accelerations, f64 *masses,
                       vector *positions)
{
  int i, j;
  
  #pragma omp parallel for schedule(static) private(i, j)
  for(i = 0; i < nbodies; i++){
    f64 cst = GRAVITY * masses[i];
    
    // Accumulateurs AVX pour traiter 4 particules en parallèle
    __m256d acc_x_vec = _mm256_setzero_pd();
    __m256d acc_y_vec = _mm256_setzero_pd();
    
    __m256d pos_i_x = _mm256_set1_pd(positions[i].x);
    __m256d pos_i_y = _mm256_set1_pd(positions[i].y);
    __m256d cst_vec = _mm256_set1_pd(cst);
    __m256d epsilon = _mm256_set1_pd(1e7);
    
    // Boucle vectorisée : traiter 4 particules j à la fois
    for(j = 0; j < nbodies - 3; j += 4){
      // Charger 4 positions j (x0, x1, x2, x3)
      __m256d pos_j_x = _mm256_set_pd(positions[j+3].x, positions[j+2].x, 
                                       positions[j+1].x, positions[j].x);
      __m256d pos_j_y = _mm256_set_pd(positions[j+3].y, positions[j+2].y, 
                                       positions[j+1].y, positions[j].y);
      
      // direction = pos_j - pos_i (4 vecteurs simultanément)
      __m256d dir_x = _mm256_sub_pd(pos_j_x, pos_i_x);
      __m256d dir_y = _mm256_sub_pd(pos_j_y, pos_i_y);
      
      // dist² = dx² + dy²
      __m256d dx2 = _mm256_mul_pd(dir_x, dir_x);
      __m256d dy2 = _mm256_mul_pd(dir_y, dir_y);
      __m256d dist_sq = _mm256_add_pd(dx2, dy2);
      
      // dist = sqrt(dist²)
      __m256d dist = _mm256_sqrt_pd(dist_sq);
      
      // denom = dist³ + epsilon
      __m256d dist3 = _mm256_mul_pd(dist_sq, dist);
      __m256d denom = _mm256_add_pd(dist3, epsilon);
      
      // scale = cst / denom
      __m256d scale = _mm256_div_pd(cst_vec, denom);
      
      // acceleration += scale * direction (FMA)
      acc_x_vec = _mm256_fmadd_pd(scale, dir_x, acc_x_vec);
      acc_y_vec = _mm256_fmadd_pd(scale, dir_y, acc_y_vec);
    }
    
    // Réduction horizontale : somme des 4 valeurs
    double temp_x[4], temp_y[4];
    _mm256_storeu_pd(temp_x, acc_x_vec);
    _mm256_storeu_pd(temp_y, acc_y_vec);
    
    f64 acc_x_total = temp_x[0] + temp_x[1] + temp_x[2] + temp_x[3];
    f64 acc_y_total = temp_y[0] + temp_y[1] + temp_y[2] + temp_y[3];
    
    // Traiter les particules restantes (code scalaire)
    for(; j < nbodies; j++){
      vector direction;
      direction.x = positions[j].x - positions[i].x;
      direction.y = positions[j].y - positions[i].y;
      f64 dist = sqrt(direction.x * direction.x + direction.y * direction.y);
      f64 denom = dist * dist * dist + 1e7;
      acc_x_total += (cst / denom) * direction.x;
      acc_y_total += (cst / denom) * direction.y;
    }
    
    accelerations[i].x = acc_x_total;
    accelerations[i].y = acc_y_total;
  }
}

//
void
compute_velocities (i32 nbodies, vector *velocities, vector *accelerations)
{
  int i;
  
  // Vectorisation SSE : traiter 2 particules à la fois
  for(i = 0; i < nbodies - 1; i += 2){
    // Charger 2 vecteurs de vitesse et accélération
    __m128d v0 = _mm_loadu_pd((double*)&velocities[i]);
    __m128d v1 = _mm_loadu_pd((double*)&velocities[i+1]);
    __m128d a0 = _mm_loadu_pd((double*)&accelerations[i]);
    __m128d a1 = _mm_loadu_pd((double*)&accelerations[i+1]);
    
    // v += a
    v0 = _mm_add_pd(v0, a0);
    v1 = _mm_add_pd(v1, a1);
    
    // Stocker les résultats
    _mm_storeu_pd((double*)&velocities[i], v0);
    _mm_storeu_pd((double*)&velocities[i+1], v1);
  }
  
  // Traiter la particule restante si nbodies est impair
  if(i < nbodies){
    velocities[i].x += accelerations[i].x;
    velocities[i].y += accelerations[i].y;
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



void
compute_positions (i32 nbodies, vector *positions, vector *velocities,
                   vector *accelerations)
{
  int i;
  __m128d half = _mm_set1_pd(0.5);
  
  // Vectorisation SSE : traiter 2 particules à la fois
  for(i = 0; i < nbodies - 1; i += 2){
    // Charger données pour 2 particules
    __m128d p0 = _mm_loadu_pd((double*)&positions[i]);
    __m128d p1 = _mm_loadu_pd((double*)&positions[i+1]);
    __m128d v0 = _mm_loadu_pd((double*)&velocities[i]);
    __m128d v1 = _mm_loadu_pd((double*)&velocities[i+1]);
    __m128d a0 = _mm_loadu_pd((double*)&accelerations[i]);
    __m128d a1 = _mm_loadu_pd((double*)&accelerations[i+1]);
    
    // scale = 0.5 * a
    __m128d scale0 = _mm_mul_pd(half, a0);
    __m128d scale1 = _mm_mul_pd(half, a1);
    
    // addi = v + scale
    __m128d addi0 = _mm_add_pd(v0, scale0);
    __m128d addi1 = _mm_add_pd(v1, scale1);
    
    // p += addi
    p0 = _mm_add_pd(p0, addi0);
    p1 = _mm_add_pd(p1, addi1);
    
    // Stocker
    _mm_storeu_pd((double*)&positions[i], p0);
    _mm_storeu_pd((double*)&positions[i+1], p1);
  }
  
  // Traiter la particule restante si nbodies est impair
  if(i < nbodies){
    vector scale;
    scale.x = 0.5 * accelerations[i].x;
    scale.y = 0.5 * accelerations[i].y;
    vector addi;
    addi.x = velocities[i].x + scale.x;
    addi.y = velocities[i].y + scale.y;
    positions[i].x += addi.x;
    positions[i].y += addi.y;
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
