
# Caller must set
#  VHDL_SRC = list of VHDL src files
#  VLOG_SRC = list of Verilog src files
#  CXX_SRC = list of C++ src files
#  CXX_INCLUDES = list of user include directories (not system includes or SystemC includes)
#  HDL_LIB_NAMES = list of logical libraries to create
#
# Available targets:
#    do_vhdl - Compile VHDL source
#    do_vlog - Compile Verilog source
#    do_cxx - Compile C++ source
#    build - compile and link
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean


# Catapult 2010a version
INVOKE_ARGS   = $(SCVerify_INVOKE_ARGS)
LINK_LIBNAMES = $(SCVerify_LINK_LIBNAMES)
VCS_HOME = $(VCS_VCS_HOME)
VG_GNU_PACKAGE = $(VCS_VG_GNU_PACKAGE)
VG_ENV32_SCRIPT = $(VCS_VG_ENV32_SCRIPT)
VG_ENV64_SCRIPT = $(VCS_VG_ENV64_SCRIPT)
FORCE_32BIT = $(VCS_FORCE_32BIT)
COMP_FLAGS = $(VCS_COMP_FLAGS)

VHDLAN_OPTS = $(VCS_VHDLAN_OPTS)
VLOGAN_OPTS = $(VCS_VLOGAN_OPTS)
VCSELAB_OPTS = $(VCS_VCSELAB_OPTS)
VCSSIM_OPTS = $(VCS_VCSSIM_OPTS)
TSFILES := .ccs_env_opts/VCS_VHDLAN_OPTS.ts .ccs_env_opts/VCS_VLOGAN_OPTS.ts
VCS_DOFILE   = $(VCS_VCS_DOFILE)
ENABLE_CODE_COVERAGE = $(VCS_ENABLE_CODE_COVERAGE)

# PSL/Coverage disabled for power estimation flows
ifeq "$(STAGE)" "power"
$(warning Code Coverage and PSL are not supported for switching activity file generation)
else ifeq "$(STAGE)" "prepwr"
$(warning Code Coverage and PSL are not supported for switching activity file generation)
else
VHCMOPTS = -psl
VLCMOPTS = -psl

ifeq "$(ENABLE_CODE_COVERAGE)" "true"
 VCSCMOPTS = -lca -psl -cm branch+cond+fsm+line+path+assert -debug -assert enable_diag
 SIMVCMOPTS = -lca -cm branch+cond+fsm+line+path+assert -cm_dir $(PROJ2SOLN)/$(TARGET)/coverage/vcs_mx
else
 VCSCMOPTS = -lca -psl -cm assert -debug -assert enable_diag
 SIMVCMOPTS = -cm assert
endif
endif

NOVAS_INST_DIR = $(Novas_NOVAS_INST_DIR)

ifeq "$(VCS_HOME)" ""
   $(error The VCS_HOME environment variable must be set to use this makefile)
endif

ifeq "$(VG_GNU_PACKAGE)" ""
VG_GNU_PACKAGE = $(VCS_HOME)/gnu/linux
endif

# Make sure user's INVOKE_ARGS does not contain any switches that are automatically passed
# to VCS (even though the user intends them to be arguments to their main/sc_main.
# From vcsmx_ug.pdf section "Specifying Runtime Options to the SystemC Simulation",
#   "... The following arguments are always recognized, and goes only to the VCS MX
#   simulator kernel:
#     -r, -restore, -pathmap, -save, -save_nocbk, -save_file, -save_file_skip, -gui
#     -ucli, -uclimode, -ucli2Proc"
ifneq "$(findstring -r ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -r switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -restore ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -restore switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -save ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -save switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -save_nocbk ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -save_nocbk switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -save_file ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -save_file switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -save_file_skip ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -save_file_skip switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -gui ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -gui switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -ucli ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -ucli switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -uclimode ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -uclimode switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif
ifneq "$(findstring -ucli2Proc ,$(INVOKE_ARGS))" ""
$(error SCVerify INVOKE_ARGS contains -ucli2Proc switch which is reserved for VCS-MX simulator kernel. Change your main/sc_main testbench)
endif


# VCS requires env var HOME to be defined
ifeq "$(HOME)" ""
$(warning The VCS-MX required environment variable 'HOME' was set to an empty value. Setting HOME to '.')
HOME = .
export HOME
endif

# Check for 64-bit option
PLAT = $(shell uname)
ifneq "$(PLAT)" "Linux"
   $(error This makefile 'ccs_vcs.mk' is only supported on Linux platforms)
