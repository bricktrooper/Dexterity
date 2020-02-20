CC = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

INIT = ../init

SRC_DIRS += out
SRC_DIRS += $(INIT)

VPATH = $(subst $(eval) ,:,$(SRC_DIRS))

INCLUDE += -I $(INIT)

HDR_MOD += uart.h

HDR_EXE += uart.h
HDR_EXE += init.h

OBJ += uart.p1
OBJ += test.p1

EXT += init.p1

OBJ_ART += $(addprefix out/, $(OBJ))
EXT_ART += $(INIT)/out/init.p1

EXE = uart.hex

$(EXE): $(OBJ)
	@make init.p1 -C $(INIT)
	@echo "LD  $(OBJ) $(EXT)"
	@$(CC) $(CFLAGS) $(OBJ_ART) $(EXT_ART) -o out/$(EXE)

clean:
	@echo "RM  uart/out/*"
	@rm -rf out/*

uart.p1: uart.c $(HDR_MOD)
	@echo "CC  uart.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c uart.c -o out/uart.p1

test.p1: test.c $(HDR_EXE)
	@echo "CC  test.c"
	@$(CC) $(CFLAGS) $(INCLUDE) -c test.c -o out/test.p1
