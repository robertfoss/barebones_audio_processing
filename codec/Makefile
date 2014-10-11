CC=gcc
CFLAGS=-I. -g
OUT=../out

all: $(OUT)/wav_dec_test $(OUT)/wav_dec_test_ndebug

$(OUT)/wav_dec_test_obj/%.o: wav_dec.c
	mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) -DTEST

$(OUT)/wav_dec_test: $(OUT)/wav_dec_test_obj/wav_dec.o
	$(CC) -o $@ $^ $(CFLAGS) -DTEST


$(OUT)/wav_dec_test_ndebug_obj/%.o: wav_dec.c
	mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) -DTEST -DNDEBUG

$(OUT)/wav_dec_test_ndebug: $(OUT)/wav_dec_test_ndebug_obj/wav_dec.o
	$(CC) -o $@ $^ $(CFLAGS) -DTEST -DNDEBUG

clean:
	rm -rf $(OUT)
