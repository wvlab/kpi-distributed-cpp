CC ?= gcc
CFLAGS := -I.
CXX ?= g++
CXXFLAGS := -I.
SRCDIR := ./src
OUTDIR := ./out

all: cli gui

cli: $(OUTDIR) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchcli.c
	$(CC) $(CFLAGS) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchcli.c -o $(OUTDIR)/cli

gui: $(OUTDIR) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchgui.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchgui.cpp -o $(OUTDIR)/gui

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -r $(OUTDIR)
