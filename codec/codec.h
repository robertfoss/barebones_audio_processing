#ifndef __CODEC__CODEC
#define __CODEC__CODEC

#include <stdint.h>
#include <stdbool.h>


#define CODEC_WAV               (0x1)


typedef struct {
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint8_t  codec_type;
    uint8_t  num_channels;
    bool     is_float;
} t_codec;

#endif//__CODEC__CODEC