endif

PLAT_ARCH = $(shell uname -i)

# Default configuration script
VG_ENV_SCRIPT = $(VG_ENV64_SCRIPT)

ifeq "$(VCS_TARGET_MODE)" "32"
  FORCE_32BIT = yes
endif

VCS_MODE =
ifeq "$(PLAT_ARCH)" "x86_64"
 # Running on a 64-bit machine
 ifeq "$(FORCE_32BIT)" "yes"
  # User wants 32-bit simulation mode
  FULL64 = 
  VCS_MODE = "32-bit simulation on 64-bit machine with 64-bit VCS"
 else
  FULL64 = -full64
  VCS_MODE = "64-bit simulation on 64-bit machine with 64-bit VCS"
 endif
else
 # Running on a 32-bit machine (presumably with a 32-bit VCS)
 VG_ENV_SCRIPT = $(VG_ENV32_SCRIPT)
 FULL64 =
 VCS_MODE = "32-bit simulation on 32-bit machine with 32-bit VCS"
endif

# Set PATH, LD_LIBRARY_PATH, etc via Synopsys env scripts
IGNORE := $(shell csh -f -c "rm -f vcs_make_env; setenv VCS_HOME $(VCS_HOME); source $(VCS_HOME)/bin/environ.csh; setenv VG_GNU_PACKAGE $(VG_GNU_PACKAGE); source $(VG_GNU_PACKAGE)/$(VG_ENV_SCRIPT); env | sed 's/=/:=/' | sed 's/^/export /' | egrep ' PATH:| LD_LIBRARY_PATH:| MANPATH:| VCSMX_HOME:| ARCH:| TK_LIBRARY:| OVA_LD_PATH:| VCS_HOME:| VG_GNU_PACKAGE:' >vcs_make_env")

include vcs_make_env
unexport GCC_EXEC_PREFIX

# optional variable to modify the executable filename when FSDB library is linked in
PWR_EXE_EXT :=

# VCD/FSDB support
export CCS_VCD_FILE
ifneq "$(CCS_VCD_FILE)" ""

ifeq "$(USE_FSDB)" ""
VCD_FILETYPE = $(suffix $(CCS_VCD_FILE))
ifeq "$(VCD_FILETYPE)" ".fsdb"
USE_FSDB = true
endif
endif
endif

ifneq "$(CCS_VCD_FILE)" ""
ifeq "$(VCD_FILETYPE)" ".vcd"
$(warning Warning: This makefile ccs_vcs.mk cannot generate a VCD file. Only FSDB and SAIF are supported)
endif
endif

ifneq "$(USE_FSDB)" ""
PWR_EXE_EXT := pwr
ifeq "$(NOVAS_INST_DIR)" ""
   $(warning Warning: The NOVAS_INST_DIR environment variable must be set to use this makefile)
endif

NOVAS_HOME = $(NOVAS_INST_DIR)
export NOVAS_HOME
DEBUSSY_HOME = $(NOVAS_INST_DIR)
export DEBUSSY_HOME
ifeq "$(FULL64)" "-full64"
DEBUSSY_LIB = $(DEBUSSY_HOME)/share/PLI/VCS/LINUX64
else
DEBUSSY_LIB = $(DEBUSSY_HOME)/share/PLI/VCS/LINUX
endif
export DEBUSSY_LIB
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(DEBUSSY_LIB)
export LD_LIBRARY_PATH
VHDL_SRC += $(DEBUSSY_LIB)/novas.vhd/novas.vhd.vhdlts
$(TARGET)/novas.vhd.vhdlts: $(DEBUSSY_LIB)/novas.vhd
$(TARGET)/novas.vhd.vhdlts: HDL_LIB=work
OPTIONAL_DU = work.novas
VCSELAB_OPTS += -load libnovas.so:FSDBDumpCmd +vcsd +memcbk
endif

NORMAL_SOLNDIR := $(subst \,/,$(SOLNDIR))
DUTINSTINFOFILE := $(PROJ2WORK)/.dut_inst_info.tcl

VLOG_INCDIR_OPT = $(foreach id,$(VLOG_INCDIRS),+incdir+$(id))
VLOG_DEFS_OPT = $(foreach d,$(VLOG_DEFS),+define+$(d))

#===============================================================================
# VCS executables
VLOGAN    = vlogan
VHDLAN    = vhdlan
SYSCAN    = syscan
VCS       = vcs
URG       = urg


