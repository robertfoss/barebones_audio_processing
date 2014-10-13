CC       = gcc
CFLAGS   = -std=c99 -Wall -I. -I../codec -I../common -DNDEBUG -DLEVEL_MIX_TEST
LINKER   = $(CC) -o
LFLAGS   = -Wall -Wextra -I. -lm
OUT      = ../out


all: $(OUT)/level_mix_test


test: all
	@echo "Testing level_mix"
	@$(OUT)/level_mix_test


$(OUT)/level_mix_test: ../codec/wav_enc.c ../codec/wav_dec.c level_mix.c
	@mkdir -p $(OUT)
	@$(CC) $(CFLAGS) ../codec/wav_enc.c ../codec/wav_dec.c level_mix.c -o $@
	@echo "level_mix_test built"
	

.PHONY: clean test
clean:
	@rm -rf $(OUT)
	@rm ../hire_me.wav
