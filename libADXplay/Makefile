
TARGET = libADXplay.elf

OBJS = libADXplay.o

all: clean $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS) 
	kos-cc -o $(TARGET) $(OBJS)  $(DATAOBJS) $(OBJEXTRA) -lADX

