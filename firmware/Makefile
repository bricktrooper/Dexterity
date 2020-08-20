CC = xc8-cc
LD = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

LDFLAGS += -mcpu=16F690
LDFLAGS += -mc90lib
LDFLAGS += -msummary=none
LDFLAGS += -mwarn=-2

SRC += test.c
SRC += init.c
SRC += uart.c
SRC += serial.c
SRC += flex.c
SRC += accel.c
SRC += adc.c

BIN = bin
OBJ = $(SRC:.c=.p1)
DEP = $(SRC:.c=.d)
EXE = dexterity.hex
ART += $(BIN)

$(shell mkdir -p $(BIN))

$(BIN)/$(EXE): $(addprefix $(BIN)/, $(OBJ))
	@echo LD $(OBJ)
	@$(LD) $(LDFLAGS) $^ -o $@

$(BIN)/%.p1: %.c
	@echo CC $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MF $@
	@mv $(subst .c,.i,$<) $(BIN)

-include $(addprefix $(BIN)/, $(DEP))

clean:
	@echo RM $(ART)
	@rm -rf $(ART)
