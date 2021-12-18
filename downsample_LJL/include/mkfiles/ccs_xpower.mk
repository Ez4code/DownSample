
# Caller must set
#
# Available targets:

XILINX        = $(Xilinx_XILINX)
XPWR_OPTS     = $(XPower_XPWR_OPTS)

ifeq "$(XILINX)" ""
   $(error The XILINX environment variable must be set to use this makefile)
endif

include $(MGC_HOME)/shared/include/mkfiles/ccs_Xilinx.mk

export XILINX
export INVOKE_ARGS
STAGE = gate
export STAGE

# The variable XILINX_PLAT is calculated in the ccs_Xilinx.mk makefile
XPWR_EXEC = $(XILINX)/bin/$(XILINX_PLAT)/xpwr

# Xilinx XPower does not support fsdb
USE_FSDB :=

# Name the VCD file
VCD_NAME = $(STAGE)_$(NETLIST)
VCD_FILE = $(VCD_NAME).vcd

# Modify SUFFIX then prefix TARGET (preserve order of files)
TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(dir $(hdlfile)))
TMP_VLOG_SRC := $(foreach hdlfile,$(VTS_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_VHDL_SRC := $(foreach hdlfile,$(VHDLTS_SRC),$(TARGET)/$(notdir $(hdlfile)))
NCD_FILE := psr_$(NETLIST)_impl/$(DESIGNNAME)_out.ncd
PCF_FILE := psr_$(NETLIST)_impl/$(DESIGNNAME).pcf
RPT_FILE := xpower_$(NETLIST).pwr

# Make the VCD file using the SCVerify flow makefile for the STAGE=gate, SIMTOOL=msim, NETLIST=vhdl|v
$(VCD_FILE): $(VHDL_SRC) $(VLOG_SRC) $(TMP_CXX_SRC) .ccs_env_opts/SCVerify_INVOKE_ARGS.ts
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating VCD file using simulation tool $(SIMTOOL)"
	$(MAKE) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk RTLTOOL=psr USE_FSDB=$(USE_FSDB) CCS_VCD_FILE=$(SOLNDIR)$(PATHSEP)$(VCD_FILE) sim

# Create the target directory
$(TARGET)/make_dir : 
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

# No HDL files need "compiling" by XPower

# Run power analysis now
$(TARGET)/$(RPT_FILE): $(TARGET)/make_dir $(VCD_FILE) $(NCD_FILE) $(PCF_FILE)
	@-$(ECHO) "============================================"
	-@$(ECHO) "Running Xilinx XPower using NCD file '$(NCD_FILE)' and PCF file '$(PCF_FILE)'"
	$(XPWR_EXEC) $(NCD_FILE) $(PCF_FILE) -o $@ -s $(VCD_FILE) $(XPWR_OPTS)

.PHONY: build
build: $(TARGET)/$(RPT_FILE)

.PHONY: help
help: helptext dumpsysvars dumpvars
ifneq "$(SIMTOOL)" ""
	@-$(ECHO) "SCVerify Makefile used = scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk"
	@-$(MAKE) --no-print-directory -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk USE_FSDB=$(USE_FSDB) dumpvars
endif

helptext:
	@-$(ECHO) "Xilinx XPower Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   build      Run synthesis, simulation and power analysis"
	@-$(ECHO) "   clean      Remove all derived objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

dumpvars:
	@-$(ECHO) "   Xilinx XPower flow options:"
	@-$(ECHO) "     XILINX                = $(XPower_XILINX)"
	@-$(ECHO) "     XPWR_OPTS             = $(XPower_XPWR_OPTS)"
	@-$(ECHO) "     XPWR_EXEC             = $(XPWR_EXEC)"
	@-$(ECHO) "     TARGET                = $(TARGET)"
	@-$(ECHO) "     RTLTOOL               = $(RTLTOOL)"
	@-$(ECHO) "     SIMTOOL               = $(SIMTOOL)"
	@-$(ECHO) "     NETLIST               = $(NETLIST)"
	@-$(ECHO) "     GATE_NETLIST_FORMAT   = $(GATE_NETLIST_FORMAT)"
	@-$(ECHO) "     RPT_FILE              = $(TARGET)/$(RPT_FILE)"
	@-$(ECHO) "     VCD_FILE              = $(VCD_FILE)"
	@-$(ECHO) ""

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

