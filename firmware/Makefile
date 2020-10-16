CC = xc8-cc
LD = xc8-cc
MM = gcc

INCLUDE += ../common

VPATH += $(INCLUDE)

MMFLAGS += $(addprefix -I , $(INCLUDE))

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib
CFLAGS += $(addprefix -I , $(INCLUDE))

LDFLAGS += -mcpu=16F690
LDFLAGS += -mc90lib
LDFLAGS += -msummary=none
LDFLAGS += -mwarn=0

TARGET = $(word 1, $(MAKECMDGOALS))

ifneq ($(TYPE), )
	TEST_TYPE = -D$(TYPE)
endif

ifeq ($(TARGET), test)
	SRC += test.c
	BIN = test.hex
else ifeq ($(TARGET), dexterity)
	SRC += main.c
	BIN = dexterity.hex
else ifeq ($(TARGET), )
	SRC += main.c
	BIN = dexterity.hex
else ifeq ($(TARGET), clean)
	# Run clean as usual
else
  $(error Unknown target '$(TARGET)')
endif

SRC += dexterity.c
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
	@echo "CC   $(notdir $<)"
	@$(CC) $(CFLAGS) $(TEST_TYPE) -c $< -o $@

$(OUT)/%.d: %.c
	@$(MM) $(MMFLAGS) -MM $< -MF $@

ifneq ($(TARGET), clean)
-include $(addprefix $(OUT)/, $(DEP))
endif

clean:
	@for ARTIFACT in $(notdir $(wildcard $(OUT)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
