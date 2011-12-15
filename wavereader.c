#include "wavereader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define RIFF_MAGIC ('R' | 'I' << 8 | 'F' << 16 | 'F' << 24)
#define WAVE_MAGIC ('W' | 'A' << 8 | 'V' << 16 | 'E' << 24)

/* supported chunk types */
#define FMT_CHUNK ('f' | 'm' << 8 | 't' << 16 | ' ' << 24)
#define DATA_CHUNK ('d' | 'a' << 8 | 't' << 16 | 'a' << 24)
#define LIST_CHUNK ('L' | 'I' << 8 | 'S' << 16 | 'T' << 24)

/* chunk types what I do not support (but will check for) */
#define FACT_CHUNK ('f' | 'a' << 8 | 'c' << 16 | 't' << 24)
#define WAVL_CHUNK ('w' | 'a' << 8 | 'v' << 16 | 'l' << 24)
#define SLNT_CHUNK ('s' | 'l' << 8 | 'n' << 16 | 't' << 24)
#define CUE_CHUNK ('c' | 'u' << 8 | 'e' << 16 | ' ' << 24)
#define PLST_CHUNK ('p' | 'l' << 8 | 's' << 16 | 't' << 24)
#define LABL_CHUNK ('l' | 'a' << 8 | 'b' << 16 | 'l' << 24)
#define NOTE_CHUNK ('n' | 'o' << 8 | 't' << 16 | 'e' << 24)
#define LTXT_CHUNK ('l' | 't' << 8 | 'x' << 16 | 't' << 24)
#define SMPL_CHUNK ('s' | 'm' << 8 | 'p' << 16 | 'l' << 24)
#define INST_CHUNK ('i' | 'n' << 8 | 's' << 16 | 't' << 24)

struct chunk_header {
    int32_t id;
    int32_t length;
} __attribute__ ((__packed__));

struct wave_fmt_chunk{
    struct chunk_header h;
    int16_t compression;
    int16_t channels;
    int32_t sample_rate;
    int32_t bytes_per_second;
    int16_t block_align;
    int16_t bits_per_sample;
    char fmt_extra[];
} __attribute__ ((__packed__));

struct wave_toc {
    int32_t id;
    int32_t length;
    long offset;
    struct wave_toc *next;
};

struct wave {
    FILE *fp;
    int32_t length;
    struct chunk_header *dataheader;
    struct wave_toc *toc;
    struct wave_fmt_chunk *fmt;
    long pcmsread;
    long internalstart;
    char *buffer;
    char *bufferend;
    char *ibuff;
    size_t bufferlength;
};

static void
dump_int32(int32_t i)
{
    printf("%c", i & 0xff);
    i >>= 8;
    printf("%c", i & 0xff);
    i >>= 8;
    printf("%c", i & 0xff);
    i >>= 8;
    printf("%c", i & 0xff);
}

static void
dump_toc(wave_t *wave)
{
    struct wave_toc *entry;

    printf(">> DUMPING TOC\n");
    for (entry = wave->toc; entry; entry = entry->next) {
        switch (entry->id) {
            case FMT_CHUNK:
                printf(" -> fmt chunk\n");
                break;
            case DATA_CHUNK:
                printf(" -> data chunk\n");
                break;
            case FACT_CHUNK:
                printf(" -> fact chunk\n");
                break;
            case LIST_CHUNK:
                printf(" -> LIST chunk\n");
                break;
            default:
                printf(" -> unsupported chunk found: ");
                dump_int32(entry->id);
                printf("\n");
                break;
        }
    }
}

static void
dump_fmt(struct wave_fmt_chunk *fmt) {
    printf(">> fmt chunk found (%d bytes long)\n", fmt->h.length);

    if (fmt->h.length > 16) {
        printf(">> DEBUG: printing fmt_extra\n");
        int i;
        for (i = 0; i < fmt->h.length - 16; i += 2)
            printf(" %02hhx %02hhx\n", fmt->fmt_extra[i], fmt->fmt_extra[i + 1]);
    }
}

