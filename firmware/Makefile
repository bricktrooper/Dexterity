CC = xc8-cc
LD = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

LDFLAGS += -mcpu=16F690
LDFLAGS += -mc90lib
LDFLAGS += -msummary=none
LDFLAGS += -mwarn=-2

TARGET = $(word 1, $(MAKECMDGOALS))

ifneq ($(TYPE), )
	TEST_TYPE = -D$(TYPE)
endif

ifeq ($(TARGET), test)
	SRC += test.c
else ifeq ($(TARGET), dexterity)
	SRC += dexterity.c
else ifeq ($(TARGET), )
	SRC += dexterity.c
else ifeq ($(TARGET), clean)
	# Run clean as usual
else
  $(error Unknown target '$(TARGET)')
endif

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

.PHONY:
dexterity: $(BIN)/$(EXE)
.PHONY:
test: $(BIN)/$(EXE)

$(BIN)/$(EXE): $(addprefix $(BIN)/, $(OBJ))
	@echo LD $(OBJ)
	@$(LD) $(LDFLAGS) $^ -o $@

$(BIN)/%.p1: %.c
	@echo CC $<
	@$(CC) $(CFLAGS) $(TEST_TYPE) -c $< -o $@

$(BIN)/%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MF $@
	@mv $(subst .c,.i,$<) $(BIN)

-include $(addprefix $(BIN)/, $(DEP))

clean:
	@echo RM $(ART)
	@rm -rf $(ART)
