CC		:=	clang
AS		:=	clang
AR		:=	ar
OBJCOPY	:=	objcopy

ODIR	:=	build
SDIR	:=	source
IDIR	:=	include
LDIR	:=	lib

LIBPS4CFLAGS += -D"LibPS4SyscallDirect"
#LIBPS4CFLAGS += -D"LibPS4SyscallWrapped"

CFLAGS	:=	$(LIBPS4CFLAGS) -I$(IDIR) -O3 -Wall -pedantic -std=c11 -m64 -mcmodel=large -ffreestanding -nostdlib -nostdinc -fno-builtin -fPIC -target x86_64-scei-ps4-elf
SFLAGS	:=	-target x86_64-scei-ps4-elf -m64

CFILES	:=	$(wildcard $(SDIR)/**/*.c)
CFILES	+=	$(wildcard $(SDIR)/**/**/*.c)
SFILES	:=	$(wildcard $(SDIR)/**/*.s)
SFILES	+=	$(wildcard $(SDIR)/**/**/*.s)
OBJS	:=	$(patsubst $(SDIR)/%.c, $(ODIR)/%.c.o, $(CFILES))
OBJS	+=	$(patsubst $(SDIR)/%.s, $(ODIR)/%.s.o, $(SFILES))

TARGET = lib/$(shell basename $(CURDIR)).a

$(TARGET): $(ODIR) $(OBJS)
	@mkdir -p $@
	@rmdir $@
	$(AR) rcs $@ $(OBJS)

$(ODIR)/%.c.o: $(SDIR)/%.c
	@mkdir -p $@
	@rmdir $@
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.s.o: $(SDIR)/%.s
	@mkdir -p $@
	@rmdir $@
	$(AS) -c -o $@ $< $(SFLAGS)

$(ODIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -fR $(TARGET) $(ODIR)
