CC		:=	clang
AS		:=	clang
AR		:=	ar
OBJCOPY	:=	objcopy

ODIR	:=	build
SDIR	:=	source
IDIR	:=	include
LDIR	:=	lib

ifndef LIBPS4CFLAGS
#LIBPS4CFLAGS := -D"LibPS4SyscallWrapped"
LIBPS4CFLAGS := -D"LibPS4SyscallDirect"
endif

CFLAGS	:=	$(LIBPS4CFLAGS) -I$(IDIR) -O3 -Wall -pedantic -std=c11 -m64 -mcmodel=large -ffreestanding -nostdlib -nostdinc -fno-builtin -fPIC -target x86_64-scei-ps4-elf
SFLAGS	:=	-target x86_64-scei-ps4-elf -m64

CFILES	:=	$(wildcard $(SDIR)/**/*.c)
CFILES	+=	$(wildcard $(SDIR)/**/**/*.c)
SFILES	:=	$(wildcard $(SDIR)/**/*.s)
SFILES	+=	$(wildcard $(SDIR)/**/**/*.s)
OBJS	:=	$(patsubst $(SDIR)/%.c, $(ODIR)/%.c.o, $(CFILES))
OBJS	+=	$(patsubst $(SDIR)/%.s, $(ODIR)/%.s.o, $(SFILES))

TDIR	:= lib
#TFILE	:= $(TDIR)/$(shell basename $(CURDIR)).a #avoid shell?
TFILE	:= $(TDIR)/libps4.a

#generate_target <type> <module> <symbol> <header>
define GENERATE_TARGET
OBJS	+= $(ODIR)/$(strip $(2))/$(strip $(3)).stub.o
$(ODIR)/$(strip $(2))/$(strip $(3)).stub.o: stub/$(strip $(1)).stub.c
ifeq ("$(wildcard $$@)", "")
	@mkdir -p $$@
	@rmdir $$@
endif
	$(CC) $(CFLAGS) \
		-DPS4MODULE="$(strip $(2))" \
		-DPS4SYMBOL="$(strip $(3))" \
		-DPS4HEADER="<$(strip $(4)).h>" \
		 $$< -c -o $$@

endef

#generate <module> <header> <fsym> <fssym> <ssym>
define GENERATE
$(call GENERATE_TARGET, module, $(1), $(2), $(2))
$(foreach i, $(3), $(call GENERATE_TARGET, function, $(1), $(i), $(2)))
$(foreach i, $(4), $(call GENERATE_TARGET, functionOrSyscall, $(1), $(i), $(2)))
$(foreach i, $(5), $(call GENERATE_TARGET, syscall, $(1), $(i), $(2)))

endef

include symbols.mk

$(TFILE): $(TDIR) $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(ODIR)/%.c.o: $(SDIR)/%.c
ifeq ("$(wildcard $@)", "")
	@mkdir -p $@
	@rmdir $@
endif
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.s.o: $(SDIR)/%.s
ifeq ("$(wildcard $@)", "")
	@mkdir -p $@
	@rmdir $@
endif
	$(AS) -c -o $@ $< $(SFLAGS)

$(ODIR):
	@mkdir $@

$(TDIR):
	@mkdir $@

.PHONY: all clean

all: $(TFILE)

clean:
	rm -fR $(TDIR) $(ODIR)
