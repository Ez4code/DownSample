#===============================================================================
# File: ccs_VivadoLibs.mk
#-------------------------------------------------------------------------------
# Support for precompiled Vivado simulation libraries
#===============================================================================
# Vivado Flow Options
#    /Vivado/SIMLIBS_V - List of verilog logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Vivado/SIMLIBS_VHD - List of VHDL logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Vivado/PCL_CACHE - Pathname to output of the compxlib
#                      (or compslib) utilities.
#                      May be empty.
#                      Default value of option is "" (empty string).
#    /Vivado/XILINX_VIVADO - Pathname to Vivado install directory
#
# Variables inside this makefile
#    Vivado_SIMLIBS_V    - The value from the option /Vivado/SIMLIBS_V (set in ccs_env.mk).
#    Vivado_SIMLIBS_VHD  - The value from the option /Vivado/SIMLIBS_VHD (set in ccs_env.mk).
#    Vivado_PCL_CACHE    - The value from the option /Vivado/PCL_CACHE (set in ccs_env.mk).
#
#    SIMLIBS_V    - Initialized to the value of Vivado_SIMLIBS_V.
#                            If empty, this makefile does nothing.
#                            If accelerated libraries are in use, top-level makefile
#                            will initialize SIMLIBS_V to the value(s) of <techlibs>/VARS/SIMLIBS_V
#                            before appending any value from Vivado_SIMLIBS_V.
#    SIMLIBS_VHD  - Initialized to the value of Vivado_SIMLIBS_VHD.
#                            If empty, this makefile does nothing.
#                            If accelerated libraries are in use, top-level makefile
#                            will initialize SIMLIBS_VHD to the value(s) of <techlibs>/VARS/SIMLIBS_VHD
#                            before appending any value from Vivado_SIMLIBS_VHD.
#    SIMLIBS - Combination of SIMLIBS_V and SIMLIBS_VHD (only used to determine if there is
#                          any library compilation/mapping work to be done by this makefile)
#    PCL_CACHE    - Initialized to the value of Vivado_PCL_CACHE.
#                          If empty, it is set to $(PROJ_DIR)/xilinx_cache (to
#                          populate a precompiled library cache in the directory for this
#                          project (would be shared by all solutions).
#
#    VENDOR_LIBMAP_FILE - If SIMTOOL=msim, this variable is $(Vivado_PCL_CACHE)/modelsim.ini
#                         If SIMTOOL=vcs, this variable is $(Vivado_PCL_CACHE)/synopsys_sim.setup
#                         If the named file does not exist it triggers running compxlib
#                         to populate the directory based on SIMTOOL.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_LIBMAPS_V   - This variable is a list of names
#                             <logical>.<physical>.libmapts
#                         Where <logical> is the logical library name (from SIMLIBS_V)
#                         and <physical> is the leaf directory name in PCL_CACHE.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_LIBMAPS_VHD - This variable is a list of names
#                             <logical>.<physical>.libmapts
#                         Where <logical> is the logical library name (from SIMLIBS_VHD)
#                         and <physical> is the leaf directory name in PCL_CACHE.
#                         This variable is processed by the simulator-specific makefile.
#    VENDOR_PHYDIR      - For Vivado, this variable is $(PCL_CACHE).
#                         This variable is processed by the simulator-specific makefile.
#    Variables VENDOR_LIBMAP_FILE, VENDOR_LIBMAPS_V and VENDOR_LIBMAPS_VHD must be added to the make target $(TARGET)/make_libs
#    in all supported simulator makefiles:
#        $(TARGET)/make_libs : ... $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAPS_V) $(VENDOR_LIBMAPS_VHD)
#------------------------------------------------------------

# Get option settings from Catapult Vivado Flow
PCL_CACHE        := $(Vivado_PCL_CACHE)
# Add those logical library names to any list of names from the top-level makefile
ifeq "$(NETLIST)" "vhdl"
SIMLIBS_VHD         += $(Vivado_SIMLIBS_VHD)
else
SIMLIBS_V           += $(Vivado_SIMLIBS_V)
endif
ifeq "$(STAGE)" "gate"
ifeq "$(NETLIST)" "vhdl"
SIMLIBS_VHD         += unisim
else
SIMLIBS_V           += unisims_ver simprims_ver
endif
endif
SIMLIBS             = $(SIMLIBS_V) $(SIMLIBS_VHD)

# =============================================================================================
# Only do something if we have a precompiled library request
ifneq "$(SIMLIBS)" ""

#------------------------------------------------------------
# Determine Vivado executable to use
ifeq "$(Vivado_XILINX_VIVADO)" ""
VIVADO_EXEC := $(XILINX_VIVADO)/bin/vivado
else
VIVADO_EXEC := $(Vivado_XILINX_VIVADO)/bin/vivado
endif

