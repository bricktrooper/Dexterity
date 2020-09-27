CC = gcc
LD = gcc

CFLAGS += -Wall -Wpedantic

LDFLAGS += -Wall -Wpedantic

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

SRC += dexterity.c
SRC += messages.c

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
