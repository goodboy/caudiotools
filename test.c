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

    if (pcm == 1) {
        int hours, minutes, seconds = length / sample_rate;
        minutes = seconds / 60;
        hours = minutes / 60;
        seconds %= 60;
        minutes %= 60;

        printf("uncompressed wave file: length (approx): %02dh %02dm %02ds\n", hours, minutes, seconds);
    } else
        printf("compressed wave file\n");

    printf("size: %d bytes\n", size);
    printf("channels: %d", channels);
    printf("sample rate: %d Hz\n", sample_rate);
    printf("average bytes/second %d\n", bytes_per_second);
    printf("block align: %d\n", block_align);
    printf("bits/sample: %d\n", bits_per_sample);
}

int
main(int argc, char *argv[])
{
    FILE *fd;
    wave_t *wave1;
    wave_t *wave2;
    buffer_t *buffer1;
    buffer_t *buffer2;
    int bytesread;
    int length = 512;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // LOAD wave to compare with Square1.wav
    if (!(fd = fopen(argv[1], "rb"))) {
        fprintf(stderr, "Couldn't open \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (!(wave2 = waveopen(fd))) {
        fprintf(stderr, "Couldn't open \"%s\" as a .wav file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // LOAD Square1.wav
    if (!(fd = fopen("./wavs/Square1.wav", "rb"))) {
        fprintf(stderr, "Couldn't open \"%s\".\n", "Square1.wav");
        exit(EXIT_FAILURE);
    }
    if (!(wave1 = waveopen(fd))) {
        fprintf(stderr, "Couldn't open \"%s\" as a .wav file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    wavegetprop(wave1, WAVE_LENGTH, &length);
    buffer1 = mkbuffer(wave1, length);
    buffer2 = mkbuffer(wave2, length);

    if (!(bytesread = getpcm(wave1, buffer1))) {
        fprintf(stderr, "Couldn't stream pcm data!\n");
        exit(EXIT_FAILURE);
    }
    if (!(bytesread = getpcm(wave2, buffer2))) {
        fprintf(stderr, "Couldn't stream pcm data!\n");
        exit(EXIT_FAILURE);
    }
    /*int16_t **sample; 
    sample = (int16_t **)buffer->pcm; 

    for(int i = 0; i < buffer->length; i++){
        printf("sample %d = %i \n", i, sample[0][i]);
    }*/

    /* char2double(wave, buffer);
    for(int i = 0; i < buffer->length; i++)
        printf("sample %d = %f \n", i, buffer->vector[0][i]);
    */

    print_waveinfo(wave1);

    rmbuffer(wave1, buffer1);
    rmbuffer(wave2, buffer2);
    waveclose(wave1);
    waveclose(wave2);
    fclose(fd);
    return 0;
}
