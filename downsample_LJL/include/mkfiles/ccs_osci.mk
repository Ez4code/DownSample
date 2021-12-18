
# Caller must set
#  CXX_SRC = list of C++ src files
#  CXX_INCLUDES = list of user include directories (not system includes or SystemC includes)
# Available targets:
#    build - Link SystemC
#    sim - invoke simulator
#    simgui - invoke simulator
#    clean
#    help

ifneq "$(FLOW)" "cxxanalysis"
INVOKE_ARGS   = $(SCVerify_INVOKE_ARGS)
endif

MANIFEST :=
ifneq "$(CXX_OS)" "Linux"
MANIFEST := $(MGC_HOME)$(PATHSEP)shared$(PATHSEP)include$(PATHSEP)mkfiles$(PATHSEP)scverify_top.exe.manifest
endif

SYSTEMC_INCDIR = $(OSCI_SYSTEMC_INCLUDE)
SYSTEMC_LIBDIR = $(OSCI_SYSTEMC_LIB)
SYSTEMC_NAME   = $(OSCI_SYSTEMC_NAME)
GDBGUI         = $(OSCI_GDBGUI)

VALGRIND       = $(Valgrind_VALGRIND)
VALGRIND_OPTS  = $(Valgrind_VALGRIND_OPTS)

#===============================================================================
ifeq "$(CXX_HOME)" ""
   $(error The CXX_HOME environment variable must be set to use this makefile)
endif
ifeq "$(SYSTEMC_INCDIR)" ""
   $(error The SYSTEMC_INCDIR environment variable must be set to use this makefile)
endif
ifeq "$(SYSTEMC_LIBDIR)" ""
   $(error The SYSTEMC_LIBDIR environment variable must be set to use this makefile)
endif

ifeq "$(CXX_TYPE)" "msvc"
   # CXX_HOME environment variable points to compiler install directory
   CXX_EXEC = "$(subst /,$(PATHSEP),$(CXX_HOME)/$(EXESUBPATH)/cl.exe)"
   PURIFY_EXEC +=
   LINK_EXEC = "$(subst /,$(PATHSEP),$(CXX_HOME)/$(EXESUBPATH)/link.exe)"
   OBJ_EXT = .obj
   LIB_PREFIX = lib
   LIB_EXT = .lib
   EXE_EXT = .exe
   F_INCDIR = /I
   F_LIBDIR = /LIBPATH:
   F_LIB =
   F_OUT =/Fo
   F_COMP =/c /Tp
   F_DBG =
   F_OPTIMIZE =
   F_LINKOUT =/out:
   CXXFLAGS += /D"WIN32" /D"_DEBUG" /D"_CONSOLE" /D"NOGDI" /D"_MBCS" /D"_CRT_SECURE_NO_DEPRECATE" /EHsc /RTCs /MTd /FD /W3 /Z7 /vmg
   MODIFIED_OSCI_COMP_FLAGS := $(filter-out -Wall,$(filter-out -Wno-%,$(OSCI_COMP_FLAGS)))
   LDFLAGS += /SUBSYSTEM:CONSOLE /DEBUG /DYNAMICBASE:NO
   PATH := "$(PATH);$(subst /,$(PATHSEP),$(MSVCPATHS))"
   CXXFLAGS += /I"C:/Program Files/Microsoft Platform SDK/Include"
   DEBUGGER = VCExpress.exe /Command "Debug.StepOver" /DebugExe
else
   CXX_EXEC = $(CXX_HOME)/bin/g++
   PURIFY_EXEC +=
   LINK_EXEC = $(CXX_HOME)/bin/g++
   OBJ_EXT = .o
   LIB_PREFIX = -l
   LIB_EXT = 
   EXE_EXT = 
   F_INCDIR = -I
   F_LIBDIR = -L
   F_LIB = -l
   F_OUT =-o 
   F_COMP =-c
   F_DBG =-g
ifeq "$(SCVerify_OPTIMIZE_WRAPPERS)" "true"
   F_WRAP_OPT = -O2
