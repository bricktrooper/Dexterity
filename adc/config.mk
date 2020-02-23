# ============== MODULE CONFIG ============== #

MOD = adc
PRG = adc-test
EXE = adc.hex

DEP += init
DEP += uart
DEP += serial

MOD_HDR += adc.h

PRG_HDR += init.h
PRG_HDR += uart.h
PRG_HDR += serial.h
PRG_HDR += adc.h

PRG_OBJ += adc.p1
PRG_OBJ += adc-test.p1
