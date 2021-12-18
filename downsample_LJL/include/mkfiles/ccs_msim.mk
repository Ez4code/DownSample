
# Caller must set
#  VHDL_SRC = list of VHDL src files
#  VLOG_SRC = list of Verilog src files
#  CXX_SRC = list of C++ src files
#  CXX_INCLUDES = list of user include directories (not system includes or SystemC includes)
#  HDL_LIB_NAMES = list of logical libraries to create
#
# Available targets:
#    build - compile and link
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean
#    help


INVOKE_ARGS   = $(SCVerify_INVOKE_ARGS)
LINK_LIBNAMES = $(SCVerify_LINK_LIBNAMES)

# SystemC selection for QuestaSim
ifeq "$(ModelSim_SYSC_VERSION)" "default"
MSIM_SYSC_VERSION := 
endif
ifeq "$(ModelSim_SYSC_VERSION)" "2.2"
MSIM_SYSC_VERSION := -sc22
endif
ifeq "$(ModelSim_SYSC_VERSION)" "2.3"
MSIM_SYSC_VERSION := 
endif

MODEL_TECH       := $(ModelSim_Path)
ENABLE_VOPT      := $(ModelSim_ENABLE_VOPT)
ENABLE_QWAVE     := $(ModelSim_ENABLE_QWAVE)
LP_SAT           := $(LowPower_SWITCHING_ACTIVITY_TYPE)
VOPT_ARGS        := $(ModelSim_VOPT_ARGS) $(MSIM_SYSC_VERSION)
VCOM_OPTS        = $(ModelSim_VCOM_OPTS)
VLOG_OPTS        += $(ModelSim_VLOG_OPTS)
# For Verilog OVL assertion
# VLOG_OPTS        += +incdir+$(MGC_HOME)/pkgs/cds_assert/hdl/std_ovl+$(MGC_HOME)/pkgs/cds_assert/hdl/std_ovl/vlog95 +define+OVL_VERILOG+OVL_ASSERT_ON

# For SystemVerilog OVL assertion
# VLOG_OPTS        += +incdir+$(MGC_HOME)/pkgs/cds_assert/hdl/std_ovl+$(MGC_HOME)/pkgs/cds_assert/hdl/std_ovl/sva05 +define+OVL_SVA+OVL_ASSERT_ON

SCCOM_OPTS       = $(ModelSim_SCCOM_OPTS) $(MSIM_SYSC_VERSION)
FORCE_32BIT      = $(ModelSim_FORCE_32BIT)
VSIM_OPTS        = $(ModelSim_VSIM_OPTS) $(MSIM_SYSC_VERSION)
GATE_VSIM_OPTS   = $(ModelSim_GATE_VSIM_OPTS)
MSIM_DOFILE      = $(ModelSim_MSIM_DOFILE)
DEF_MODELSIM_INI = $(ModelSim_DEF_MODELSIM_INI)
VCD_SIZE_LIMIT   = $(ModelSim_VCD_SIZE_LIMIT)
QUESTA_DISABLE_WLF = $(ModelSim_SUPPRESS_WAVEFORMS)
VOPT_DBG_FILE   := $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)design.bin
QWAVE_FILE      := $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)qwave.db
QWAVE_SIGNAL_FILE := $(PROJ2WORK)/.qwave_signal

ENABLE_CODE_COVERAGE = $(ModelSim_ENABLE_CODE_COVERAGE)
QHOME = $(ModelSim_QHOME)

ifneq "$(CCS_VCD_FILE)" ""
VCD_FILETYPE = $(suffix $(CCS_VCD_FILE))
endif

ifeq "$(VCD_FILETYPE)" ".db"
QWAVE_FILE := $(CCS_VCD_FILE)
ENABLE_QWAVE = true
endif

# Force required optimization levels
ifeq "$(ENABLE_CODE_COVERAGE)" "true"
  ENABLE_VOPT      = true
  ENABLE_QWAVE     = false
  VOPT_ARGS        += +cover=bcsexf -coverdeglitch 5ps
  VSIM_OPTS        += -coverage -onfinish stop
endif

