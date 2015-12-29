###################################

ifdef libps4
LibPS4 := $(libps4)
endif
ifdef LIBPS4
LibPS4 := $(LIBPS4)
endif
ifndef LibPS4
$(error Neither LIBPS4, LibPS4 nor libps4 set)
endif

###################################

AssemblerFlags += -I$(LibPS4)/include
CompilerFlags += -D__PS4__ -I$(LibPS4)/include
LinkerFlags += -L$(LibPS4)/lib

Libraries += -lps4

###################################

CrtFile ?= $(LibPS4)/crt0.s
#link = $(Linker) $(LibPS4)/crt0.s $? $(LinkerFlags) $(Libraries) -o $@

###################################
