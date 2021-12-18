# Olympus Tcl Script

# Run Variables - Design Name, Technology, Utilization, ...
set technology "65"
set max_fanout "32"
set location [pwd]

###########################################################

## Import files
puts "****Importing Files****"

# Setup application title
config_title -display { product_name version design_name } -message ""

# Configure the search path
config_search_path -dir { $location }

# Import technology files 
read_lef -files $leffiles -power_pins_pattern { VDD } -ground_pins_pattern { VSS }

# Import timing library files 

read_library -files $synlibs

# Import process technology file (ptf) with interconnect parasitics [If any]
#

# Import netlist files 
read_$lang -files [list $netlistfile ] -top $top_name

# Import timing constraints files 
read_constraints -file $sdcfile
# additional timing constraints for combinational
set_max_delay $clk_per -from [all_inputs]
set_max_delay $clk_per -to [all_outputs]

# Pre-Floorplan Checks, Reports and db
check_technology
report_technology
check_design
report_design
write_db -file $top_name.pfp.db


## Floorplanning
puts "****Floorplanning****"

# Creating a Floorplan and tracks and db
if {$coreareax ne 0} {
    create_chip -xl_area 0 -yb_area 0 -xr_area $coreareax -yt_area $coreareay -core_site $CORE -xl_margin 0a -yb_margin 0a -xr_margin 0a -yt_margin 0a -orient north -double_backed true -place_ports
} else {
    create_chip -aspect $aspect -utilization $util -core_site $CORE -double_backed true -place_ports true -xl_margin 10000 -xr_margin 10000 -yb_margin 10000 -yt_margin 10000
}
create_tracks
write_db -file $top_name.fp.db


## Power Grid Generation
puts "****Creating Power Grid****"

# Creating Power Nets, Stripes and Rings and connecting them
# Power Nets
create_net -name VSS -type ground
create_net -name VDD -type power
# Power Stripes across the chip, Horizontal and Veritical
create_power_stripes -net VSS -layer $srcmetal1 -width 4000 -pitch 100000 -offset 50000
create_power_stripes -net VDD -layer $srcmetal1 -width 4000 -pitch 100000 -offset 40000
create_power_stripes -net VSS -layer $srcmetal2 -width 4000 -pitch 100000 -offset 50000
create_power_stripes -net VDD -layer $srcmetal2 -width 4000 -pitch 100000 -offset 40000
# Power Ring around the chip
create_power_ring -net VSS -hor_layer $srcmetal1 -ver_layer $srcmetal2 -width 20000 -offset 20000
create_power_ring -net VDD -hor_layer $srcmetal1 -ver_layer $srcmetal2 -width 20000 -offset 60000
# Configuring
config_route_power
route_power -partition $top_name -apply_to stripes
# Saving
write_db -file $top_name.ppl.db

# Pre-Placement (Pre-CTS) configure and report
config_scan_chains -enable false
config_timing -zero_rc_delays false
set_rcd_model -stage pre_cts
set_property -name max_layer -value $othrmetal [get_top_part]

report_design -name floorplan
report_timing

# Optimize - Remove buffer/inverter trees
optimize_precondition -force -objective {repeater_tree unconnected_logic demorgan}
report_design -name precondition
compare_reports -names {floorplan precondition} -merge columns


## Placement
puts "****Placement****"

# Placement - Global, Detail, report and check
new_place_global
place_detail
report_design -name place_detail
check_placement

# Buffering High-Fanout NetS
perform_hfns -max_fanout $max_fanout
# Reporting HFNS Cells
get_cells hfns*
report_design -name hfns
compare_reports -names {floorplan place_detail hfns} -merge columns

#Check and Report Placement Timing
check_placement
report_placement
slack_histogram
report_timing
report_path_group
create_path_line -obj [report_timing -obj]

# Pre-CTS Global Optimization - Worst negative slack and DRC violations
config_name_rules -cell_prefix PRECTS_GOPT
config_name_rules -net_prefix PRECTS_GOPT
optimize -mode global -obj {area wns drc}
create_marker -obj [get_cells PRECTS_GOPT*]

