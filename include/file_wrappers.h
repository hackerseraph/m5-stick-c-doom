// File operation wrappers for PROGMEM WAD support
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Custom file operations that handle PROGMEM WAD
int progmem_fseek(FILE* stream, long offset, int whence);
long progmem_ftell(FILE* stream);
size_t progmem_fread(void* ptr, size_t size, size_t count, FILE* stream);
int progmem_fclose(FILE* stream);
FILE* progmem_fopen(const char* filename, const char* mode);

// Override macros for DOOM code
#define fopen progmem_fopen
#define fseek progmem_fseek
#define ftell progmem_ftell
#define fread progmem_fread
#define fclose progmem_fclose

#ifdef __cplusplus
}
#endif
