#include "misc.h"

bool allEq(const float *l, const float *r, int c) {
  fr(i,c) {if (l[i] != r[i]) return false;}
  return true;
}

#include <math.h>
double sinTau(double n) {return sin(tau*n);}
double fractionalPart(double n) {return n - (long)n;}
