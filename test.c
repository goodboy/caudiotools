#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "wavereader.h"

void
print_waveinfo(wave_t *wave)
{
    int32_t size, sample_rate, bytes_per_second, length;
    int16_t pcm, channels, block_align, bits_per_sample;

    /* read properties so we can dump wave file information */
    wavegetprop(wave, WAVE_COMPRESS_CODE, &pcm);
    wavegetprop(wave, WAVE_CHANNELS, &channels);
    wavegetprop(wave, WAVE_FILESIZE, &size);
    wavegetprop(wave, WAVE_SAMPLE_RATE, &sample_rate);
    wavegetprop(wave, WAVE_BYTES_PER_SEC, &bytes_per_second);
    wavegetprop(wave, WAVE_BLOCK_ALIGN, &block_align);
    wavegetprop(wave, WAVE_BITS_PER_SAMPLE, &bits_per_sample);
    wavegetprop(wave, WAVE_LENGTH, &length);

    printf("size: %d bytes\n", size);
    printf("channels: %d bit", channels);

    if (channels == 1)
        printf(" (mono)\n");
    else if (channels == 2)
        printf(" (stereo)\n");
    else
        printf("\n");

    if (pcm == 1) {
        int hours, minutes, seconds = length / sample_rate;
        minutes = seconds / 60;
        hours = minutes / 60;
        seconds %= 60;
        minutes %= 60;

        printf("uncompressed wave file: length (approx): %02dh %02dm %02ds\n", hours, minutes, seconds);
    } else
        printf("compressed wave file\n");

    printf("sample rate: %d Hz\n", sample_rate);
    printf("average bytes/second %d\n", bytes_per_second);
    printf("block align: %d\n", block_align);
    printf("bits/sample: %d\n", bits_per_sample);
}

int
main(int argc, char *argv[])
{
    FILE *fd;
    wave_t *wave;
    buffer_t *buffer;
    int bytesread;
    int length;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(fd = fopen(argv[1], "rb"))) {
        fprintf(stderr, "Couldn't open \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (!(wave = waveopen(fd))) {
        fprintf(stderr, "Couldn't open \"%s\" as a .wav file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    wavegetprop(wave, WAVE_LENGTH, &length);
    buffer = mkbuffer(wave, buffer, length + 18);

    if (!(bytesread = getpcm(wave, buffer))) {
        fprintf(stderr, "Couldn't stream pcm data!\n");
        exit(EXIT_FAILURE);
    }
   /* 
    rmbuffer(wave, pcm);

    length = 30;
    pcm = mkbuffer(wave, length);

    if (!(bytesread = getpcm(wave, length, pcm))) {
        fprintf(stderr, "Couldn't stream pcm data!\n");
        exit(EXIT_FAILURE);
    }
  */ 
    int16_t **sample; 
    sample = (int16_t **)buffer->pcm; 

    for(int i = 0; i < length; i++){
        printf("sample %d = %i \n", i, sample[0][i]);
    }

    print_waveinfo(wave);

    rmbuffer(wave, buffer);
    waveclose(wave);
    fclose(fd);
    return 0;
}


