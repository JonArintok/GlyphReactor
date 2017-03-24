#include "misc.h"

bool allEq(const float *l, const float *r, int c) {
  fr(i,c) {if (l[i] != r[i]) return false;}
  return true;
}
