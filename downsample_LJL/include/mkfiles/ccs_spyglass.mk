
# Caller must set
#
# Available targets:

SPYGLASS_HOME = $(SpyGlassPower_Path)

ifeq "$(SPYGLASS_HOME)" ""
SPYGLASS_HOME = $(SpyGlassLint_Path)
endif

ifeq "$(SPYGLASS_HOME)" ""
   $(error The SPYGLASS_HOME environment variable must be set to use this makefile)
endif

ifeq "$(STAGE)" "gate"
ifeq "$(RTLTOOL)" ""
   $(error The RTLTOOL environment variable must be set to use this makefile)
endif
export RTLTOOL
endif

ifeq "$(LIB_FILES)" ""
ifneq "$(SIMTOOL)" ""
   $(error The LIB_FILES variable is empty indicating that no synthesis .lib files were found)
endif
endif

export TCLSH_CMD
export GATE_FNAME_BASE
export RTL_NETLIST_FNAME


ifeq "$(LINTMODE)" ""
 # Makefile is being used for Power Analysis
 TSFILES :=
 SG_OPTS += $(SpyGlassPower_SG_POWER_OPTS)
 POLICIES := -policies='$(SpyGlassPower_SG_POLICIES)'
 SG_RULES := $(SpyGlassPower_SG_RULES)
 ifeq "$(STAGE)" "gate"
  SG_RULES += ,$(SpyGlassPower_SG_GATE_RULES)
 endif
 RULES := -rules '$(SG_RULES)'
else
 # Makefile is being used for Linting
 TSFILES := .ccs_env_opts/SpyGlassLint_SG_RULES.ts .ccs_env_opts/SpyGlassLint_SG_POLICIES.ts .ccs_env_opts/SpyGlassLint_SG_OPTS.ts
 SG_OPTS := $(SpyGlassLint_SG_OPTS) -hdllibdu
 ifeq "$(SpyGlassLint_SG_POLICIES)" ""
 POLICIES := -policies='lint'
 else
 POLICIES := -policies='$(SpyGlassLint_SG_POLICIES)'
 endif
 ifeq "$(SpyGlassLint_SG_RULES)" ""
 RULES := ""
 else
 RULES := -rules '$(SpyGlassLint_SG_RULES)'
 endif
endif

export SPYGLASS_HOME
export INVOKE_ARGS

SPYGLASS = $(SPYGLASS_HOME)$(PATHSEP)bin$(PATHSEP)spyglass
SPYGLASS_LC = $(SPYGLASS_HOME)$(PATHSEP)bin$(PATHSEP)spyglass_lc

VCD_NAME = $(STAGE)_$(NETLIST)

ifneq "$(SIMTOOL)" ""
# Pass actual VCD filename to SpyGlassPower as env var
# (SpyGlass Power doesn't accept compressed VCD)
ifneq "$(USE_FSDB)" ""
VCD_FILE = $(VCD_NAME).fsdb
else
VCD_FILE = $(VCD_NAME).vcd
ifeq "$(SIMTOOL)" "msim"
ifneq "$(ALLOW_VCD_COMPRESSION)" ""
VCD_FILE = $(VCD_NAME).vcd.gz
endif
endif
endif
else
VCD_FILE =
endif
export VCD_FILE