#------------------------------------------------------------
# Determine location of Vivado GLBL source and add it as HDL dependency
ifeq "$(NETLIST)" "v"
GLBL_V_SRC := $(Vivado_XILINX_VIVADO)/data/verilog/src/glbl.v
VLOG_SRC += $(GLBL_V_SRC).vts
$(TARGET)/glbl.v.vts: $(GLBL_V_SRC)
$(TARGET)/glbl.v.vts: HDL_LIB=work
OPTIONAL_DU += glbl
endif
# include the Verilog GSR model since it is easier to include than the VHDL version
ifeq "$(NETLIST)" "vhdl"
GLBL_V_SRC := $(Vivado_XILINX_VIVADO)/data/verilog/src/glbl.v
VLOG_SRC += $(GLBL_V_SRC).vts
$(TARGET)/glbl.v.vts: $(GLBL_V_SRC)
$(TARGET)/glbl.v.vts: HDL_LIB=work
OPTIONAL_DU += glbl
endif

SIMULATOR := unknown
SIM_PATH := unknown

# ---------------------------------------------------------------------------------------------
# BEGIN QuestaSim SUPPORT
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

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# If the user is controlling the location of precompiled library
# mappings directly by setting the default MODELSIM_INI option
# in the ModelSim flow then we have nothing to do.
# So only do something for ModelSim/QuestaSim if no default 
# modelsim.ini file is specified.
ifeq "$(ModelSim_DEF_MODELSIM_INI)" ""

