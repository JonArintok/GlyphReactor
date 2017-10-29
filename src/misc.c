#include <stdlib.h>
#include <math.h>
#include "misc.h"

double sinTau(double n) {return sin(tau*n);}
double fractionalPart(double n) {return n - (long)n;}
double lerp(double l, double r, double n) {return l + (r-l)*n;}
double frand(double lb, double hb) {
	return lb + (hb - lb)*((double)rand()/RAND_MAX);
}