LD_LIBRARY_PATH := $(MGC_HOME)/lib:$(MGC_HOME)/shared/lib:$(if $(LD_LIBRARY_PATH),:$(LD_LIBRARY_PATH),)

CPP_EXEC = g++
CC_EXEC = gcc

SC_ARGS    := $(INVOKE_ARGS)

CXX_TYPE   = gcc


# C++ Compile options
F_COMP     = -c
F_INCDIR   = -I
F_LIBDIR   = -L
LIB_PREFIX = -l
LIB_EXT    = .a
ifeq "$(SCVerify_OPTIMIZE_WRAPPERS)" "true"
F_WRAP_OPT = -O2
else
F_WRAP_OPT =
endif

ifeq "$(Option_CppStandard)" "c++11"
$(warning Warning: Applying the gcc option for the C++11 language standard)
CXXFLAGS += -std=gnu++11
endif

ADDED_LIBPATHS := $(foreach lp,$(LINK_LIBPATHS),$(F_LIBDIR) $(lp))
ADDED_LIBNAMES := $(foreach ln,$(LINK_LIBNAMES),$(LIB_PREFIX)$(ln))

# Check for license queue enabled
ifeq "$(VCS_LIC_QUEUE)" "true"
VCS_LIC_WAIT = +vcs+lic+wait
endif

# SDF Simulation support
ifneq "$(VNDR_SDFINST)" ""
SDF_OPT = -sdfmax $(VNDR_SDFINST)=$(PROJ2SOLN)/$(TARGET)/scverify_gate.sdf
endif

SYSC_VERSION := 220
ifeq "$(VCS_SYSC_VERSION)" ""
VCS_SYSC_VERSION := 2.3
endif
ifeq "$(VCS_SYSC_VERSION)" "2.2"
SYSC_VERSION := 220
endif
ifeq "$(VCS_SYSC_VERSION)" "2.3"
SYSC_VERSION := 230
CXX_INCLUDES += $(VCS_HOME)/include/systemc23
endif
ifeq "$(VCS_SYSC_VERSION)" "2.3.1"
SYSC_VERSION := 231
CXX_INCLUDES += $(VCS_HOME)/include/systemc231
endif

# Build up include directory path
CXX_INCLUDES += $(INCL_DIRS)
CXX_INCLUDES += $(MGC_HOME)/shared/include
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/src
CXX_INCLUDES += $(MGC_HOME)/pkgs/siflibs
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/mgc_comps_src
CXX_INC    := $(foreach idir,$(CXX_INCLUDES),$(F_INCDIR)$(idir))

# Assemble final CXXFLAGS
CXXFLAGS   += $(COMP_FLAGS) $(CXX_INC) -DSC_USE_STD_STRING

# Build sc_model option
VCS_SC_MODEL1 = $(if $(VLOG_TOP),$(foreach du_e,$(DUT_E),-sysc=$(SYSC_VERSION)),)
VCS_SC_MODEL2 = $(if $(VLOG_TOP),$(foreach du_e,$(DUT_E),-sysc=$(SYSC_VERSION) -sc_model $(du_e) -sc_portmap $(WORK2SOLN)/scverify/dut_v_ports.map),)
VCS_HDL_LIB = $(if $(DUT_E),,-work $(HDL_LIB))
# Special for VHDL
VCS_SC_MODEL3 = $(if $(VHDL_TOP),$(foreach du_e,$(DUT_E),-Mdir=$(TARGET) -sysc=$(SYSC_VERSION)),)
VCS_SC_MODEL4 = $(if $(VHDL_TOP),$(foreach du_e,$(DUT_E),-sysc=$(SYSC_VERSION) -sc_model $(du_e) -sc_portmap $(WORK2SOLN)/scverify/dut_vhdl_ports.map),)

# Assemble list of library names for verilog
VLOG_LIBS := $(foreach lib,$(VNDR_HDL_LIBS),-L $(subst .log2phyts,,$(notdir $(lib))))  $(foreach lib,$(HDL_LIB_NAMES),-L $(NORMAL_SOLNDIR)/$(TARGET)/$(lib)) $(foreach lib,$(ADDED_VLOGLIBS),-L $(lib))

