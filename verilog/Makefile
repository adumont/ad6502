# call with make MODULE=moduleName sim|svg|upload

include Boards.mk
BOARD_BUILDDIR:=build/$(BOARD)
BUILDDIR:=$(BOARD_BUILDDIR)

include Design.mk

# Functions
FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
FILTER_IN  = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v),))

# From all .v* files, we exclude .vh and _tb.v
SOURCES_V:=$(call FILTER_OUT,_tb.v, $(call FILTER_OUT,.vh, $(wildcard *.v)))

# load dependencies if exists
#-include $(SOURCES_V:.v=.v.d))

# populate DEPS
-include top.v.d

FAKE_HEX =	rom.hex
REAL_HEX =  code.hex
ICEBRAM = icebram

.PRECIOUS: %.v.d %.json

%.v.d: %.v $(DEPS) $(MUSTACHE_GENERATED) $(FAKE_HEX)
	echo Generating dependencies file $@
	$(YOSYS) -q -E $@.tmp $<
	sed 's/:/DEPS:=/g' < $@.tmp > $@
	rm $@.tmp

.v.d:
	@true


$(FAKE_HEX):
	$(ICEBRAM) -g 8 4096 > $(FAKE_HEX)

diagrams: $(addprefix assets/, $(SOURCES_V:.v=.svg)) # $(addprefix assets/, $(SOURCES_V:.v=_dot.svg))
	cd mustache; ./mkDiagrams.py

# Use Docker images
DOCKER=docker
#
PWD = $(shell pwd)
DOCKERARGS = run --rm -v $(PWD):/src -w /src
#
# DOCKER_YOSYS="adumont/yosys:master"
ifneq ($(DOCKER_YOSYS),)
YOSYS     = $(DOCKER) $(DOCKERARGS) $(DOCKER_YOSYS)
else
YOSYS=yosys
endif

NEXTPNR   = $(DOCKER) $(DOCKERARGS) ghdl/synth:nextpnr-ice40 nextpnr-ice40

BUILDDIR?=build/$(BOARD)
ARACHNE_SEED?=36747270

all: bin svg dot sim

bin: $(BUILDDIR)/$(MODULE).bin
vcd: $(MODULE)_tb.vcd
sim: vcd gtkwave
json: $(BUILDDIR)/$(MODULE)-netlist.json
svg: assets/$(MODULE).svg
dot: assets/$(MODULE)_dot.svg
lint: $(BOARD_BUILDDIR)/$(MODULE).lint
pipe: $(MUSTACHE_GENERATED)
deps: $(MODULE).v.d
mustache: $(MUSTACHE_GENERATED)

# https://devhints.io/makefile & https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html 

# @echo '@: $@' # file name of the target
# @echo '%: $%' # name of the archive member

# @echo '<: $<' # name of the first prerequisite
# @echo '?: $?' # names of all prerequisites newer than the target
# @echo '^: $^' # names of all prerequisites
# @echo '|: $|' # names of all the order-only prerequisites
# @echo '*: $*' # stem with which an implicit rule matches
# @echo $(word 2, $?) 2nd word names of all prerequisites 

PCF:=$(MODULE)-$(BOARD).pcf

ifeq ($(PROGRAM),test/$(LEVEL)/program)
CLEAN_PROGRAM:=$(PROGRAM)

$(PROGRAM): test/$(LEVEL)/PROG
	make -C test/$(LEVEL) -f ../tester.mk program
endif

# TODO rewrite with  %.v %.v.d $(DEPS_%)
$(MODULE)_tb.vcd: $(MODULE).v $(DEPS) $(MODULE)_tb.v $(PROGRAM) $(ROMFILE)

	iverilog $(MODULE).v $(DEPS) $(MODULE)_tb.v $(IVERILOG_MACRO) -o $(MODULE)_tb.out
	./$(MODULE)_tb.out

$(BOARD_BUILDDIR)/%.lint: %.v %.v.d $(DEPS_%) | $(BUILDDIR)

	verilator --lint-only $< && > $@ || ( rm -f $@; false )

gtkwave: $(MODULE).v $(DEPS) $(MODULE)_tb.v $(MODULE)_tb.vcd

	gtkwave $(MODULE)_tb.vcd $(MODULE)_tb.gtkw &

$(BOARD_BUILDDIR)/%.json: %.v %.v.d $(DEPS_%) $(AUXFILES) $(BOARD_BUILDDIR)/build.config | $(BUILDDIR)
	
	$(YOSYS) -p "synth_ice40 -top $* -json $@ $(YOSYSOPT)" \
              -l $(BUILDDIR)/$*.log -q $<

