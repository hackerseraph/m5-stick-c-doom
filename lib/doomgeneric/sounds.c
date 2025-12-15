//
// ESP32 M5StickC: SOUND STRIPPED - No speaker, no sound data
// Saves ~6KB RAM + eliminates sound processing overhead
//

#include <stdlib.h>
#include "doomtype.h"
#include "sounds.h"

// Minimal music table stub (1 entry)
musicinfo_t S_music[] = {
    { NULL, 0, NULL, NULL }
};

// Minimal sfx table stub (1 entry - game references sfx_None at index 0)
sfxinfo_t S_sfx[] = {
    { NULL, "none", 0, NULL, -1, -1, 0, 0, -1, NULL }
};
