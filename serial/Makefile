CC = xc8-cc
DEVICE = 16F690
CFLAGS = -mcpu=$(DEVICE) -mc90lib

INIT = ../init
UART = ../uart

HDR += serial.h
HDR += $(INIT)/init.h
HDR += $(UART)/uart.h

EXE = program

OBJ += serial.p1
OBJ += test.p1

ART += $(addprefix out/, $(OBJ))

EXT += $(INIT)/out/init.p1
EXT += $(UART)/out/uart.p1

.PHONY: serial.p1
.PHONY: test.p1

$(EXE): $(ART)
	@make init.p1 -C $(INIT)
	@make uart.p1 -C $(UART)
	@echo "LD  $(OBJ) init.p1 uart.p1"
	@$(CC) $(CFLAGS) $(ART) $(EXT) -o out/$(EXE)

all: $(OBJ)
serial.p1: out/serial.p1
test.p1: out/test.p1

clean:
	@echo "RM  serial/out/*"
	@rm -rf out/*

out/serial.p1: serial.c serial.h
	@echo "CC  serial.c"
	@$(CC) $(CFLAGS) -c serial.c -o out/serial.p1

out/test.p1: test.c $(HDR)
	@echo "CC  test.c"
	@$(CC) $(CFLAGS) -c test.c -o out/test.p1
