
#include "optionsAndErrors.h"

enum {
	voice_spiro0,
	voice_typo = visSpirosSize,
	voice_bksp,
	voice_misbksp,
	voice_menuUp,
	voice_menuDn,
	voice_menuSelect,
	voice_menuEnd,
	voice_COUNT
};
#define firstMainMenuLoopVoice     voice_menuUp
#define lastMainMenuLoopVoice      voice_menuEnd
#define firstGlyphReactorLoopVoice voice_spiro0
#define lastGlyphReactorLoopVoice  voice_misbksp
#define firstSpiroViewerLoopVoice  voice_spiro0
#define lastSpiroViewerLoopVoice  (voice_spiro0 + visSpirosSize - 1)

void initAudio(void);
void cleanupAudio(void);