ifeq "$(ENABLE_QWAVE)" "true"
  ENABLE_VOPT    = true
  VOPT_ARGS      += -designfile $(VOPT_DBG_FILE) -debug
  QUESTA_DISABLE_WLF = true
ifeq "$(LP_SAT)" "qwave"
  VSIM_OPTS      += -qwavedb=+signal=$(QWAVE_SIGNAL_FILE)+wavefile=$(QWAVE_FILE)
else
  VSIM_OPTS      += -qwavedb=+signal+wavefile=$(QWAVE_FILE)
endif
endif

ifeq "$(QUESTA_DISABLE_WLF)" "false"
  VSIM_OPTS += -wlf $(WLFFILE)
endif

ifeq "$(ENABLE_VOPT)" "true"
  TOP_DU_SIM       = $(TOP_DU)_opt
else
  TOP_DU_SIM       = $(TOP_DU)
ifeq "$(COMPAT_novopt)" "true"
  $(warning Warning: Using deprecated -novopt switch, enable vopt.) 
  VCOM_OPTS        += -novopt -note 12110
  VLOG_OPTS        += -novopt -note 12110
  VSIM_OPTS        += -novopt -note 12110
endif
endif

NOVAS_INST_DIR = $(Novas_NOVAS_INST_DIR)

export VCD_SIZE_LIMIT
export QUESTA_DISABLE_WLF
export CCS_VCD_FILE

ifeq "$(MODEL_TECH)" ""
   $(error No ModelSim/QuestaSim installation found. Check your ModelSim flow Path option in Catapult)
endif

ifeq "$(OS)" "Windows_NT"
  MTI_VCO_MODE = 32
else
  # Check for 64-bit option
  PLAT = $(shell uname)
  PLAT_ARCH = $(shell uname -i)
  ifeq "$(PLAT_ARCH)" "x86_64"
    # Running on a 64-bit machine
    ifneq "$(FORCE_32BIT)" ""
      ifeq "$(FORCE_32BIT)" "yes"
        MTI_VCO_MODE = 32
      else
        MTI_VCO_MODE = 64
      endif
    else
      ifeq "$(MTI_VCO_MODE)" ""
        MTI_VCO_MODE = 64
      endif
    endif
  else
    MTI_VCO_MODE = 32
  endif
endif
export MTI_VCO_MODE

#===============================================================================
# ModelSim executables
VLIB       = $(MODEL_TECH)/vlib
VMAP       = $(MODEL_TECH)/vmap
VCOM       = $(MODEL_TECH)/vcom
VLOG       = $(MODEL_TECH)/vlog
SCCOM      = $(MODEL_TECH)/sccom
VOPT       = $(MODEL_TECH)/vopt
VSIM       = $(MODEL_TECH)/vsim
QVERIFY    = $(QHOME)/bin/qverify
HOME_0IN := $(QHOME)

SC_ARGS    := $(foreach arg,$(INVOKE_ARGS),-sc_arg $(arg))

ifeq "$(CXX_VCO)" "ixn"
ifeq "$(SCVerify_USE_CCS_BLOCK)" "true"
ifeq "$(ModelSim_ALLOW_DUP_SYMS)" "true"
# mingw compiler 4.2.1 does not distinguish specializations of classes
# with member functions as "W" (weak) and "T" (text code). So
# for CCS_BLOCKS on windows with templatized classes we have to turn
# off multiple symbol definitions
SCCOM_LD_ARGS += -Wl,--allow-multiple-definition
endif
endif
endif

# ModelSim/QuestaSim always use GNU C++ compiler
# (Though on Windows it is MinGW)
CXX_TYPE   = gcc

# C++ Compile options
F_COMP     = -c
F_INCDIR   = -I
F_LIBDIR   = -L
LIB_PREFIX = -l
LIB_EXT    = .a
SC_LIB_SW  = -lib
ifeq "$(SCVerify_OPTIMIZE_WRAPPERS)" "true"
F_WRAP_OPT = -O2
else
F_WRAP_OPT =
endif

