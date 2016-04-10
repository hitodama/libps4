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

#-D"LibPS4SyscallWrapped"
#-D"LibPS4SyscallDirect"
#-D"LibPS4KernelAndUser"
#-D"LibPS4KernelOnly"
#-D"LibPS4KernelNone"
LibPS4Flags ?= -D"LibPS4SyscallWrapped" -D"LibPS4KernelAndUser"

###################################

include $(CURDIR)/make/libps4.mk

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

#generate <module> <header> <fsym> <fssym> <ssym> <fksym> <fsksym> <sksym>
define generate
$(call generateTarget, module, $(1), $(2), $(2))
$(foreach i, $(3), $(call generateTarget, function, $(1), $(i), $(2)))
$(foreach i, $(4), $(call generateTarget, functionOrSyscall, $(1), $(i), $(2)))
$(foreach i, $(5), $(call generateTarget, functionAndKernelFunction, $(1), $(i), $(2)))
$(foreach i, $(6), $(call generateTarget, functionOrSyscallAndKernelFunction, $(1), $(i), $(2)))

endef


#generateKernel <type> <sym>
define generateNonModule
$(foreach i, $(2), $(call generateTarget, $(1), nomodule, $(i), ps4/kernel))

endef

include $(CURDIR)/symbols.mk

$(OutPath)/$(TargetFile).a: $(ObjectFiles)
	$(dirp)
	$(archive)

###################################
