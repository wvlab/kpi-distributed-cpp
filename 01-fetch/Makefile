CC ?= gcc
WINCC ?= x86_64-w64-mingw32-gcc
CFLAGS ?=
CFLAGS += -I.
SRCDIR := ./src
OUTDIR := ./out

all: cli gui

cli: $(OUTDIR) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchcli.c
	$(CC) $(CFLAGS) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchcli.c -o $(OUTDIR)/cli

ifeq ($(OS), Windows_NT)
gui: wingui
else
gui: gtkgui
endif

gtkgui:
	$(CC) \
		$(CFLAGS) \
		$(shell pkg-config --cflags gtk4) \
		$(SRCDIR)/fetchbase.c \
		$(SRCDIR)/fetchgui_gtk.c \
		-o $(OUTDIR)/gui \
		$(shell pkg-config --libs gtk4)

wingui: $(OUTDIR) $(SRCDIR)/fetchbase.c $(SRCDIR)/fetchgui_win.c
	$(WINCC) \
		$(CFLAGS) \
		$(SRCDIR)/fetchbase.c \
		$(SRCDIR)/fetchgui_win.c \
		-o $(OUTDIR)/gui.exe \
		-lgdi32 \
		-luser32 \
		-municode

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)