#------------------------------------------------------------
# New support for precompiled vendor libraries
$(TARGET)/%.libmapts: VAL_RELDIR=$(if $(basename $(basename $(basename $(notdir $@)))),$(basename $(basename $(basename $(notdir $@)))),.)
$(TARGET)/%.libmapts: VAL_LOGICAL=$(subst .,,$(suffix $(basename $(basename $(notdir $@)))))
$(TARGET)/%.libmapts: VAL_PHYSICAL=$(subst .,,$(suffix $(basename $(notdir $@))))
$(TARGET)/%.libmapts:
	-@echo "============================================"
	-@echo "Mapping vendor logical library '$(VAL_LOGICAL)' to physical path '$(VENDOR_PHYDIR)/$(VAL_RELDIR)/$(VAL_PHYSICAL)'"
	$(ECHO) "$(VAL_LOGICAL) : $(VENDOR_PHYDIR)/$(VAL_RELDIR)/$(VAL_PHYSICAL)" >>$(TARGET)/synopsys_sim.setup
	$(TIMESTAMP)
VLOG_LIBS += $(foreach lib,$(VENDOR_LIBMAPS_V),-L "$(VENDOR_PHYDIR)/$(if $(basename $(basename $(basename $(notdir $(lib))))),$(basename $(basename $(basename $(notdir $(lib))))),.)/$(subst .,,$(suffix $(basename $(notdir $(lib)))))")
#------------------------------------------------------------

# The following env variables are used to map libraries when compiling from
# the solution directory so that they can be referenced when executing simulation
# from the project directory
SYNOPSYS_SIM_SETUP=
export SYNOPSYS_SIM_SETUP
export SCVLIBS
export PRECISION_EXE

# Modify SUFFIX then prefix TARGET (preserve order of files)
TMP_VLOG_SRC := $(foreach hdlfile,$(VLOG_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_VHDL_SRC := $(foreach hdlfile,$(VHDL_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile)))

# Custom time-stamp dependencies for scverify_top.cpp/mc_testbench.cpp
$(TARGET)/scverify_top.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts
$(TARGET)/mc_testbench.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts

# Translate rule to compile VHDL with VCS
$(TARGET)/%.vhdlts :
	-@echo "============================================"
	-@echo "Compiling VHDL file: $<"
	$(VHDLAN) $(FULL64) $(VHCMOPTS) -verbose $(VCS_HDL_LIB) $(VCS_SC_MODEL3) $(VHDLAN_OPTS) $< $(VCS_SC_MODEL4)
	$(TIMESTAMP)

# Translate rule to compile Verilog with VCS
$(TARGET)/%.vts :
	-@echo "============================================"
	-@echo "Compiling Verilog file: $<"
	$(VLOGAN) $(FULL64) $(VLCMOPTS) -verbose $(VCS_HDL_LIB) -Mdir=$(TARGET) $(VCS_SC_MODEL1) $(VLOGAN_OPTS) $(VCS_SC_MODEL2) $(VCS_F_OPTS) $(VLOG_DEFS_OPT) $(VLOG_INCDIR_OPT) $<
	$(TIMESTAMP)

# Translate rule to compile SystemC with VCS
$(TARGET)/%.cxxts :
	-@echo "============================================"
	-@echo "Compiling C++ file: $<"
	$(SYSCAN) $(FULL64) -sysc=$(SYSC_VERSION) -Mdir=$(TARGET) -debug_all -cflags "$(CXXFLAGS) $(CXX_OPTS)" $<
	$(TIMESTAMP)

# Make sure that all rules executing based on the pattern %ts have the correct SYNOPSYS_SIM_SETUP and SCVLIBS
# variable values
%ts: SYNOPSYS_SIM_SETUP=$(TARGET)$(PATHSEP)synopsys_sim.setup
%ts: SCVLIBS=.
build: SYNOPSYS_SIM_SETUP=$(TARGET)$(PATHSEP)synopsys_sim.setup
build: SCVLIBS=.
sim: SYNOPSYS_SIM_SETUP=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)synopsys_sim.setup
sim: SCVLIBS=$(PROJ2WORK)
simgui: SYNOPSYS_SIM_SETUP=$(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)synopsys_sim.setup
simgui: SCVLIBS=$(PROJ2WORK)

$(TARGET)/$(TOP_DU)$(PWR_EXE_EXT)ts: VCS_LOG=$(TARGET)/elab.log
sim: VCS_LOG=$(PROJ2WORK)/$(TARGET)/sim.log
simgui: VCS_LOG=$(PROJ2WORK)/$(TARGET)/sim.log

