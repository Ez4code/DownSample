
# Caller must set
#
# Available targets:

QUARTUS_ROOTDIR = $(PowerPlay_QUARTUS_ROOTDIR)


ifeq "$(QUARTUS_ROOTDIR)" ""
   $(error The QUARTUS_ROOTDIR environment variable must be set to use this makefile)
endif

include $(MGC_HOME)/shared/include/mkfiles/ccs_Altera.mk

export QUARTUS_ROOTDIR
export INVOKE_ARGS
STAGE = gate
export STAGE

# The variable QUARTUS_PLAT is calculated in the ccs_Altera.mk makefile
PWRPLAY_EXEC = $(QUARTUS_ROOTDIR)/bin/quartus_pow

# Create the target directory
$(TARGET)/make_dir : 
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

# Make the VCD file using the SCVerify flow makefile for the STAGE=gate, SIMTOOL=msim, NETLIST=vhdl|v
$(TARGET)/$(STAGE)_$(NETLIST).vcd: $(TARGET)/make_dir scverify/Verify_$(STAGE)_$(NETLIST)_msim.mk
	$(MAKE) -f scverify/Verify_$(STAGE)_$(NETLIST)_msim.mk RTLTOOL=psr CCS_VCD_FILE=$(SOLNDIR)$(PATHSEP)$(TARGET)/$(STAGE)_$(NETLIST).vcd sim

# would prefer the report to be written to $(TARGET)/powerplay_$(NETLIST).pow.rpt
psr_$(NETLIST)_impl/$(DESIGNNAME).pow.rpt: $(TARGET)/$(STAGE)_$(NETLIST).vcd psr_$(NETLIST)_impl/$(DESIGNNAME).qsf
	$(PWRPLAY_EXEC) --input_vcd=../$(TARGET)/$(STAGE)_$(NETLIST).vcd psr_$(NETLIST)_impl/$(DESIGNNAME).qsf

.PHONY: build
build: psr_$(NETLIST)_impl/$(DESIGNNAME).pow.rpt

.PHONY: help
help:
	-@echo "PowerPlay Makefile"
	-@echo "The valid targets are:"
	-@echo "   build      Run synthesis, simulation and power analysis"
	-@echo "   clean      Remove all derived objects"
	-@echo "   help       Show this help text"
	-@echo ""
	-@echo "The current variables settings are:"
	-@echo "QUARTUS_ROOTDIR        = $(QUARTUS_ROOTDIR)"
	-@echo "PWRPLAY_EXEC           = $(PWRPLAY_EXEC)"
	-@echo "TARGET                 = $(TARGET)"
	-@echo ""
	-@echo "SCVerify Makefile used = scverify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk"
	$(MAKE) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk help

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

