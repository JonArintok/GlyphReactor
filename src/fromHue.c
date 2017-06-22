
#include <math.h>
#include "misc.h"
#include "optionsAndErrors.h"

double redFromHue(double hue) {
	hue = fabs(fractionalPart(hue));
	switch ((long)(hue*6.0)) {
		case 0: return 1.0;
		case 1: return 1.0 - (hue - 1.0/6.0)*6.0;
		case 2: return 0.0;
		case 3: return 0.0;
		case 4: return (hue - 4.0/6.0)*6.0;
		case 5: return 1.0;
	}
	_SHOULD_NOT_BE_HERE_;
	return 0;
}
double grnFromHue(double hue) {
	hue = fabs(fractionalPart(hue));
	switch ((long)(hue*6.0)) {
		case 0: return hue*6.0;
		case 1: return 1.0;
		case 2: return 1.0;
		case 3: return 1.0 - (hue - 3.0/6.0)*6.0;
		case 4: return 0.0;
		case 5: return 0.0;
	}
	_SHOULD_NOT_BE_HERE_;
	return 0;
}
double bluFromHue(double hue) {
	hue = fabs(fractionalPart(hue));
	switch ((long)(hue*6.0)) {
		case 0: return 0.0;
		case 1: return 0.0;
		case 2: return (hue - 2.0/6.0)*6.0;
		case 3: return 1.0;
		case 4: return 1.0;
		case 5: return 1.0 - (hue - 5.0/6.0)*6.0;
	}
	_SHOULD_NOT_BE_HERE_;
	return 0;
}
