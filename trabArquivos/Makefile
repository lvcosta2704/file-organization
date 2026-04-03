CC := gcc
CFLAGS := -Iinclude

SRCDIR := src
INCDIR := include
BINDIR := bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
TARGET := $(BINDIR)/main

.PHONY: all clean run

all: 
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$<

clean:
	rm -rf $(OBJDIR) $(BINDIR)