ifeq "$(Option_CppStandard)" "c++11"
ifeq "$(OS)" "Windows_NT"
$(warning Warning: Modelsim's Windows GNU complier does not support the C++11 language standard, setting ignored.)
else
CXXFLAGS += -std=gnu++11
SCCOM_OPTS += -scpoptions "--c++11"
endif
endif

ADDED_LIBPATHS := $(foreach lp,$(LINK_LIBPATHS),$(F_LIBDIR) $(lp))
ADDED_LIBNAMES := $(foreach ln,$(LINK_LIBNAMES),$(LIB_PREFIX)$(ln))
NORMAL_SOLNDIR := $(subst \,/,$(SOLNDIR))

VLOG_INCDIR_OPT = $(foreach id,$(VLOG_INCDIRS),+incdir+$(id))
VLOG_DEFS_OPT = $(foreach d,$(VLOG_DEFS),+define+$(d))

CMDFILE := $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)scverify_msim_wave.tcl
LOGFILE := $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)sim.log
WLFFILE := $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)vsim.wlf
DUTINSTINFOFILE := $(PROJ2WORK)/.dut_inst_info.tcl

# t/r
ADDED_SC_LIBS := $(foreach lsc,$(LINK_SC_LIBS),$(SC_LIB_SW) $(lsc))

# Default Questa SIM simulator options
ifeq "$(VSIM_OPTS)" ""
VSIM_OPTS = -t ps
endif

ifneq "$(STAGE)" "cycle"
ifneq "$(STAGE)" "rtl"
ifneq "$(STAGE)" "prepwr"
ifneq "$(STAGE)" "power"
VSIM_OPTS += $(GATE_VSIM_OPTS)
endif
endif
endif
endif

# SDF Simulation support
ifneq "$(VNDR_SDFINST)" ""
ifneq "$(SDF_FNAME)" ""
SDF_OPT = -sdfmax $(VNDR_SDFINST)=$(PROJ2SOLN)/$(SDF_FNAME)
else
# compatibility with the flows that do not set the SDF_FNAME (and assume its name to be $(TARGET)/scverify_gate.sdf)
SDF_OPT = -sdfmax $(VNDR_SDFINST)=$(PROJ2SOLN)/gate.$(NETLIST).sdf
endif
endif

# Build up include directory path
CXX_INCLUDES += $(INCL_DIRS)
##CXX_INCLUDES += $(MODEL_TECH)/../include/systemc
CXX_INCLUDES += $(MGC_HOME)/shared/include
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/src
CXX_INCLUDES += $(MGC_HOME)/pkgs/siflibs
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/mgc_comps_src
# CXX_INCLUDES += $(MGC_HOME)/pkgs/cds_assert/hdl/std_ovl
CXX_INC    := $(foreach idir,$(CXX_INCLUDES),$(F_INCDIR)$(idir))

# Assemble final CXXFLAGS (Questa SIM requires use of -DUSE_STD_STRING instead of -DSC_USE_STD_STRING)
CXXFLAGS   += $(CXX_INC) -DUSE_STD_STRING

ifeq "$(ModelSim_MSIM_AC_TYPES)" "true"
CXXFLAGS   += -DSC_INCLUDE_MTI_AC
endif

# Assemble list of library names for verilog
VLOG_LIBS := $(foreach lib,$(VNDR_HDL_LIBS),-L "$(subst .log2phyts,,$(notdir $(lib)))")  $(foreach lib,$(HDL_LIB_NAMES),-L "$(PROJ2WORK)/$(TARGET)/$(lib)") $(foreach lib,$(ADDED_VLOGLIBS),-L "$(lib)")

#------------------------------------------------------------
# New support for precompiled vendor libraries
$(TARGET)/%.libmapts: VAL_RELDIR=$(if $(basename $(basename $(basename $(notdir $@)))),$(basename $(basename $(basename $(notdir $@)))),.)
$(TARGET)/%.libmapts: VAL_LOGICAL=$(subst .,,$(suffix $(basename $(basename $(notdir $@)))))
$(TARGET)/%.libmapts: VAL_PHYSICAL=$(subst .,,$(suffix $(basename $(notdir $@))))
$(TARGET)/%.libmapts:
	-@echo "============================================"
	-@echo "Mapping vendor logical library '$(VAL_LOGICAL)' to physical path '$(VENDOR_PHYDIR)/$(VAL_RELDIR)/$(VAL_PHYSICAL)'"
	$(VMAP) $(VAL_LOGICAL) $(VENDOR_PHYDIR)/$(VAL_RELDIR)/$(VAL_PHYSICAL)
	$(TIMESTAMP)
