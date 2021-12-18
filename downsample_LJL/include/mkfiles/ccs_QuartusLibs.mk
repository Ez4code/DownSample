#===============================================================================
# File: ccs_QuartusLibs.mk
#-------------------------------------------------------------------------------
# Support for precompiled Quartus simulation libraries
#===============================================================================
# Quartus Flow Options
#    /Quartus/SIMLIBS_V - List of verilog logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Quartus/SIMLIBS_VHD - List of VHDL logical library names requiring 
#                      logical-to-physical mapping for simulation.
#                      May be empty.
#                      Default value of option is "" (empty list)
#    /Quartus/PCL_CACHE - Pathname to output of the simlib_comp utility.
#                      May be empty.
#                      Default value of option is "" (empty string).
#    /Quartus/Path - Pathname to Quartus install directory
#
# Variables inside this makefile
#    Quartus_SIMLIBS_V    - The value from the option /Quartus/SIMLIBS_V (set in ccs_env.mk).
#    Quartus_SIMLIBS_VHD  - The value from the option /Quartus/SIMLIBS_VHD (set in ccs_env.mk).
#    Quartus_PCL_CACHE - The value from the option /Quartus/PCL_CACHE (set in ccs_env.mk).
#
#    SIMLIBS_V    - Initialized to the value of Quartus_SIMLIBS_V.
#                            If empty, this makefile does nothing.
#                            If accelerated libraries are in use, top-level makefile
#                            will initialize SIMLIBS_V to the value(s) of <techlibs>/VARS/SIMLIBS_V
#                            before appending any value from Quartus_SIMLIBS_V.
#    SIMLIBS_VHD  - Initialized to the value of Quartus_SIMLIBS_VHD.
#                            If empty, this makefile does nothing.
#                            If accelerated libraries are in use, top-level makefile
#                            will initialize SIMLIBS_VHD to the value(s) of <techlibs>/VARS/SIMLIBS_VHD
#                            before appending any value from Quartus_SIMLIBS_VHD.
#    SIMLIBS - Combination of SIMLIBS_V and SIMLIBS_VHD (only used to determine if there is
#                          any library compilation/mapping work to be done by this makefile)
#    PCL_CACHE - Initialized to the value of Quartus_PCL_CACHE.
#                          If empty, it is set to $(PROJ_DIR)/altera_cache (to
#                          populate a precompiled library cache in the directory for this
#                          project (would be shared by all solutions).
#
#    VENDOR_LIBMAP_FILE - If SIMTOOL=msim, this variable is $(Quartus_PCL_CACHE)/modelsim.ini
#                         If SIMTOOL=vcs, this variable is $(Quartus_PCL_CACHE)/synopsys_sim.setup
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
#    VENDOR_PHYDIR      - For Quartus, this variable is $(PCL_CACHE).
#                         This variable is processed by the simulator-specific makefile.
#    Variables VENDOR_LIBMAP_FILE, VENDOR_LIBMAPS_V and VENDOR_LIBMAPS_VHD must be added to the make target $(TARGET)/make_libs
#    in all supported simulator makefiles:
#        $(TARGET)/make_libs : ... $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAPS_V) $(VENDOR_LIBMAPS_VHD)
#------------------------------------------------------------

# Get option settings from Catapult Quartus Flow
PCL_CACHE        := $(Quartus_PCL_CACHE)
# Add those logical library names to any list of names from the top-level makefile
ifeq "$(NETLIST)" "vhdl"
SIMLIBS_VHD         += $(Quartus_SIMLIBS_VHD)
else
SIMLIBS_V           += $(Quartus_SIMLIBS_V)
endif
ifeq "$(STAGE)" "gate"
ifeq "$(NETLIST)" "vhdl"
SIMLIBS_VHD         += altera_mf lpm altera
else
SIMLIBS_V           += altera_mf_ver lpm_ver altera_ver
endif
endif
SIMLIBS             = $(SIMLIBS_V) $(SIMLIBS_VHD)


# =============================================================================================
# Only do something if we have a precompiled library request
ifneq "$(SIMLIBS)" ""

#------------------------------------------------------------
# Determine Quartus executable to use
ifeq "$(QUARTUS_ROOTDIR)" ""
ifneq "$(Quartus_Path)" ""
QUARTUS_ROOTDIR := $(Quartus_Path)/..
else
$(error Error: QUARTUS_ROOTDIR is undefined and Quartus Path not set)
endif
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
  SIMULATOR := questasim
