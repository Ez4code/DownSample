
#
# ALTERA simulation settings
#

ifeq "$(QUARTUS_LIB)" ""
   $(warning Warning: The QUARTUS_LIB variable was not set. Setting it to QUARTUS_ROOTDIR/../modelsim_ae/altera)
QUARTUS_LIB=$(QUARTUS_ROOTDIR)/../modelsim_ae/altera
endif

ifeq "$(SIMTOOL)" "msim"
QUARTUS_VHDL_LIB = $(QUARTUS_LIB)/vhdl
QUARTUS_VERILOG_LIB = $(QUARTUS_LIB)/verilog
export QUARTUS_VHDL_LIB
export QUARTUS_VERILOG_LIB
else
ifeq "$(SIMTOOL)" "ncsim"
$(warning Warning: Altera precompiled simulation libraries for NCSim are not supported)
else
endif
endif

ifeq "$(OS)" "Windows_NT"
QUARTUS_PLAT = 
else
ifeq "$(shell uname -s)" "Linux"
QUARTUS_PLAT = lin
else
ifeq "$(shell uname -s)" "SunOS"
QUARTUS_PLAT = sol
else
$(error Unknown platform - could not determine location of Altera installation)
endif
endif
endif

ifneq "$(OS)" "Windows_NT"
LD_LIBRARY_PATH=$(MGC_HOME)/lib:$(MGC_HOME)/shared/lib:$(QUARTUS_ROOTDIR)/bin/$(QUARTUS_PLAT)
export LD_LIBRARY_PATH
endif

#------------------------------------------------------------
# The following is for VHDL and Verilog netlist simulation only
ifneq "$(NETLIST)" "cxx"

# Variables for performing mapped or gate simulation
VNDR_NETDIR    = simulation/modelsim
ifeq "$(STAGE)" "gate"
VNDR_SDFINST   = scverify_top/rtl
else
VNDR_SDFINST   =
endif
ifeq "$(NETLIST)" "vhdl"
VNDR_NETSUF    = .vho
VNDR_SDFSUF    = _vhd.sdo
endif
ifeq "$(NETLIST)" "v"
VNDR_NETSUF    = .vo
VNDR_SDFSUF    = _v.sdo
endif

ifeq "$(NETLIST)" "vhdl"
QLIBS = $(QUARTUS_VHDL_LIB)
QUARTUS_LANG = vhdl
endif
ifeq "$(NETLIST)" "v"
QLIBS = $(QUARTUS_VERILOG_LIB)
QUARTUS_LANG = verilog
endif

# Define the list of logical libraries for Altera simulation (only if the user has not provided there own modelsim.ini)
ifeq "$(ModelSim_DEF_MODELSIM_INI)" ""
ALTERA_SIM_LIBS = altera_mf altera lpm cyclone cycloneii cycloneiii cycloneiv cyclonev stratix stratixii stratixiii stratixgx stratixiigx stratixiv stratixv arriagx arriaii arriaiigz arriav
endif

# Specify location of physical library and name of logical library (often the same)
$(TARGET)/altera_mf.log2phyts: $(QLIBS)/altera_mf
$(TARGET)/altera_mf.log2phyts: HDL_LOGICAL=altera_mf

$(TARGET)/altera.log2phyts: $(QLIBS)/altera
$(TARGET)/altera.log2phyts: HDL_LOGICAL=altera

$(TARGET)/lpm.log2phyts: $(QLIBS)/220model
$(TARGET)/lpm.log2phyts: HDL_LOGICAL=lpm

$(TARGET)/cyclone.log2phyts: $(QLIBS)/cyclone
$(TARGET)/cyclone.log2phyts: HDL_LOGICAL=cyclone

$(TARGET)/cycloneii.log2phyts: $(QLIBS)/cycloneii
$(TARGET)/cycloneii.log2phyts: HDL_LOGICAL=cycloneii

$(TARGET)/cycloneiii.log2phyts: $(QLIBS)/cycloneiii
$(TARGET)/cycloneiii.log2phyts: HDL_LOGICAL=cycloneiii

$(TARGET)/cycloneiv.log2phyts: $(QLIBS)/cycloneiv
$(TARGET)/cycloneiv.log2phyts: HDL_LOGICAL=cycloneiv

$(TARGET)/cyclonev.log2phyts: $(QLIBS)/cyclonev
$(TARGET)/cyclonev.log2phyts: HDL_LOGICAL=cyclonev

$(TARGET)/stratix.log2phyts: $(QLIBS)/stratix
$(TARGET)/stratix.log2phyts: HDL_LOGICAL=stratix

$(TARGET)/stratixii.log2phyts: $(QLIBS)/stratixii
$(TARGET)/stratixii.log2phyts: HDL_LOGICAL=stratixii

$(TARGET)/stratixiii.log2phyts: $(QLIBS)/stratixiii
$(TARGET)/stratixiii.log2phyts: HDL_LOGICAL=stratixiii

$(TARGET)/stratixgx.log2phyts: $(QLIBS)/stratixgx
$(TARGET)/stratixgx.log2phyts: HDL_LOGICAL=stratixgx

$(TARGET)/stratixgx_hssi.log2phyts: $(QLIBS)/stratixgx_hssi
$(TARGET)/stratixgx_hssi.log2phyts: HDL_LOGICAL=stratixgx_hssi