VLOG_LIBS += $(foreach lib,$(VENDOR_LIBMAPS_V),-L "$(VENDOR_PHYDIR)/$(if $(basename $(basename $(basename $(notdir $(lib))))),$(basename $(basename $(basename $(notdir $(lib))))),.)/$(subst .,,$(suffix $(basename $(notdir $(lib)))))")
#------------------------------------------------------------

# The following env variables are used to map libraries when compiling from
# the solution directory so that they can be referenced when executing simulation
# from the project directory
ifeq "$(DEF_MODELSIM_INI)" ""
DEF_MODELSIM_INI =$(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)
endif
# Make a platform-specific version of DEF_MODELSIM_INI
X_DEF_MODELSIM_INI =$(subst /,$(PATHSEP),$(subst \,$(PATHSEP),$(DEF_MODELSIM_INI)))
# Back-quote any embedded white space for make dependency check
DEP_X_DEF_MODELSIM_INI = $(subst $(space),\$(space),$(X_DEF_MODELSIM_INI))
MODELSIM=
export MODELSIM
export SCVLIBS

ifneq "$(CCS_VCD_FILE)" ""
ifeq "$(USE_FSDB)" ""
VCD_FILETYPE = $(suffix $(CCS_VCD_FILE))
ifeq "$(VCD_FILETYPE)" ".fsdb"
USE_FSDB = true
endif
endif
endif
ifneq "$(USE_FSDB)" ""
ifeq "$(NOVAS_INST_DIR)" ""
   $(warning Warning: The NOVAS_INST_DIR environment variable must be set to use this makefile)
endif
ifeq "$(MTI_VCO_MODE)" "32"
NOVAS_FLI_DIR = $(NOVAS_INST_DIR)/share/PLI/MODELSIM/LINUX
else
NOVAS_FLI_DIR = $(NOVAS_INST_DIR)/share/PLI/MODELSIM/LINUX64
endif
VHDL_SRC += $(NOVAS_FLI_DIR)/novas.vhd/novas.vhd.vhdlts
$(TARGET)/novas.vhd.vhdlts: $(NOVAS_FLI_DIR)/novas.vhd
$(TARGET)/novas.vhd.vhdlts: HDL_LIB=work
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(NOVAS_FLI_DIR)
export LD_LIBRARY_PATH
OPTIONAL_DU += work.novas
VSIM_OPTS += -pli $(NOVAS_FLI_DIR)/novas_fli.so
endif

ifeq "$(ENABLE_VOPT)" "true"
ifeq "$(STAGE)" "power" 
# (vsim-3197) Load of "novas_fli.so" failed: novas_fli.so: cannot open shared object file: No such file or directory.
   VSIM_OPTS += -note 3197
endif
else
ifneq "$(OPTIONAL_DU)" ""
   $(warning Warning: Secondary design units '$(OPTIONAL_DU)' need to be merged, enable vopt.)
endif
endif

