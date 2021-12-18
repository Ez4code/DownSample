
# Caller must set
#  CXX_SRC = list of C++ src files
#  CXX_INCLUDES = list of user include directories (not system includes or SystemC includes)
# Available targets:
#    build - Link SystemC
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean
#    help

#===============================================================================
INVOKE_ARGS   = $(SCVerify_INVOKE_ARGS)

VISTA_HOME = $(Vista_VISTA_HOME)
MODEL_BUILDER_HOME = $(Vista_MODEL_BUILDER_HOME)

ifeq "$(VISTA_HOME)" ""
   $(error The VISTA_HOME environment variable must be set to use this makefile)
endif

ifeq "$(CXX_VCO)" "ixn"
VISTA_GCC_VER = gcc343
CXX_EXEC = $(VISTA_HOME)/bin/vista_sh_batch.exe vista_c++
LINK_EXEC = $(VISTA_HOME)/bin/vista_sh_batch.exe vista_c++
VISTAEXEC = $(VISTA_HOME)/bin/vista_sh_batch.exe vista
VISTAEXEC_B = $(VISTA_HOME)/bin/vista_sh_batch.exe vista_batch
SYSTEMC_LIBDIR = $(VISTA_HOME)/systemc-22-$(VISTA_GCC_VER)/lib-win32
EXE_EXT = .exe
else
VISTA_GCC_VER = gcc412
CXX_EXEC = $(VISTA_HOME)/bin/vista_c++
LINK_EXEC = $(VISTA_HOME)/bin/vista_c++
VISTAEXEC = $(VISTA_HOME)/bin/vista
VISTAEXEC_B = $(VISTA_HOME)/bin/vista_batch
SYSTEMC_LIBDIR = $(VISTA_HOME)/systemc-22-$(VISTA_GCC_VER)/lib-linux
EXE_EXT = 
endif

OBJ_EXT = .o
LIB_PREFIX = -l
LIB_EXT = 
F_INCDIR = -I
F_LIBDIR = -L
F_LIB = -l
F_OUT =-o 
F_COMP =-c
F_DBG =-g
F_LINKOUT =-o 
LDFLAGS =
LD_LIBRARY_PATH=$(MGC_HOME)/lib:$(MGC_HOME)/shared/lib:$(VISTA_HOME)/lib
export LD_LIBRARY_PATH
SYSTEMC_INCDIR = $(VISTA_HOME)/systemc-22-$(VISTA_GCC_VER)/include

# Add the SystemC OSCI library to the link library list for OSCI simulation
LINK_LIBPATHS += $(SYSTEMC_LIBDIR)
LINK_LIBNAMES += systemc

ADDED_LIBPATHS := $(foreach lp,$(LINK_LIBPATHS),$(F_LIBDIR)$(lp))
ADDED_LIBNAMES := $(foreach ln,$(LINK_LIBNAMES),$(LIB_PREFIX)$(ln)$(LIB_EXT))
NORMAL_SOLNDIR := $(subst \,/,$(SOLNDIR))

# Build up include directory path
CXX_INCLUDES += $(INCL_DIRS)
CXX_INCLUDES += $(SYSTEMC_INCDIR)
CXX_INCLUDES += $(MGC_HOME)/shared/include
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/src
CXX_INCLUDES += $(MGC_HOME)/pkgs/siflibs
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/mgc_comps_src
CXX_INC    := $(foreach idir,$(CXX_INCLUDES),$(F_INCDIR)$(idir))

# Assemble final CXXFLAGS 
CXXFLAGS   += $(F_DBG) $(CXX_INC) -DSC_USE_STD_STRING -$(VISTA_GCC_VER) -sc22 -sc-static
LDFLAGS += -$(VISTA_GCC_VER) -sc22 -sc-static

TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_CXX_OBJ := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile))$(OBJ_EXT))

# Translate rule to compile SystemC with Questa SIM
$(TARGET)/%.cxxts :
	-@echo "============================================"
	-@echo "Compiling C++ file: $<"
	$(CXX_EXEC) $(CXXFLAGS) $(CXX_OPTS) $(F_COMP) $< $(F_OUT)$@$(OBJ_EXT)
	$(TIMESTAMP)

$(TARGET)/dir.ts: 
	-@$(ECHO) "============================================"
	-@$(ECHO) "Creating directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

$(TARGET)/$(TOP_DU)$(EXE_EXT): $(TARGET)/dir.ts $(TMP_CXX_SRC)
	-@echo "============================================"
	-@echo "Linking executable"
	$(LINK_EXEC) $(LDFLAGS) $(ADDED_LIBPATHS) $(TMP_CXX_OBJ) $(ADDED_LIBNAMES) $(F_LINKOUT)$@


ifneq "$(STAGE)" "orig"
$(TARGET)/scverify_vista.tcl: scverify/ccs_wave_signals.dat
	-@echo "============================================"
	-@echo "Creating Vista wave TCL file '$@' from '$<'"
	$(TCLSH_CMD) $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_vistasim.flo create_vista_wave $< $@ ""
else
$(TARGET)/scverify_vista.tcl:
	$(TCLSH_CMD) $(MGC_HOME)/pkgs/sif/userware/En_na/flows/app_vistasim.flo create_vista_wave "" $@ ""
endif

build: $(TARGET)/dir.ts $(TARGET)/scverify_vista.tcl $(TARGET)/$(TOP_DU)$(EXE_EXT)

PRJDIR := $(shell cd $(PROJDIR);pwd)
simgui: build
	-@echo "============================================"
	-@echo "Simulating design entity: $(TOP_DU)$(EXE_EXT) interactively"
	-@echo "$(SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(EXE_EXT)"
	-@echo "$(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP)scverify_vista.tcl"
	-@echo "$(INVOKE_ARGS)"
	-@echo "$(PRJDIR)"
	$(CD) $(PROJDIR)$(;) $(VISTAEXEC) -simdir $(PRJDIR)$(PATHSEP)$(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET) -wd ./$(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET) -exec $(SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(EXE_EXT) -simscript $(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP)scverify_vista.tcl -event-debugging -trace-all-signals -with-cause -tsv $(INVOKE_ARGS)

sim: build
	-@echo "============================================"
	-@echo "Simulating design entity: $(TOP_DU)$(EXE_EXT)"
	$(CD) $(PROJDIR)$(;) $(VISTAEXEC) -simdir $(PRJDIR)$(PATHSEP)$(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET) -wd ./$(NORMAL_SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP) -batch -exec $(SOLNDIR)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(EXE_EXT) -event-debugging -trace-all-signals -with-cause -tsv $(INVOKE_ARGS)

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
	@-$(ECHO) "Vista Makefile"
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
	@-$(ECHO) "   Vista flow options:"
	@-$(ECHO) "     VISTA_HOME            = $(VISTA_HOME)"
