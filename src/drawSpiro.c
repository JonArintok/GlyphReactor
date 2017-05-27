
#include "misc.h"
#include "initSprites.h"
#include "initOpenGl.h"

void drawSpiro(spirograph *spiro) {
	return;
}

void drawSpiros(void) {
	fr (i, visSpirosSize) {
		if (visSpiros[i].exploPhase < 0 || visSpiros[i].exploPhase > 1) continue;
		drawSpiro(&visSpiros[i]);
	}
}
