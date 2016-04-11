###################################

include $(MakePath)/traits/freestanding.mk
include $(MakePath)/traits/ps4.mk

###################################

ifdef KEEPELF
KeepElf := $(KEEPELF)
endif
ifdef keepelf
KeepElf := $(keepelf)
endif

###################################

#Text ?= 0x926200000
Text ?= 0x93a300000
#Data ?= 0x926300000
Data ?= 0x93a400000

###################################

LinkerFlags += -Wl,-Ttext,$(Text) -Wl,-Tdata,$(Data)
#LinkerFlags += -Wl,--build-id=none -Ttext $(Text) -Tdata $(Data)

###################################

bincopy = $(ObjectCopy) $@ -O binary $@
copy = cp $@ $@.elf

###################################

$(OutPath)/$(TargetFile):: $(ObjectFiles)
	$(dirp)
	$(link)
ifdef KeepElf
	$(copy)
endif
	$(bincopy)

###################################

all:: $(OutPath)/$(TargetFile)

###################################
