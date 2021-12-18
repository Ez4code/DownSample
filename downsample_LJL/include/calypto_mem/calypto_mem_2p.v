//------------------------------------------------------------------------------
// Catapult Synthesis - Sample I/O Port Library
//
// Copyright (c) 2003-2015 Mentor Graphics Corp.
//       All Rights Reserved
//
// This document may be used and distributed without restriction provided that
// this copyright statement is not removed from the file and that any derivative
// work contains this copyright notice.
//
// The design information contained in this file is intended to be an example
// of the functionality which the end user may study in preparation for creating
// their own custom interfaces. This design does not necessarily present a 
// complete implementation of the named protocol or standard.
//
//------------------------------------------------------------------------------

//=========================================================================================
// DUAL PORT SYNCHRONOUS MEMORY
//  -- Models cycle accurate behaviour
//  -- No memory hazard / coruption  is modeled  
//  -- Models Advanced Power Modes (Light Sleep, Deep Sleep, Shutdown) 
//  -- Models bit-level write bitmask (if mask bit is 1 then write is not done on that bit)
//  -- Models write-through behavior
//  -- Models read-write & write-write contentions
//==========================================================================================

module calypto_mem_2p (qa, qb, clka,
                 mea, rwa, wma, wadra, radra, da,
                 clkb, meb, rwb, wmb, wadrb, radrb, db,
                 ls, ds, sd);
parameter AW = 5;
parameter DW = 8;
parameter NW = 1<<AW;
parameter WT = 0;  // set to 1 for synchronous write through feature 
parameter ds_sd_zero = 1;
parameter UHD_arch = 0;
parameter READ_BEFORE_WRITE = 0;
parameter LS_DEPTH = 1;
parameter TCSEP = 0.001;
parameter WC_CORRUPT_OVERLAP_BITS = 0;  // in case of the write conflict, corrupt only the overlapping bits being written
parameter RC_CORRUPT_ALL_BITS = 0;  // in case of the read conflict, corrupt all bits irrespective of the mask
parameter RC_CORRUPT_WRITE = 0;  // if there is a read-conflict, write is corrupted


output [DW-1:0] qa; //read data (valid on read)
output [DW-1:0] qb; //read data (valid on read)

input clka; // clock positive edge triggered
input mea;  // portA selected when high
input rwa;  // write happens if rwa is 1 else read
input [AW-1:0] wadra; // write  address 
input [AW-1:0] radra; // read  address 
input [DW-1:0] da; //write data (used on write)
input [DW-1:0] wma;  // set to 1 to mask out any write

input clkb; // clock positive edge triggered
input meb;  // portB selected when high
input rwb;  // write happens if rwa is 1 else read
input [AW-1:0] wadrb; // write  address 
input [AW-1:0] radrb; // read  address 
input [DW-1:0] db; //write data (used on write)
input [DW-1:0] wmb;  // set to 1 to mask out any write
input ls;   // lightsleep
input ds;   // deep sleep
input sd;   // shut down


reg CLK_A_T, CLK_B_T;
wire same_clk_edge;
wire rena, renb, wena, wenb;
   
reg ls_int;
   
reg [DW-1:0] rda_ff, rdb_ff;
reg [DW-1:0] mem_core_array [0:(1<<AW)-1];




