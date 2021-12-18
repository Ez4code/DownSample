#===============================================================================
# ccs_default_cmds.mk
# DEFAULT COMMANDS
#
ifneq "$(shell echo %DATE%)" "%DATE%"
  ifneq "$(ComSpec)" ""
    MAKE_type := $(ComSpec)
  else
  ifneq "$(COMSPEC)" ""
    MAKE_type := $(COMSPEC)
  endif
  endif
  ;         := &                   # command.com uses a "&" to combine multiple commands.
  PATHSEP   := \$(null)
  DIRSEP    := ;
  CD        := chdir
  ECHO      := echo
  RM        := del /F
  MV        := move /Y
  COPY      := copy /Y
  RMDIR     := rmdir /s /q
  MKDIR     := -mkdir
  CAT       := type
  RN        := rename
  DATE      := @$(ECHO) %DATE% %TIME%
  DIE        = || $(RM) $(subst /,\,$@)
  TIMESTAMP  = $(DATE) >> $(subst /,\,$@)
  GETENV     = %$(1)%
  CXX_OS    := Windows_NT
  TEE        = $(subst /,$(PATHSEP),$(TCLSH_CMD)) $(subst /,$(PATHSEP),$(MGC_HOME)/pkgs/sif/userware/En_na/tee.tcl)
  EXE_EXT   := .exe
else
  MAKE_type := $(SHELL)
  ;         := ;
  PATHSEP   := /
  DIRSEP    := :
  CD        := cd
  ECHO      := echo
  MV        := mv -f
  COPY      := cp -f
  RM        := rm -f
  RMDIR     := rm -rf
  MKDIR     := mkdir -p
  CAT       := cat
  RN        := mv
  DATE      := date
  DIE        = || $(RM) $@
  TIMESTAMP  = @$(DATE) >> $@
  GETENV     = $$$(1)
  CXX_OS    := $(shell uname -s)
  TEE        = tee
  EXE_EXT   :=
endif

# Platform independent commands
TCLSH_CMD  = $(MGC_HOME)/bin/tclsh8.5$(EXE_EXT)
TCLSED     = $(subst /,$(PATHSEP),$(TCLSH_CMD)) $(subst /,$(PATHSEP),$(MGC_HOME)/pkgs/sif/userware/En_na/util_sed.tcl)

ifeq "$(MAKE_type)" ""
  $(error Unable to determine make shell type)
endif

ifeq "$(OS)" "Windows_NT"
# Ensure SYSTEMROOT set with correct case and slashes for Windows environment
SYSTEMROOT_TMP   := $(SYSTEMROOT)
ifeq "$(SYSTEMROOT)" ""
  SYSTEMROOT_TMP   := $(SystemRoot)
endif
unexport SYSTEMROOT
unexport SystemRoot
export SYSTEMROOT := $(subst /,\,$(SYSTEMROOT_TMP))
endif

ifeq "$(SCVerify_USE_CCS_BLOCK)" "true"
CXXFLAGS += -DCCS_SCVERIFY_USE_CCS_BLOCK
endif

ifeq "$(STAGE)" "cycle"
CXXFLAGS += -DCCS_DUT_CYCLE
endif

ifeq "$(STAGE)" "rtl"
CXXFLAGS += -DCCS_DUT_RTL
endif

ifeq "$(STAGE)" "gate"
CXXFLAGS += -DCCS_DUT_GATE
endif

ifeq "$(STAGE)" "power"
CXXFLAGS += -DCCS_DUT_RTL -DCCS_DUT_POWER
endif

ifeq "$(STAGE)" "prepwr"
CXXFLAGS += -DCCS_DUT_RTL -DCCS_DUT_POWER
endif

ifeq "$(NETLIST)" "vhdl"
CXXFLAGS += -DCCS_DUT_VHDL
endif

ifeq "$(NETLIST)" "v"
CXXFLAGS += -DCCS_DUT_VERILOG
endif

ifeq "$(NETLIST)" "cxx"
CXXFLAGS += -DCCS_DUT_SYSC
endif

## This will cause ANY target to be deleted if it fails (and not marked .PHONY)
## GNU make specific extention
.DELETE_ON_ERROR:


comma := ,
empty :=
space := $(empty) $(empty)
semi  := ;
quote := "

# Take a list of double-quoted spaced delimited paths $(3) with potential
# embedded whitespace and apply the prefix text $(2) to each
# element using an intermediate char $(1) to mark the whitespace
# during translation. Example:
# CXX_INCLUDEDIRS += "/a/b/c"
# CXX_INCLUDEDIRS += "/d/e f/g"
# CXX_INC := $(call prefix_foreach,$(semi),-I,$(CXX_INCLUDEDIRS))
# should result in CXX_INC being -I"/a/b/c" -I"/d/e f/g"
prefix_foreach = $(subst $(1),$(space),$(foreach a,$(filter-out $(1),$(subst $(quote),$(space),$(subst $(space),$(1),$(3)))),$(2)$(quote)$(a)$(quote)))

.PHONY: dumpsysvars
dumpsysvars:
	@-$(ECHO) "   Current Makefile: $(WORK_DIR)/$(firstword $(MAKEFILE_LIST))"
	@-$(ECHO) "   Shell/Make environment variables:"
	@-$(ECHO) "     MAKE shell            = $(MAKE_type)"
	@-$(ECHO) "     CXX_OS                = $(CXX_OS)"
	@-$(ECHO) "     CXX_TYPE              = $(CXX_TYPE)"
ifeq "$(OS)" "Windows_NT"
	@-$(ECHO) "     SYSTEMROOT            = $(SYSTEMROOT)"
endif
	@-$(ECHO) "     MGC_HOME              = $(MGC_HOME)"
	@-$(ECHO) "     WORK_DIR              = $(WORK_DIR)"
	@-$(ECHO) "     TARGET                = $(TARGET)"
	@-$(ECHO) "     WORK2PROJ             = $(WORK2PROJ)"
	@-$(ECHO) "     WORK2SOLN             = $(WORK2SOLN)"
	@-$(ECHO) "     PROJ2WORK             = $(PROJ2WORK)"
	@-$(ECHO) "     PROJ2SOLN             = $(PROJ2SOLN)"

