
# Turn off old code if new flow is selected (Beta)
ifeq "$(Xilinx_NEW_SIMLIB_FLOW)" "false"

# 
# XILINX simulation settings
# 

XILINX = $(Xilinx_XILINX)
XILINX_LIB = $(Xilinx_XILINX_LIB)

ifeq "$(XILINX)" ""
   $(error The XILINX environment variable must be set to use this makefile)
endif

ifeq "$(XILINX_LIB)" ""
   $(warning Warning: The XILINX_LIB variable was not set. Setting it to XILINX)
XILINX_LIB = $(XILINX)
endif

export XILINX

ifeq "$(CXX_OS)" "Windows_NT"
XILINX_PLAT = nt
else
ifeq "$(findstring CYGWIN,$(shell uname))" "CYGWIN"
XILINX_PLAT = nt
else
ifeq "$(shell uname -s)" "Linux"
XILINX_PLAT = lin
else
ifeq "$(shell uname -s)" "SunOS"
XILINX_PLAT = sol
else
$(error Unknown platform - could not determine location of Xilinx installation)
endif
endif
endif
endif

ifneq "$(CXX_OS)" "Windows_NT"
LD_LIBRARY_PATH=$(MGC_HOME)/lib:$(MGC_HOME)/shared/lib:$(XILINX)/bin/$(XILINX_PLAT)
export LD_LIBRARY_PATH
endif

COMPXLIB_EXEC = $(XILINX)/bin/$(XILINX_PLAT)/compxlib

XPWR_EXEC = $(XILINX)/bin/$(XILINX_PLAT)/xpwr

SIMULATOR := unknown
SIM_PATH := unknown

# Possible compxlib -s <sim> settings
#   mti_se, mti_pe, questa, ncsim, vcs_mx, vcs_mxi
ifeq "$(SIMTOOL)" "msim"
  BANNER := $(shell $(MODEL_TECH)/vsim -version)
  SIM_PATH := $(MODEL_TECH)
  ifneq "$(findstring Questa,$(BANNER))" ""
    SIMULATOR := questa
  else
    SIMULATOR := mti_se
  endif
else
  ifeq "$(SIMTOOL)" "ncsim"
    $(warning Warning: Xilinx precompiled simulation libraries for NCSim are not supported)
    SIM_PATH := $(NC_ROOT)
    SIMULATOR := ncsim
  else
    ifeq "$(SIMTOOL)" "vcs"
      $(warning Warning: Xilinx precompiled simulation libraries for VCS-MX are not supported)
      SIM_PATH := $(VCS_HOME)
      SIMULATOR := vcs_mx
    endif
  endif
endif

# Make variables XILINX_VHDL_LIB and XILINX_VERILOG_LIB specialize the
# path to the precompiled libraries.
ifeq "$(XILINX_VHDL_LIB)" ""
XILINX_VHDL_LIB = $(XILINX_LIB)/vhdl/$(SIMULATOR)
endif
ifeq "$(XILINX_VERILOG_LIB)" ""
XILINX_VERILOG_LIB = $(XILINX_LIB)/verilog/$(SIMULATOR)
endif

#------------------------------------------------------------
# The following is for VHDL and Verilog netlist simulation only
ifneq "$(NETLIST)" "cxx"

# Variables for performing mapped or gate simulation
VNDR_NETDIR    = .
VNDR_SDFSUF    = _out.sdf
ifeq "$(STAGE)" "gate"
VNDR_SDFINST   = scverify_top/rtl
else
VNDR_SDFINST   =
endif

#---------------------------------------
# VHDL-specific settings and libraries
ifeq "$(NETLIST)" "vhdl"

# Variables for performing mapped or gate simulation
VNDR_NETSUF    = _out.vhd

# Define the list of logical libraries for Xilinx simulation (only if the user has not provided there own modelsim.ini)
ifeq "$(DEF_MODELSIM_INI)" ""
# Start with libraries needed for gate simulation
X_XILINX_SIM_LIBS = unisim simprim
ifneq "$(XILINX_SIM_LIBS)" ""
# Add in accelerated libraries (passed from flow)
X_XILINX_SIM_LIBS += $(XILINX_SIM_LIBS)
endif

# Now establish path to physical library
XILINX_VHDL_UNISIM  := $(XILINX_VHDL_LIB)/unisim
XILINX_VHDL_SIMPRIM := $(XILINX_VHDL_LIB)/simprim
XILINX_VHDL_CORELIB := $(XILINX_VHDL_LIB)/XilinxCoreLib

