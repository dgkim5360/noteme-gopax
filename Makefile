include config.mk

SRC=main.c request-gopax.c noteme.c
OBJ=$(SRC:.c=.o)

all: main
	@mv $< noteme-gopax

options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"
	@echo "SRC      = $(SRC)"
	@echo "OBJ      = $(OBJ)"

$(OBJ): request-gopax.h noteme.h

main: $(OBJ)

clean:
	$(RM) noteme-gopax $(OBJ)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f noteme-gopax $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/noteme-gopax

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/noteme-gopax
.PHONY: all options clean
