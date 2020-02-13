MODULES += init
MODULES += uart
MODULES += serial

.PHONY: $(MODULES)

all: $(MODULES)

init:
	@make init.p1 -C init
uart:
	@make uart.p1 -C uart
serial:
	@make serial.p1 -C serial

tests:
	@make -C init
	@make -C uart
	@make -C serial

init-test:
	@make -C init
uart-test:
	@make -C uart
serial-test:
	@make -C serial

clean:
	@make clean -C init
	@make clean -C uart
	@make clean -C serial
