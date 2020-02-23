# ============== COMPILER ============== #

CC = xc8-cc

CFLAGS += -mcpu=16F690
CFLAGS += -mc90lib

# ============== BUILD CONFIG ============== #

include config.mk

DEP_OBJ = $(addsuffix .p1, $(DEP))
DEP_DIR = $(addprefix ../, $(DEP))

ART += $(addprefix out/, $(PRG_OBJ))
ART += $(foreach MODULE,$(DEP),../$(MODULE)/out/$(MODULE).p1)

INC += $(addprefix -I , $(DEP_DIR))

VPATH += out
VPATH += $(DEP_DIR)
VPATH += $(addsuffix /out, $(DEP_DIR))

# ============== MODULE RULES ============== #

$(EXE): $(PRG_OBJ) $(DEP_OBJ)
	@echo "LD  $(PRG_OBJ) $(DEP_OBJ)"
	@$(CC) $(CFLAGS) $(ART) -o out/$(EXE)

clean:
	@echo "RM  $(MOD)/out/*"
	@rm -rf out/*

$(MOD).p1: $(MOD).c $(MOD_HDR)
	@echo "CC  $(MOD).c"
	@$(CC) $(CFLAGS) $(INC) -c $(MOD).c -o out/$(MOD).p1

$(PRG).p1: $(PRG).c $(PRG_HDR)
	@echo "CC  $(PRG).c"
	@$(CC) $(CFLAGS) $(INC) -c $(PRG).c -o out/$(PRG).p1

# ============== DEPENDENCY RULES ============== #

$(DEP_OBJ): %.p1: %.c
	@make $@ -C ../$(basename $@)
