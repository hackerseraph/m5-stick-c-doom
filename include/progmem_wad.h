#pragma once

#include <stddef.h>
#include <stdio.h>

// Initialize the PROGMEM WAD virtual filesystem
void progmem_wad_init();

// Override fopen to handle the embedded WAD
FILE* progmem_fopen(const char* filename, const char* mode);