# Expand out the list of VCS libraries to create
# (this currently only creates the first library named in the list)
$(foreach lib,$(HDL_LIB_NAMES),$(TARGET)/$(lib).libts): $(TARGET)/synopsys_sim.setupts
	-@echo "============================================"
	-@echo "Creating physical library '$(subst .libts,,$@)'"
	$(MKDIR) $(subst .libts,,$@)
	-@echo "============================================"
	-@echo "Mapping logical library '$(subst .libts,,$(notdir $@))' to physical path '$(SCVLIBS)/$(TARGET)/$(subst .libts,,$(notdir $@))'"
	$(ECHO) "$(subst .libts,,$(notdir $@)) : $(SCVLIBS)/$(TARGET)/$(subst .libts,,$(notdir $@))" >>$(TARGET)/synopsys_sim.setup
	$(TIMESTAMP)

$(foreach lib,$(VNDR_HDL_LIBS),$(TARGET)/$(notdir $(lib))):
	-@echo "============================================"
	-@echo "Mapping vendor logical library '$(HDL_LOGICAL)' to physical path '$(subst .log2phyts,,$<)'"
	$(ECHO) "$(HDL_LOGICAL) : $(subst .log2phyts,,$<)" >>$(TARGET)/sysopsys_sim.setup
	$(TIMESTAMP)

# Create the target directory
$(TARGET)/make_dir: 
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))$(PATHSEP)sysc
	$(TIMESTAMP)

# Create the initial synopsys_sim.setup file
# This must preceed any actual VCS compilation rules
$(TARGET)/synopsys_sim.setupts: $(TARGET)/make_dir
	-@echo "============================================"
	-@echo "Setting up synopsys_sim.setup file"
	$(ECHO) "WORK > DEFAULT" >>$(TARGET)$(PATHSEP)synopsys_sim.setup
	$(ECHO) "DEFAULT : $(TARGET)$(PATHSEP)work" >>$(TARGET)$(PATHSEP)synopsys_sim.setup
	$(TIMESTAMP)

# Targets start here
$(TARGET)/make_libs : $(foreach lib,$(HDL_LIB_NAMES),$(TARGET)/$(lib).libts) $(VNDR_HDL_LIBS) $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAPS_V) $(VENDOR_LIBMAPS_VHD)
	$(TIMESTAMP)

# Since VCS-MX uses Make under the hood, avoid passing down --always-make
# in the MAKEFLAGS variable by filtering out the switch 'B' from the
# current MAKEFLAGS value and overriding that variable for the top
# three SCVerify targets.
MAKEFLAGS_OVERRIDE := $(subst B,,$(MAKEFLAGS))
build: MAKEFLAGS=$(MAKEFLAGS_OVERRIDE)
sim: MAKEFLAGS=$(MAKEFLAGS_OVERRIDE)
simgui: MAKEFLAGS=$(MAKEFLAGS_OVERRIDE)

# May need to place SC_ARGS during elab using "-syscelab $(SC_ARGS)" on vcs line below
$(TARGET)/$(TOP_DU)$(PWR_EXE_EXT)ts : SYNOPSYS_SIM_SETUP=$(TARGET)$(PATHSEP)synopsys_sim.setup
$(TARGET)/$(TOP_DU)$(PWR_EXE_EXT)ts : $(TMP_VHDL_SRC) $(TMP_VLOG_SRC) $(TMP_CXX_SRC) .ccs_env_opts/VCS_VCSELAB_OPTS.ts
ifneq "$(CXX_SRC)" ""
	-@echo "============================================"
	-@echo "Elaborating design"
	-@echo "SYNOPSYS_SIM_SETUP: $(SYNOPSYS_SIM_SETUP)"
ifneq "$(LINK_LIBNAMES)" ""
	$(VCS) $(FULL64) +libverbose $(VCSCMOPTS) $(VCS_LIC_WAIT) -Mdir=$(TARGET) $(VCSELAB_OPTS) -sysc=$(SYSC_VERSION) sc_main -o $(TARGET)/$(TOP_DU)$(PWR_EXE_EXT) -LDFLAGS "$(ADDED_LIBPATHS)" -syslib $(ADDED_LIBNAMES) -l $(VCS_LOG)
else
	$(VCS) $(FULL64) +libverbose $(VCSCMOPTS) $(VCS_LIC_WAIT) -Mdir=$(TARGET) $(VCSELAB_OPTS) -sysc=$(SYSC_VERSION) sc_main -o $(TARGET)/$(TOP_DU)$(PWR_EXE_EXT) -LDFLAGS "$(ADDED_LIBPATHS)" -l $(VCS_LOG)
