CC = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

VPATH += out

HDR_MOD += init.h

OBJ += init.p1
OBJ += test.p1

OBJ_ART = $(addprefix out/, $(OBJ))

EXE = init.hex

$(EXE): $(OBJ)
	@echo "LD  $(OBJ)"
	@$(CC) $(CFLAGS) $(OBJ_ART) -o out/$(EXE)

clean:
	@echo "RM  init/out/*"
	@rm -rf out/*

init.p1: init.c $(HDR_MOD)
	@echo "CC  init.c"
	@$(CC) $(CFLAGS) -c init.c -o out/init.p1

test.p1: test.c $(HDR_MOD)
	@echo "CC  test.c"
	@$(CC) $(CFLAGS) -c test.c -o out/test.p1
