CC = xc8-cc
DEVICE = 16F690
CFLAGS = -mcpu=$(DEVICE) -mc90lib

INIT = ../init

HDR += uart.h
HDR += $(INIT)/init.h

EXE = program

OBJ += uart.p1
OBJ += test.p1

ART += $(addprefix out/, $(OBJ))
ART += $(INIT)/out/init.p1

$(EXE): $(ART)
	@echo "LD  $(OBJ) init.p1"
	@mkdir -p out
	@$(CC) $(CFLAGS) $(ART) -o out/$(EXE)

uart.p1: out/uart.p1
test.p1: out/test.p1

clean:
	@echo "RM  uart/out/"
	@rm -rf out

out/uart.p1: uart.c uart.h
	@echo "CC  uart.c"
	@mkdir -p out
	@$(CC) $(CFLAGS) -c uart.c -o out/uart.p1

out/test.p1: test.c $(HDR)
	@echo "CC  test.c"
	@mkdir -p out
	@$(CC) $(CFLAGS) -c test.c -o out/test.p1

$(INIT)/out/init.p1:
	@make init.p1 -C $(INIT)
