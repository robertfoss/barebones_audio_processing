OUT = out

.PHONY: test make all
all:
	@make -C codec/ $a
	@make -C mix/ $@

test:
	@make -C codec/ $@
	@make -C mix/ $@

clean:
	@make -C codec/ $@
	@make -C mix/ $@
	@rm -rf $(OUT)
