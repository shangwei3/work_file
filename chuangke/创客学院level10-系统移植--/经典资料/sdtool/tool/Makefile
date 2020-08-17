CC		= gcc
CFLAGS		= -Wall -Os

CSRCS		= $(wildcard *.c)
COBJS		= $(CSRCS:.c=.o)
TARGET		= mkbl2

all:	$(COBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

clean:
	@rm -rf $(COBJS) $(TARGET) *.d

.PHONY: clean 

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

sinclude $(CSRCS:.c=.d)

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,/($*/).o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
