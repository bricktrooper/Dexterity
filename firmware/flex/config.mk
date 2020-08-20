# ============== MODULE CONFIG ============== #

MOD = flex
PRG = flex-test
EXE = flex.hex

DEP += init
DEP += uart
DEP += serial
DEP += adc

MOD_HDR += adc.h
MOD_HDR += flex.h

PRG_HDR += init.h
PRG_HDR += uart.h
PRG_HDR += serial.h
PRG_HDR += adc.h
PRG_HDR += flex.h

PRG_OBJ += flex.p1
PRG_OBJ += flex-test.p1