else
  SIMULATOR := modelsim
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
PCL_CACHE := $(WORK_DIR)/$(WORK2PROJ)/altera_cache
# Make the directory if it doesnt exist
$(PCL_CACHE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory for Quartus precompiled simulation library cache $<"
	$(MKDIR) $(subst /,$(PATHSEP),$@)
endif

ifeq "$(CXX_OS)" "Windows_NT"
QSHELL = $(QUARTUS_ROOTDIR)/bin64/quartus_sh
else
QSHELL = $(QUARTUS_ROOTDIR)/bin/quartus_sh
endif

# Set VENDOR_LIBMAP_FILE to mark dependency, define translate rule to run compxlib if needed
VENDOR_LIBMAP_FILE := $(PCL_CACHE)/modelsim.ini
DFI = $(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)
XDFI = $(subst /,$(PATHSEP),$(subst \,$(PATHSEP),$(DFI)))
DXDFI = $(subst $(space),\$(space),$(XDFI))
$(VENDOR_LIBMAP_FILE): MODELSIM=$(PCL_CACHE)/modelsim.ini
$(VENDOR_LIBMAP_FILE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating Altera precompiled libraries using 'simlib_comp' with QuestaSim (This may take a few minutes)."
	-@$(ECHO) "Precompiled library cache location: $(PCL_CACHE)"
	@$(MKDIR) $(subst /,$(PATHSEP),$(PCL_CACHE))
	-@$(ECHO) " - Configuring $(SIMULATOR) to suppress message 12110"
	@$(TCLSED) $(XDFI) $(PCL_CACHE)/modelsim.ini "suppress = " "suppress = 12110,"
#	@$(CAT) "$(XDFI)" >$(PCL_CACHE)/modelsim.ini
	@$(QSHELL) --simlib_comp -tool $(SIMULATOR) -family all -language verilog -tool_path $(SIM_PATH) -directory $(PCL_CACHE) -rtl_only -suppress_messages
	@$(QSHELL) --simlib_comp -tool $(SIMULATOR) -family all -language vhdl -tool_path $(SIM_PATH) -directory $(PCL_CACHE) -rtl_only -suppress_messages

# Build list of lib mappings
VENDOR_PHYDIR := $(PCL_CACHE)
VENDOR_LIBMAPS_V   = $(foreach i,$(sort $(SIMLIBS_V)),$(TARGET)/verilog_libs.$(i).$(i).libmapts)
VENDOR_LIBMAPS_VHD = $(foreach i,$(sort $(SIMLIBS_VHD)),$(TARGET)/vhdl_libs.$(i).$(i).libmapts)

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
SIMULATOR := vcsmx

# If user's options do not specify a home or site location for precompiled libraries...
ifeq "$(PCL_CACHE)" ""
# ...default to the working directory of the project
PCL_CACHE := $(WORK_DIR)/$(WORK2PROJ)/altera_cache
# Make the directory if it doesnt exist
$(PCL_CACHE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory for Quartus precompiled simulation library cache $<"
	$(MKDIR) $@
endif

# Set VENDOR_LIBMAP_FILE to mark dependency, define translate rule to run simlib_comp if needed
VENDOR_LIBMAP_FILE := $(PCL_CACHE)/.synopsys_vss.setup

# Pass environment variable to VCS-MX for simulator mapping file
$(VENDOR_LIBMAP_FILE): SYNOPSYS_SIM_SETUP=$(VENDOR_LIBMAP_FILE)

$(VENDOR_LIBMAP_FILE):
	-@$(ECHO) "============================================"
	-@$(ECHO) "Generating Altera precompiled libraries using 'simlib_comp' with VCS-MX (This may take a few minutes)."
	-@$(ECHO) "Precompiled library cache location: $(PCL_CACHE)"
	@$(MKDIR) $(PCL_CACHE)
	@$(QUARTUS_ROOTDIR)/bin/quartus_sh --simlib_comp -tool $(SIMULATOR) -cmd_file simlib.vcs -family all -language verilog -tool_path $(SIM_PATH) -directory $(PCL_CACHE) -rtl_only -suppress_messages
	@$(COPY) $(VENDOR_LIBMAP_FILE) $(VENDOR_LIBMAP_FILE).verilog
	@$(QUARTUS_ROOTDIR)/bin/quartus_sh --simlib_comp -tool $(SIMULATOR) -cmd_file simlib.vcs -family all -language vhdl -tool_path $(SIM_PATH) -directory $(PCL_CACHE) -rtl_only -gen_only
ifeq "$(VCS_TARGET_MODE)" "32"
	@$(COPY) $(PCL_CACHE)/simlib.vcs $(PCL_CACHE)/simlib_vhd.vcs
else
	@sed -e 's/vhdlan /vhdlan -full64/' $(PCL_CACHE)/simlib.vcs >$(PCL_CACHE)/simlib_vhd.vcs
endif
	. $(VCS_HOME)/bin/environ.sh ; sh $(PCL_CACHE)/simlib_vhd.vcs
	@$(CAT) $(VENDOR_LIBMAP_FILE).verilog >>$(VENDOR_LIBMAP_FILE)

# Build list of lib mappings
VENDOR_PHYDIR := $(PCL_CACHE)
VENDOR_LIBMAPS_V   = $(foreach i,$(sort $(SIMLIBS_V)),$(TARGET)/verilog_libs.$(i).$(i).libmapts)
VENDOR_LIBMAPS_VHD = $(foreach i,$(sort $(SIMLIBS_VHD)),$(TARGET)/vhdl_libs.$(i).$(i).libmapts)

blip:
	echo "VENDOR_LIBMAP_FILE='$(VENDOR_LIBMAP_FILE)'"
	echo "VENDOR_PHYDIR='$(VENDOR_PHYDIR)'"
	echo "SIMLIBS_V='$(SIMLIBS_V)'"
	echo "SIMLIBS_VHD='$(SIMLIBS_VHD)'"
	echo "VENDOR_LIBMAPS_V='$(VENDOR_LIBMAPS_V)'"
	echo "VENDOR_LIBMAPS_VHD='$(VENDOR_LIBMAPS_VHD)'"

# end ifeq "$(SIMTOOL)" "vcs"
endif
# END VCS-MX SUPPORT
# ---------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------
# BEGIN NCSim SUPPORT
ifeq "$(SIMTOOL)" "ncsim"
SIM_PATH := $(NC_ROOT)
SIMULATOR := ncsim
$(error Warning: Quartus precompiled simulation libraries for NCSim are not supported)
endif
# BEGIN NCSim SUPPORT
# ---------------------------------------------------------------------------------------------

# end ifeq "$(SIMLIBS)" ""
endif

.PHONY: dump_QuartusLib_vars
dump_Quartus_vars:
	-@$(ECHO) "   SYNTHESIS_FLOWPKG   = $(SYNTHESIS_FLOWPKG)"
	-@$(ECHO) "   SYN_FLOW            = $(SYN_FLOW)"
	-@$(ECHO) "   Quartus_SIMLIBS_V    = $(Quartus_SIMLIBS_V)"
	-@$(ECHO) "   Quartus_SIMLIBS_VHD  = $(Quartus_SIMLIBS_VHD)"
	-@$(ECHO) "   SIMLIBS_V           = $(SIMLIBS_V)"
	-@$(ECHO) "   SIMLIBS_VHD         = $(SIMLIBS_VHD)"
	-@$(ECHO) "   SIMLIBS             = $(SIMLIBS)"
	-@$(ECHO) "   QUARTUS_ROOTDIR     = $(QUARTUS_ROOTDIR)"
	-@$(ECHO) "   PCL_CACHE           = $(Quartus_PCL_CACHE)"
	-@$(ECHO) "   VENDOR_LIBMAP_FILE  = $(VENDOR_LIBMAP_FILE)"
	-@$(ECHO) "   VENDOR_PHYDIR       = $(VENDOR_PHYDIR)"
	-@$(ECHO) "   VENDOR_LIBMAPS_V    = $(VENDOR_LIBMAPS_V)"
	-@$(ECHO) "   VENDOR_LIBMAPS_VHD  = $(VENDOR_LIBMAPS_VHD)"
	-@$(ECHO) "   ModelSim_DEF_MODELSIM_INI  = $(ModelSim_DEF_MODELSIM_INI)"
# =============================================================================================
