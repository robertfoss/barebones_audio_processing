#include "wav_dec.h"


#define READ_CHECKED(fp, buf, byte_len)     if (fread(buf, 1, byte_len, fp) < byte_len)\
                                            {\
                                                fprintf(stderr, "%s:%u  fread() failed!\n", __FILE__, __LINE__);\
                                                return RET_READ_ERR;\
                                            }


typedef struct {
    FILE        *fp;
    t_sample    *sample;
    t_codec     *codec;
    t_codec_wav *wav;
    uint32_t chunk_len_tot;
} t_wav_parser;


static inline int parse_riff_chunk(t_wav_parser *p)
{
    uint32_t buf;

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("Chunk ID: %c%c%c%c", ((unsigned char*) &buf)[0], ((unsigned char*) &buf)[1],
                                ((unsigned char*) &buf)[2], ((unsigned char*) &buf)[3]);
     if (memcmp(&buf, &("RIFF"), 4))
    {
        fprintf(stderr, "RIFF chunk descriptor not found!\n");
        return RET_PARSE_FAIL;
    }

    READ_CHECKED(p->fp, &buf, 4);
    p->chunk_len_tot = buf;
    DEBUG("Chunk size: %u", p->chunk_len_tot);

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("RIFF-type: %c%c%c%c", ((unsigned char*) &buf)[0], ((unsigned char*) &buf)[1],
                                 ((unsigned char*) &buf)[2], ((unsigned char*) &buf)[3]);
    if (memcmp(&buf, &("WAVE"), 4))
    {
        fprintf(stderr, "RIFF format is not WAVE!\n");
        return RET_PARSE_FAIL;
    }

    return RET_OK;
}


static inline int parse_fmt_chunk(t_wav_parser *p)
{
    uint32_t buf;

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("Subchunk ID: %c%c%c%c", ((unsigned char*) &buf)[0], ((unsigned char*) &buf)[1],
                                ((unsigned char*) &buf)[2], ((unsigned char*) &buf)[3]);
    if (memcmp(&buf, &("fmt "), 4))
    {
        fprintf(stderr, "fmt chunk not found!\n");
        return RET_PARSE_FAIL;
    }

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("Subchunk size: %u", buf);
    if (p->chunk_len_tot < buf)
    {
        fprintf(stderr, "Corruption found!\n");
        return RET_PARSE_CORRUPT;
    }

    READ_CHECKED(p->fp, &buf, 2);
    if ((buf & 0xFFFF) == WAVE_FORMAT_PCM)
    {
        DEBUG("Audio format: WAVE_FORMAT_PCM");
        p->codec->is_float = false;
    }
    else if ((buf & 0xFFFF) == WAVE_FORMAT_IEEE_FLOAT)
    {
        DEBUG("Audio format: WAVE_FORMAT_IEEE_FLOAT");
        p->codec->is_float = true;
    }
    else
    {
        DEBUG("Audio format: %u", buf & 0xFFFF);
        fprintf(stderr, "Unsupported sample format!\n");
        return RET_UNSUPPORTED;
    }

    READ_CHECKED(p->fp, &buf, 2);
    p->codec->num_channels = buf & 0xFFFF;
    DEBUG("Number of channels: %u", p->codec->num_channels);
    if (p->codec->num_channels != 1)
    {
        fprintf(stderr, "Unsupported number of channels (%u)\n", p->codec->num_channels);
        return RET_UNSUPPORTED;
    }

    READ_CHECKED(p->fp, &buf, 4);
    p->codec->sample_rate = buf;
    DEBUG("Sample rate: %u", p->codec->sample_rate);

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("Byte rate: %u", buf);

    READ_CHECKED(p->fp, &buf, 2);
    DEBUG("Block align: %u", buf & 0xFFFF);

    READ_CHECKED(p->fp, &buf, 2);
    p->codec->bits_per_sample = buf & 0xFFFF;
    DEBUG("Bits per sample: %u", p->codec->bits_per_sample);
    if (!(!(p->codec->is_float && p->codec->bits_per_sample == 8) ||
           (p->codec->is_float && p->codec->bits_per_sample == 32)))
    {
        fprintf(stderr, "Unsupported number of bits per sample!\n");
        return RET_UNSUPPORTED;
    }
    return RET_OK;
}


static inline int parse_data_chunk(t_wav_parser *p)
{
    uint32_t buf;

    READ_CHECKED(p->fp, &buf, 4);
    DEBUG("Subchunk ID: %c%c%c%c", ((unsigned char*) &buf)[0], ((unsigned char*) &buf)[1],
                                   ((unsigned char*) &buf)[2], ((unsigned char*) &buf)[3]);

    if (memcmp(&buf, &("data"), 4))
    {
        READ_CHECKED(p->fp, &buf, 4);
        DEBUG("Subchunk size: %u", buf);
        fseek(p->fp, buf, SEEK_CUR);
        return RET_OK; // This isn't the chunk type we're looking for but
                       // no fatal errors have occured. Let's keep looking
    }

    unsigned int byte_length;
    READ_CHECKED(p->fp, &byte_length, 4);
    DEBUG("Subchunk size: %u", byte_length);

    if (byte_length >= p->chunk_len_tot)
    {
        fprintf(stderr, "Subchunk length longer than total chunk length!\n");
        return RET_PARSE_CORRUPT;
    }

    unsigned long old_size = p->sample->nbr_samples * (p->codec->bits_per_sample / 8);
    size_t new_size = old_size + byte_length;
    p->sample->samples[0] = realloc(p->sample->samples[0], new_size);
    if (p->sample->samples[0] == NULL)
    {
        fprintf(stderr, "realloc(%p, %lu) failed\n", p->sample->samples[0], old_size + byte_length);
        return RET_PARSE_FAIL;
    }
    p->sample->nbr_samples += byte_length / (p->codec->bits_per_sample / 8);
    READ_CHECKED(p->fp, p->sample->samples[0] + old_size, byte_length);

    return RET_OK;
}


int wav_dec(FILE *fp, t_sample *sample)
{
    assert(fp != NULL && "Invalid file");
    assert(sample != NULL && "Sample ptr is NULL");

    memset(sample, 0, sizeof(t_sample));

    t_wav_parser p;
    p.fp = fp;
    p.sample = sample;
    p.codec = &(sample->codec.generic);
    p.wav = &(sample->codec.wav);
    p.codec->codec_type = CODEC_WAV;

    fseek(fp, 0L, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    DEBUG("Input file length: %ld bytes", file_len);

    int r;
    r = parse_riff_chunk(&p);
    CHECK_ERRORS(r);

    r = parse_fmt_chunk(&p);
    CHECK_ERRORS(r);

    while (ftell(fp) < file_len)
    {
        r = parse_data_chunk(&p);
        CHECK_ERRORS(r);
    }

    return RET_OK;
}


#ifdef WAV_DEC_TEST

static int test (const char *filename)
{
    t_sample sample;

    printf("wav_dec testing: %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    int r = wav_dec(fp, &sample);

    fclose(fp);
    free(sample.samples[0]);

    return r;
}


int main()
{
    int r = RET_OK;
    r |= test("../resources/hey_32float_44khz.wav");
    r |= test("../resources/hey_8pcm_44khz.wav");

    if (r != RET_OK)
    {
        printf("Failure\n");
        return -1;
    }
    printf("Success\n");

    return RET_OK;
}
#endif
