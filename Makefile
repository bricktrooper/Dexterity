MODULES += init
MODULES += uart
MODULES += serial
MODULES += adc

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

all-tests:
	@make -C init
	@make -C uart
	@make -C serial
	@make -C adc

init-test:
	@make -C init
uart-test:
	@make -C uart
serial-test:
	@make -C serial
adc-test:
	@make -C adc

clean:
	@make clean -C init
	@make clean -C uart
	@make clean -C serial
	@make clean -C adc
