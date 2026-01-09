#include "kernels.h"
#include "render.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/*
  Get the rdtsc value
  input : void
  output : unsigned long long (u64)
*/
unsigned long long
rdtsc (void)
{
  unsigned long long a, d;

  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));

  return (d << 32) | a;
}

/*
  This funtion runs a benchmark for a given kernel. It uses rdtsc and
  clock_gettime as job mesurement tool. RDTSC allows to get the cycles count
  and clock_gettime, with its setup, return a precise elapsed time.
*/
void
benchmark_function (void (*kernel) (int, f64 *, vector *, vector *, vector *),
                    i32 nbodies, f64 *masses, vector *positions,
                    vector *velocities, vector *accelerations)
{
  struct timespec t0, t1;
  unsigned long long before, after;
  clock_gettime (CLOCK_MONOTONIC_RAW, &t0);
  before = rdtsc ();
  kernel (nbodies, masses, positions, velocities, accelerations);
  after = rdtsc ();
  clock_gettime (CLOCK_MONOTONIC_RAW, &t1);
  float time
      = (t1.tv_sec + t1.tv_nsec * 1e-9) - (t0.tv_sec + t0.tv_nsec * 1e-9);
  float fps = 1.0 / time;
  printf ("%f fps; %lld cycles\n", fps, (after - before));
}

/*
  Main function
  This function only need to call benchmark or kernel function and
  initialize variables
*/
int
main (i32 argc, char **argv)
{
  // render simulation initialization var
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *renderer;
  render_initialisation (&window, &renderer);

  // This values can be modified
  i32 nbodies = 500;
  i32 time_stemp = 3000;

  f64 *masses;
  vector *positions;
  vector *velocities;
  vector *accelerations;

  masses = malloc (nbodies * sizeof (f64));
  positions = malloc (nbodies * sizeof (vector));
  velocities = malloc (nbodies * sizeof (vector));
  accelerations = malloc (nbodies * sizeof (vector));

  init_system (nbodies, masses, positions, velocities);
  i32 quit = 0;
  for (i32 i = 0; !quit && i < time_stemp; i++)
    {
      benchmark_function (simulate, nbodies, masses, positions, velocities,
                          accelerations);

      clear_window (renderer);
      for (i32 i = 0; i < nbodies; i++)
        {
          draw_body (renderer, positions[i].x, positions[i].y);
        }
      quit = render_and_event (renderer, &event);
    }

  close_and_clean (renderer, window);
  free_memory (nbodies, masses, positions, velocities, accelerations);
}
