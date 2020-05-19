TOP:=top

MODULE?=$(TOP)

AUXFILES_TOP:= \
    $(AUXFILES_font) \
    rom.hex

# MUSTACHE_GENERATED:=Labels.lst vgaModulesPipe.v vgaLabels.v

ifeq ($(MODULE), $(TOP))

  AUXFILES:=$(AUXFILES_TOP)

else ifeq ($(MODULE), font)

  AUXFILES:=$(AUXFILES_font)

endif

YOSYSOPT:=-abc9
#YOSYSOPT:=-retime -abc2

ifndef MEMORY
	MEMORY="1k"
endif