else

# Assume the user has configured all libs already
$(warning DEF_MODELSIM_INI set)
# Now establish path to physical library using vmap (Questa SIM specific)
XILINX_VHDL_UNISIM_VMAP := $(shell $(MODEL_TECH)/vmap unisim)
XILINX_VHDL_UNISIM := $(basename $(word $(words $(XILINX_VHDL_UNISIM_VMAP)),$(XILINX_VHDL_UNISIM_VMAP)))
ifeq "$(XILINX_VHDL_UNISIM)" "unisim"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'unisim'. Check your modelsim.ini file.)
endif
XILINX_VHDL_SIMPRIM_VMAP := $(shell $(MODEL_TECH)/vmap simprim)
XILINX_VHDL_SIMPRIM := $(basename $(word $(words $(XILINX_VHDL_SIMPRIM_VMAP)),$(XILINX_VHDL_SIMPRIM_VMAP)))
ifeq "$(XILINX_VHDL_SIMPRIM)" "simprim"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'simprim'. Check your modelsim.ini file.)
endif
XILINX_VHDL_CORELIB_VMAP := $(shell $(MODEL_TECH)/vmap XilinxCoreLib)
XILINX_VHDL_CORELIB := $(basename $(word $(words $(XILINX_VHDL_CORELIB_VMAP)),$(XILINX_VHDL_CORELIB_VMAP)))
ifeq "$(XILINX_VHDL_CORELIB)" "XilinxCoreLib"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'XilinxCoreLib'. Check your modelsim.ini file.)
endif

endif

# Specify location of physical library and name of logical library (often the same)
$(TARGET)/unisim.log2phyts: $(XILINX_VHDL_UNISIM)
$(TARGET)/unisim.log2phyts: HDL_LOGICAL=unisim

$(TARGET)/simprim.log2phyts: $(XILINX_VHDL_SIMPRIM)
$(TARGET)/simprim.log2phyts: HDL_LOGICAL=simprim

$(TARGET)/XilinxCoreLib.log2phyts: $(XILINX_VHDL_CORELIB)
$(TARGET)/XilinxCoreLib.log2phyts: HDL_LOGICAL=XilinxCoreLib

#---------------------------------------
# Verilog-specific settings and libraries
else
ifeq "$(NETLIST)" "v"

# Variables for performing mapped or gate simulation
VNDR_NETSUF    = _out.v
VLOG_OPTS     += $(XILINX)/verilog/src/glbl.v
SIM_OPTS      += glbl

# Define the list of logical libraries for Xilinx simulation (only if the user has not provided there own modelsim.ini)
ifeq "$(DEF_MODELSIM_INI)" ""
# Start with libraries needed for gate simulation
X_XILINX_SIM_LIBS = unisims_ver simprims_ver
ifneq "$(XILINX_SIM_LIBS)" ""
# Add in accelerated libraries (passed from flow)
X_XILINX_SIM_LIBS += $(foreach lib,$(XILINX_SIM_LIBS),$(lib)_ver)
endif

# Now establish path to physical library
XILINX_VER_UNISIM := $(XILINX_VERILOG_LIB)/unisims_ver
XILINX_VER_SIMPRIM := $(XILINX_VERILOG_LIB)/simprims_ver
XILINX_VER_CORELIB := $(XILINX_VERILOG_LIB)/XilinxCoreLib_ver

else

# Assume the user has configured all libs already
$(warning DEF_MODELSIM_INI set)
# Now establish path to physical library using vmap (Questa SIM specific)
XILINX_VER_UNISIM_VMAP := $(shell $(MODEL_TECH)/vmap unisims_ver)
XILINX_VER_UNISIM := $(basename $(word $(words $(XILINX_VER_UNISIM_VMAP)),$(XILINX_VER_UNISIM_VMAP)))
ifeq "$(XILINX_VER_UNISIM)" "unisims_ver"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'unisims_ver'. Check your modelsim.ini file.)
endif
XILINX_VER_SIMPRIM_VMAP := $(shell $(MODEL_TECH)/vmap simprims_ver)
XILINX_VER_SIMPRIM := $(basename $(word $(words $(XILINX_VER_SIMPRIM_VMAP)),$(XILINX_VER_SIMPRIM_VMAP)))
ifeq "$(XILINX_VER_SIMPRIM)" "simprims_ver"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'simprims_ver'. Check your modelsim.ini file.)
endif
XILINX_VER_CORELIB_VMAP := $(shell $(MODEL_TECH)/vmap XilinxCoreLib_ver)
XILINX_VER_CORELIB := $(basename $(word $(words $(XILINX_VER_CORELIB_VMAP)),$(XILINX_VER_CORELIB_VMAP)))
ifeq "$(XILINX_VER_CORELIB)" "XilinxCoreLib_ver"
$(error Error: ccs_Xilinx.mk - Could not locate library mapping for 'XilinxCoreLib_ver'. Check your modelsim.ini file.)
endif

