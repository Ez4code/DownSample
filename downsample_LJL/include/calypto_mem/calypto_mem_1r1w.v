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
// 1read/1write PORT SYNCHRONOUS MEMORY
//  -- Models cycle accurate behaviour
//  -- No memory hazard / coruption  is modeled  
//  -- Models Advanced Power Modes (Light Sleep, Deep Sleep, Shutdown) 
//  -- Models bit-level write bitmask (if mask bit is 1 then write is not done on that bit)
//  -- Models write-through behavior
//  -- Models read-write contention
//==========================================================================================

// MEMORY INTERFACE PINS
//  rclk - read port clock (rising edge)
//  rme  - read port select (active high)
//  radr - read port address
//  q    - read data output
//
//  wclk - write port clock (rising edge)
//  wme  - write port select (active high)
//  wadr - write port address
//  d    - write port data
//  wm   - write mask (if bit == 1 write is masked)
//
//  ls   - light sleep mode (active high?)
//  ds   - deep sleep mode (active high?)
//  sd   - shut down (active high?)
//
// MEMORY CONFIGURATION PARAMETERS
//  AW                      - address bus width
//  DW                      - data bus width
//  WT                      - write-through mode (1 for synchronous write through feature)
//  NW                      - number of words (must fit in space addressable by AW)
//  ds_sd_zero              - ???
//  UHD_arch                - ???
//  READ_BEFORE_WRITE       - read before write mode (1 == read-before-write, 0 == read-after-write)
//  LS_DEPTH                - ???
//  TCSEP                   - ???
//  WC_CORRUPT_OVERLAP_BITS - ???
//  RC_CORRUPT_ALL_BITS     - ??? // in case of the read conflict, corrupt all bits irrespective of the mask
//  RC_CORRUPT_WRITE        - ???

module calypto_mem_1r1w (q, rclk, rme, radr, wclk, wme, wm, wadr, d,ls, ds, sd);
   parameter AW = 5;
   parameter DW = 8;
   parameter NW = 32;
   parameter WT = 0;  // set to 1 for synchronous write through feature
   parameter ds_sd_zero = 1;
   parameter UHD_arch = 0;
   parameter READ_BEFORE_WRITE = 0;
   parameter LS_DEPTH = 1;
   parameter TCSEP = 0.001;
   parameter WC_CORRUPT_OVERLAP_BITS = 0;
   parameter RC_CORRUPT_ALL_BITS = 0;  // in case of the read conflict, corrupt all bits irrespective of the mask
   parameter RC_CORRUPT_WRITE = 0;

   output [DW-1:0] q;    // read data (valid on read)
   input           rclk; // clock positive edge triggered
   input           rme;  // read port selected when high
   input [AW-1:0]  radr; // read  address 

   input           wclk; // clock positive edge triggered
   input           wme;  // write port selected when high
   input [AW-1:0]  wadr; // write  address 
   input [DW-1:0]  d;    // write data (used on write)
   input [DW-1:0]  wm;   // write mask (if bit == 1 write is masked)

   input           ls;   // lightsleep
   input           ds;   // deep sleep
   input           sd;   // shut down

   localparam R_B_W = READ_BEFORE_WRITE ? READ_BEFORE_WRITE : UHD_arch;

   ///-- LOCAL SIGNALS
   wire [DW-1:0]   din;
   reg [DW-1:0]    rd_ff;
   reg [DW-1:0]    mem_core_array [0:(1<<AW)-1];
   reg             ls_int;
   wire            ren;
   wire            wen;
   wire            sel_pwr;
   wire            same_clk_edge;

   ///-- READ WITH CONFLICT CHECK
   task ReadPort_CC;
   begin
      if (sel_pwr)   
         rd_ff = (ds_sd_zero) ? {DW{1'b0}} : {DW{1'bX}};
      else
         if (ren)
            begin
               if (!R_B_W & wen & (radr == wadr))
                  begin
                     //rd_ff =  (WT == 1'b0) ? (mem_core_array[radr] ^ ({DW{1'bX}} & ~ wm)) : din;
                     rd_ff = (WT == 1'b0) ? RC_CORRUPT_ALL_BITS?{DW{1'bX}}:(mem_core_array[radr] ^ ({DW{1'bX}} & ~wm)) : din;
                     $display("READ Conflict for address %d at time %t", radr, $realtime);
                  end
               else
                  rd_ff = mem_core_array[radr];
            end
         else 
            if (WT & wen)
               begin
                  rd_ff =  din;
               end
   end
   endtask

`ifndef CALYPTO_SYNTH

   reg CLK_R_T, CLK_W_T;
   always @(posedge rclk)
      begin
         CLK_R_T = 1'b1;
         #TCSEP CLK_R_T = 1'b0;
      end

   always @(posedge wclk)
      begin
         CLK_W_T = 1'b1;
         #TCSEP CLK_W_T = 1'b0;
      end

   assign same_clk_edge = CLK_R_T & CLK_W_T;

   //-- READ WITHOUT CONFLICT
   always @(posedge rclk)
      begin
         if (sel_pwr)   
            rd_ff =  ds_sd_zero ? {DW{1'b0}} : {DW{1'bX}};
         else
            if (WT ? (wen | ren) : ren)
               rd_ff = mem_core_array[radr];
      end

   integer i;
   always @(posedge sd)
      begin
         for (i = 0 ; i < NW ; i = i + 1)
            begin
               mem_core_array[i] = {DW{1'bX}};
            end
      end

   always @(ls)
      ls_int = ls;

`else

   (* ls_regs *) // registers with ls_regs attribute
   reg [LS_DEPTH-1:0] ls_regs_rclk, ls_regs_wclk;

   integer 	      i;

   generate
      if (LS_DEPTH < 2)
         begin
            always @ (posedge rclk)
               ls_regs_rclk = ls;
            always @ (posedge wclk)
               ls_regs_wclk = ls;
         end
      else
         begin
            always @ (posedge rclk)
               ls_regs_rclk = {ls_regs_rclk[LS_DEPTH-2:0], ls};
            always @ (posedge wclk)
               ls_regs_wclk = {ls_regs_wclk[LS_DEPTH-2:0], ls};
         end
   endgenerate

   always @(*)
      begin
         ls_int = ls;
         for (i=0; i<LS_DEPTH; i = i + 1)
            begin
               ls_int = ls_int | (ls_regs_rclk[i] & ls_regs_wclk[i]);
            end
      end

   assign same_clk_edge = rclk;
   
`endif

   assign ren = rme & !ls_int & !ds & !sd ;
   assign wen = wme & !ls_int  & !ds & !sd;
   assign sel_pwr = sd | ds;
   assign q = (sel_pwr) ? ((ds_sd_zero) ? {DW{1'b0}} : {DW{1'bX}}) : rd_ff;

   //-- Override when conflict occurs on aligned edges
   always @(posedge same_clk_edge)
      begin
         ReadPort_CC;
      end

   ///-- WRITE 
   assign din = (d & ~wm) | (mem_core_array[wadr] & wm); 
   always @(posedge wclk)
     if (!R_B_W && wen)
        begin
           mem_core_array[wadr] = din;
        end

endmodule

