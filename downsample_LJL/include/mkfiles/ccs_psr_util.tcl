   
namespace eval CatapultC_Extensions {
   variable q
   
   proc locate_simulation_libs { vendor tech lang simtool stage } {
      # vendor =  { Xilinx Altera }
      # tech = {...}
      # lang =    { vhdl | verilog }
      # simtool = { mti | ncsim }
      # stage =   { mapped | gate }
      # returns list of pairs of {libname libpath}
      switch -glob -- "${vendor}-${tech}" {
         "Xilinx*" {
            return [xilinx_simulation_libs $lang $simtool $stage]
         }
         "Altera*" {
            return [altera_simulation_libs $tech $lang $simtool $stage]
         }
      }
      return {}
   }
   
   
   proc xilinx_simulation_libs { lang simtool stage } {
      global env
      # lang =    { vhdl | v }
      # simtool = { mti | ncsim }
      # stage =   { mapped | gate }
      # returns list of pairs of {libname libpath}
      switch -glob -- $lang {
         "vhd*" { set lang vhdl }
         "v"    { set lang verilog }
      }
      # Try to locate library based on lang and simtool
      set matches [glob -nocomplain -types {d} [file join $env(XILINX) $lang $simtool*]]
      if { ([llength $matches] == 0) && ![info exists env(XILINX_LIB)] } {
         puts "Error: Could not locate Xilinx simulation libraries at default location \$XILINX/$lang/$simtool='[file join $env(XILINX) $lang $simtool*]'."
      }
      if { ([llength $matches] == 0) && [info exists env(XILINX_LIB)] } {
         # DCS groups installation is weird
         set matches [glob -nocomplain -types {d} [file join $env(XILINX_LIB) $lang data $simtool*]]
         if { ([llength $matches] == 0) } {
            set matches [glob -nocomplain [file join $env(XILINX_LIB) $lang]]
         }
         if { ([llength $matches] == 0) } {
            puts "Error: Could not locate Xilinx simulation libraries at alternate location \$XILINX_LIB/$lang/$simtool='[file join $env(XILINX_LIB) $lang data $simtool*]'."
         }
      }
      set liblist {}
      set lang_suffix ""
      if { $lang == "verilog" } {
         set lang_suffix "s_ver"
      }
      if { [llength $matches] > 0 } {
         set baselibpath [lindex $matches 0]
         lappend liblist [list unisim$lang_suffix [file join $baselibpath unisim$lang_suffix]]
         lappend liblist [list simprim$lang_suffix [file join $baselibpath simprim$lang_suffix]]
      } else {
         puts "Error: Check your XILINX and XILINX_LIB environment variables and Xilinx installation."
      }

      return $liblist
   }
   
   proc altera_simulation_libs { tech lang simtool stage } {
      global env
   	# lang =    { vhdl | verilog }
      # simtool = { mti | ncsim }
      # stage =   { mapped | gate }
   	# returns list of pairs of {libname libpath}
      # Try to locate library based on lang and simtool
      set baselibpath ""
      switch -glob -- $lang {
         "vhd*" { set lang vhdl }
         "v"    { set lang verilog }
      }
      # HACK - in DCS test harness, we (rightly) compile the "lpm" code contained in 220model
      # into the library "lpm". In the Modelsim-Altera tree, the logical library name "lp" is
      # mapped to the physical library "220model"
      set real_lpm_phys_name "lpm"
      if { ($simtool == "mti") && [info exists env(MODEL_TECH)] } {
         set baselibpath [file join $env(MODEL_TECH) .. altera $lang]
         if { ![file isdirectory $baselibpath] } {
            set baselibpath ""
         } else {
            set real_lpm_phys_name "220model"
         }
      }
      if { ($baselibpath == "") && ![info exists env(QUARTUS_LIB)] } {
         puts "Error: Could not locate Altera simulation libraries at default location \$MODEL_TECH/../altera/$lang='[file join $env(MODEL_TECH) .. altera $lang]'."
         puts "Error: Check your MODEL_TECH environment variable and Modelsim-Altera installation."
      }
      if { ($baselibpath == "") && [info exists env(QUARTUS_LIB)] } {
         # DCS groups installation is weird
         set baselibpath [file join $env(QUARTUS_LIB) $lang]
         if { $baselibpath == "" } {
            puts "Error: Could not locate Altera simulation libraries at default location \$MODEL_TECH/../altera/$lang='[file join $env(MODEL_TECH) .. altera $lang]'."
            puts "Error: Check your MODEL_TECH environment variable and Modelsim-Altera installation."
            puts "Error: Could not locate Altera simulation libraries at alternate location \$QUARTUS_LIB/$lang='[file join $env(QUARTUS_LIB) $lang]'."
            puts "Error: Check your QUARTUS_LIB environment variable."
         }
      }
      set liblist {}
      if { ($baselibpath != "") && ([file isdirectory $baselibpath]) } {
         lappend liblist [list altera [file join $baselibpath altera]]
         lappend liblist [list altera_mf [file join $baselibpath altera_mf]]
         lappend liblist [list lpm [file join $baselibpath $real_lpm_phys_name]]
         lappend liblist [list $tech [file join $baselibpath $tech]]
      }
      return $liblist
   }
   
