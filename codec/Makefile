CC       = gcc
CFLAGS   = -std=c99 -Wall -I. -I../common
LINKER   = $(CC) -o
LFLAGS   = -Wall -Wextra -I. -lm

BINDIR   = ../out/
SOURCES  := $(wildcard *.c)
INCLUDES := $(wildcard *.h)

all: $(BINDIR)/wav_dec_test $(BINDIR)/wav_enc_test

WAV_DEC_OBJDIR   = ../out/obj_wav_dec
WAV_DEC_OBJECTS  := $(SOURCES:%.c=$(WAV_DEC_OBJDIR)/%.o)

$(BINDIR)/wav_dec_test: $(WAV_DEC_OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(WAV_DEC_OBJECTS) -DWAV_DEC_TEST
	@echo "wav_dec_test built"

$(WAV_DEC_OBJECTS): $(WAV_DEC_OBJDIR)/%.o : %.c
	@mkdir -p $(WAV_DEC_OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@ -DWAV_DEC_TEST
	
	
WAV_ENC_OBJDIR   = ../out/obj_wav_enc
WAV_ENC_OBJECTS  := $(SOURCES:%.c=$(WAV_ENC_OBJDIR)/%.o)

$(BINDIR)/wav_enc_test: $(WAV_ENC_OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(WAV_ENC_OBJECTS) -DWAV_ENC_TEST
	@echo "wav_enc_test built"
	
$(WAV_ENC_OBJECTS): $(WAV_ENC_OBJDIR)/%.o : %.c
	@mkdir -p $(WAV_ENC_OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@ -DWAV_ENC_TEST
	

.PHONEY: clean
clean:
	@ rm -rf ../out