# Modify SUFFIX then prefix TARGET (preserve order of files)
TMP_VLOG_SRC := $(foreach hdlfile,$(VLOG_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_VHDL_SRC := $(foreach hdlfile,$(VHDL_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile)))

# add dependency on .ccs_env_opts/ModelSim_ENABLE_CODE_COVERAGE.ts

# Custom time-stamp dependencies for scverify_top.cpp/mc_testbench.cpp
$(TARGET)/scverify_top.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts
$(TARGET)/mc_testbench.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts
# Custom time-stamp dependencies for all C++ files
$(TMP_CXX_SRC): .ccs_env_opts/ModelSim_MSIM_AC_TYPES.ts

# Translate rule to compile VHDL with Questa SIM 
$(TARGET)/%.vhdlts : 
	-@echo "============================================"
	-@echo "Compiling VHDL file: $<"
	$(VCOM) -work $(HDL_LIB) $(VCOM_OPTS) $(VHDL_F_OPTS) $<
	$(TIMESTAMP)

# Translate rule to compile Verilog with Questa SIM 
$(TARGET)/%.vts :
	-@echo "============================================"
	-@echo "Compiling Verilog file: $<"
	$(VLOG) -work $(HDL_LIB) $(VLOG_OPTS) $(VLOG_F_OPTS) $(VLOG_DEFS_OPT) $(VLOG_INCDIR_OPT) $<
	$(TIMESTAMP)

# Translate rule to compile SystemC with Questa SIM 
$(TARGET)/%.cxxts :
	-@echo "============================================"
	-@echo "Compiling C++ file: $<"
	$(SCCOM) $(SCCOM_OPTS) $(CXXFLAGS) $(CXX_OPTS) $(F_COMP) $<
	$(TIMESTAMP)

# Translate rule to compile SystemC with Questa SIM 
$(TARGET)/%.cxxrts :
	-@echo "============================================"
	-@echo "Compiling C++ file: $<"
	$(SCCOM) -work replay $(SCCOM_OPTS) $(CXXFLAGS) $(CXX_OPTS) $(F_COMP) $<
	$(TIMESTAMP)

# Make sure that all rules executing based on the pattern %ts have the correct MODELSIM and SCVLIBS
# variable values
%ts: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
%ts: SCVLIBS=.
build: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
build: SCVLIBS=.
$(TARGET)/$(TOP_DU)vopts : MODELSIM=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)modelsim.ini
$(TARGET)/$(TOP_DU)vopts : SCVLIBS=$(PROJ2WORK)
sim: MODELSIM=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)modelsim.ini
sim: SCVLIBS=$(PROJ2WORK)
simgui: MODELSIM=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)modelsim.ini
simgui: SCVLIBS=$(PROJ2WORK)
# autocheck and covercheck are run from the solution directory
autocheck/autocheck_verify.db: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
autocheck/autocheck_verify.db: SCVLIBS=.
covercheck/covercheck_verify.db: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
covercheck/covercheck_verify.db: SCVLIBS=.
$(TARGET)/$(TOP_HDL_ENTITY).ucdb: MODELSIM=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)modelsim.ini
$(TARGET)/$(TOP_HDL_ENTITY).ucdb: SCVLIBS=$(PROJ2WORK)

# Expand out the list of Questa SIM libraries to create
# (this currently only creates the first library named in the list)
$(foreach lib,$(HDL_LIB_NAMES),$(TARGET)/$(lib).libts): $(TARGET)/modelsim.inits
	-@echo "============================================"
	-@echo "Creating physical library '$(subst .libts,,$@)'"
	$(VLIB) $(subst .libts,,$@)
	-@echo "============================================"
	-@echo "Mapping logical library '$(subst .libts,,$(notdir $@))' to physical path '\$$SCVLIBS/$(TARGET)/$(subst .libts,,$(notdir $@))'"
	$(VMAP) $(subst .libts,,$(notdir $@)) \$$SCVLIBS/$(TARGET)/$(subst .libts,,$(notdir $@))
	$(TIMESTAMP)

$(foreach lib,$(VNDR_HDL_LIBS),$(TARGET)/$(notdir $(lib))):
	-@echo "============================================"
	-@echo "Mapping vendor logical library '$(HDL_LOGICAL)' to physical path '$(subst .log2phyts,,$<)'"
	$(VMAP) $(HDL_LOGICAL) $(subst .log2phyts,,$<)
	$(TIMESTAMP)

# Create the target directory
$(TARGET)/make_dir: 
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating simulation directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

# Create the initial modelsim.ini file
# This must preceed any actual modelsim compilation rules
$(TARGET)/modelsim.inits: $(DEP_X_DEF_MODELSIM_INI) $(TARGET)/make_dir
	-@echo "============================================"
	-@echo "Setting up default modelsim.ini file from '$(X_DEF_MODELSIM_INI)'"
	$(CAT) "$(X_DEF_MODELSIM_INI)" >$(TARGET)$(PATHSEP)modelsim.ini
	$(TIMESTAMP)

