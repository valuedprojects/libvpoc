
#include <time.h>
#include <stdlib.h>

static void randomNumberConstructor() __attribute__((constructor));

void randomNumberConstructor()
{
  srand((unsigned int)(time(0)));
  // For some reason the 1st random number is not very random. So shed it.
  rand();
}

unsigned int randomNumber(unsigned int low, unsigned int hi)
{
  unsigned int returnValue;
  double x;
  unsigned int range = hi-low;
  unsigned int r = rand();
  x = ((double)(r) / (double)(RAND_MAX)) * (double)range;
  returnValue = (unsigned int)x + low;
  return returnValue;
}
