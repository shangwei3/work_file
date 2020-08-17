CROSS_COMPILE	= arm-linux-
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
OBJCOPY		= $(CROSS_COMPILE)objcopy
CFLAGS		= -fno-builtin -nostdinc -c
LDFLAGS		= -Tmap.lds

SSRCS		= $(wildcard *.S)
CSRCS		= $(wildcard *.c)
SOBJS		= $(SSRCS:.S=.o)
COBJS		= $(CSRCS:.c=.o)
TARGET		= stage2

all:	$(SOBJS) $(COBJS)
	$(LD) $(LDFLAGS) -o $(TARGET).elf $^
	$(OBJCOPY) -O binary  $(TARGET).elf $(TARGET).bin

clean:
	@rm -rf $(SOBJS) $(COBJS) $(TARGET)* *.d *.d.* bl2.bin

.PHONY: clean 

%.o:%.S
	$(CC) $(CFLAGS) -o $@ $<
%.o:%.c
	$(CC) $(CFLAGS) -o $@ $<

sinclude $(SSRCS:.S=.d)
sinclude $(CSRCS:.c=.d)

%.d:%.S
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,/($*/).o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,/($*/).o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