# Check for overlapping cells and Legalize based on timing (less critical cells)
check_placement
place_detail -driven_by timing

# Pre-CTS Global routing for more accurate CTS (delay insertion)
route_global
slack_histogram
report_constraint -all_violators
write_db -file $top_name.pl.db

if {$combinational eq "false"} {
    ## Clock Tree Synthesis (CTS)
    puts "****CTS****"

    # CTS Initialize
    set_rcd_model -stage cts
    config_timing -zero_rc_delays false
    config_report_timing -expand_clock_network

    # Creating CTS Specifications - 
    config_cts -use_inverters -buffers [get_lib_cells * -filter "@is_buffer"] -inverters [get_lib_cells * -filter "@is_inverter"] -min_route_layer $clkmetal2 -max_route_layer $clkmetal1
    # -max_trans - satisfy maximum transition constraint
    # -max_slew - satisfy max. skew constraint
    # -use_inverters - preference, use inverters else buffers
    set cts_targets [filter_collection -obj [get_clocks *] -expr "!@is_virtual && @is_ideal"]
    foreach clock $cts_targets {
        set name [get_property -name name -obj $clock]
        create_cts_spec -name $name -root [get_pin [get_property -name source [get_clock $clock]]] -opt_clock_gates size -cell_name_prefix "CTS"
    }
    report_property [get_object cts_spec]

    # CTS - Compile and Report
    compile_cts
    place_detail
    report_cts -cts_spec [get_obj cts_spec]
    check_placement

    # CTS - Set propagated clock and check timing
    set_propagated_clock [get_obj clock]
    report_timing
    slack_histogram
    report_constraint -max_delay -min_delay -max_transition
    create_marker -obj [get_cells PRECTS_GOPT*]
    write_db -file $top_name.cts.db

    ## Post-CTS Optimization
    puts "****POST-CTS OPTIMIZATION****"
} else {
    puts "****RE-PLACEMENT & RE-ROUTING OPTIMIZATION****"
}

# Re-Place and Re-Route (global) based on CTS information & Report
check_placement
place_detail -driven_by timing
route_global
slack_histogram
report_constraint -all_violators

# Post-CTS Optimization - Initialize and Report
set_analysis_corner -corner fast -enable -hold
config_report_timing -expand_clock_network
set_rcd_model -stage post_cts
report_variability
report_timing
report_timing -min
report_constraint -min_delay -max_delay -max_transition

# Post-CTS Optimization - Fix setup and hold Violations
config_name_rules -cell_prefix POSTCT_OPT
config_name_rules -net_prefix POSTCTS_OPT
optimize -mode global -obj {area wns hold max_trans}

# .. Recovering area
report_placement -name plc_before_area_recovery
optimize -mode global -obj area
report_placement -name plc_after_area_recovery
compare_report -names {plc_before_area_recovery plc_after_area_recovery} -merge rows

# .. Legalize and Perform GR
place_detail -driven_by timing
route_global

# .. optimize (local) to remove any negative slack & Report
optimize -mode local -obj {area wns hold}
place_detail -driven_by timing
route_global
report_variability
report_timing
report_timing -min
report_constraint -all_violators -min_delay -max_delay -max_transition
write_db -file $top_name.prt.db


puts "****ROUTING****"

# Routing - Initialize
set_rcd_model -stage post_cts
set_property -name max_layer -value $othrmetal [get_top_part]

# Routing - Check design if good for routing
flip_objects -objects [get_cells U77676] -axis x
move_objects -objects [get_cells U984] -dx 850 -dy 0
check_technology
check_floorplan
check_placement -check {overlap wrong_orientation}

# .. Fix illegal placement
flip_objects -objects [get_cells U77676] -axis x
move_objects -objects [get_cells U984] -dx -850 -dy 0
check_placement -check {overlap wrong_orientation}

# .. report technology, drc
report_tech layers
report_tech brief_drc

# .. set clock nets
set ClockNets [collect_clock_nets]
puts "[llength $ClockNets] clock nets detected"

