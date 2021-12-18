
# Caller must set
#  DC_CMD = Design Compiler command 
#
# Available targets:
#    build - synthesis
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean

ifeq "$(SYNOPSYS)" ""
   $(error The SYNOPSYS environment variable must be set to use this makefile)
endif

export SYNOPSYS

ifeq "$(RTL_NETLIST_FNAME)" ""
   $(error The RTL_NETLIST_FNAME environment variable must be set to use this makefile)
endif

export RTL_NETLIST_FNAME

ifeq "$(GATE_FNAME_BASE)" ""
   $(error The GATE_FNAME_BASE environment variable must be set to use this makefile)
endif

export GATE_FNAME_BASE

ifeq "$(TCLSH_CMD)" ""
   $(error The TCLSH_CMD environment variable must be set to use this makefile)
endif
export TCLSH_CMD

# include $(MGC_HOME)/shared/include/mkfiles/ccs_default_cmds.mk

# Must clear these variables for dc_shell to work properly
TCL_LIBRARY :=
export TCL_LIBRARY
TK_LIBRARY :=
export TK_LIBRARY

# Translate the flow option DesignCompiler/OutNetlistFormat into either "v" or "vhdl"
ifeq "$(DesignCompiler_OutNetlistFormat)" "verilog"
OutNetlistFormat2 = v
else
ifeq "$(DesignCompiler_OutNetlistFormat)" "vhdl"
OutNetlistFormat2 = vhdl
endif
endif

# DesignCompiler_OutNetlistName
# DesignCompiler_OutNetlistFormat
# DesignCompiler_SearchPath
ifeq "$(DesignCompiler_ShellType)" "dctcl"
DC_SHELL_OPT = -tcl_mode
else
ifeq "$(DesignCompiler_ShellType)" "topo"
DC_SHELL_OPT = -topographical
else
$(error Unsupported DC shell mode $(DesignCompiler_ShellType))
endif
endif

ifneq "$(DesignCompiler_Path)" ""
DC_CMD = $(DesignCompiler_Path)/$(DesignCompiler_ShellExe) $(DC_SHELL_OPT)
else
DC_CMD = $(DesignCompiler_ShellExe) $(DC_SHELL_OPT)
endif

# Build gate netlist dependency file based on user's output netlist option
ifeq "$(OutNetlistFormat2)" "vhdl"
GATE_VHDL_DEP = $(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)/$(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)ts
endif
ifeq "$(OutNetlistFormat2)" "v"
GATE_VLOG_DEP = $(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)/$(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2)ts
endif

RTL_SYN_SCRIPT = $(RTL_NETLIST_FNAME).dc
DDC_FNAME = $(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).ddc

# Variables for proper SDF backannotation
VNDR_SDFINST := scverify_top/rtl
export VNDR_SDFINST
ifeq "$(SDF_FNAME)" ""
   SDF_FNAME := $(GATE_FNAME_BASE).sdf
endif
export SDF_FNAME

# Path to TCL script to parse DC output log
RTL_FLOW_FNAME = $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_dc.flo

# TRANSLATION RULES

# Synthesize RTL to DDC (typically first rule executed by 'build')
$(SYN_DIR)/$(DDC_FNAME): $(RTL_SYN_SCRIPT) $(RTL_NETLIST_FNAME)
	@-$(ECHO) "============================================"
	@-$(ECHO) "Synthesizing $(SYN_DIR)/$(DDC_FNAME), logging to $(RTL_SYN_SCRIPT).log"
	$(DC_CMD) -f $< 1> $(RTL_SYN_SCRIPT).log

# Translate DDC to Verilog HDL (typically second rule executed by 'build' for Verilog gate netlist output)
$(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).v : $(SYN_DIR)/$(DDC_FNAME)
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating output netlist $@"
	-$(RM) $(SYN_DIR)/tmp.tcl
	@-$(ECHO) "# TCL script to convert DDC file to vhdl" >$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "cd $(SYN_DIR)" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "set search_path \"\$$search_path $(DesignCompiler_SearchPath)\"" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "read_ddc $(DDC_FNAME)" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "change_names -hierarchy -rules verilog" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "write -hierarchy -format verilog -output ../$@" >>$(SYN_DIR)/tmp.tcl
	$(DC_CMD) -f $(SYN_DIR)/tmp.tcl

# Translate DDC to VHDL (typically second rule executed by 'build' for VHDL gate netlist output)
$(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).vhdl : $(SYN_DIR)/$(DDC_FNAME)
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating output netlist $@"
	-$(RM) $(SYN_DIR)/tmp.tcl
	@-$(ECHO) "# TCL script to convert DDC file to vhdl" >$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "cd $(SYN_DIR)" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "set search_path \"\$$search_path $(DesignCompiler_SearchPath)\"" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "read_ddc $(DDC_FNAME)" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "change_names -hierarchy -rules vhdl" >>$(SYN_DIR)/tmp.tcl
	@-$(ECHO) "write -hierarchy -format vhdl -output ../$@" >>$(SYN_DIR)/tmp.tcl
	$(DC_CMD) -f $(SYN_DIR)/tmp.tcl

# Create clock_skew file by parsing DC synthesis log (typically third rule executed by 'build')
$(HLD_CONSTRAINT_FNAME): $(RTL_SYN_SCRIPT).log
	@-$(ECHO) "============================================"
	@-$(ECHO) "Parsing $< to produce clock skew file $@"
	$(TCLSH_CMD) $(RTL_FLOW_FNAME) SA_parse_area_delay $(RTL_SYN_SCRIPT) $(RTL_SYN_SCRIPT).log > $(RTL_SYN_SCRIPT).log.parsing.log

ifneq "$(RTLTOOL)" ""
ifeq "$(STAGE)" "gate"
.PHONY: build
build: $(DesignCompiler_OutNetlistName)$(suffix $(RTL_NETLIST_FNAME)).$(OutNetlistFormat2) $(HLD_CONSTRAINT_FNAME)
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
	@-$(ECHO) "DesignCompiler Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   build    Run synthesis and compile the gate netlist"
	@-$(ECHO) "   sim      Run synthesis, compile the gate netlist and simulate"
	@-$(ECHO) "   simgui   Run synthesis, compile the gate netlist and simulate interactively"
	@-$(ECHO) "   clean    Remove the simulation results"
	@-$(ECHO) "   help     Print this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variable settings are:"

dumpvars:
	@-$(ECHO) "   DesignCompiler flow options:"
	@-$(ECHO) "     SearchPath            = $(DesignCompiler_SearchPath)"
	@-$(ECHO) "     OutNetlistName        = $(DesignCompiler_OutNetlistName)"
	@-$(ECHO) "     OutNetlistFormat      = $(DesignCompiler_OutNetlistFormat)"
	@-$(ECHO) "   Catapult RTL output:"
	@-$(ECHO) "     RTL_SYN_SCRIPT        = $(RTL_SYN_SCRIPT)"
	@-$(ECHO) "     RTL_NETLIST_FNAME     = $(RTL_NETLIST_FNAME)"
	@-$(MAKE) --no-print-directory RECUR=1 RTLTOOL= -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk dumpvars

