CXX ?= g++
WINCXX ?= x86_64-w64-mingw32-g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra
CXXFLAGS += -I.
SRCDIR := ./src
OUTDIR := ./out

all: cli gui

cli: $(OUTDIR) $(SRCDIR)/fetchbase.cpp $(SRCDIR)/fetchcli.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/fetchbase.cpp $(SRCDIR)/fetchcli.cpp -o $(OUTDIR)/cli

ifeq ($(OS), Windows_NT)
gui: wingui
else
gui: gtkgui
endif

gtkgui:
	$(CXX) \
		$(CXXFLAGS) \
		$(shell pkg-config --cflags gtk4) \
		$(SRCDIR)/fetchbase.cpp \
		$(SRCDIR)/fetchgui_gtk.cpp \
		-o $(OUTDIR)/gui \
		$(shell pkg-config --libs gtk4)

wingui: $(OUTDIR) $(SRCDIR)/fetchbase.cpp $(SRCDIR)/fetchgui_win.cpp
	$(WINCXX) \
		$(CXXFLAGS) \
		$(SRCDIR)/fetchbase.cpp \
		$(SRCDIR)/fetchgui_win.cpp \
		-o $(OUTDIR)/gui.exe \
		-lgdi32 \
		-luser32 \
		-lws2_32 \
		-liphlpapi \
		-municode \
		-static-libgcc -static-libstdc++

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)
