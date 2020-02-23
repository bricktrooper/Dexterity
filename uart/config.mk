# ============== MODULE CONFIG ============== #

MOD = uart
PRG = uart-test
EXE = uart.hex

DEP += init

MOD_HDR += uart.h

PRG_HDR += init.h
PRG_HDR += uart.h

PRG_OBJ += uart.p1
PRG_OBJ += uart-test.p1
