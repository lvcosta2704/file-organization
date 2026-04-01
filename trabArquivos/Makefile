CC := gcc
CFLAGS := -lmd
LDFLAGS :=
LDLIBS :=

SRCDIR := src
INCDIR := include
OBJDIR := obj
BINDIR := bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET := $(BINDIR)/main

.PHONY: all clean run

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJDIR) $(BINDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/Registros.h
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

run: $(TARGET)
	./$<

clean:
	rm -rf $(OBJDIR) $(BINDIR)
