CC = gcc
LD = gcc

CFLAGS += -Wall
CFLAGS += -Wpedantic
CFLAGS += -std=c18

LDFLAGS += -Wall
LDFLAGS += -Wpedantic
LDFLAGS += -std=c18

TARGET = $(word 1, $(MAKECMDGOALS))

ifeq ($(TARGET), dexterity)
	SRC += dexterity.c
	BIN = dexterity
else ifeq ($(TARGET), )
	SRC += dexterity.c
	BIN = dexterity
else ifeq ($(TARGET), clean)
	# Run clean as usual
else
  $(error Unknown target '$(TARGET)')
endif

SRC += log.c
SRC += serial.c

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
	@echo "CC   $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MF $@

-include $(addprefix $(OUT)/, $(DEP))

clean:
	@echo "RM   $(ART)"
	@rm -rf $(ART)