else
   F_WRAP_OPT =
endif
   GCOV_ENABLED = false
ifeq "$(OSCI_GCOV)" "true"
ifneq "$(findstring _$(CXX_VCO)_,_aol_ixl_)" ""
   GCOV_ENABLED = true
   LCOV = $(MGC_HOME)/bin/lcov
   GENHTML = $(MGC_HOME)/bin/genhtml
   CXXFLAGS += -fprofile-arcs -ftest-coverage
   LINK_LIBNAMES += gcov
endif
endif
ifeq "$(Option_CppStandard)" "c++11"
   CXXFLAGS += -std=gnu++11
endif
# SystemC 2.3.1 requires pthread library for async_reset update support
ifneq "$(findstring _$(CXX_VCO)_,_aol_ixl_)" ""
   LINK_LIBNAMES += pthread
endif
   F_LINKOUT =-o 
   LDFLAGS +=
   LD_LIBRARY_PATH:=$(MGC_HOME)/lib:$(MGC_HOME)/shared/lib:/usr/lib:$(LD_LIBRARY_PATH)
   MODIFIED_OSCI_COMP_FLAGS := $(OSCI_COMP_FLAGS)
   export LD_LIBRARY_PATH
   DEBUGGER = $(GDBGUI)
   ifeq "$(CXX_VCO)" "ixl"
      # For Linux: Determine if the G++ compiler in use is the one shipped with Catapult 
      # (which is a 32bit only compiler) and add the 32bit compile mode flag if needed
      LIBGCC_FILENAME=$(shell $(CXX_EXEC) -print-libgcc-file-name)
      IS_CCS_GCC=$(if $(findstring dcs_gcc,$(LIBGCC_FILENAME)),yes,no)
      # The -Wa,--32 is for pre-4.9 gcc. The -m32 is for 4.9 and newer.
      ifeq "$(IS_CCS_GCC)" "yes"
         CXXFLAGS+=-Wa,--32
         LDFLAGS+=-Wa,--32
      endif
   endif
endif

ifneq "$(LINK_SYSTEMC)" ""
# Add the SystemC OSCI library to the link library list for OSCI simulation
LINK_LIBPATHS += $(SYSTEMC_LIBDIR)
LINK_LIBNAMES += $(SYSTEMC_NAME)
endif

ADDED_LIBPATHS := $(foreach lp,$(LINK_LIBPATHS),$(F_LIBDIR)$(lp))
ADDED_LIBNAMES := $(foreach ln,$(LINK_LIBNAMES),$(LIB_PREFIX)$(ln)$(LIB_EXT))

# Build up include directory path
CXX_INCLUDES += $(INCL_DIRS)
CXX_INCLUDES += $(SYSTEMC_INCDIR)
CXX_INCLUDES += $(MGC_HOME)/shared/include
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/src
CXX_INCLUDES += $(MGC_HOME)/pkgs/siflibs
CXX_INCLUDES += $(MGC_HOME)/pkgs/hls_pkgs/mgc_comps_src
CXX_INC    := $(foreach idir,$(CXX_INCLUDES),$(F_INCDIR)$(idir))

# Assemble final CXXFLAGS 
CXXFLAGS   += $(MODIFIED_OSCI_COMP_FLAGS)
CXXFLAGS   += $(F_DBG) $(CXX_INC) -DSC_USE_STD_STRING

TMP_CXX_SRC := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile)))
TMP_CXX_OBJ := $(foreach hdlfile,$(CXX_SRC),$(TARGET)/$(notdir $(hdlfile))$(OBJ_EXT))

# Convert pathnames if running in DOS windows
PWORK_DIR   := $(subst /,$(PATHSEP),$(WORK_DIR))
PWORK2PROJ  := $(subst /,$(PATHSEP),$(WORK2PROJ))
PWORK2SOLN  := $(subst /,$(PATHSEP),$(WORK2SOLN))
PPROJ2WORK  := $(subst /,$(PATHSEP),$(PROJ2WORK))
PPROJ2SOLN  := $(subst /,$(PATHSEP),$(PROJ2SOLN))