static void *
read_chunk(wave_t *wave, int id)
{
    char *chunk;
    struct wave_toc *entry;
    size_t chunk_size;

    for (entry = wave->toc; entry->id != id; entry = entry->next);

    if (!entry)
    return NULL;
   
    // if its the data only read the header.
    if (id == DATA_CHUNK) {
        
        chunk_size = sizeof(struct chunk_header);
        wave->dataheader = malloc(chunk_size);

        fseek(wave->fp, entry->offset, SEEK_SET);
        fread(wave->dataheader, chunk_size, 1, wave->fp);
        wave->internalstart = ftell(wave->fp);
        fread(wave->buffer, (size_t)wave->fmt->block_align, wave->bufferlength, wave->fp);

        return NULL;
    }else{

    chunk_size = sizeof(struct chunk_header) + entry->length;

    chunk = malloc(chunk_size);
    fseek(wave->fp, entry->offset, SEEK_SET);
    fread(chunk, chunk_size, 1, wave->fp);

    return chunk;
    }
}

static void
build_toc(wave_t *wave)
{
    struct wave_toc *entry;
    struct chunk_header header;
    int offset;

    while (1) {
        /* get offset of next header */
        offset = ftell(wave->fp);

        // read out header
        if (!fread(&header, sizeof(struct chunk_header), 1, wave->fp))
            break; 

        if (!wave->toc)
            entry = wave->toc = malloc(sizeof(struct wave_toc));
        else
            entry = entry->next = malloc(sizeof(struct wave_toc));

        entry->id = header.id;
        entry->length = header.length;
        entry->offset = offset;
        entry->next = NULL;

        if (fseek(wave->fp, header.length, SEEK_CUR) != 0) {
        // fseek returns 0 if successful
            perror("seek failed");
            break;
        }
    }
}

wave_t *
waveopen(FILE *fp)
{
    wave_t *wave = NULL;
    int32_t header[3];

    /* check for magic numbers */
    fread(header, sizeof(int32_t), 3, fp);
    if (header[0] != RIFF_MAGIC || header[2] != WAVE_MAGIC)
        return NULL;

    /* header is okay, allocate the wave_t struct */
    if (!(wave = malloc(sizeof(wave_t)))) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    wave->fp = fp;
    wave->length = header[1];
    wave->toc = NULL;
    wave->fmt = NULL;
    wave->bufferlength = 512; // # of samples in internal buffer

    build_toc(wave);
    dump_toc(wave);

    wave->fmt = read_chunk(wave, FMT_CHUNK);
    if (wave->fmt->channels > 2) {
        fprintf(stderr, "wavreader does not support more than stereo \n");
        return NULL;
    }
    if (wave->fmt->compression != 1) {
        fprintf(stderr, "Only uncompressed wave files supported\n");
        return NULL;
    }

    assert(wave->fmt->h.length >= 16);
    dump_fmt(wave->fmt);

    /* allocate mem for internal buffer of 512 samples and set pointer to 
    end : 512 samples are allocated regardless of the number of channels */

    wave->buffer = malloc(wave->bufferlength * wave->fmt->block_align);
    wave->bufferend = wave->buffer + (wave->bufferlength * wave->fmt->block_align);
    wave->ibuff = wave->buffer;

    /* this read fills the buffer and grabs the data header */
    read_chunk(wave, DATA_CHUNK);
    printf(">> DEBUG: data frame %d bytes\n", wave->dataheader->length);

    return wave;
}

int
waveclose(wave_t *wave)
{
    struct wave_toc *temp, *entry = wave->toc;

    while (entry) {
        temp = entry;
        entry = entry->next;
        free(temp);
    }

    free(wave->fmt);
    free(wave);
    return 0;
}

