CC       = gcc
CFLAGS   = -std=c99 -Wall -I -I../common -DNDEBUG
LINKER   = $(CC) -o
LFLAGS   = -Wall -Wextra -I. -lm
OUT      = ../out


all: $(OUT)/wav_dec_test $(OUT)/wav_enc_test


test: all
	@echo "Testing wav_dec"
	@$(OUT)/wav_dec_test
	@echo "Testing wav_enc"
	@$(OUT)/wav_enc_test


$(OUT)/wav_dec_test: wav_dec.c
	@mkdir -p $(OUT)
	@$(CC) $(CFLAGS) wav_dec.c -o $@ -DWAV_DEC_TEST
	@echo "level_mix_test built"
	
	
$(OUT)/wav_enc_test: wav_enc.c wav_dec.c
	@mkdir -p $(OUT)
	@$(CC) $(CFLAGS) wav_enc.c wav_dec.c -o $@ -DWAV_ENC_TEST
	@echo "level_mix_test built"	


.PHONY: clean test
clean:
	@ rm -rf ../out
