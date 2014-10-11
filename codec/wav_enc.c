#include "wav_enc.h"


#define WRITE_CHECKED(fp, buf, byte_len)     if (fwrite(buf, 1, byte_len, fp) < byte_len)\
                                             {\
                                                 fprintf(stderr, "%s:%u  fwrite() failed!\n", __FILE__, __LINE__);\
                                                 return RET_READ_ERR;\
                                             }

#define TOTAL_CHUNK_HDR_SIZE    (36)

typedef struct {
    FILE        *fp;
    t_sample    *sample;
    t_codec     *codec;
    t_codec_wav *wav;
} t_wav_encoder;


static inline int write_riff_chunk(t_wav_encoder *e)
{
    uint32_t buf;

    DEBUG("Chunk ID: %s", "RIFF");
    WRITE_CHECKED(e->fp, &("RIFF"), 4);

    buf = TOTAL_CHUNK_HDR_SIZE + e->sample->nbr_samples * (e->codec->bits_per_sample / 8);
    DEBUG("Chunk size: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 4);

    DEBUG("RIFF-type: %s", "WAVE");
    WRITE_CHECKED(e->fp, &("WAVE"), 4);

    return RET_OK;
}


static inline int write_fmt_chunk(t_wav_encoder *e)
{
    uint32_t buf;

    DEBUG("Subchunk ID: %s", "fmt ");
    WRITE_CHECKED(e->fp, &("fmt "), 4);

    DEBUG("Subchunk size: %u", 16);
    buf = 16;
    WRITE_CHECKED(e->fp, &buf, 4);

    if (e->codec->is_float)
    {
        DEBUG("Audio format: WAVE_FORMAT_IEEE_FLOAT");
        buf = WAVE_FORMAT_IEEE_FLOAT;
        WRITE_CHECKED(e->fp, &buf, 2);
    }
    else
    {
        DEBUG("Audio format: WAVE_FORMAT_PCM");
        buf = WAVE_FORMAT_PCM;
        WRITE_CHECKED(e->fp, &buf, 2);
    }

    DEBUG("Number of channels: %u", e->codec->num_channels);
    buf = e->codec->num_channels;
    WRITE_CHECKED(e->fp, &buf, 2);

    buf = e->codec->sample_rate;
    DEBUG("Sample rate: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 4);

    buf = e->codec->sample_rate * e->codec->num_channels * (e->codec->bits_per_sample / 8);
    DEBUG("Byte rate: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 4);

    buf = e->codec->num_channels * (e->codec->bits_per_sample / 8);
    DEBUG("Block align: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 2);

    buf = e->codec->bits_per_sample;
    DEBUG("Bits per sample: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 2);

    return RET_OK;
}


static inline int write_data_chunk(t_wav_encoder *e)
{
    uint32_t buf;

    DEBUG("Subchunk ID: %s", "data");
    WRITE_CHECKED(e->fp, &("data"), 4);

    buf = e->sample->nbr_samples * (e->codec->bits_per_sample / 8);
    DEBUG("Subchunk size: %u", buf);
    WRITE_CHECKED(e->fp, &buf, 4);

    DEBUG("Data");
    WRITE_CHECKED(e->fp, e->sample->samples[0], buf);

    return RET_OK;
}


int wav_enc(FILE *fp, t_sample *sample)
{
    assert(fp != NULL && "File ptr is NULL!");
    assert(sample != NULL && "Sample ptr is NULL!");

    t_wav_encoder e;
    e.fp = fp;
    e.sample = sample;
    e.codec = &(sample->codec.generic);
    e.wav = &(sample->codec.wav);

    int r;
    r = write_riff_chunk(&e);
    CHECK_ERRORS(r);

    r = write_fmt_chunk(&e);
    CHECK_ERRORS(r);

    r = write_data_chunk(&e);
    CHECK_ERRORS(r);

    return RET_OK;
}


#ifdef WAV_ENC_TEST
#include "wav_dec.h"

static int test(const char *filename)
{
    t_sample sample;

    printf("wav_enc testing: %s\n", filename);

    // Decode a file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    int r = wav_dec(fp, &sample);
    assert(r == RET_OK);
    fclose(fp);

    // Encode a file
    fp = fopen("/tmp/wav_enc_test.wav", "w+");
    r = wav_enc(fp, &sample);
    fclose(fp);

    free(sample.samples[0]);

    if (r != RET_OK)
    {
        return r;
    }

    memset(&sample, 0, sizeof(t_sample));

    // Verify that the encoded file parses
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    r = wav_dec(fp, &sample);

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
