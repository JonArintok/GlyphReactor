
#include <stdio.h>
#include "spiroViewerLoop.h"
#include "drawSpiro.h"


bool spirographEditorLoop(char charEntered, int curFrame) {
	if (charEntered) triggerSpiro(charEntered);
	drawSpiros();
	return true;
}
