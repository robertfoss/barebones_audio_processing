#ifndef __CODEC__WAV_ENC
#define __CODEC__WAV_ENC


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/error.h"
#include "../common/sample.h"

#include "wav.h"


int wav_enc(FILE *fp, t_sample *sample);

#endif//__CODEC_WAV_ENC
