# ============== MODULE CONFIG ============== #

MOD = serial
PRG = serial-test
EXE = serial.hex

DEP += init
DEP += uart

MOD_HDR += uart.h
MOD_HDR += serial.h

PRG_HDR += init.h
PRG_HDR += uart.h
PRG_HDR += serial.h

PRG_OBJ += serial.p1
PRG_OBJ += serial-test.p1
