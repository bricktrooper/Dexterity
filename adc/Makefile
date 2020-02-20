CC = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

INIT = ../init
UART = ../uart
SERIAL = ../serial

SRC_DIRS += out
SRC_DIRS += $(INIT)
SRC_DIRS += $(UART)
SRC_DIRS += $(SERIAL)

VPATH = $(subst $(eval) ,:,$(SRC_DIRS))

INCLUDE += -I $(INIT)
INCLUDE += -I $(UART)
INCLUDE += -I $(SERIAL)

HDR_MOD += adc.h

HDR_EXE += init.h
HDR_EXE += uart.h
HDR_EXE += serial.h
HDR_EXE += adc.h

OBJ += adc.p1
OBJ += test.p1

EXT += init.p1
EXT += uart.p1
EXT += serial.p1

OBJ_ART += $(addprefix out/, $(OBJ))

EXT_ART += $(INIT)/out/init.p1
EXT_ART += $(UART)/out/uart.p1
EXT_ART += $(SERIAL)/out/serial.p1

EXE = adc.hex

$(EXE): $(OBJ)
	@make init.p1 -C $(INIT)
	@make uart.p1 -C $(UART)
	@make serial.p1 -C $(SERIAL)
	@echo "LD  $(OBJ) $(EXT)"
	@$(CC) $(CFLAGS) $(OBJ_ART) $(EXT_ART) -o out/$(EXE)

clean:
	@echo "RM  adc/out/*"
	@rm -rf out/*

adc.p1: adc.c $(HDR_MOD)
	@echo "CC  adc.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c adc.c -o out/adc.p1

test.p1: test.c $(HDR_EXE)
	@echo "CC  test.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c test.c -o out/test.p1
