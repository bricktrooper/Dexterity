MAKE = /usr/bin/make

FIRMWARE = firmware.hex
SOFTWARE = software.bin

ART = artifacts

$(shell mkdir -p $(ART))

.PHONY: dexterity
dexterity: firmware software

.PHONY: firmware
firmware:
	@echo Building firmware.......
	@$(MAKE) -C firmware
	@cp firmware/bin/dexterity.hex $(ART)/$(FIRMWARE)

.PHONY: software
software:
	@echo Building software.......
	@$(MAKE) -C software
	@cp software/bin/dexterity $(ART)/$(SOFTWARE)

clean:
	@$(MAKE) clean -C firmware
	@$(MAKE) clean -C software
	@for ARTIFACT in $(notdir $(wildcard $(ART)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
