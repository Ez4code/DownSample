
# Caller must set
#  RTL_SCRIPT = name of Precision RTL TCL command script to generate the RTL synthesis netlist
#
# Available targets:
#    build - synthesis
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean

ifeq "$(Precision_rtlplus)" "true"
PSR_OPTS = -rtlplus
else
PSR_OPTS =
endif
ifeq "$(Precision_Path)" ""
PRECISION_EXE = precision
else
PRECISION_EXE = $(subst /,$(PATHSEP),$(Precision_Path))$(PATHSEP)precision
endif
export PRECISION_EXE

# Move from netlist/simulator-specific TARGET directory up to 'scverify' directory
SCVERIFY_DIR := $(dir $(TARGET))
# Expand out source file timestamp names
PSRVHDL_SRC2 := $(patsubst %.psrvhdl,%.vhdl,$(filter-out %.psrv,$(notdir $(RTL_SCRIPT))))
TMP_PSRVHDL_SRC := $(foreach hdlfile,$(PSRVHDL_SRC2),$(SCVERIFY_DIR)$(hdlfile))
PSRV_SRC2 := $(patsubst %.psrv,%.v,$(filter-out %.psrvhdl,$(notdir $(RTL_SCRIPT))))
TMP_PSRV_SRC := $(foreach hdlfile,$(PSRV_SRC2),$(SCVERIFY_DIR)$(hdlfile))

ifeq "$(NETLIST)" "vhdl"
GATE_VHDL_DEP=$(STAGE).$(NETLIST)/$(STAGE).$(NETLIST).$(NETLIST)ts
else
ifeq "$(NETLIST)" "v"
GATE_VLOG_DEP=$(STAGE).$(NETLIST)/$(STAGE).$(NETLIST).$(NETLIST)ts
endif
endif

RTL_SYN_SCRIPT = $(RTL_NETLIST_FNAME).psr
SYN_DIR := psr_$(NETLIST)_impl

# Path to TCL script to parse Precision output log
RTL_FLOW_FNAME = $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_psr.flo

# Translate rule to run PrecisionRTL synthesis
$(SYN_DIR)/$(TOP_HDL_ENTITY).xdb: $(RTL_SYN_SCRIPT) $(RTL_NETLIST_FNAME)
	-@echo "============================================"
	-@echo "Launching Precision RTL to produce synthesized design database '$@' using script '$<'"
ifeq "$(findstring CYGWIN,$(shell uname))" "CYGWIN"
	$(PRECISION_EXE) -shell $(PSR_OPTS) -file $(RTL_SYN_SCRIPT) -fileargs -run_state\ $(STAGE) | $(TEE) $(RTL_SYN_SCRIPT).log
else
	$(PRECISION_EXE) -shell $(PSR_OPTS) -file $(RTL_SYN_SCRIPT) -fileargs "-run_state $(STAGE)" | $(TEE) $(RTL_SYN_SCRIPT).log
endif

# Translate rule to generate mapped or gate netlist
$(STAGE).$(NETLIST) : $(SYN_DIR)/$(TOP_HDL_ENTITY).xdb
	-@echo "============================================"
	-@echo "Launching Precision RTL to produce netlist '$@' using saved implementation database '$<'"
ifeq "$(findstring CYGWIN,$(shell uname))" "CYGWIN"
	$(PRECISION_EXE) -shell $(PSR_OPTS) -file $(RTL_SYN_SCRIPT) -fileargs -run_state\ $(STAGE) | $(TEE) $(RTL_SYN_SCRIPT).hdlgen.log
else
	$(PRECISION_EXE) -shell $(PSR_OPTS) -file $(RTL_SYN_SCRIPT) -fileargs "-run_state $(STAGE)" | $(TEE) $(RTL_SYN_SCRIPT).hdlgen.log
endif

# Create clock_skew file by parsing Precision synthesis log (typically third rule executed by 'build')
$(HLD_CONSTRAINT_FNAME): $(RTL_SYN_SCRIPT).log
	@-$(ECHO) "============================================"
	@-$(ECHO) "Parsing $< to produce clock skew file $@"
	$(subst /,$(PATHSEP),$(TCLSH_CMD)) $(RTL_FLOW_FNAME) SA_parse_area_delay $(RTL_SYN_SCRIPT) $(RTL_SYN_SCRIPT).log > $(RTL_SYN_SCRIPT).log.parsing.log

# Create the target directory
$(TARGET)/make_dir : 
	-@echo "============================================"
	-@echo "Creating simulation directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

ifneq "$(RTLTOOL)" ""
# recursively reinvoke Make to perform the final compilation and simulation
build: $(STAGE).$(NETLIST) $(HLD_CONSTRAINT_FNAME)
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk build

sim: build
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk sim

simgui: build
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk simgui

clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

cleanpsr:
	$(RMDIR) psr_$(NETLIST)_impl psr_$(NETLIST)_temp_1 $(SCVERIFY_DIR)mapped.$(NETLIST) psr_$(NETLIST).psp

endif

help: helptext dumpsysvars dumpvars

helptext:
	@-$(ECHO) "Precision Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   build    Run synthesis and compile the gate netlist"
	@-$(ECHO) "   sim      Run synthesis, compile the gate netlist and simulate"
	@-$(ECHO) "   simgui   Run synthesis, compile the gate netlist and simulate interactively"
	@-$(ECHO) "   clean    Remove the simulation results"
	@-$(ECHO) "   help     Print this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variable settings are:"

dumpvars:
	@-$(ECHO) "   Precision flow options:"
	@-$(ECHO) "     Path                  = $(Precision_Path)"
	@-$(ECHO) "     rtlplus               = $(Precision_rtlplus)"
	@-$(ECHO) "   Catapult RTL output:"
	@-$(ECHO) "     RTL_SYN_SCRIPT        = $(RTL_SYN_SCRIPT)"
	@-$(ECHO) "     RTL_NETLIST_FNAME     = $(RTL_NETLIST_FNAME)"
	@-$(MAKE) --no-print-directory RECUR=1 RTLTOOL= -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk dumpvars

