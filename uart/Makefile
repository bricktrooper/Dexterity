CC = xc8-cc
DEVICE = 16F690
CFLAGS = -mcpu=$(DEVICE) -mc90lib

OBJ = test init
OUT = uart

OUT_DIR = out

object_files = $(addprefix $(OUT_DIR)/,$(addsuffix .p1,$(1)))

program : $(OBJ)
	$(CC) $(CFLAGS) $(call object_files,$(OBJ)) -o $(OUT_DIR)/$(OUT)

clean :
	rm -rf $(OUT_DIR)/*

init : init.c
	$(CC) $(CFLAGS) -c init.c -o $(call object_files,init)

test : test.c
	$(CC) $(CFLAGS) -c test.c -o $(call object_files,test)

# uart : uart.c uart.h
# 	$(CC) $(CFLAGS) -c uart.c -o $(call object_files,uart)
