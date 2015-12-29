###################################

libps4=.
target=ps4-lib

###################################

ifdef libps4flags
LibPS4Flags := $(libps4flags)
endif
ifdef LIBPS4CFLAGS
LibPS4Flags := $(LIBPS4CFLAGS)
endif

ifndef LibPS4Flags
#LibPS4Flags := -D"LibPS4SyscallWrapped"
LibPS4Flags := -D"LibPS4SyscallDirect"
endif
AssemblerFlags += -I$(LibPS4)/include

###################################

include make/libps4.mk

#generate_target <type> <module> <symbol> <header>
define generateTarget
ObjectFiles	+= $(BuildPath)/$(strip $(2))/$(strip $(3)).stub.o
$(BuildPath)/$(strip $(2))/$(strip $(3)).stub.o: stub/$(strip $(1)).stub.c
ifeq ("$(wildcard $$@)", "")
	@mkdir -p $$@
	@rmdir $$@
endif
	$$(compile) \
		-DPS4MODULE="$(strip $(2))" \
		-DPS4SYMBOL="$(strip $(3))" \
		-DPS4HEADER="<$(strip $(4)).h>"

endef

#generate <module> <header> <fsym> <fssym> <ssym>
define generate
$(call generateTarget, module, $(1), $(2), $(2))
$(foreach i, $(3), $(call generateTarget, function, $(1), $(i), $(2)))
$(foreach i, $(4), $(call generateTarget, functionOrSyscall, $(1), $(i), $(2)))
$(foreach i, $(5), $(call generateTarget, syscall, $(1), $(i), $(2)))

endef

include $(CURDIR)/symbols.mk

$(OutPath)/$(TargetFile).a: $(ObjectFiles)
	$(dirp)
	$(archive)

###################################
