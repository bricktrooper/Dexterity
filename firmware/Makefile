MODULES += init
MODULES += uart
MODULES += serial
MODULES += adc
MODULES += accel
MODULES += flex

.PHONY: $(MODULES)

all: $(MODULES)

init:
	@make init.p1 -C init
uart:
	@make uart.p1 -C uart
serial:
	@make serial.p1 -C serial
adc:
	@make adc.p1 -C adc
accel:
	@make accel.p1 -C accel
flex:
	@make flex.p1 -C flex

all-tests:
	@make -C init
	@make -C uart
	@make -C serial
	@make -C adc
	@make -C accel
	@make -C flex

init-test:
	@make -C init
uart-test:
	@make -C uart
serial-test:
	@make -C serial
adc-test:
	@make -C adc
accel-test:
	@make -C accel
flex-test:
	@make -C flex

clean:
	@make clean -C init
	@make clean -C uart
	@make clean -C serial
	@make clean -C adc
	@make clean -C accel
	@make clean -C flex
