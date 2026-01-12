// Sound stub for M5StickC Plus (no speaker)
#include "i_sound.h"

// Stub implementations - M5StickC Plus has no speaker

void I_InitSound(void) {
    // No sound hardware
}

void I_PauseSong(int handle) {
    // No-op
    (void)handle;
}

void I_ResumeSong(int handle) {
    // No-op
    (void)handle;
}

void I_SetMusicVolume(int volume) {
    // No-op
    (void)volume;
}

void I_StopSong(int handle) {
    // No-op
    (void)handle;
}

void I_PlaySong(int handle, int looping) {
    // No-op
    (void)handle;
    (void)looping;
}

int I_StartSound(int id, int channel, int vol, int sep) {
    // No-op
    (void)id;
    (void)channel;
    (void)vol;
    (void)sep;
    return 0;
}

void I_Init(void) {
    // Initialize any other systems here
    I_InitSound();
}
