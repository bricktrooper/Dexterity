CC = gcc
LD = gcc
MM = gcc

INCLUDE += ../common

VPATH += $(INCLUDE)

MMFLAGS += $(addprefix -I , $(INCLUDE))

CFLAGS += -Wall
CFLAGS += -Wpedantic
CFLAGS += -std=c18
CFLAGS += -g
CFLAGS += $(addprefix -I , $(INCLUDE))

LDFLAGS += -Wall
LDFLAGS += -Wpedantic
LDFLAGS += -std=c18
LDFLAGS += -g

TARGET = $(word 1, $(MAKECMDGOALS))

ifeq ($(TARGET), dexterity)
	SRC += main.c
	BIN = dexterity
else ifeq ($(TARGET), )
	SRC += main.c
	BIN = dexterity
else ifeq ($(TARGET), clean)
	# Run clean as usual
else
  $(error Unknown target '$(TARGET)')
endif

SRC += dexterity.c
SRC += log.c
SRC += utils.c
SRC += serial.c
SRC += calibration.c

OUT = bin
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
ART += $(OUT)

$(shell mkdir -p $(OUT))

.PHONY:
dexterity: $(OUT)/$(BIN)

$(OUT)/$(BIN): $(addprefix $(OUT)/, $(OBJ))
	@echo "LD   $(OBJ)"
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "BIN  $(BIN)"

$(OUT)/%.o: %.c
	@echo "CC   $(notdir $<)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/%.d: %.c
	@$(CC) $(MMFLAGS) -MM $< -MF $@

-include $(addprefix $(OUT)/, $(DEP))

clean:
	@echo "RM   $(OUT)/" "$(subst $(subst ,, ),\nRM   , $(notdir $(wildcard $(ART)/*)))"
	@rm -rf $(ART)