endif

# Specify location of physical library and name of logical library (often the same)
$(TARGET)/unisims_ver.log2phyts: $(XILINX_VER_UNISIM)
$(TARGET)/unisims_ver.log2phyts: HDL_LOGICAL=unisims_ver

$(TARGET)/simprims_ver.log2phyts: $(XILINX_VER_SIMPRIM)
$(TARGET)/simprims_ver.log2phyts: HDL_LOGICAL=simprims_ver

$(TARGET)/XilinxCoreLib_ver.log2phyts: $(XILINX_VER_CORELIB)
$(TARGET)/XilinxCoreLib_ver.log2phyts: HDL_LOGICAL=XilinxCoreLib_ver

endif
endif

endif
# End of VHDL and Verilog section
#------------------------------------------------------------

#===============================================================================
# DO NOT EDIT ANYTHING BELOW THIS LINE
# This variable is processed by the actual simulator makefile to create the
# library mappings
VNDR_HDL_LIBS = $(foreach lib,$(X_XILINX_SIM_LIBS),$(TARGET)/$(lib).log2phyts)

.PHONY: vendor_precompile
vendor_precompile: make_xilinx_lib vendor_precompile_vhdl vendor_precompile_verilog

N_XILINX_VHDL_LIB = $(subst /,$(PATHSEP),$(subst \,/,$(XILINX_VHDL_LIB)))
N_XILINX_VERILOG_LIB = $(subst /,$(PATHSEP),$(subst \,/,$(XILINX_VERILOG_LIB)))

MSIM_INI = $(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)

.PHONY: make_xilinx_lib
make_xilinx_lib:
	-@$(MKDIR) $(N_XILINX_VHDL_LIB)
	-@$(CAT) $(MSIM_INI) >$(N_XILINX_VHDL_LIB)$(PATHSEP)modelsim.ini
	-@$(MKDIR) $(N_XILINX_VERILOG_LIB)
	-@$(CAT) $(MSIM_INI) >$(N_XILINX_VERILOG_LIB)$(PATHSEP)modelsim.ini

vendor_precompile_vhdl: MODELSIM=$(XILINX_VHDL_LIB)/modelsim.ini

.PHONY: vendor_precompile_vhdl
vendor_precompile_vhdl: make_xilinx_lib
	$(ECHO) "Generating XILINX VHDL precompiled libraries using Questa SIM"
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l vhdl -arch all -lib unisim -p $(SIM_PATH) -dir $(XILINX_VHDL_LIB)
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l vhdl -arch all -lib simprim -p $(SIM_PATH) -dir $(XILINX_VHDL_LIB)
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l vhdl -arch all -lib xilinxcorelib -p $(SIM_PATH) -dir $(XILINX_VHDL_LIB)

vendor_precompile_verilog: MODELSIM=$(XILINX_VERILOG_LIB)/modelsim.ini

.PHONY: vendor_precompile_verilog
vendor_precompile_verilog: make_xilinx_lib
	$(ECHO) "Generating XILINX Verilog precompiled libraries using Questa SIM"
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l verilog -arch all -lib unisim -p $(SIM_PATH) -dir $(XILINX_VERILOG_LIB)
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l verilog -arch all -lib simprim -p $(SIM_PATH) -dir $(XILINX_VERILOG_LIB)
	$(COMPXLIB_EXEC) -w -s $(SIMULATOR) -l verilog -arch all -lib xilinxcorelib -p $(SIM_PATH) -dir $(XILINX_VERILOG_LIB)


.PHONY: dump_Xilinx_vars
dump_Xilinx_vars:
	-@$(ECHO) "   XILINX              = $(XILINX)"
	-@$(ECHO) "   XILINX_LIB          = $(XILINX_LIB)"

else