# Targets start here
$(TARGET)/make_libs : $(foreach lib,$(HDL_LIB_NAMES),$(TARGET)/$(lib).libts) $(VNDR_HDL_LIBS) $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAPS_V) $(VENDOR_LIBMAPS_VHD)
	$(TIMESTAMP)

$(TARGET)/$(TOP_DU)ts : $(TMP_VHDL_SRC) $(TMP_VLOG_SRC) $(TMP_CXX_SRC)
ifneq "$(CXX_SRC)" ""
	-@echo "============================================"
	-@echo "Linking executable"
	$(SCCOM) -link $(SCCOM_LD_ARGS) $(MSIM_SYSC_VERSION) $(ADDED_LIBPATHS) $(ADDED_LIBNAMES) $(ADDED_SC_LIBS)
endif
	$(TIMESTAMP)

ifneq "$(STAGE)" "orig"
$(TARGET)/scverify_msim_wave.tcl: $(WORK2SOLN)/scverify/ccs_wave_signals.dat
	-@echo "============================================"
	-@echo "Creating Questa SIM wave TCL file '$@' from '$<'"
	$(TCLSH_CMD) $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_msim.flo create_wave_script $(PROJ2SOLN)/scverify/ccs_wave_signals.dat $@ $(DUTINSTINFOFILE) $(QWAVE_SIGNAL_FILE) $(MSIM_DOFILE)
else
$(TARGET)/scverify_msim_wave.tcl:
	$(TCLSH_CMD) $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_msim.flo create_wave_script "" $@ $(DUTINSTINFOFILE) $(QWAVE_SIGNAL_FILE) $(MSIM_DOFILE)
endif

$(TARGET)/$(TOP_DU)vopts : $(TARGET)/$(TOP_DU)ts
ifeq "$(ENABLE_VOPT)" "true"
	-@echo "============================================"
	-@echo "Optimizing design: $(TOP_DU)"
	$(CD) $(WORK2PROJ)$(;) "$(VOPT)" $(VOPT_ARGS) $(SC_ARGS) $(VLOG_LIBS) $(TOP_DU) $(OPTIONAL_DU) -o $(TOP_DU)_opt
endif
	$(TIMESTAMP)

build: $(TARGET)/make_libs $(TARGET)/scverify_msim_wave.tcl $(TARGET)/$(TOP_DU)vopts

# Export special env var to indicate batch or gui mode
sim:    CCS_SIM_MODE=batch
simgui: CCS_SIM_MODE=gui
export  CCS_SIM_MODE

sim: build
	-@echo "============================================"
ifneq "$(CCS_VCD_FILE)" ""
	-@echo "Simulating design entity: $(TOP_DU) to produce Switching Activity File: $(CCS_VCD_FILE)"
else
	-@echo "Simulating design entity: $(TOP_DU)"
endif
	$(CD) $(WORK2PROJ)$(;) "$(VSIM)" $(VSIM_OPTS) -l "$(LOGFILE)" $(VLOG_LIBS) $(TOP_DU_SIM) $(SIM_OPTS) $(SDF_OPT) $(SC_ARGS) -c -do "do {$(CMDFILE)}"

simgui: build
	-@echo "============================================"
ifneq "$(CCS_VCD_FILE)" ""
	-@echo "Simulating design entity: $(TOP_DU) to produce Switching Activity File: $(CCS_VCD_FILE)"
else
	-@echo "Simulating design entity: $(TOP_DU)"
endif
	$(CD) $(WORK2PROJ)$(;) "$(VSIM)" -i $(VSIM_OPTS) $(SIM_OPTS) -l "$(LOGFILE)" $(VLOG_LIBS) $(TOP_DU_SIM) $(SDF_OPT) $(SC_ARGS) -do "do {$(CMDFILE)}"

#=============================================================================================
ac_mkdir:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating Autocheck directory 'autocheck'"
	$(MKDIR) autocheck
	$(TIMESTAMP)