# Create and apply non-default rule for clocks
#create_nondefault_rule -name CLK -min $clkmetal2 -max $clkmetal1
#set_nondefault_rule -obj [get_obj nond CLK] -spacing 2000 -layers {$clkmetal2 $clkmetal1}
#set_nondefault_rule -obj [get_obj nond CLK] -layers [get_layers -type via] -cut 2
#set_property -name nondefault_rule -value CLK -obj $ClockNets
#set_property -name balanced_gr -value true -obj $ClockNets
#report_tech nondefault

# Clock routing - Global -> Track -> Final
#route_global  -grid fine -nets $ClockNets -min $clkmetal2 -max $clkmetal1
#route_global -nets $ClockNets -min $clkmetal2 -max $clkmetal1
#report_routing -nets $ClockNets -global -name clk_gr
#route_track -follow_global -nets $ClockNets
#check_design -check {}
#route_final -nets $ClockNets
#route_final -mode accurate -nets $ClockNets
#report_routing -nets $ClockNets -name clk_dr
#puts "Examine clock routing. Note - it's mostly on $clkmetal2/$clkmetal1 w dbl-cut vias"

# Routing data nets
# .. Global routing
route_global -grid small
if {$combinational eq false} {
    report_route_cong
}

# .. Track routing
#set_property -name is_dont_route  -value true -obj $ClockNets
route_track
report_routing -name track_routing
check_drc -name track_drc

# .. clean LVS
check_lvs
clean_lvs -type {tail loop}

# .. Final Routing
#set_property -name is_dont_route -value false -obj $ClockNets
route_final -cpu 4
route_final -cpu 4 -mode accurate

# Check routing and compare reports
report_routing -name final_routing
check_drc -name final_drc
check_ndr
check_lvs
compare_reports -names {track_routing final_routing} -merge row
compare_reports -names {track_drc final_drc} -merge row
write_db -file $top_name.rt.db

## Post-Routing Optimization
puts "****Post-Routing Optimization****"

# Post-Routing optimization - Init and report
set_rcd_model -stage post_route
config_name_rules -cell_prefix POSTROUTE_OPT
config_name_rules -net_prefix POSTROUTE_OPT

report_constr -min_delay -max_delay -max_trans
report_timing
report_path_group -name before_opt

# .. Optimize
optimize -mode post_route -obj {area wns max_trans hold} -post_route_driven_by white_space
check_lvs

# .. Legalize Placement (if any illegal cells)
if {[llength [get_illegal_cells]] > 0} {
    place_detail
    create_move_lines -cell [get_cells]
}

# .. Route
route_final -effort expensive
route_final -mode accurate

# .. check lvs and drc
check_drc
check_lvs

# .. report
report_constr -min_delay -max_delay -max_trans
report_path_group -name after_opt
compare_reports -names {before_opt after_opt} -merge columns

# Reroute
route_global -grid small
set_property -name is_dont_route -value true -obj $ClockNets
route_track
set_property -name is_dont_route -value false -obj $ClockNets
route_final -cpu 4 -effort expensive
route_final -cpu 4 -mode accurate

# .. reporting
report_routing -name final_routing
check_drc -name final_drc
check_ndr
check_lvs
write_db -file $top_name.f1.db


# Final Try ..
set_rcd_model -stage post_route
config_name_rules -cell_prefix POSTROUTE_OPT
config_name_rules -net_prefix POSTROUTE_OPT
report_constr -min_delay -max_delay -max_trans
report_timing
report_path_group -name before_opt
optimize -mode post_route -obj {area wns hold} -post_route_driven_by white_space
check_lvs
if {[llength [get_illegal_cells]] > 0} {
place_detail
create_move_lines -cell [get_cells]
}
route_final
route_final -mode accurate
check_drc
check_lvs
report_constr -min_delay -max_delay -max_trans
report_path_group -name after_opt
write_db -file $top_name.f2.db
write_gds
report_design > final.rpt
report_timing > final_timing.rpt
