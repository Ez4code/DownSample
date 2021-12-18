

# Caller must set
#  VHDL_SRC = list of VHDL src files
#  VLOG_SRC = list of Verilog src files
#  HDL_LIB_NAMES = list of logical libraries to create
#

HOME_0IN       := $(0in_HOME_0IN)
VCOM_OPTS        = $(ModelSim_VCOM_OPTS)
VLOG_OPTS        += $(ModelSim_VLOG_OPTS)

ifeq "$(HOME_0IN)" ""
   $(error No 0-in installation found. Check your HOME_0IN option in Catapult)
endif

#===============================================================================
# ModelSim executables from 0-in installation
VLIB       = $(HOME_0IN)/modeltech/bin/vlib
VMAP       = $(HOME_0IN)/modeltech/bin/vmap
VCOM       = $(HOME_0IN)/modeltech/bin/vcom
VLOG       = $(HOME_0IN)/modeltech/bin/vlog

# Assemble list of library names for verilog
VLOG_LIBS := $(foreach lib,$(VNDR_HDL_LIBS),-L "$(subst .log2phyts,,$(notdir $(lib)))") $(foreach lib,$(HDL_LIB_NAMES),-L "./$(TARGET)/$(lib)") $(foreach lib,$(ADDED_VLOGLIBS),-L "$(lib)")

# The following env variables are used to map libraries when compiling from
# the solution directory so that they can be referenced when executing simulation
# from the project directory
ifeq "$(DEF_MODELSIM_INI)" ""
DEF_MODELSIM_INI =$(subst /,$(PATHSEP),$(HOME_0IN)/modeltech/modelsim.ini)
endif
# Make a platform-specific version of DEF_MODELSIM_INI
X_DEF_MODELSIM_INI =$(subst /,$(PATHSEP),$(subst \,$(PATHSEP),$(DEF_MODELSIM_INI)))
# Back-quote any embedded white space for make dependency check
DEP_X_DEF_MODELSIM_INI = $(subst $(space),\$(space),$(X_DEF_MODELSIM_INI))
MODELSIM=
export MODELSIM
export SCVLIBS

# Modify SUFFIX then prefix TARGET (preserve order of files)
TMP_VLOG_SRC := $(foreach hdlfile,$(VLOG_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_VHDL_SRC := $(foreach hdlfile,$(VHDL_SRC),$(TARGET)/$(notdir $(hdlfile)))

# Translate rule to compile VHDL with Modelsim
$(TARGET)/%.vhdlts :
	-@echo "============================================"
	-@echo "Compiling VHDL file: $<"
	$(VCOM) -work $(HDL_LIB) $(VCOM_OPTS) $<
	$(TIMESTAMP)

# Translate rule to compile Verilog with Modelsim
$(TARGET)/%.vts :
	-@echo "============================================"
	-@echo "Compiling Verilog file: $<"
	$(VLOG) -work $(HDL_LIB) $(VLOG_OPTS) $<
	$(TIMESTAMP)

# Make sure that all rules executing based on the pattern %ts have the correct MODELSIM and SCVLIBS
# variable values
%ts: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
%ts: SCVLIBS=.
build: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
build: SCVLIBS=.
check: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
check: SCVLIBS=.
checkgui: MODELSIM=$(TARGET)$(PATHSEP)modelsim.ini
checkgui: SCVLIBS=.

# Expand out the list of Modelsim libraries to create
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
	@-$(ECHO) "Creating working directory '$(subst /,$(PATHSEP),$(TARGET))'"
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
$(TARGET)/make_libs : $(foreach lib,$(HDL_LIB_NAMES),$(TARGET)/$(lib).libts) $(VNDR_HDL_LIBS)
	$(TIMESTAMP)

$(TARGET)/$(TOP_DU)ts : $(TMP_VHDL_SRC) $(TMP_VLOG_SRC)
	$(TIMESTAMP)

build: $(TARGET)/make_libs $(TARGET)/$(TOP_DU)ts
	$(TIMESTAMP)

$(TARGET)/0in_autocheck.db: build
	-@echo "============================================"
	-@echo "Checking design entity: $(TOP_DU)"
	$(HOME_0IN)/bin/0in_autocheck -c -od $(TARGET) $(VLOG_LIBS) -d $(TOP_DU) -ctrl $(TARGET)/../$(TOP_DU)_ctrl.v -effort high

check: $(TARGET)/0in_autocheck.db

checkgui: $(TARGET)/0in_autocheck.db
	-@echo "============================================"
	-@echo "Launching 0-in GUI on $<"
	$(HOME_0IN)/bin/0in_autocheck $<

.PHONY: clean
clean:
	@-$(ECHO) "============================================"
	@-$(ECHO) "Removing working directory $(TARGET)"
	-$(RMDIR) $(subst /,$(PATHSEP),$(TARGET))

.PHONY : help
help: helptext dumpsysvars dumpvars

helptext:
	@-$(ECHO) "0-in Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   checkgui   Compile and execute autocheck in"
	@-$(ECHO) "              interactive mode"
	@-$(ECHO) "   check      Compile and execute autocheck in"
	@-$(ECHO) "              batch mode"
	@-$(ECHO) "   build      Compile the models only"
	@-$(ECHO) "   clean      Remove all compiled objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

dumpvars:
	@-$(ECHO) "   0-in flow options:"
	@-$(ECHO) "     HOME_0IN              = $(HOME_0IN)"
	@-$(ECHO) "     VCOM_OPTS             = $(VCOM_OPTS)"
	@-$(ECHO) "     VLOG_OPTS             = $(VLOG_OPTS)"