autocheck/autocheck_verify.db: ac_mkdir $(TARGET)/make_libs $(TMP_VHDL_SRC) $(TMP_VLOG_SRC)
	-@echo "============================================"
	-@echo "Running Autocheck on: $(TOP_HDL_ENTITY)"
	-@echo "onerror {exit}" >run_ac.do
	-@echo "global env" >>run_ac.do
	-@echo "netlist constant use_1clk_i 1'b0" >>run_ac.do
	-@echo "autocheck enable" >>run_ac.do
	-@echo "autocheck disable -type ARITH*" >>run_ac.do
	-@echo "autocheck compile -d $(TOP_HDL_ENTITY) -work work -modelsimini [set env(MODELSIM)]" >>run_ac.do
	-@echo "autocheck verify -effort unlimited -timeout 2m" >>run_ac.do
	-@echo "exit" >>run_ac.do
	$(QVERIFY) -c -od autocheck -do run_ac.do

# End-user targets
autocheck: autocheck/autocheck_verify.db

autocheck_gui: autocheck/autocheck_verify.db
	-@echo "============================================"
	-@echo "Running Autocheck GUI on: $(TOP_HDL_ENTITY) with db $<"
	$(QVERIFY) $<

#=============================================================================================
# UCDB and Covercheck targets are only available for RTL and only if the options are enabled
ifeq "$(STAGE)" "rtl"
ifeq "$(ModelSim_ENABLE_CODE_COVERAGE)" "true"

cc_mkdir:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating Covercheck directory 'covercheck'"
	$(MKDIR) covercheck
	$(TIMESTAMP)

# This is the same action as the sim target
$(TARGET)/$(TOP_HDL_ENTITY).ucdb: build
	$(CD) $(WORK2PROJ)$(;) "$(VSIM)" $(VSIM_OPTS) -l "$(LOGFILE)" $(VLOG_LIBS) $(TOP_DU_SIM) $(SIM_OPTS) $(SDF_OPT) $(SC_ARGS) -c -do "do {$(CMDFILE)}"

ifeq "$(ModelSim_WITNESS_WAVEFORM)" "true"
WITNESS := -witness_waveforms
endif

ifneq "$(VLOG_SRC)" ""
COV_LIBS := $(foreach lib,$(VLOG_LIBS),"$(subst $(PROJ2WORK),.,$(lib))")
endif

covercheck/covercheck_verify.db: cc_mkdir $(TARGET)/$(TOP_HDL_ENTITY).ucdb .ccs_env_opts/ModelSim_WITNESS_WAVEFORM.ts .ccs_env_opts/ModelSim_COVERCHECK_TIMEOUT.ts .ccs_env_opts/ModelSim_COVERCHECK_TCL.ts
	-@echo "============================================"
	-@echo "Running Covercheck on: $(TOP_HDL_ENTITY) with UCDB $(TARGET)/$(TOP_HDL_ENTITY).ucdb"
	-@echo "onerror {exit}" >run_cc.do
	-@echo "global env" >>run_cc.do
	-@echo "if {[file exists [file join $(ModelSim_COVERCHECK_TCL) covercheck_init.tcl]]} { do [file join $(ModelSim_COVERCHECK_TCL) covercheck_init.tcl] }" >>run_cc.do
	-@echo "covercheck compile -d $(TOP_HDL_ENTITY) -work work -modelsimini $(MODELSIM) $(COV_LIBS)" >>run_cc.do
	-@echo "if {[file exists [file join $(ModelSim_COVERCHECK_TCL) covercheck_postcompile.tcl]]} { do [file join $(ModelSim_COVERCHECK_TCL) covercheck_postcompile.tcl] }" >>run_cc.do
	-@echo "covercheck load ucdb $(TARGET)/$(TOP_HDL_ENTITY).ucdb" >>run_cc.do
	-@echo "if {[file exists [file join $(ModelSim_COVERCHECK_TCL) covercheck_postload.tcl]]} { do [file join $(ModelSim_COVERCHECK_TCL) covercheck_postload.tcl] }" >>run_cc.do
	-@echo "covercheck verify $(WITNESS) -timeout $(ModelSim_COVERCHECK_TIMEOUT)" >>run_cc.do
	-@echo "if {[file exists [file join $(ModelSim_COVERCHECK_TCL) covercheck_postverify.tcl]]} { do [file join $(ModelSim_COVERCHECK_TCL) covercheck_postverify.tcl] }" >>run_cc.do
	-@echo "covercheck generate exclude cc_ex.do" >>run_cc.do
	-@echo "exit" >>run_cc.do
	$(QVERIFY) -c -od covercheck -do run_cc.do

