###################################

include $(MakePath)/traits/freestanding.mk
include $(MakePath)/traits/pie.mk
include $(MakePath)/traits/link.mk

###################################

#CrtFile ?= crt0/crt0.s
CrtFile ?= $(LibPS4)/crt0.s
#link = $(Linker) $? $(LinkerFlags) $(Libraries) -o $@

###################################

all:: $(OutPath)/$(TargetFile)

###################################
