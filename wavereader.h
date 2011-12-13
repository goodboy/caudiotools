#ifndef WAVEREADER_46GP1GUA
#define WAVEREADER_46GP1GUA

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


struct wave;
typedef struct wave wave_t;

typedef struct {
    char** pcm;
    int length;
} buffer_t;

typedef enum {
    WAVE_FILESIZE,
    WAVE_COMPRESS_CODE,
    WAVE_CHANNELS,
    WAVE_SAMPLE_RATE,
    WAVE_BYTES_PER_SEC,
    WAVE_BLOCK_ALIGN,
    WAVE_BITS_PER_SAMPLE,
    WAVE_BYTES_IN_DATA,
    WAVE_LENGTH,                // number of samples in file
} wave_prop_t;

/**********************************************************************
  Opens a wave file pointed to by *file, 
  if buffersizj is not specified waveopen grabs the 
  entire files worth of pcm and places it in the wave stuctures
  wave_data_chunk->pcm[] field

 Inputs:
      *file		  :  file pointer as specified in stdio.h
      buffersize  :  length of simulated input FIFO. This sets the maximum size 
                     of the amount of data readable from the PCM data buffer.
                     Currently data is read lazily as long as buffersize is specified.
  ********************************************************************/
wave_t *waveopen(FILE *file);

/**********************************************************************
 Closes the wave file and frees memory.
  ********************************************************************/
int waveclose(wave_t *wave);
int getpcm(wave_t *wave, int length, char **ptr);
buffer_t  *mkbuffer(wave_t *wave, buffer_t *buffer, int length);
int rmbuffer(wave_t *wave, buffer_t *buffer);
int wavegetprop(wave_t *wave, wave_prop_t prop, void *data);
int waveseek(wave_t *wave, long offset, int whence);
int waveeof(wave_t *wave);

#endif
