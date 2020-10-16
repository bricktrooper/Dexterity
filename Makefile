MAKE = /usr/bin/make

FIRMWARE = firmware
SOFTWARE = software

ART = bin

$(shell mkdir -p $(ART))

.PHONY: dexterity
dexterity: firmware software artifacts

.PHONY: firmware
firmware:
	@echo
	@echo "-------------------< FIRMWARE >-------------------"
	@$(MAKE) -C firmware
	@echo "--------------------------------------------------"

.PHONY: software
software:
	@echo
	@echo "-------------------< SOFTWARE >-------------------"
	@$(MAKE) -C software
	@echo "--------------------------------------------------"

.PHONY: artifacts
artifacts:
	@echo
	@echo "-------------------< ARTIFACTS >------------------"
	@echo "BIN  $(FIRMWARE)"
	@cp firmware/bin/dexterity.hex $(ART)/$(FIRMWARE)
	@echo "BIN  $(SOFTWARE)"
	@cp software/bin/dexterity $(ART)/$(SOFTWARE)
	@echo "--------------------------------------------------"

clean:
	@$(MAKE) clean -C firmware
	@$(MAKE) clean -C software
	@for ARTIFACT in $(notdir $(wildcard $(ART)/*)); do echo "RM   $$ARTIFACT"; done
	@rm -rf $(ART)