$(TARGET)/stratixiigx.log2phyts: $(QLIBS)/stratixiigx
$(TARGET)/stratixiigx.log2phyts: HDL_LOGICAL=stratixiigx

$(TARGET)/stratixiigx_hssi.log2phyts: $(QLIBS)/stratixiigx_hssi
$(TARGET)/stratixiigx_hssi.log2phyts: HDL_LOGICAL=stratixiigx_hssi

$(TARGET)/stratixiv.log2phyts: $(QLIBS)/stratixiv
$(TARGET)/stratixiv.log2phyts: HDL_LOGICAL=stratixiv

$(TARGET)/stratixiv_hssi.log2phyts: $(QLIBS)/stratixiv_hssi
$(TARGET)/stratixiv_hssi.log2phyts: HDL_LOGICAL=stratixiv_hssi

$(TARGET)/stratixv.log2phyts: $(QLIBS)/stratixv
$(TARGET)/stratixv.log2phyts: HDL_LOGICAL=stratixv

$(TARGET)/stratixv_hssi.log2phyts: $(QLIBS)/stratixv_hssi
$(TARGET)/stratixv_hssi.log2phyts: HDL_LOGICAL=stratixv_hssi

$(TARGET)/arriaii.log2phyts: $(QLIBS)/arriaii
$(TARGET)/arriaii.log2phyts: HDL_LOGICAL=arriaii

$(TARGET)/arriaii_hssi.log2phyts: $(QLIBS)/arriaii_hssi
$(TARGET)/arriaii_hssi.log2phyts: HDL_LOGICAL=arriaii_hssi

$(TARGET)/arriaiigz.log2phyts: $(QLIBS)/arriaiigz
$(TARGET)/arriaiigz.log2phyts: HDL_LOGICAL=arriaiigz

$(TARGET)/arriaiigz_hssi.log2phyts: $(QLIBS)/arriaiigz_hssi
$(TARGET)/arriaiigz_hssi.log2phyts: HDL_LOGICAL=arriaiigz_hssi

$(TARGET)/arriav.log2phyts: $(QLIBS)/arriav
$(TARGET)/arriav.log2phyts: HDL_LOGICAL=arriav

$(TARGET)/arriagx.log2phyts: $(QLIBS)/arriagx
$(TARGET)/arriagx.log2phyts: HDL_LOGICAL=arriagx

$(TARGET)/arriagx_hssi.log2phyts: $(QLIBS)/arriagx_hssi
$(TARGET)/arriagx_hssi.log2phyts: HDL_LOGICAL=arriagx_hssi

# End of VHDL and Verilog section
endif

#===============================================================================
# DO NOT EDIT ANYTHING BELOW THIS LINE
# This variable is processed by the actual simulator makefile to create the
# library mappings
VNDR_HDL_LIBS = $(foreach lib,$(ALTERA_SIM_LIBS),$(TARGET)/$(lib).log2phyts)

.PHONY: vendor_precompile
vendor_precompile: make_altera_lib vendor_precompile_vhdl vendor_precompile_verilog

N_QUARTUS_VHDL_LIB = $(subst /,$(PATHSEP),$(subst \,/,$(QUARTUS_VHDL_LIB)))
N_QUARTUS_VERILOG_LIB = $(subst /,$(PATHSEP),$(subst \,/,$(QUARTUS_VERILOG_LIB)))

MSIM_INI = $(subst /,$(PATHSEP),$(MODEL_TECH)/../modelsim.ini)

.PHONY: make_altera_lib
make_altera_lib:
	-@$(MKDIR) $(N_QUARTUS_VHDL_LIB)
	-@$(CAT) $(MSIM_INI) >$(N_QUARTUS_VHDL_LIB)$(PATHSEP)modelsim.ini
	-@$(MKDIR) $(N_QUARTUS_VERILOG_LIB)
	-@$(CAT) $(MSIM_INI) >$(N_QUARTUS_VERILOG_LIB)$(PATHSEP)modelsim.ini

vendor_precompile_vhdl: MODELSIM=$(QUARTUS_VHDL_LIB)/modelsim.ini

.PHONY: vendor_precompile_vhdl
vendor_precompile_vhdl: make_altera_lib
	@$(ECHO) "Generating Altera VHDL precompiled libraries using ModelSim"
	$(MODEL_TECH)/vsim -c -do "do $(MGC_HOME)/pkgs/sif/userware/En_na/compile_altera_libs.tcl -lang vhdl"

vendor_precompile_verilog: MODELSIM=$(QUARTUS_VERILOG_LIB)/modelsim.ini

.PHONY: vendor_precompile_verilog
vendor_precompile_verilog: make_altera_lib
	@$(ECHO) "Generating Altera Verilog precompiled libraries using ModelSim"
	$(MODEL_TECH)/vsim -c -do "do $(MGC_HOME)/pkgs/sif/userware/En_na/compile_altera_libs.tcl -lang verilog"

.PHONY: dump_Altera_vars
dump_Altera_vars:
	-@$(ECHO) "   QUARTUS_ROOTDIR     = $(QUARTUS_ROOTDIR)"
	-@$(ECHO) "   QUARTUS_LIB         = $(QUARTUS_LIB)"


