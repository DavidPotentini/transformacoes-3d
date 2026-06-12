CC      = gcc
CFLAGS  = -Wall -Iinclude $(shell sdl2-config --cflags 2>/dev/null || pkg-config --cflags sdl2)
LDFLAGS = $(shell sdl2-config --libs 2>/dev/null || pkg-config --libs sdl2) -lm

SRCDIR  = src
OBJDIR  = build
OBJS    = $(OBJDIR)/main.o $(OBJDIR)/objeto.o $(OBJDIR)/camera.o $(OBJDIR)/algebra.o $(OBJDIR)/tela.o
ALVO    = trabalho2

all: $(ALVO)

$(ALVO): $(OBJS)
	$(CC) $(OBJS) -o $(ALVO) $(LDFLAGS)

# Dependencias de cabecalho (recompila se um .h mudar)
$(OBJDIR)/main.o:    include/objeto.h include/camera.h include/tela.h include/algebra.h
$(OBJDIR)/objeto.o:  include/objeto.h include/algebra.h
$(OBJDIR)/camera.o:  include/camera.h include/algebra.h
$(OBJDIR)/algebra.o: include/algebra.h
$(OBJDIR)/tela.o:    include/tela.h

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(ALVO)

.PHONY: all clean
