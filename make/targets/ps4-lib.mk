###################################

include $(MakePath)/traits/freestanding.mk
include $(MakePath)/traits/ps4.mk

###################################

OutPath = lib

###################################

CompilerFlags += $(LibPS4Flags) -fPIC

###################################

all:: $(OutPath)/$(TargetFile).a

###################################