# Modify SUFFIX then prefix TARGET (preserve order of files)
TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(dir $(hdlfile)))
TMP_VLOG_SRC := $(foreach hdlfile,$(VTS_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_VHDL_SRC := $(foreach hdlfile,$(VHDLTS_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_LIB_FILES := $(foreach lib,$(LIB_FILES),$(TARGET)/$(basename $(notdir $(lib))).sglib)
TMP_LIB_FILES_ARG := $(foreach lib,$(LIB_FILES),-sglib $(TARGET)/$(basename $(notdir $(lib))).sglib)


# Make the VCD file using the SCVerify flow makefile for the STAGE=rtl, SIMTOOL=msim, NETLIST=vhdl|v
$(VCD_FILE): $(VHDL_SRC) $(VLOG_SRC) $(TMP_CXX_SRC) .ccs_env_opts/SCVerify_INVOKE_ARGS.ts
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating VCD file using simulation tool $(SIMTOOL)"
	$(MAKE) -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk USE_FSDB=$(USE_FSDB) CCS_VCD_FILE=$(SOLNDIR)$(PATHSEP)$(VCD_FILE) sim

# Translate rule to create Atrenta compiled library from the technology library
$(TARGET)/%.sglib:
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating Atrenta compiled library $@ for technology library: $(notdir $<)"
	$(SPYGLASS_LC) $(SpyGlassPower_SG_TECHLIB_OPTS) -wdir=$(TARGET) -gateslib $< -outsglib $@ >>$(TARGET)/gateslib.log

# Create the target directory
$(TARGET)/make_dir:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

# Translate rule to compile VHDL with Atrenta
$(TARGET)/%.vhdlts :
	-@$(ECHO) "============================================"
	-@$(ECHO) "Compiling VHDL file: $<"
	$(SPYGLASS) $(SpyGlassPower_SG_COMP_OPTS) -vhdl -batch -policy=$(POLICY) -hdlin_translate_off_skip_text -wdir=$(TARGET) $(TMP_LIB_FILES_ARG) -work $(WORK_LIB) $(LIB_MAPS) $(ELAB_OPT) $<
	$(TIMESTAMP)

# Translate rule to compile Verilog with Atrenta
$(TARGET)/%.vts :
	-@$(ECHO) "============================================"
	-@$(ECHO) "Compiling Verilog file: $<"
	$(SPYGLASS) $(SpyGlassPower_SG_COMP_OPTS) -mixed -batch -policy=$(POLICY) -wdir=$(TARGET) $(TMP_LIB_FILES_ARG) -work $(WORK_LIB) $(LIB_MAPS) $(ELAB_OPT) $(SpyGlassPower_SG_COMP_F_OPTS) $< -enable_precompile_vlog
	$(TIMESTAMP)

%.$(GATE_NETLIST_FORMAT): $(RTL_NETLIST_FNAME)
ifeq "$(STAGE)" "gate"
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating gate netlist $@"
	$(MAKE) -f $(MGC_HOME)/shared/include/mkfiles/ccs_$(RTLTOOL).mk NETLIST=$(NETLIST) RTL_NETLIST_FNAME=$(RTL_NETLIST_FNAME) $@
endif

$(HLD_CONSTRAINT_FNAME):
ifeq "$(STAGE)" "gate"
	$(MAKE) -f $(MGC_HOME)/shared/include/mkfiles/ccs_$(RTLTOOL).mk NETLIST=$(NETLIST) RTL_NETLIST_FNAME=$(RTL_NETLIST_FNAME) $@
endif

$(TARGET)/$(RPT_FILE): $(TARGET)/make_dir $(VCD_FILE) $(TMP_LIB_FILES) $(TMP_VHDL_SRC) $(TMP_VLOG_SRC) $(TSFILES)
ifneq "$(LINTMODE)" ""
	-@$(ECHO) "============================================"
	-@$(ECHO) "Linting rules in effect:"
	-@$(SPYGLASS)  -batch -mixed -wdir $(TARGET) $(POLICIES) $(RULES) $(SG_OPTS) -printrules
endif
	-@$(ECHO) "============================================"
	-@$(ECHO) "Running Atrenta SpyGlass $(SG_POLICIES) Analysis"
	$(SPYGLASS)  -batch -mixed -wdir $(TARGET) $(POLICIES) $(RULES) $(SG_OPTS) $(LIB_MAPS) $(TMP_LIB_FILES_ARG) -top $(TOP_DU)
ifneq "$(LINTMODE)" ""
	-@$(ECHO) "============================================"
	-@$(ECHO) "Contents of report '$(TARGET)$(PATHSEP)spyglass_reports$(PATHSEP)moresimple.rpt':"
	-@$(CAT) $(TARGET)$(PATHSEP)spyglass_reports$(PATHSEP)moresimple.rpt
endif
	-@touch $@

LINT_VLOG_SRC := $(foreach tsfile,$(VTS_SRC),$(abspath $(dir $(tsfile))))
LINT_VHDL_SRC := $(foreach tsfile,$(VHDLTS_SRC),$(abspath $(dir $(tsfile))))

lint: $(LINT_VLOG_SRC) $(LINT_VHDL_SRC)
	$(SpyGlassLint_Path)$(PATHSEP)bin$(PATHSEP)spyglass -batch $(SpyGlassLint_SG_OPTS) -lib $(TARGET) $(TARGET) -wdir $(TARGET) $(LINT_POLICIES) $(LINT_RULES) $(LINT_VLOG_SRC) $(LINT_VHDL_SRC) -top $(TOP_DU)

.PHONY: build
build: $(TARGET)/$(RPT_FILE)

buildgui: build
	$(SPYGLASS) -vdbfile $(TARGET)/spyglass.vdb

.PHONY: help
help: helptext dumpsysvars dumpvars
ifneq "$(SIMTOOL)" ""
	@-$(ECHO) "SCVerify Makefile used = scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk"
	@-$(MAKE) --no-print-directory -f scverify/Verify_$(STAGE)_$(NETLIST)_$(SIMTOOL).mk USE_FSDB=$(USE_FSDB) dumpvars
endif

helptext:
	@-$(ECHO) "Atrenta SpyGlassPower Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   build      Run synthesis, simulation and $(SG_POLICIES) analysis"
	@-$(ECHO) "   buildgui   Run synthesis, simulation and $(SG_POLICIES) analysis and view results in GUI"
	@-$(ECHO) "   clean      Remove all derived objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

dumpvars:
	@-$(ECHO) "   SpyGlassPower flow options:"
	@-$(ECHO) "     Path                  = $(SpyGlassPower_SPYGLASS_HOME)"
	@-$(ECHO) "     VCD_FORMAT            = $(SpyGlassPower_VCD_FORMAT)"
	@-$(ECHO) "     SG_TECHLIB_OPTS       = $(SpyGlassPower_SG_TECHLIB_OPTS)"
	@-$(ECHO) "     SG_COMP_OPTS          = $(SpyGlassPower_SG_COMP_OPTS)"
	@-$(ECHO) "     SG_POWER_OPTS         = $(SpyGlassPower_SG_POWER_OPTS)"
	@-$(ECHO) "     ALLOW_VCD_COMPRESSION = $(SpyGlassPower_ALLOW_VCD_COMPRESSION)"
	@-$(ECHO) "     wire_load_model       = $(SpyGlassPower_wire_load_model)"
	@-$(ECHO) "     wire_load_selection_group = $(SpyGlassPower_wire_load_selection_group)"
	@-$(ECHO) "     SG_POLICIES           = $(SG_POLICIES)"
	@-$(ECHO) "     SG_RULES              = $(SG_RULES)"
	@-$(ECHO) "     TARGET                 = $(TARGET)"
	@-$(ECHO) "     RTLTOOL                = $(RTLTOOL)"
	@-$(ECHO) "     SIMTOOL                = $(SIMTOOL)"
	@-$(ECHO) "     NETLIST                = $(NETLIST)"
	@-$(ECHO) "     GATE_NETLIST_FORMAT    = $(GATE_NETLIST_FORMAT)"
	@-$(ECHO) "     RPT_FILE               = $(TARGET)/$(RPT_FILE)"
	@-$(ECHO) "     VCD_FILE               = $(VCD_FILE)"
	@-$(ECHO) "     LIB_FILES              = $(LIB_FILES)"
	@-$(ECHO) ""

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

