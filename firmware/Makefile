CC = xc8-cc
LD = xc8-cc
MM = xc8-cc

INCLUDE += ../common

VPATH += $(INCLUDE)

MMFLAGS += -mcpu=16F690
MMFLAGS += -mc90lib
MMFLAGS += $(addprefix -I , $(INCLUDE))

CCFLAGS += -mcpu=16F690
CCFLAGS += -mc90lib
CCFLAGS += $(addprefix -I , $(INCLUDE))

LDFLAGS += -mcpu=16F690
LDFLAGS += -mc90lib
LDFLAGS += -mwarn=0

TARGET = $(word 1, $(MAKECMDGOALS))

ifneq ($(TYPE), )
	TEST_TYPE = -D$(TYPE)
endif

ifeq ($(MEMORY), show)
	LDFLAGS += -msummary=mem
else
	LDFLAGS += -msummary=none
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

OBJ = $(SRC:.c=.p1)
DEP = $(SRC:.c=.d)

ART = bin

$(shell mkdir -p $(ART))

.PHONY:
dexterity: $(ART)/$(BIN)
.PHONY:
test: $(ART)/$(BIN)

$(ART)/$(BIN): $(addprefix $(ART)/, $(OBJ))
	@echo "LD   $(OBJ)"
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "BIN  $(BIN)"

$(ART)/%.p1: %.c
	@echo "CC   $(notdir $<)"
	@$(CC) $(CCFLAGS) $(TEST_TYPE) -c $< -o $@

$(ART)/%.d: %.c
	@$(MM) $(MMFLAGS) -MM $< -MF $@
	@mv $(subst .c,.i,$(notdir $<)) $(ART)

ifneq ($(TARGET), clean)
-include $(addprefix $(ART)/, $(DEP))
endif

clean:
	@for ARTIFACT in $(notdir $(wildcard $(ART)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