$(BOARD_BUILDDIR)/%.blif: %.v %.v.d $(DEPS_%) $(AUXFILES) $(BOARD_BUILDDIR)/build.config | $(BUILDDIR)
	
	$(YOSYS) -p "synth_ice40 -top $* -blif $@ $(YOSYSOPT)" \
              -l $(BUILDDIR)/$*.log -q $<

# set ARACHNEPNR=1 to force arachne-pnr
ifneq ($(ARACHNEPNR),)
%.pnr: %.blif $(PCF)
	$(warning Building with arachne-pnr, because ARACHNEPNR=$(ARACHNEPNR))
	arachne-pnr -s $(ARACHNE_SEED) -d $(PNRDEV) -p $(PCF) $< -o $@
else
%.pnr: %.json $(PCF)
	$(warning Building with NextPnR, because ARACHNEPNR=$(ARACHNEPNR))
	$(NEXTPNR) --hx$(PNRDEV) --package $(PNRPACK) --json $< --pcf $(PCF) --asc $@
endif

ifdef LEVEL
$(warning LEVEL=$(LEVEL))
$(BUILDDIR)/%.pnr: $(BOARD_BUILDDIR)/%.pnr $(PROGRAM) $(ROMFILE) | $(BUILDDIR)

	icebram dummy_prg.hex $(PROGRAM) < $< > $@.tmp && \
	icebram dummy_ram.hex $(ROMFILE) < $@.tmp > $@ && \
	rm $@.tmp
endif

$(REAL_HEX):
	$(MAKE) -C ../assembly/
	cp ../assembly/$(REAL_HEX) .

$(BUILDDIR)/%.bin: $(BUILDDIR)/%.pnr $(REAL_HEX) #$(BOARD_BUILDDIR)/%.lint
	$(ICEBRAM) $(FAKE_HEX) $(REAL_HEX) < $< > temp.asc
	icepack $< $@

upload: $(BUILDDIR)/$(MODULE).bin

	md5sum $< | cmp $(BOARD_BUILDDIR)/flashed.md5 && \
	( echo "INFO: FPGA $(BOARD) bitstream hasn't changed: Skipping programming and reseting board:" ; iceprog -t ) || \
	( iceprog $< && md5sum $< > $(BOARD_BUILDDIR)/flashed.md5 || rm $(BOARD_BUILDDIR)/flashed.md5 )

$(BUILDDIR)/%-netlist.json: %.v %.v.d $(DEPS_%) | $(BUILDDIR)
	yosys -p "prep -top $*; write_json $@" $<


$(BUILDDIR)/%-netlist-svg.json: %.v $(DEPS_%) | $(BUILDDIR)
	yosys -DYOSYS_PLOT -q -p "prep -top $*; write_json -compat-int $@" $*.v

$(BUILDDIR)/%-netlist-svg.json: %.v $(DEPS_%) | $(BUILDDIR)
	yosys -DYOSYS_PLOT -q -p "prep -top $*; write_json -compat-int $@" $*.v

assets/%.svg: $(BUILDDIR)/%-netlist-svg.json
	netlistsvg $< -o $@

assets/%_dot.svg: %.v %.v.d $(DEPS_%)
	$(YOSYS) -DYOSYS_PLOT -q -p "read_verilog $*.v; hierarchy -check; proc; opt; fsm; opt; memory; opt; clean; stat; show -colors 1 -format svg -stretch -prefix $*_dot $*;"
	mv $(@F) $@
	[ -f $*_dot.dot ] && rm $*_dot.dot

$(MUSTACHE_GENERATED): $(wildcard mustache/*.mustache) mustache/modules.yaml
	@echo Generating mustache $@
	cd mustache && ./mkPipe.py

# We save AUXFILES names to build.config. Force a rebuild if they have changed
$(BOARD_BUILDDIR)/build.config: $(AUXFILES) .force | $(BUILDDIR)
	@echo '$(AUXFILES)' | cmp -s - $@ || echo '$(AUXFILES)' > $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BOARD_BUILDDIR)/top_wrapper.v: top_wrapper.m4 $(BOARD_BUILDDIR)/build.config
	m4 $(M4_OPTIONS) top_wrapper.m4 > $(BOARD_BUILDDIR)/top_wrapper.v

test:
	$(MAKE) -C test all

clean:
	rm -rf $(BOARD_BUILDDIR) $(BUILDDIR) $(CLEAN_PROGRAM) $(MUSTACHE_GENERATED) *.v.d

clean_svg:
	echo rm $(addprefix assets/, $(SOURCES_V:.v=.svg))

.PHONY: all clean json vcd svg bin sim dot .force test upload lint clean_svg