ifneq "$(STAGE)" "orig"
# Custom time-stamp dependencies for scverify_top.cpp/mc_testbench.cpp
$(TARGET)/scverify_top.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts
$(TARGET)/mc_testbench.cpp.cxxts: .ccs_env_opts/SCVerify_OPTIMIZE_WRAPPERS.ts
endif

# Translate rule to compile SystemC with Questa SIM 
$(TARGET)/%.cxxts :
	-@echo "============================================"
	-@echo "Compiling C++ file: $<"
	$(CXX_EXEC) $(CXXFLAGS) $(CXX_OPTS) $(F_COMP) $< $(F_OUT)$@$(OBJ_EXT)
	$(TIMESTAMP)

$(TARGET)/make_dir: 
	@-$(ECHO) "============================================"
	@-$(ECHO) "Creating simulation directory '$(subst /,$(PATHSEP),$(TARGET))'"
	$(MKDIR) $(subst /,$(PATHSEP),$(TARGET))
	$(TIMESTAMP)

$(TARGET)/$(TOP_DU)$(EXE_EXT): $(TARGET)/make_dir $(TMP_CXX_SRC)
	-@echo "============================================"
	-@echo "Linking executable"
	$(PURIFY_EXEC) $(LINK_EXEC) $(LDFLAGS) $(F_LIBDIR)$(SYSTEMC_LIBDIR) $(F_LIBDIR)$(MGC_HOME)/shared/lib $(ADDED_LIBPATHS) $(TMP_CXX_OBJ) $(ADDED_LIBNAMES) $(F_LINKOUT)$@
ifneq "$(MANIFEST)" ""
	$(COPY) $(subst /,$(PATHSEP),$(MANIFEST)) $(subst /,$(PATHSEP),$(TARGET))$(PATHSEP)$(TOP_DU).exe.manifest
endif

build: $(TARGET)/$(TOP_DU)$(EXE_EXT)

simgui: build
	$(CD) $(PWORK2PROJ)$(;) $(DEBUGGER) $(PPROJ2WORK)$(PATHSEP)$(subst /,$(PATHSEP),$(TARGET))$(PATHSEP)$(TOP_DU)$(EXE_EXT) $(INVOKE_ARGS)

GCOV_OPTS := --quiet --rc geninfo_auto_base=1 --rc lcov_branch_coverage=1 --external
GCOV_DIRS  := --directory $(TARGET)
GCOV_FILT_PAT := '/usr/include/*' '/usr/lib/*' '4.2.2/*' '$(MGC_HOME)/shared/include/*'
GCOV_BASE     := $(TARGET)$(PATHSEP)base.info
GCOV_TEST     := $(TARGET)$(PATHSEP)test.info
GCOV_TOTAL    := $(TARGET)$(PATHSEP)total.info
GCOV_FILT     := $(TARGET)$(PATHSEP)filt.info
GCOV_HTML_DIR := $(TARGET)$(PATHSEP)test-lcov

sim: build
ifeq "$(GCOV_ENABLED)" "true"
	@-$(ECHO) "============================================"
	@-$(ECHO) "Preparing GCOV baseline"
	-$(LCOV) $(GCOV_OPTS) --capture --initial $(GCOV_DIRS) --output-file $(GCOV_BASE)
endif
	@-$(ECHO) "============================================"
	@-$(ECHO) "Simulating design"
	$(CD) $(PWORK2PROJ)$(;) $(PPROJ2WORK)$(PATHSEP)$(subst /,$(PATHSEP),$(TARGET))$(PATHSEP)$(TOP_DU)$(EXE_EXT) $(INVOKE_ARGS)