   proc remove_sdf_annotate { infile outfile } {
      if { ![file exists $infile] } {
         puts "Error - input file $infile not found"
         return
      }
      set s [open $infile "r"]
      set d [open $outfile "w"]
      while { ! [eof $s] } {
         gets $s line
         if { [string match "*\$sdf_annotate*" $line] == 0 } {
            puts $d $line
         }
      }
      close $s
      close $d
   }

   proc vendor_vars { vendor tech lang simtool stage } {
      # returns a list { netlist_output_directory netlist_file_suffix sdf_file_suffix sdf_inst sim_opts }
      set SDFINST ""
      switch -glob -- "${vendor}-${tech}" {
         "Xilinx*" {
            if { $stage == "gate" } {
               set SDFINST scverify_top/rtl
            }
            if { $lang == "vhdl" } {
               return [list VNDR_NETDIR . VNDR_NETSUF _out.vhd VNDR_SDFSUF _out.sdf VNDR_SDFINST $SDFINST]
            } else {
               return [list VLOG_OPTS \$(XILINX)/verilog/src/glbl.v SIM_OPTS glbl VNDR_NETDIR . VNDR_NETSUF _out.v VNDR_SDFSUF _out.sdf VNDR_SDFINST $SDFINST]
            }
         }
         "Altera*" {
            if { $stage == "gate" } {
               set SDFINST scverify_top/rtl
            }
            if { $lang == "vhdl" } {
               return [list VNDR_NETDIR simulation/modelsim VNDR_NETSUF .vho VNDR_SDFSUF _vhd.sdo VNDR_SDFINST $SDFINST]
            } else {
               return [list VNDR_NETDIR simulation/modelsim VNDR_NETSUF .vo VNDR_SDFSUF _v.sdo VNDR_SDFINST $SDFINST]
            }
         }
      }
   }

   proc generate_netlist { TARGET_DIR lang simtool stage } {
      set IMPL_DIR [MGS_Core::get_design_impls -active]
      set DESIGNNAME [report_project -basename]
      set vendor [report_project -manufacturer]
      set tech [report_project -libname]
      set vendor_var_list [vendor_vars $vendor $tech $lang $simtool $stage]
      foreach { vname vval } $vendor_var_list {
         set $vname $vval
      }
      set USE_SIM [lindex [split $TARGET_DIR _] 3]
      if { $stage == "mapped" } {
         set NETLIST_FILE ${TARGET_DIR}/scverify_mapped_$USE_SIM.$lang
         puts "auto_write $NETLIST_FILE"
         auto_write $NETLIST_FILE
      } else {
         set NETLIST_FILE ${IMPL_DIR}/${VNDR_NETDIR}/${DESIGNNAME}${VNDR_NETSUF}
         if { $lang == "v" } {
            puts "Copying vendor netlist '$NETLIST_FILE' to '$TARGET_DIR/scverify_${stage}_${USE_SIM}.$lang' and removing \$sdf_annotate"
            remove_sdf_annotate $NETLIST_FILE [file join $TARGET_DIR scverify_${stage}_${USE_SIM}.$lang]
         } else {
            puts "Copying vendor netlist '$NETLIST_FILE' to '$TARGET_DIR/scverify_${stage}_${USE_SIM}.$lang'"
            file copy -force $NETLIST_FILE [file join $TARGET_DIR scverify_${stage}_${USE_SIM}.$lang]
         }
         set NETLIST_SDF ${IMPL_DIR}/${VNDR_NETDIR}/${DESIGNNAME}${VNDR_SDFSUF}
         puts "Copying SDF file '$NETLIST_SDF' to '$TARGET_DIR/scverify_$stage.sdf'"
         file copy -force $NETLIST_SDF [file join $TARGET_DIR scverify_$stage.sdf]
      }
   }
   
   proc write_vendormaps_mk { TARGET_DIR lang simtool stage } {
      set vendor [report_project -manufacturer]
      set tech [report_project -libname]
      if { $TARGET_DIR != "" } {
         set mkfile [open [file join $TARGET_DIR vendormaps.mk] "w"]
      } else {
         set mkfile stdout
      }
      puts $mkfile "# Export all variables"
      puts $mkfile "export"
      puts $mkfile ""
      set DESIGNNAME [report_project -basename]
      puts $mkfile "DESIGNNAME = $DESIGNNAME"
      puts $mkfile ""
      puts $mkfile "vendormaps: \$(TARGET)/make_libs "

      puts $mkfile "# Copy output netlist to standard location"
      puts $mkfile "# Include simulator specific makefile"
      puts $mkfile "include Verify_\$(STAGE)_\$(NETLIST)_\$(SIMTOOL).mk"
      if { $TARGET_DIR != "" } {
         close $mkfile
      }
   }
}
