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
	BIN = test.hex
else ifeq ($(TARGET), dexterity)
	SRC += dexterity.c
	BIN = dexterity.hex
else ifeq ($(TARGET), )
	SRC += dexterity.c
	BIN = dexterity.hex
else ifeq ($(TARGET), clean)
	# Run clean as usual
else
  $(error Unknown target '$(TARGET)')
endif

SRC += utils.c
SRC += init.c
SRC += uart.c
SRC += flex.c
SRC += accel.c
SRC += adc.c
SRC += led.c
SRC += button.c

OUT = bin
OBJ = $(SRC:.c=.p1)
DEP = $(SRC:.c=.d)
ART += $(OUT)

$(shell mkdir -p $(OUT))

.PHONY:
dexterity: $(OUT)/$(BIN)
.PHONY:
test: $(OUT)/$(BIN)

$(OUT)/$(BIN): $(addprefix $(OUT)/, $(OBJ))
	@echo "LD   $(OBJ)"
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "BIN  $(BIN)"

$(OUT)/%.p1: %.c
	@echo "CC   $<"
	@$(CC) $(CFLAGS) $(TEST_TYPE) -c $< -o $@

$(OUT)/%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MF $@
	@mv $(subst .c,.i,$<) $(OUT)

-include $(addprefix $(OUT)/, $(DEP))

clean:
	@echo "RM   $(ART)"
	@rm -rf $(ART)