ifeq "$(GCOV_ENABLED)" "true"
	@-$(ECHO) "============================================"
	@-$(ECHO) "Processing GCOV data"
	@-$(LCOV) $(GCOV_OPTS) --capture $(GCOV_DIRS) --output-file $(GCOV_TEST)
	@-$(LCOV) $(GCOV_OPTS) --add-tracefile $(GCOV_BASE) --add-tracefile $(GCOV_TEST) --output-file $(GCOV_TOTAL)
	@-$(LCOV) $(GCOV_OPTS) --remove $(GCOV_TOTAL) $(GCOV_FILT_PAT) -o $(GCOV_FILT)
	@-$(GENHTML) --rc geninfo_auto_base=1 --rc lcov_branch_coverage=1 --ignore-errors source $(GCOV_FILT) --legend --output-directory=$(GCOV_HTML_DIR)
	@-$(ECHO) "============================================"
	@-$(ECHO) "HTML coverage report written to: $(GCOV_HTML_DIR)$(PATHSEP)index.html"
#	@-$(MGC_HOME)/pkgs/sif/.bin/www_browser $(GCOV_HTML_DIR)$(PATHSEP)index.html
endif

ifneq "$(OS)" "Windows_NT"
valgrind: build
	$(CD) $(PWORK2PROJ)$(;) $(VALGRIND) $(VALGRIND_OPTS) $(PPROJ2WORK)$(PATHSEP)$(TARGET)$(PATHSEP)$(TOP_DU)$(EXE_EXT) $(INVOKE_ARGS)
endif

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

helptext:
	@-$(ECHO) "OSCI/C++ Makefile"
	@-$(ECHO) "The valid targets are:"
	@-$(ECHO) "   simgui     Compile and Execute the simulation using the"
	@-$(ECHO) "              interactive mode of the simulator (if available)"
	@-$(ECHO) "   sim        Compile and Execute the simulation using the"
	@-$(ECHO) "              batch mode of the simulator"
ifneq "$(OS)" "Windows_NT"
	@-$(ECHO) "   valgrind   Same as 'sim' but run with Valgrind to detect memory usage errors (linux only)"
endif
	@-$(ECHO) "   build      Compile the models only"
	@-$(ECHO) "   clean      Remove all compiled objects"
	@-$(ECHO) "   help       Show this help text"
	@-$(ECHO) ""
	@-$(ECHO) "The current variables settings are:"

dumpvars:
	@-$(ECHO) "   OSCI flow options:"
ifneq "$(OS)" "Windows_NT"
	@-$(ECHO) "     VALGRIND          = $(VALGRIND)"
	@-$(ECHO) "     VALGRIND_OPTS     = $(VALGRIND_OPTS)"
endif
ifeq "$(CXX_TYPE)" "msvc"
	@-$(ECHO) "     PATH              = $(PATH)"
	@-$(ECHO) "     INCLUDE           = $(INCLUDE)"
	@-$(ECHO) "     LIB               = $(LIB)"
endif
	@-$(ECHO) "     OSCI_COMP_FLAGS   = $(OSCI_COMP_FLAGS)"
	@-$(ECHO) "     OSTYPE            = $(OSTYPE)"
	@-$(ECHO) "     CXX_OS            = $(CXX_OS)"
	@-$(ECHO) "     CXX_TYPE          = $(CXX_TYPE)"
	@-$(ECHO) "     CXX_HOME          = $(CXX_HOME)"
	@-$(ECHO) "     CXX_EXEC          = $(CXX_EXEC)"
	@-$(ECHO) "     CXXFLAGS          = $(CXXFLAGS)"
	@-$(ECHO) "     LDFLAGS           = $(LDFLAGS)"
	@-$(ECHO) "     DEBUGGER          = $(DEBUGGER)"
	@-$(ECHO) "     LINK_EXEC         = $(LINK_EXEC)"
	@-$(ECHO) "     SYSTEMC_INCDIR    = $(SYSTEMC_INCDIR)"
	@-$(ECHO) "     SYSTEMC_LIBDIR    = $(SYSTEMC_LIBDIR)"