wire [DW-1:0] dina, dinb, Xina, Xinb, Xovl;
assign dina = ((da & ~wma) | (mem_core_array[wadra] & wma)) ^ (wma ^ wma); 
assign dinb = (db & ~wmb) | (mem_core_array[wadrb] & wmb) ^ (wmb ^ wmb); 
assign Xina = ({DW{1'bX}} & ~wma) | (mem_core_array[wadra] & wma)  ^ (wma ^ wma);
assign Xinb = ({DW{1'bX}} & ~wmb) | (mem_core_array[wadrb] & wmb) ^ (wmb ^ wmb); 
assign Xovl = (da & ~wma & wmb) | (db & ~wmb & wma) | ({DW{1'bX}} & ~wmb & ~wma) | (mem_core_array[wadrb] & wma & wmb); 
   
//-- check for  write contention
task WritePortsCC_A;
begin
if (wena & wenb & (wadra == wadrb) & (~&wma) & (~&wmb)) // write conflict
  begin
   mem_core_array[wadra] = WC_CORRUPT_OVERLAP_BITS?Xovl:Xina;
   $display("Write Conflict: Port A/B for address %d at time %t", wadra, $realtime);
  end
end
endtask

task WritePortsCC_B;
begin
if (wena & wenb & (wadra == wadrb) & (~&wma) & (~&wmb)) // write conflict
  begin
   mem_core_array[wadrb] = WC_CORRUPT_OVERLAP_BITS?Xovl:Xinb;
   $display("Write Conflict: Port A/B for address %d at time %t", wadra, $realtime);
  end
end
endtask

wire sel_pwr = sd | ds;

//-- check for read conflicts
task ReadPortsCC_A;
begin
 ///-- READ A
 if (sel_pwr  )
     rda_ff = (ds_sd_zero ) ? {DW{1'b0}} : {DW{1'bX}};
 else if (rena) begin
  if (wenb && (wadrb == radra) & wmb!=={DW{1'b1}}) // Read contention
  begin 
     if (RC_CORRUPT_WRITE)
       mem_core_array[wadrb] = Xinb;
     rda_ff = (WT == 1'b0) ? RC_CORRUPT_ALL_BITS?{DW{1'bX}}:(mem_core_array[radra] ^ ({DW{1'bX}} & ~wmb)) : dinb;
     $display("Read  Conflict: Port A for address %d at time %t", radra, $realtime);
  end
 end
end
endtask

task ReadPortsCC_B;
begin
 ///-- READ B
 if (sel_pwr )
     rdb_ff = (ds_sd_zero ) ? {DW{1'b0}} : {DW{1'bX}};
 else if (renb) begin
  if (wena && (wadra == radrb) & wma!=={DW{1'b1}}) // Read contention
  begin
    if (RC_CORRUPT_WRITE)
       mem_core_array[wadra] = Xina;
    rdb_ff = (WT == 1'b0) ? RC_CORRUPT_ALL_BITS?{DW{1'bX}}:(mem_core_array[radrb] ^ ({DW{1'bX}} & ~wma)) : dina;
    $display("Read  Conflict: Port B for address %d at time %t", radrb, $realtime);
  end
 end
end
endtask

reg [DW-1:0] mema_o, memb_o;
always @(rda_ff) begin
 mema_o <= rda_ff;
end
always @(rdb_ff) begin
 memb_o <= rdb_ff;
end

`ifndef CALYPTO_SYNTH
always @(posedge clka) begin
  CLK_A_T = 1'b1;
  #TCSEP CLK_A_T = 1'b0;
end

always @(posedge clkb) begin
  CLK_B_T = 1'b1;
  #TCSEP CLK_B_T = 1'b0;
end

assign same_clk_edge = CLK_A_T & CLK_B_T;

// Normal operation
always @(posedge clka) begin
  if (wena & ~same_clk_edge)
    mem_core_array[wadra] = dina;
  if (sel_pwr )
     rda_ff = ds_sd_zero ? {DW{1'b0}} : {DW{1'bX}};
  else if (WT ? (wena | rena) : rena)
     rda_ff = mem_core_array[radra];
 end


always @(posedge clkb) begin
  if (wenb & ~same_clk_edge)
    mem_core_array[wadrb] = dinb;
  if (sel_pwr )
     rdb_ff = ds_sd_zero ? {DW{1'b0}} : {DW{1'bX}};
  else if (WT ? (wenb | renb) : renb)
     rdb_ff = mem_core_array[radrb];
end

//-- Handle contentions only when edges align
//-- This overrides normal mode assignment above
always @(posedge same_clk_edge) begin
  WritePortsCC_A;
  WritePortsCC_B;
  ReadPortsCC_A;
  ReadPortsCC_B;
end

integer i;
always @(posedge sd )
begin
  for ( i = 0 ; i < NW ; i = i + 1 )
  begin
    mem_core_array[i] = {DW{1'bX}};
  end
end
   always @(ls)
     ls_int = ls;
 
`else
//-- Synthesis cannot model sensitivity to two clocks for same seq. element. So writes always assumed to 
//-- happen on clka. Upto design to optimize only under this scenario 
always @(posedge clka ) begin
        WritePortsCC_A;
        WritePortsCC_B;
end
//-- Assumes Read contention occurs when read is issued even if write clock edge is not aligned
always @(posedge clka) begin
        ReadPortsCC_A;
end
always @(posedge clkb) begin
        ReadPortsCC_B;
end

(* ls_regs *)   reg [LS_DEPTH-1:0] ls_regs_clka, ls_regs_clkb;
   integer 	      i;

generate
if ( LS_DEPTH < 2)
begin
    always @ ( posedge clka) 
       ls_regs_clka <= ls;
   
    always @ ( posedge clkb) 
       ls_regs_clkb <= ls;
end
else
begin
    always @ ( posedge clka) 
       ls_regs_clka = {ls_regs_clka[LS_DEPTH-2:0], ls};
    always @ ( posedge clkb) 
       ls_regs_clkb = {ls_regs_clkb[LS_DEPTH-2:0], ls};
end
endgenerate
   
   always @(*)
     begin
	ls_int = ls;
	for (i=0; i<LS_DEPTH; i = i + 1)
	  begin
	     ls_int = ls_int | (ls_regs_clka[i] & ls_regs_clkb[i]);
	  end
     end
   
`endif
  

wire normal_mode = !ls_int & !ds & !sd;
assign wena = mea & normal_mode & rwa ;
assign rena = mea & normal_mode & ~rwa;
assign wenb = meb & normal_mode & rwb ;
assign renb = meb & normal_mode & ~rwb;
always @(ls_int or mea or meb) begin
 if (ls_int & (mea | meb))
    $display("No operation as Memory in LightSleep at time %t", $realtime);
end

assign qa = (sel_pwr) ? ((ds_sd_zero) ? {DW{1'b0}} : {DW{1'bX}}) : mema_o;
assign qb = (sel_pwr) ? ((ds_sd_zero) ? {DW{1'b0}} : {DW{1'bX}}) : memb_o;

endmodule


