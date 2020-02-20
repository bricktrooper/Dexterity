CC = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

INIT = ../init
UART = ../uart

SRC_DIRS += out
SRC_DIRS += $(INIT)
SRC_DIRS += $(UART)

VPATH = $(subst $(eval) ,:,$(SRC_DIRS))

INCLUDE += -I $(INIT)
INCLUDE += -I $(UART)

HDR_MOD += uart.h
HDR_MOD += serial.h

HDR_EXE += init.h
HDR_EXE += uart.h
HDR_EXE += serial.h

OBJ += serial.p1
OBJ += test.p1

EXT += init.p1
EXT += uart.p1

OBJ_ART += $(addprefix out/, $(OBJ))

EXT_ART += $(INIT)/out/init.p1
EXT_ART += $(UART)/out/uart.p1

EXE = serial.hex

$(EXE): $(OBJ)
	@make init.p1 -C $(INIT)
	@make uart.p1 -C $(UART)
	@echo "LD  $(OBJ) $(EXT)"
	@$(CC) $(CFLAGS) $(OBJ_ART) $(EXT_ART) -o out/$(EXE)

clean:
	@echo "RM  serial/out/*"
	@rm -rf out/*

serial.p1: serial.c $(HDR_MOD)
	@echo "CC  serial.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c serial.c -o out/serial.p1

test.p1: test.c $(HDR_EXE)
	@echo "CC  test.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c test.c -o out/test.p1
