CC = xc8-cc
DEVICE = 16F690
CFLAGS = -mcpu=$(DEVICE) -mc90lib

HDR += uart.h
HDR += init.h

OBJ += uart.p1
OBJ += init.p1
OBJ += test.p1

EXE = program
OUT = out

# can we simplify this makefile?

program: $(addprefix $(OUT)/, $(OBJ))
	@echo "Linking: $(OBJ) ....."
	@$(CC) $(CFLAGS) $(addprefix $(OUT)/, $(OBJ)) -o $(OUT)/$(EXE)

clean:
	@echo "Cleaning: $(OUT)/ ....."
	@rm -rf $(OUT)/*

$(OUT)/test.p1: test.c $(HDR)
	@echo "Compiling: test.c ....."
	@$(CC) $(CFLAGS) -c test.c -o $(OUT)/test.p1

$(OUT)/init.p1: init.c init.h
	@echo "Compiling: init.c ....."
	@$(CC) $(CFLAGS) -c init.c -o $(OUT)/init.p1

$(OUT)/uart.p1: uart.c uart.h
	@echo "Compiling: uart.c ....."
	@$(CC) $(CFLAGS) -c uart.c -o $(OUT)/uart.p1