#===============================================================================
# New support for precompiled Xilinx simulation libraries (Beta)
#===============================================================================
# Xilinx Flow Options
#    /Xilinx/SIMLIBS_V - List of verilog logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Xilinx/SIMLIBS_VHD - List of VHDL logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Xilinx/COMPXLIB_DIR - Pathname to output of the compxlib
#                      (or compslib) utilities.
#                      May be empty.
#                      Default value of option is "" (empty string).
#    /Xilinx/XILINX        - Pathname to ISE install directory
#    /Xilinx/XILINX_VIVADO - Pathname to Vivado install directory
#
# Variables inside this makefile
#    Xilinx_SIMLIBS_V    - The value from the option /Xilinx/SIMLIBS_V (set in ccs_env.mk).
#    Xilinx_SIMLIBS_VHD  - The value from the option /Xilinx/SIMLIBS_VHD (set in ccs_env.mk).
#    Xilinx_COMPXLIB_DIR - The value from the option /Xilinx/COMPXLIB_DIR (set in ccs_env.mk).
#
#    SIMLIBS_V    - Initialized to the value of Xilinx_SIMLIBS_V.
#                            If empty, this makefile does nothing.
#                            (future) If empty AND accelerated libraries are in use, will be set to
#                            "unisims_ver simprims_ver".
#    SIMLIBS_VHD  - Initialized to the value of Xilinx_SIMLIBS_VHD.
#                            If empty, this makefile does nothing.
#                            (future) If empty AND accelerated libraries are in use, will be set to
#                            "unisim".
#    COMPXLIB_DIR - Initialized to the value of Xilinx_COMPXLIB_DIR.
#                          If empty, it is set to $(PROJ_DIR)/xilinx_cache (to
#                          populate a precompiled library cache in the directory for this
#                          project (would be shared by all solutions).
#
#    VENDOR_LIBMAP_FILE - If SIMTOOL=msim, this variable is $(Xilinx_COMPXLIB_DIR)/modelsim.ini
#                         If SIMTOOL=vcs, this variable is $(Xilinx_COMPXLIB_DIR)/synopsys_sim.setup
#                         If the named file does not exist it triggers running compxlib
#                         to populate the directory based on SIMTOOL.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_LIBMAPS_V   - This variable is a list of names
#                             <logical>.<physical>.libmapts
#                         Where <logical> is the logical library name (from SIMLIBS_V)
#                         and <physical> is the leaf directory name in COMPXLIB_DIR.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_LIBMAPS_VHD - This variable is a list of names
#                             <logical>.<physical>.libmapts
#                         Where <logical> is the logical library name (from SIMLIBS_VHD)
#                         and <physical> is the leaf directory name in COMPXLIB_DIR.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_PHYDIR      - For Xilinx Xilinx, this variable is $(COMPXLIB_DIR).
#                         This variable is processed by the simulator-specific makefile.
#    Variables VENDOR_LIBMAP_FILE, VENDOR_LIBMAPS_V and VENDOR_LIBMAPS_VHD must be added to the make target $(TARGET)/make_libs
#    in all supported simulator makefiles:
#        $(TARGET)/make_libs : ... $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAPS_V) $(VENDOR_LIBMAPS_VHD)
#------------------------------------------------------------

# Get option settings from Catapult Xilinx Flow
SIMLIBS_V    := $(Xilinx_SIMLIBS_V)
SIMLIBS_VHD  := $(Xilinx_SIMLIBS_VHD)
SIMLIBS      := $(Xilinx_SIMLIBS_V) $(Xilinx_SIMLIBS_VHD)
COMPXLIB_DIR := $(Xilinx_COMPXLIB_DIR)

# =============================================================================================
# Only do something if we have a precompiled library request
ifneq "$(SIMLIBS)" ""

#------------------------------------------------------------
# Determine Vivado executable to use
VIVADO_EXEC := $(Vivado_XILINX_VIVADO)/bin/vivado

#------------------------------------------------------------
# Determine location of Xilinx GLBL source and add it as HDL dependency
ifeq "$(NETLIST)" "v"
GLBL_V_SRC := $(Vivado_XILINX_VIVADO)/data/verilog/src/glbl.v
VLOG_SRC += $(GLBL_V_SRC).vts
$(TARGET)/glbl.v.vts: $(GLBL_V_SRC)
$(TARGET)/glbl.v.vts: HDL_LIB=work
OPTIONAL_DU += glbl
endif

SIMULATOR := unknown
SIM_PATH := unknown

# ---------------------------------------------------------------------------------------------
# QuestaSim SUPPORT
ifeq "$(SIMTOOL)" "msim"

MODEL_TECH := $(ModelSim_Path)
BANNER := $(shell $(MODEL_TECH)/vsim -version)
SIM_PATH := $(MODEL_TECH)
ifneq "$(findstring Questa,$(BANNER))" ""
  SIMULATOR := questa