# End-user targets
covercheck: covercheck/covercheck_verify.db

covercheck_gui: covercheck/covercheck_verify.db
	-@echo "============================================"
	-@echo "Running Covercheck GUI on: '$(TOP_HDL_ENTITY)' with db $<"
	$(QVERIFY) $<

endif
endif

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	@-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))
	@-$(RMDIR) autocheck covercheck

.PHONY : help
help: helptext dumpsysvars dump_msim_vars
	@-$(ECHO) "   SCVerify flow options:"
	@-$(ECHO) "     INVOKE_ARGS           = $(INVOKE_ARGS)"
	@-$(ECHO) "     INCL_DIRS             = $(INCL_DIRS)"
	@-$(ECHO) "     ADDED_LIBPATHS        = $(ADDED_LIBPATHS)"
	@-$(ECHO) "     ADDED_LIBNAMES        = $(ADDED_LIBNAMES)"
	@-$(ECHO) "     CCS_VCD_FILE          = $(CCS_VCD_FILE)"
	@-$(ECHO) "     VNDR_HDL_LIBS         = $(VNDR_HDL_LIBS)"
	@-$(ECHO) "     VLOC_SRC              = $(VLOG_SRC)"
helptext:
	@-$(ECHO) "ModelSim Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   simgui     Compile and Execute the simulation using the"
	@-$(ECHO) "              interactive mode of the simulator (if available)"
	@-$(ECHO) "   sim        Compile and Execute the simulation using the"
	@-$(ECHO) "              batch mode of the simulator"
	@-$(ECHO) "   build      Compile the models only"
	@-$(ECHO) "   covercheck Run 'sim' if needed, then run QuestaSim CoverCheck"
	@-$(ECHO) "   clean      Remove all compiled objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

.PHONY: dumpvars
dumpvars: dump_msim_vars

dump_msim_vars:
	@-$(ECHO) "   ModelSim flow options:"
	@-$(ECHO) "     Path                  = $(MODEL_TECH)"
	@-$(ECHO) "     MTI_VCO_MODE          = $(MTI_VCO_MODE)"
	@-$(ECHO) "     FORCE_32BIT           = $(FORCE_32BIT)"
	@-$(ECHO) "     ENABLE_VOPT           = $(ENABLE_VOPT)"
	@-$(ECHO) "     VOPT_ARGS             = $(VOPT_ARGS)"
	@-$(ECHO) "     VCOM_OPTS             = $(VCOM_OPTS)"
	@-$(ECHO) "     VLOG_OPTS             = $(VLOG_OPTS)"
	@-$(ECHO) "     SCCOM_OPTS            = $(SCCOM_OPTS)"
	@-$(ECHO) "     VSIM_OPTS             = $(VSIM_OPTS)"
	@-$(ECHO) "     CC_EX                 = $(CC_EX)"
	@-$(ECHO) "     GATE_VSIM_OPTS        = $(GATE_VSIM_OPTS)"
	@-$(ECHO) "     ENABLE_CODE_COVERAGE  = $(ENABLE_CODE_COVERAGE)"
	@-$(ECHO) "     MSIM_DOFILE           = $(MSIM_DOFILE)"
	@-$(ECHO) "     MSIM_SYSC_VERSION     = $(MSIM_SYSC_VERSION)"
	@-$(ECHO) "     DEF_MODELSIM_INI      = $(DEF_MODELSIM_INI)"
	@-$(ECHO) "     VLOG_INCDIR_OPT       = $(VLOG_INCDIR_OPT)"
	@-$(ECHO) "     VLOG_DEFS_OPT         = $(VLOG_DEFS_OPT)"
ifneq "$(USE_FSDB)" ""
	@-$(ECHO) "   Novas flow options:"
	@-$(ECHO) "     NOVAS_INST_DIR        = $(NOVAS_INST_DIR)"
endif

