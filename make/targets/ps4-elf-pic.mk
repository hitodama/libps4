###################################

include $(MakePath)/traits/freestanding.mk
include $(MakePath)/traits/ps4.mk
include $(MakePath)/traits/link.mk

###################################

AssemblerFlags += $(LibPS4Flags) -fPIC
CompilerFlags += $(LibPS4Flags) -fPIC
#LinkerFlags += -pie

###################################

all:: $(OutPath)/$(TargetFile)

###################################
