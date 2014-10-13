#ifndef __MIX__LEVEL_MIX
#define __MIX__LEVEL_MIX


#include <stdlib.h>
#include <string.h>

#include "../codec/codec.h"
#include "../common/error.h"
#include "../common/sample.h"


int level_mix(t_sample *out, t_sample *in1, t_sample *in2, float level1, float level2);

#endif//__MIX__LEVEL_MIX
