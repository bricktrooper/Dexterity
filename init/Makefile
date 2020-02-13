CC = xc8-cc
DEVICE = 16F690
CFLAGS = -mcpu=$(DEVICE) -mc90lib

HDR += init.h

EXE = program

OBJ += init.p1
OBJ += test.p1

ART = $(addprefix out/, $(OBJ))

.PHONY: init.p1
.PHONY: test.p1

$(EXE): $(ART)
	@echo "LD  $(OBJ)"
	@mkdir -p out
	@$(CC) $(CFLAGS) $(ART) -o out/$(EXE)

all: $(OBJ)
init.p1: out/init.p1
test.p1: out/test.p1

clean:
	@echo "RM  init/out/"
	@rm -rf out

out/init.p1: init.c init.h
	@echo "CC  init.c"
	@mkdir -p out
	@$(CC) $(CFLAGS) -c init.c -o out/init.p1

out/test.p1: test.c $(HDR)
	@echo "CC  test.c"
	@mkdir -p out
	@$(CC) $(CFLAGS) -c test.c -o out/test.p1