/* Use this function for reading pcm data.  Do not expect
things like channels or buffering to be handled for you.*/
int 
getpcm(wave_t *wave, buffer_t *buffer)
{
    int16_t channels = wave->fmt->channels;
    int16_t blockalign = wave->fmt->block_align;
    int16_t samplealign = blockalign/channels;
    int samplesleft = (wave->dataheader->length - wave->pcmsread) / blockalign;

    int length = buffer->length;
    int isample;
    int dim;
    int ibyte;
    int bytes = 0;

    if(length > samplesleft) { 
        length = samplesleft; 
        perror("Requested length longer then file");
        printf("Output truncated to %d samples \n", samplesleft);
    }

    // Copy data into ptr
    for(isample = 0; isample < length; isample++ ) { 
        // each sample

        for(dim = 0; dim < channels; dim++) {            
            // each channel

            for(ibyte = 0; ibyte < samplealign; ibyte++) {   
                // each byte

                buffer->pcm[dim][ibyte+isample*samplealign] = *(wave->ibuff + (ibyte + dim*samplealign));
                
            }
            bytes += samplealign;
        }
        wave->ibuff += blockalign;
        wave->pcmsread += blockalign;

        if(wave->ibuff >= wave->bufferend) {
            //reload internal buffer
            fread(wave->buffer, (size_t)blockalign, wave->bufferlength, wave->fp);
            wave->ibuff = wave->buffer;
        }
    }
    return bytes;
}

buffer_t  * 
mkbuffer(wave_t *wave, int length)
{
    int16_t samplealign = wave->fmt->block_align/wave->fmt->channels;
    buffer_t *buffer = malloc(sizeof(buffer_t));
    buffer->length = (size_t)length;

    buffer->pcm = malloc(wave->fmt->channels* sizeof(char *));
    for(int dim = 0; dim < wave->fmt->channels; dim++) {
        buffer->pcm[dim]= calloc((size_t)length, (size_t)samplealign);
    }
    return buffer;
}

int
rmbuffer(wave_t *wave, buffer_t *buffer)
{
    for(short dim = 0; dim < wave->fmt->channels; dim++) {
        free(buffer->pcm[dim]);
    }
    free(buffer->pcm);
    free(buffer);
    return 0;
}    

int
wavegetprop(wave_t *wave, wave_prop_t prop, void *data)
{
    switch (prop) {
        case WAVE_FILESIZE:
            *(int32_t *)data = wave->length;
            return 0;
        case WAVE_COMPRESS_CODE:
            *(int16_t *)data = wave->fmt->compression;
            return 0;
        case WAVE_CHANNELS:
            *(int16_t *)data = wave->fmt->channels;
            return 0;
        case WAVE_SAMPLE_RATE:
            *(int32_t *)data = wave->fmt->sample_rate;
            return 0;
        case WAVE_BYTES_PER_SEC:
            *(int32_t *)data = wave->fmt->bytes_per_second;
                return 0;
        case WAVE_BLOCK_ALIGN:
            *(int16_t *)data = wave->fmt->block_align;
            return 0;
        case WAVE_BITS_PER_SAMPLE:
            *(int16_t *)data = wave->fmt->bits_per_sample;
            return 0;
        case WAVE_LENGTH:
            *(int32_t *)data = wave->dataheader->length / wave->fmt->block_align; 
            return 0;
        default:
            /* property not defined*/
            return -1;
    }
}

int
waveseek(wave_t *wave, long offset, int whence)
{
    /* TODO */
    return 0;
}

int
waveeof(wave_t *wave)
{
    /* TODO */
    return 0;
}

double 
char2double(wave_t *wave, buffer_t *buffer)
{
//x[i + 0] << 24 || x[i + 1] << 16 || x[i + 2] << 8 || x[i + 3] / (double)(2 << bitdepth)

    int16_t samplealign = wave->fmt->block_align/wave->fmt->channels;
    int accum = 0;
    int ibyte;
    double result;

    /* allocate for vector of doubles */
    buffer->vector = malloc(wave->fmt->channels * sizeof(double *));
    for(int dim = 0; dim < wave->fmt->channels; dim++) {
        buffer->vector[dim]= calloc(buffer->length, sizeof(double));
    } 
    
    for(ibyte = samplealign - 1; ibyte > -1; ibyte--) {
        accum <<= 8;
        accum |= buffer->pcm[0][ibyte] & 0xff; //*++x
    }
    result = accum / ((double)(1 << (wave->fmt->bits_per_sample - 1)));
    return result;
}
