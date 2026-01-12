// GBA DOOM API
#ifndef GBADOOM_H
#define GBADOOM_H

#ifdef __cplusplus
extern "C" {
#endif

// Main DOOM functions
void I_Init(void);
void I_PreInitGraphics(void);
void Z_Init(void);
void InitGlobals(void);
void D_DoomMain(void);

// M5StickC Plus hardware
void lcd_init(void);
void input_init(void);

// WAD data (memory-mapped from flash)
extern const unsigned char *doom_iwad;
extern const unsigned int doom_iwad_len;

#ifdef __cplusplus
}
#endif

#endif // GBADOOM_H
