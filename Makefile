MAKE = /usr/bin/make

FIRMWARE = firmware
SOFTWARE = software

OUT = bin

ART += $(OUT)

$(shell mkdir -p $(OUT))

.PHONY: dexterity
dexterity: firmware software

.PHONY: firmware
firmware:
	@$(MAKE) -C firmware
	@echo "BIN  $(FIRMWARE)"
	@cp firmware/bin/dexterity.hex $(OUT)/$(FIRMWARE)

.PHONY: software
software:
	@$(MAKE) -C software
	@echo "BIN  $(SOFTWARE)"
	@cp software/bin/dexterity $(OUT)/$(SOFTWARE)

clean:
	@$(MAKE) clean -C firmware
	@$(MAKE) clean -C software
	@for ARTIFACT in $(notdir $(wildcard $(OUT)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
