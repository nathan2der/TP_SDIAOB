#include "random.h"

/*
  This function sets up a random int.
  input : void
  output :
    - random int
*/
int
randxy (i32 x, i32 y)
{
  return (rand () % (y - x + 1)) + x;
}

/*
  This function sets up a random f64.
  input : void
  output :
    - random f64
*/
f64
randreal ()
{
  i32 s = (randxy (0, 1)) ? 1 : -1;
  i32 a = randxy (1, RAND_MAX), b = randxy (1, RAND_MAX);

  return s * ((f64)a / (f64)b);
}
