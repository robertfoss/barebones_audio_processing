#include "level_mix.h"


static inline void sample_add_f32(t_sample *out, t_sample *in1, t_sample *in2, uint64_t sample_idx, float level1, float level2)
{
    assert(out->codec.generic.is_float && out->codec.generic.bits_per_sample == 32);
    assert(in1->codec.generic.is_float && in1->codec.generic.bits_per_sample == 32);
    assert(in2->codec.generic.is_float && in2->codec.generic.bits_per_sample == 32);
    assert(in1->codec.generic.nbr_channels == in2->codec.generic.nbr_channels);
    assert(sizeof(float) == 4);
    
    float *f_out = (float *) out->samples[0];
    float *f_in1 = (float *) in1->samples[0];
    float *f_in2 = (float *) in2->samples[0];
    
    f_out[sample_idx] = f_in1[sample_idx] * level1 + f_in2[sample_idx] * level2;
}


static inline void sample_add_u8(t_sample *out, t_sample *in1, t_sample *in2, uint64_t sample_idx, float level1, float level2)
{
    assert(!out->codec.generic.is_float && out->codec.generic.bits_per_sample == 8);
    assert(!in1->codec.generic.is_float && in1->codec.generic.bits_per_sample == 8);
    assert(!in2->codec.generic.is_float && in2->codec.generic.bits_per_sample == 8);
    assert(in1->codec.generic.nbr_channels == in2->codec.generic.nbr_channels);
    
    uint8_t *u_out = (uint8_t *) out->samples[0];
    uint8_t *u_in1 = (uint8_t *) in1->samples[0];
    uint8_t *u_in2 = (uint8_t *) in2->samples[0];
    
    uint16_t mixed_sample = (uint16_t) ((float) u_in1[sample_idx] * level1  + (float) u_in2[sample_idx] * level2) - 128;
    if (mixed_sample > 255)
    {
        mixed_sample = 255;
    }
    u_out[sample_idx] = mixed_sample;
}


int level_mix(t_sample *out, t_sample *in1, t_sample *in2, float level1, float level2)
{
    assert(!memcmp(&(in1->codec.generic), &(in2->codec.generic), sizeof(t_codec)));
    memcpy(&(out->codec.generic), &(in1->codec.generic), sizeof(t_codec));
    assert(out->codec.generic.nbr_channels == 1);
    
    uint64_t max_nbr_samples = 0;
    if (in1->nbr_samples > in2->nbr_samples)
    {
        max_nbr_samples = in1->nbr_samples;
    }
    else
    {
        max_nbr_samples = in2->nbr_samples;
    }
    
    out->nbr_samples = max_nbr_samples;
    out->samples[0] = malloc(max_nbr_samples * out->codec.generic.bits_per_sample / 8);
    if (out->samples[0] == NULL)
    {
        return RET_MIX_ERR;
    } 
    
    if (out->codec.generic.is_float)
    {
        //float32
        for (uint64_t i = 0; i < max_nbr_samples; ++i)
        {
            sample_add_f32(out, in1, in2, i, level1, level2);
        }        
    }
    else
    {
        //uint8
        for (uint64_t i = 0; i < max_nbr_samples; ++i)
        {
            sample_add_u8(out, in1, in2, i, level1, level2);
        }
    }
    
    return RET_OK;
}


#ifdef LEVEL_MIX_TEST
#include "../codec/wav_dec.h"
#include "../codec/wav_enc.h"

static int load_sample(t_sample *out, const char *filename)
{
    // Decode a file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    int r = wav_dec(fp, out);
    fclose(fp);
    
    return r;
}

int main()
{
    const char *out_file = "../hire_me.wav";
    t_sample s0, s1, s2;
    int r;
    FILE *fp;

    printf("level_mix testing: %s\n", out_file);
    
    // Load and mix samples
    r  = load_sample(&s0, "../resources/hire_8pcm_44khz.wav");
    r |= load_sample(&s1, "../resources/me_8pcm_44khz.wav");
    r |= level_mix(&s2, &s0, &s1, 1.0, 1.0);


    // Encode a file
    fp = fopen(out_file, "w+");
    r |= wav_enc(fp, &s2);
    fclose(fp);

    free(s0.samples[0]);
    free(s1.samples[0]);
    free(s2.samples[0]);


    // Verify that the encoded file parses
    fp = fopen(out_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file\n");
        return -1;
    }
    r |= wav_dec(fp, &s0);

    free(s0.samples[0]);

    
    if (r != RET_OK)
    {
        printf("Failure\n");
        return -1;
    }
    printf("Success\n");

    return RET_OK;
}

#endif

