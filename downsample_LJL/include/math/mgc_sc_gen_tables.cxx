////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2011 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Source:         mgc_sc_gen_tables.cxx
//  Description:    generate tables
//////////////////////////////////////////////////////////////////////////////
//  Compilation:
//    c++ -DSC_INCLUDE_FX -I$MGC_HOME/shared/include mgc_sc_gen_tables.cxx \
//        -L$MGC_HOME/shared/lib -o gen_tables -lsc_vsi
//////////////////////////////////////////////////////////////////////////////


#define MGC_SC_TRIG_GEN_TABLES
//#include "systemc/math/mgc_sc_trig.h"
#include "mgc_sc_trig.h"

#ifndef __SYNTHESIS__
#include <math.h>
#include "stdlib.h"
#include "fstream"

static void print_table(char *file_name, table_t (*f)(int)) {
  ofstream fp;
  fp.open(file_name);
  if(fp.fail()) {
    cerr << "Cannot open file " << file_name << endl;
    exit(-1);
  }
  for(int i=0; i < TE; i++) {
    if(i)
      fp << "," << endl;
    fp << "  " <<  (*f)(i);  
  }
  fp << endl;
  fp.close();
}

int main() {
  print_table("mgc_sc_atan_pow2.tab", atan_2mi);
  print_table("mgc_sc_atan2_pow2.tab", atan_pi_2mi);
  print_table("mgc_sc_K_cordic.tab", K);
/*
  ofstream fp;
  fp.open("mgc_sc_atan_pow2.tab");
  if(fp.fail()) {
    cerr << "Cannot open file mgc_sc_atan_pow2.tab" << endl;
    exit(-1);
  }
  for(int i=0; i < TE; i++) {
    if(i)
      fp << "," << endl;
    fp << "  " <<  atan_2mi(i);  
  }
  fp << endl;
  fp.close();
  fp.open("mgc_sc_K_cordic.tab");
  if(fp.fail()) {
    cerr << "Cannot open file mgc_sc_K_cordic.tab" << endl;
    exit(-1);
  }
  for(int i=0; i < TE; i++) {
    if(i)
      fp << "," << endl;
    fp << "  " <<  K(i);  
  }
  fp << endl;
  fp.close();
*/
}
#endif
