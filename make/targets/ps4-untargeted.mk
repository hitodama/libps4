###################################

include $(MakePath)/traits/freestanding.mk
include $(MakePath)/traits/ps4-untargeted.mk
include $(MakePath)/traits/pie.mk
include $(MakePath)/traits/link.mk

###################################

all:: $(OutPath)/$(TargetFile)

###################################
