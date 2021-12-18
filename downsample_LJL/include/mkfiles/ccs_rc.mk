
# Caller must set
#  RC_SRC = name of RTL Compiler command
#
# Available targets:
#    build - synthesis
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean


export RC_CMD

ifeq "$(SYN_DIR)" ""
   $(error The SYN_DIR environment variable must be set to use this makefile)
endif
export SYN_DIR

ifeq "$(RTL_NETLIST_FNAME)" ""
   $(error The RTL_NETLIST_FNAME environment variable must be set to use this makefile)
endif
export RTL_NETLIST_FNAME

ifeq "$(GATE_FNAME_BASE)" ""
   $(error The GATE_FNAME_BASE environment variable must be set to use this makefile)
endif
export GATE_FNAME_BASE

# include $(MGC_HOME)/shared/include/mkfiles/ccs_default_cmds.mk

# Translate the flow option RTLCompiler/OutNetlistFormat into either "v" or "vhdl"
ifeq "$(RTLCompiler_OutNetlistFormat)" "verilog"
OutNetlistFormat2 = v
else
ifeq "$(RTLCompiler_OutNetlistFormat)" "vhdl"
OutNetlistFormat2 = vhdl
endif
endif

ifneq "$(RTLCompiler_Path)" ""
RC_CMD = $(RTLCompiler_Path)/$(RTLCompiler_ShellExe)
else
RC_CMD = $(RTLCompiler_ShellExe)
endif

# Build gate netlist dependency file based on user's output netlist option
ifeq "$(OutNetlistFormat2)" "vhdl"
GATE_VHDL_DEP = $(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)/$(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)ts
endif
ifeq "$(OutNetlistFormat2)" "v"
GATE_VLOG_DEP = $(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)/$(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)ts
endif

RTL_SYN_SCRIPT = $(RTL_NETLIST_FNAME).$(RTLTOOL)

VNDR_SDFINST := scverify_top/rtl
export VNDR_SDFINST
SDF_FNAME := $(GATE_FNAME_BASE).sdf
export SDF_FNAME

# Path to TCL script to parse RC output log
RTL_FLOW_FNAME = $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_$(RTLTOOL).flo

# TRANSLATION RULES

# Synthesize RTL to HDL gate netlist
$(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2) : $(RTL_SYN_SCRIPT) $(RTL_NETLIST_FNAME)
	@-$(ECHO) "============================================"
	@-$(ECHO) "Synthesizing $@, logging to $(RTL_SYN_SCRIPT).log"
	$(RC_CMD) -f $< 1> $(RTL_SYN_SCRIPT).log

$(RTL_SYN_SCRIPT).log: $(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2) 

# Create clock_skew file by parsing DC synthesis log (typically third rule executed by 'build')
$(HLD_CONSTRAINT_FNAME): $(RTL_SYN_SCRIPT).log
	@-$(ECHO) "============================================"
	@-$(ECHO) "Parsing $< to produce clock skew file $@"
	$(TCLSH_CMD) $(RTL_FLOW_FNAME) SA_parse_area_delay $(RTL_SYN_SCRIPT) $(RTL_SYN_SCRIPT).log > $(RTL_SYN_SCRIPT).log.parsing.log


#$(GATE_FNAME): $(RTL_NETLIST_FNAME).$(RTLTOOL) $(RTL_NETLIST_FNAME)
#	@-$(ECHO) "============================================"
#	@-$(ECHO) "Synthesizing $@ with $<"
#	$(RC_CMD) -f $< 1> $<.log
#	cd $(SYN_DIR) && $(RC_CMD) -f ../$< 1> ../$<.log


ifneq "$(RTLTOOL)" ""
ifeq "$(STAGE)" "gate"
.PHONY: build
build: $(RTLCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2) $(HLD_CONSTRAINT_FNAME)
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk build

clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

sim: build
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk sim

simgui: build
	$(MAKE) RECUR=1 RTLTOOL= GATE_VHDL_DEP=$(GATE_VHDL_DEP) GATE_VLOG_DEP=$(GATE_VLOG_DEP) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk simgui

endif
endif

help: helptext dumpsysvars dumpvars

helptext:
	@-$(ECHO) "RTLCompiler Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   build    Run synthesis and compile the gate netlist"
	@-$(ECHO) "   sim      Run synthesis, compile the gate netlist and simulate"
	@-$(ECHO) "   simgui   Run synthesis, compile the gate netlist and simulate interactively"
	@-$(ECHO) "   clean    Remove the simulation results"
	@-$(ECHO) "   help     Print this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variable settings are:"

dumpvars:
	@-$(ECHO) "   RTLCompiler flow options:"
	@-$(ECHO) "     SearchPath            = $(RTLCompiler_SearchPath)"
	@-$(ECHO) "     OutNetlistName        = $(RTLCompiler_OutNetlistName)"
	@-$(ECHO) "     OutNetlistFormat      = $(RTLCompiler_OutNetlistFormat)"
	@-$(ECHO) "   Catapult RTL output:"
	@-$(ECHO) "     RTL_SYN_SCRIPT        = $(RTL_SYN_SCRIPT)"
	@-$(ECHO) "     RTL_NETLIST_FNAME     = $(RTL_NETLIST_FNAME)"
	@-$(MAKE) --no-print-directory RECUR=1 RTLTOOL= -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk dumpvars

