#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "wavereader.h"
#include "stats.h"

//prototypes
int doublecmp(const void *d1, const void *d2);

int
main(int argc, char *argv[])
{
    FILE *fd1;
    FILE *fd2;
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
    if (!(fd1 = fopen(argv[1], "rb"))) {
        fprintf(stderr, "Couldn't open \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (!(wave2 = waveopen(fd1))) {
        fprintf(stderr, "Couldn't open \"%s\" as a .wav file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // LOAD Pulse1.wav
    if (!(fd2 = fopen("./wavs/Pulse1.wav", "rb"))) {
        fprintf(stderr, "Couldn't open \"%s\".\n", "Square1.wav");
        exit(EXIT_FAILURE);
    }
    if (!(wave1 = waveopen(fd2))) {
        fprintf(stderr, "Couldn't open \"%s\" as a .wav file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    wavegetprop(wave1, WAVE_LENGTH, &length);
    //length = 16;
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

    char2double(wave1, buffer1);
    char2double(wave2, buffer2);
    /*int16_t **sample; 
    sample = (int16_t **)buffer->pcm; 

    for(int i = 0; i < buffer->length; i++){
        printf("sample %d = %i \n", i, sample[0][i]);
    }*/

    print_waveinfo(wave1);

    /* Test signal processing routines
     --------------------------------------------------------------*/
    printf("\nSIGNAL PROCESSING RESULTS:\n");

    double **R = xcorr(0, buffer1, buffer2);
    qsort(R[1], 2*length - 1, sizeof(double), doublecmp);

    printf("max xcorr o/p = %f \n", R[1][2*length-2]);
/*    for(int i = 1024 - 300; i < 1024 + 300; i++)
        printf("index %d R=%f @ lag = %f \n", i, R[1][i],R[0][i]);
     look at the output and lag 100 should have R ~= 1;*/

    
    /* clean up mem allocs */
    free(R[0]);
    free(R[1]);
    free(R);
    rmbuffer(wave1, buffer1);
    rmbuffer(wave2, buffer2);
    waveclose(wave1);
    waveclose(wave2);
    fclose(fd1);
    fclose(fd2);
    return 0;
}

int 
doublecmp(const void *p1, const void *p2)
{
    const double *dp1 = (const double *)p1;
    const double *dp2 = (const double *)p2;

    return (int)(*dp1 - *dp2);
}
