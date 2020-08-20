# ============== MODULE CONFIG ============== #

MOD = accel
PRG = accel-test
EXE = accel.hex

DEP += init
DEP += uart
DEP += serial
DEP += adc

MOD_HDR += adc.h
MOD_HDR += accel.h

PRG_HDR += init.h
PRG_HDR += uart.h
PRG_HDR += serial.h
PRG_HDR += adc.h
PRG_HDR += accel.h

PRG_OBJ += accel.p1
PRG_OBJ += accel-test.p1