# If user's options do not specify a home or site location for precompiled libraries...
ifeq "$(PCL_CACHE)" ""
# ...default to the working directory of the project
PCL_CACHE := $(WORK_DIR)/$(WORK2PROJ)/xilinx_cache
# Make the directory if it doesnt exist
$(PCL_CACHE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory for Vivado precompiled simulation library cache $<"
	$(MKDIR) $(subst /,$(PATHSEP),$@)
endif

# Set VENDOR_LIBMAP_FILE to mark dependency, define translate rule to run compxlib if needed
VENDOR_LIBMAP_FILE := $(PCL_CACHE)/modelsim.ini
DFI = $(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)
XDFI = $(subst /,$(PATHSEP),$(subst \,$(PATHSEP),$(DFI)))
DXDFI = $(subst $(space),\$(space),$(XDFI))
$(VENDOR_LIBMAP_FILE): MODELSIM=$(PCL_CACHE)/modelsim.ini
$(VENDOR_LIBMAP_FILE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating Xilinx precompiled libraries using 'compile_simlib' with ModelSim/QuestaSim (This may take a few minutes)."
	-@$(ECHO) "Precompiled library cache location: $(PCL_CACHE)"
	-@$(ECHO) "Compilation logfile saved at $(PCL_CACHE)/compxlib.log"
	@$(MKDIR) $(subst /,$(PATHSEP),$(PCL_CACHE))
	-@$(ECHO) " - Configuring $(SIMULATOR) to suppress message 12110"
	@$(TCLSED) $(XDFI) $(PCL_CACHE)/modelsim.ini "suppress = " "suppress = 12110,"
#	@$(CAT) "$(XDFI)" >$(PCL_CACHE)/modelsim.ini
	@$(ECHO) "compile_simlib $(COMP_SIMLIB_OPTS) -simulator $(SIMULATOR) -simulator_exec_path $(SIM_PATH) -dir $(PCL_CACHE)" >$(PCL_CACHE)/input.tcl
	@$(ECHO) "exit" >>$(PCL_CACHE)/input.tcl
	@$(VIVADO_EXEC) -mode tcl -source $(PCL_CACHE)/input.tcl -log $(PCL_CACHE)/compxlib.log

# Build list of lib mappings
VENDOR_PHYDIR := $(PCL_CACHE)
VENDOR_LIBMAPS_V   = $(foreach i,$(sort $(SIMLIBS_V)),$(TARGET)/.$(i).$(i).libmapts)
VENDOR_LIBMAPS_VHD = $(foreach i,$(sort $(SIMLIBS_VHD)),$(TARGET)/.$(i).$(i).libmapts)

# end ifeq "$(ModelSim_DEF_MODELSIM_INI)" ""
endif

# end ifeq "$(SIMTOOL)" "msim"
endif
# END QuestaSim SUPPORT
# ---------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------
# BEGIN VCS-MX SUPPORT
ifeq "$(SIMTOOL)" "vcs"
SIM_PATH := $(VCS_HOME)/bin
SIMULATOR := vcs_mx

# If user's options do not specify a home or site location for precompiled libraries...
ifeq "$(PCL_CACHE)" ""
# ...default to the working directory of the project
PCL_CACHE := $(WORK_DIR)/$(WORK2PROJ)/xilinx_cache
# Make the directory if it doesnt exist
$(PCL_CACHE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory for Xilinx precompiled simulation library cache $<"
	$(MKDIR) $@
endif

# Set VENDOR_LIBMAP_FILE to mark dependency, define translate rule to run simlib_comp if needed
VENDOR_LIBMAP_FILE := $(PCL_CACHE)/.synopsys_vss.setup

# Pass environment variable to VCS-MX for simulator mapping file
$(VENDOR_LIBMAP_FILE): SYNOPSYS_SIM_SETUP=$(VENDOR_LIBMAP_FILE)
$(VENDOR_LIBMAP_FILE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating Xilinx precompiled libraries using 'compile_simlib' with VCS-MX (This may take a few minutes)."
	-@$(ECHO) "Precompiled library cache location: $(PCL_CACHE)"
	-@$(ECHO) "Compilation logfile saved at $(PCL_CACHE)/compxlib.log"
	@$(MKDIR) $(PCL_CACHE)
	@$(ECHO) "WORK > DEFAULT" >>$(VENDOR_LIBMAP_FILE)
	@$(ECHO) "DEFAULT : $(TARGET)$(PATHSEP)work" >>$(VENDOR_LIBMAP_FILE)
ifeq "$(VCS_TARGET_MODE)" "32"
	@$(ECHO) "compile_simlib $(COMP_SIMLIB_OPTS) -simulator $(SIMULATOR) -simulator_exec_path $(SIM_PATH) -32bit -dir $(PCL_CACHE)" >$(PCL_CACHE)/input.tcl
else
	@$(ECHO) "compile_simlib $(COMP_SIMLIB_OPTS) -simulator $(SIMULATOR) -simulator_exec_path $(SIM_PATH) -dir $(PCL_CACHE)" >$(PCL_CACHE)/input.tcl
endif
	@$(ECHO) "exit" >>$(PCL_CACHE)/input.tcl
	@$(VIVADO_EXEC) -mode tcl -source $(PCL_CACHE)/input.tcl -log $(PCL_CACHE)/compxlib.log

# Build list of lib mappings
VENDOR_PHYDIR := $(PCL_CACHE)
VENDOR_LIBMAPS_V   = $(foreach i,$(sort $(SIMLIBS_V)),$(TARGET)/.$(i).$(i).libmapts)
VENDOR_LIBMAPS_VHD = $(foreach i,$(sort $(SIMLIBS_VHD)),$(TARGET)/.$(i).$(i).libmapts)

endif
# END VCS-MX SUPPORT
# ---------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------
# BEGIN NCSim SUPPORT
ifeq "$(SIMTOOL)" "ncsim"
SIM_PATH := $(NC_ROOT)
SIMULATOR := ncsim
$(error Warning: Vivado precompiled simulation libraries for NCSim are not supported)
endif
# END NCSim SUPPORT
# ---------------------------------------------------------------------------------------------

# end ifeq "$(SIMLIBS)" ""
endif

.PHONY: dump_VivadoLib_vars
dump_Vivado_vars:
	-@$(ECHO) "   SYNTHESIS_FLOWPKG   = $(SYNTHESIS_FLOWPKG)"
	-@$(ECHO) "   SYN_FLOW            = $(SYN_FLOW)"
	-@$(ECHO) "   Vivado_SIMLIBS_V    = $(Vivado_SIMLIBS_V)"
	-@$(ECHO) "   Vivado_SIMLIBS_VHD  = $(Vivado_SIMLIBS_VHD)"
	-@$(ECHO) "   SIMLIBS_V           = $(SIMLIBS_V)"
	-@$(ECHO) "   SIMLIBS_VHD         = $(SIMLIBS_VHD)"
	-@$(ECHO) "   SIMLIBS             = $(SIMLIBS)"
	-@$(ECHO) "   PCL_CACHE           = $(Vivado_PCL_CACHE)"
	-@$(ECHO) "   VENDOR_LIBMAP_FILE  = $(VENDOR_LIBMAP_FILE)"
	-@$(ECHO) "   VENDOR_PHYDIR       = $(VENDOR_PHYDIR)"
	-@$(ECHO) "   VENDOR_LIBMAPS_V    = $(VENDOR_LIBMAPS_V)"
	-@$(ECHO) "   VENDOR_LIBMAPS_VHD  = $(VENDOR_LIBMAPS_VHD)"
	-@$(ECHO) "   ModelSim_DEF_MODELSIM_INI  = $(ModelSim_DEF_MODELSIM_INI)"
# =============================================================================================
