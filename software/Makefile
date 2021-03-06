CC = clang
LD = clang
MM = gcc

INCLUDE += ../common

VPATH += $(INCLUDE)

MMFLAGS += $(addprefix -I , $(INCLUDE))

CFLAGS += -Wall
CFLAGS += -Wpedantic
CFLAGS += -Wformat
CFLAGS += -Wextra
CFLAGS += -std=c17
CFLAGS += -g
CFLAGS += $(addprefix -I , $(INCLUDE))

LDFLAGS += -Wall
LDFLAGS += -Wpedantic
LDFLAGS += -std=c17
LDFLAGS += -g
LDFLAGS += -framework ApplicationServices
LDFLAGS += -framework AppKit

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
SRC += command.c
SRC += mouse.c
SRC += keyboard.c
SRC += gesture.c
SRC += action.c
SRC += control.c
SRC += special.m

OBJ := $(SRC:.c=.o)   # C object files
OBJ := $(OBJ:.m=.o)   # Objective-C object files

DEP := $(SRC:.c=.d)   # C dependency files
DEP := $(DEP:.m=.d)   # Objective-C dependency files

ART = bin

$(shell mkdir -p $(ART))

.PHONY:
dexterity: $(ART)/$(BIN)

$(ART)/$(BIN): $(addprefix $(ART)/, $(OBJ))
	@echo "LD   $(OBJ)"
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "BIN  $(BIN)"

$(ART)/%.o: %.c
	@echo "CC   $(notdir $<)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(ART)/%.d: %.c
	@$(MM) $(MMFLAGS) -MM $< -MT $(subst .d,.o,$@) -MF $@

$(ART)/%.o: %.m
	@echo "CC   $(notdir $<)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(ART)/%.d: %.m
	@$(MM) $(MMFLAGS) -MM $< -MT $(subst .d,.o,$@) -MF $@

ifneq ($(TARGET), clean)
-include $(addprefix $(ART)/, $(DEP))
endif

clean:
	@for ARTIFACT in $(notdir $(wildcard $(ART)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
