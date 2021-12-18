#===============================================================================
# ccs_chck_vndr.mk
#
# Makes the precompiled library cache in the current working directory for
# either Xilinx or Altera.
#
# Required environment variables:
#   MGC_HOME         to locate support files
#   MODEL_TECH       (if using Questa SIM)
#   VCS_HOME         (if using VCS-MX)
#   QUARTUS_ROOTDIR  (if compiling Altera simulation libraries)
#   XILINX_VIVADO    (if compiling Altera simulation libraries)

.DEFAULT_GOAL := usage

ifneq "$(VNDR)" "Xilinx"
ifneq "$(VNDR)" "Altera"
  $(error The VNDR variable must be one of: Xilinx, Altera)
  VNDR =
endif
endif

ifneq "$(SIMTOOL)" "msim"
ifneq "$(SIMTOOL)" "vcs"
  $(error The SIMTOOL variable must be one of: msim, vcs)
  SIMTOOL=
endif
endif

include $(MGC_HOME)/shared/include/mkfiles/ccs_default_cmds.mk

ifeq "$(IN_CCS_FLOW)" "true"
include ./ccs_env.mk
else
# Provide mappings for Questa SIM options that normally are provided in ccs_env.mk
ModelSim_Path := $(MODEL_TECH)
VCS_VCS_HOME := $(VCS_HOME)
endif

ifeq "$(VNDR)" "Xilinx"
ifeq "$(XILINX_VIVADO)" ""
$(error The environment variable XILINX_VIVADO is not defined)
else
Vivado_PCL_CACHE      = $(realpath ./xilinx_cache)
Vivado_SIMLIBS_V     = "unisims_ver"
Vivado_SIMLIBS_VHD   = "unisim"
include $(MGC_HOME)/shared/include/mkfiles/ccs_VivadoLibs.mk
endif
endif

ifeq "$(VNDR)" "Altera"
ifeq "$(QUARTUS_ROOTDIR)" ""
$(error The environment variable QUARTUS_ROOTDIR is not defined)
else
Quartus_PCL_CACHE     = $(realpath ./altera_cache)
Quartus_SIMLIBS_V     = "altera_mf_ver lpm_ver altera_ver"
Quartus_SIMLIBS_VHD   = "altera_mf lpm altera"
include $(MGC_HOME)/shared/include/mkfiles/ccs_QuartusLibs.mk
endif
endif

ifneq "$(SIMTOOL)" ""
include $(MGC_HOME)/shared/include/mkfiles/ccs_$(SIMTOOL).mk
endif

.PHONY: usage
usage: print_usage 

.PHONY: print_usage
print_usage:
	-@$(ECHO) "Use this makefile to check and/or create the simulation libraries for the selected vendors"
	-@$(ECHO) "   $$MGC_HOME/bin/make -B -f $$MGC_HOME/shared/include/ccs_check_vendor.mk VNDR=<vendor> SIMTOOL=<tool> <action>"
	-@$(ECHO) "where:"
	-@$(ECHO) "   <vendor> is either Xilinx or Altera"
	-@$(ECHO) "   <tool>   is either mism or vcs"
	-@$(ECHO) "   <action> is either 'usage' to display this message"
	-@$(ECHO) "                      'create' to create (compile) the vendor simulation libraries"
	-@$(ECHO) "Current settings:"
	-@$(ECHO) "   VNDR                = $(VNDR)"
	-@$(ECHO) "   SIMTOOL             = $(SIMTOOL)"
	-@$(ECHO) "   PCL_CACHE           = $(PCL_CACHE)"
	-@$(ECHO) "   VENDOR_LIBMAP_FILE  = $(VENDOR_LIBMAP_FILE)"

.PHONY: make_pcl_cache
make_pcl_cache:
	-@$(MKDIR) $(subst /,$(PATHSEP),$(PCL_CACHE))

.PHONY: check
check:
	-@$(ECHO) "The check target is no longer supported"

.PHONY: create
create: make_pcl_cache $(PCL_CACHE) $(VENDOR_LIBMAP_FILE)
	-@$(ECHO) "Simulation libraries successfully created"
	-@$(ECHO) "Add the following option settings to your Catapult setup:"
ifeq "$(VNDR)" "Xilinx"
	-@$(ECHO) "   flow package option set /Vivado/PCL_CACHE $(PCL_CACHE)"
endif
ifeq "$(VNDR)" "Altera"
	-@$(ECHO) "   flow package option set /Quartus/PCL_CACHE $(PCL_CACHE)"
endif