endif
endif
	$(TIMESTAMP)

$(TARGET)/scverify_vcs_wave.tcl: $(WORK2SOLN)/scverify/ccs_wave_signals.dat
	-@echo "============================================"
	-@echo "Creating VCS-MX wave TCL file '$@'"
	$(TCLSH_CMD) $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_vcs.flo create_vcs_wave $(PROJ2SOLN)/scverify/ccs_wave_signals.dat $@ $(DUTINSTINFOFILE) $(VCS_DOFILE) 0

build: $(TARGET)/make_libs $(TARGET)/scverify_vcs_wave.tcl $(TARGET)/$(TOP_DU)$(PWR_EXE_EXT)ts

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
	-@echo "SYNOPSYS_SIM_SETUP: $(SYNOPSYS_SIM_SETUP)"
	$(CD) $(WORK2PROJ)$(;) $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(PWR_EXE_EXT) -systemcrun $(SC_ARGS) $(VCSSIM_OPTS) -verilogrun $(SIMVCMOPTS) $(VCS_LIC_WAIT) -ucli -ucli2Proc -i $(PROJ2WORK)/$(TARGET)/scverify_vcs_wave.tcl -l $(VCS_LOG)
ifeq "$(ENABLE_CODE_COVERAGE)" "true"
endif

simgui: build
	-@echo "============================================"
	-@echo "Simulating design entity: $(TOP_DU)"
	$(CD) $(WORK2PROJ)$(;) $(PROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(PWR_EXE_EXT) -systemcrun $(SC_ARGS) $(VCSSIM_OPTS) -verilogrun $(SIMVCMOPTS) $(VCS_LIC_WAIT) -ucli -gui -i $(PROJ2WORK)/$(TARGET)/scverify_vcs_wave.tcl -l $(VCS_LOG) 

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

.PHONY : help
help: helptext dumpsysvars dumpvars
	@-$(ECHO) "   SCVerify flow options:"
	@-$(ECHO) "     INVOKE_ARGS           = $(INVOKE_ARGS)"
	@-$(ECHO) "     INCL_DIRS             = $(INCL_DIRS)"
	@-$(ECHO) "     ADDED_LIBPATHS        = $(ADDED_LIBPATHS)"
	@-$(ECHO) "     ADDED_LIBNAMES        = $(ADDED_LIBNAMES)"
	@-$(ECHO) "     CCS_VCD_FILE          = $(CCS_VCD_FILE)"
	@-$(ECHO) "     VNDR_HDL_LIBS         = $(VNDR_HDL_LIBS)"

helptext:
	@-$(ECHO) "VCS-MX Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   simgui     Compile and Execute the simulation using the"
	@-$(ECHO) "              interactive mode of the simulator (if available)"
	@-$(ECHO) "   sim        Compile and Execute the simulation using the"
	@-$(ECHO) "              batch mode of the simulator"
	@-$(ECHO) "   build      Compile the models only"
	@-$(ECHO) "   clean      Remove all compiled objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

dumpvars:
	@-$(ECHO) "   VCS flow options:"
	@-$(ECHO) "     VCS_HOME              = $(VCS_HOME)"
	@-$(ECHO) "     VG_GNU_PACKAGE        = $(VG_GNU_PACKAGE)"
	@-$(ECHO) "     VG_ENV_SCRIPT         = $(VG_ENV_SCRIPT)"
	@-$(ECHO) "     VCS_MODE              = $(VCS_MODE)"
	@-$(ECHO) "     FORCE_32BIT           = $(FORCE_32BIT)"
	@-$(ECHO) "     COMP_FLAGS            = $(COMP_FLAGS)"
	@-$(ECHO) "     LD_LIBRARY_PATH       = $(LD_LIBRARY_PATH)"
	@-$(ECHO) "     VHDLAN_OPTS           = $(VHDLAN_OPTS)"
	@-$(ECHO) "     VLOGAN_OPTS           = $(VLOGAN_OPTS)"
	@-$(ECHO) "     VCSELAB_OPTS          = $(VCSELAB_OPTS)"
	@-$(ECHO) "     VCSSIM_OPTS           = $(VCSSIM_OPTS)"
	@-$(ECHO) "     VLOG_INCDIR_OPT       = $(VLOG_INCDIR_OPT)"
	@-$(ECHO) "     VLOG_DEFS_OPT         = $(VLOG_DEFS_OPT)"
	@-$(ECHO) "     VCS_DOFILE            = $(VCS_DOFILE)"


