
#include "optionsAndErrors.h"

enum {
	voice_spiro0,
	voice_typo = visSpirosSize,
	voice_bksp,
	voice_menuUpDn,
	voice_menuSelect,
	voice_menuEnd,
	voice_COUNT
};

void initAudio(void);
void cleanupAudio(void);