else
  SIMULATOR := mti_se
endif

COMP_SIMLIB_OPTS := -32bit
ifeq "$(ModelSim_FORCE_32BIT)" "no"
# change default to empty to allow 64bit compile
COMP_SIMLIB_OPTS := 
endif

# Only do something for Questa SIM if no default modelsim.ini file specified
ifeq "$(ModelSim_DEF_MODELSIM_INI)" ""

# If user's options do not specify a home or site location for precompiled libraries...
ifeq "$(COMPXLIB_DIR)" ""
# ...default to the working directory of the project
COMPXLIB_DIR := $(WORK_DIR)/$(WORK2PROJ)/xilinx_cache
# Make the directory if it doesnt exist
$(COMPXLIB_DIR):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory for Xilinx precompiled simulation library cache $<"
	$(MKDIR) $@
endif

# Set VENDOR_LIBMAP_FILE to mark dependency, define translate rule to run compxlib if needed
VENDOR_LIBMAP_FILE := $(COMPXLIB_DIR)/modelsim.ini
DFI = $(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)
XDFI = $(subst /,$(PATHSEP),$(subst \,$(PATHSEP),$(DFI)))
DXDFI = $(subst $(space),\$(space),$(XDFI))
$(VENDOR_LIBMAP_FILE): MODELSIM=$(COMPXLIB_DIR)/modelsim.ini
$(VENDOR_LIBMAP_FILE): 
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating XILINX precompiled libraries using 'compile_simlib' with Questa SIM (This may take a few minutes)."
	-@$(ECHO) "Precompiled library cache location: $(COMPXLIB_DIR)"
	-@$(ECHO) "Compilation logfile saved at $(COMPXLIB_DIR)/compxlib.log"
	@$(MKDIR) $(COMPXLIB_DIR)
	@$(CAT) "$(XDFI)" >$(COMPXLIB_DIR)/modelsim.ini
	@$(ECHO) "compile_simlib $(COMP_SIMLIB_OPTS) -simulator $(SIMULATOR) -simulator_exec_path $(SIM_PATH) -dir $(COMPXLIB_DIR)" >$(COMPXLIB_DIR)/input.tcl
	@$(VIVADO_EXEC) -mode tcl -source $(COMPXLIB_DIR)/input.tcl -log $(COMPXLIB_DIR)/compxlib.log

# Build list of lib mappings
VENDOR_PHYDIR := $(COMPXLIB_DIR)
VENDOR_LIBMAPS_V := $(foreach i,$(SIMLIBS_V),$(TARGET)/$(i).$(i).libmapts)
VENDOR_LIBMAPS_VHD := $(foreach i,$(SIMLIBS_VHD),$(TARGET)/$(i).$(i).libmapts)

# end ifeq "$(ModelSim_DEF_MODELSIM_INI)" ""
endif
# end ifeq "$(SIMTOOL)" "msim"
endif
# ---------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------
# VCS-MX SUPPORT
ifeq "$(SIMTOOL)" "vcs"
SIM_PATH := $(VCS_HOME)
SIMULATOR := vcs_mx
$(error Warning: Xilinx precompiled simulation libraries for VCS-MX are not supported)
endif
# ---------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------
# NCSim SUPPORT
ifeq "$(SIMTOOL)" "ncsim"
SIM_PATH := $(NC_ROOT)
SIMULATOR := ncsim
$(error Warning: Xilinx precompiled simulation libraries for NCSim are not supported)
endif
# ---------------------------------------------------------------------------------------------

# end ifeq "$(SIMLIBS)" ""
endif

.PHONY: dump_Xilinx_vars
dump_Xilinx_vars:
	-@$(ECHO) "   SIMLIBS_V           = $(Xilinx_SIMLIBS_V)"
	-@$(ECHO) "   SIMLIBS_VHD         = $(Xilinx_SIMLIBS_VHD)"
	-@$(ECHO) "   COMPXLIB_DIR        = $(Xilinx_COMPXLIB_DIR)"
	-@$(ECHO) "   VENDOR_LIBMAP_FILE  = $(VENDOR_LIBMAP_FILE)"
	-@$(ECHO) "   VENDOR_PHYDIR       = $(VENDOR_PHYDIR)"
	-@$(ECHO) "   VENDOR_LIBMAPS_V    = $(VENDOR_LIBMAPS_V)"
	-@$(ECHO) "   VENDOR_LIBMAPS_VHD  = $(VENDOR_LIBMAPS_VHD)"
# =============================================================================================

endif
