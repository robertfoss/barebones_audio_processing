#ifndef __CODEC__WAV
#define __CODEC__WAV

#include "codec.h"


#define WAVE_FORMAT_PCM         (0x0001)
#define WAVE_FORMAT_IEEE_FLOAT  (0x0003)


typedef struct {
    t_codec codec; // Must be first in this struct to allow for unknown codecs to be accessed using the common type t_codec

} t_codec_wav;

#endif//__CODEC__WAV
