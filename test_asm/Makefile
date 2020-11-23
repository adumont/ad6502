# build tools & options
CL65 = cl65
CLFLAGS  = -v -d -t none -O --cpu 6502 -C sbc.cfg -m $*.map
HEXDUMP = hexdump
HEXDUMP_ARGS = -v -e '1/1 "%02x " "\n"'

%.hex: %.bin
	$(HEXDUMP) $(HEXDUMP_ARGS) $< > $@

%.bin: %.s interrupt.s vectors.s copydata.s
	$(CL65) $(CLFLAGS) -Ln $*.lbl -o $@ $^ none.lib

clean:
	-rm -f *.o *.hex *.map *.bin *.lbl
