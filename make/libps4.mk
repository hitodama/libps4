###################################

Assembler ?= clang
Compiler ?= clang
Linker ?= clang
Archiver ?= ar
ObjectCopy ?= objcopy

###################################

MakePath := $(dir $(lastword $(MAKEFILE_LIST)))

SourcePath := source
IncludePath := -I. -Iinclude
LibPath := -L. -Llib
BuildPath := build
OutPath := bin

###################################

ifdef DEBUG
Debug := $(DEBUG)
endif
ifdef debug
Debug := $(debug)
endif
ifdef Debug
Debug := -DDebug #-g
endif

ifdef TARGETFILE
TargetFile := $(TARGETFILE)
endif
ifdef targetfile
TargetFile := $(targetfile)
endif

ifdef LIBRARIES
Libraries := $(LIBRARIES)
endif
ifdef libraries
Libraries := $(libraries)
endif

Libraries ?=

ifdef CF
Cf := $(CF)
endif
ifdef cf
Cf := $(cf)
endif

Cf ?=

ifdef SF
Sf := $(SF)
endif
ifdef sf
Sf := $(sf)
endif

Sf ?=

ifdef LF
Lf := $(LF)
endif
ifdef lf
Lf := $(lf)
endif

Lf ?=

###################################

AssemblerFlags = -m64
CompilerFlags = -std=c11 -O3 -Wall -pedantic -m64 -mcmodel=large $(IncludePath) $(Debug)
LinkerFlags = -O3 -Wall -m64 $(LibPath) $(Debug)
ArchiverFlags = rcs

###################################

findwildcard_ = $(wildcard $1$2) $(strip $(foreach d,$(wildcard $1*),$(call findwildcard_,$d/,$2)))
findwildcard = $(call findwildcard_,$(strip $(1))/,$(strip $(2)))
dirp = @mkdir -p $(@D)

###################################

assemble = $(Assembler) $(Sf) -c $< $(AssemblerFlags) -o $@
compile = $(Compiler) $(Cf) -c $< $(CompilerFlags) -o $@
link = $(Linker) $(Lf) $(CrtFile) $? $(LinkerFlags) $(Libraries) -o $@
compileAndLink = $(Cf) $(Compiler) $? $(CompilerFlags) $(LinkerFlags) $(Libraries) -o $@
archive = $(Archiver) $(ArchiverFlags) $@ $?

###################################

SourceFilesC += $(call findwildcard, $(SourcePath), *.c)
SourceFilesS += $(call findwildcard, $(SourcePath), *.s)
ObjectFiles	+=	$(patsubst $(SourcePath)/%.c, $(BuildPath)/%.c.o, $(SourceFilesC)) \
				$(patsubst $(SourcePath)/%.s, $(BuildPath)/%.s.o, $(SourceFilesS))

TargetFile ?= $(basename $(notdir $(CURDIR)))

###################################

.PHONY:: all clean
.DEFAULT_GOAL := all

###################################

ifeq ($(target), x64)
	include $(MakePath)/targets/x86-64.mk
else ifeq ($(target), x86-64)
		include $(MakePath)/targets/x86-64.mk
else ifeq ($(target), x86-64-nostd)
	include $(MakePath)/targets/x86-64-nostd.mk
else ifeq ($(target), ps4-untargeted)
	include $(MakePath)/targets/ps4-untargeted.mk
else ifeq ($(target), ps4)
	include $(MakePath)/targets/ps4-elf.mk
else ifeq ($(target), ps4-elf)
	include $(MakePath)/targets/ps4-elf.mk
else ifeq ($(target), ps4-lib)
	include $(MakePath)/targets/ps4-lib.mk
else ifeq ($(target), ps4-bin)
	include $(MakePath)/targets/ps4-bin.mk
else ifeq ($(target), )
	include $(MakePath)/targets/ps4-elf.mk
else
$(error Specified target does not exist)
endif

###################################

$(BuildPath)/%.s.o: $(SourcePath)/%.s
	$(dirp)
	$(assemble)

$(BuildPath)/%.c.o: $(SourcePath)/%.c
	$(dirp)
	$(compile)

###################################

all::

clean::
	rm -fR $(BuildPath) $(OutPath)

###################################
