//-------------------------------------------------
// Conversion functions from various types to SystemC TLM2 Generic Payloads
//-------------------------------------------------

// Check for macro definitions that will conflict with template parameter names in this file
#if defined(Twidth)
#define Twidth 0
#error The macro name 'Twidth' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Twidth' giving the location of the previous definition.
#endif
#if defined(Ibits)
#define Ibits 0
#error The macro name 'Ibits' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Ibits' giving the location of the previous definition.
#endif
#if defined(Qmode)
#define Qmode 0
#error The macro name 'Qmode' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Qmode' giving the location of the previous definition.
#endif
#if defined(Omode)
#define Omode 0
#error The macro name 'Omode' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Omode' giving the location of the previous definition.
#endif
#if defined(Nbits)
#define Nbits 0
#error The macro name 'Nbits' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Nbits' giving the location of the previous definition.
#endif
#if defined(Tclass)
#define Tclass 0
#error The macro name 'Tclass' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'Tclass' giving the location of the previous definition.
#endif
#if defined(TclassW)
#define TclassW 0
#error The macro name 'TclassW' conflicts with a Template Parameter name.
#error The compiler should have produced a warning about redefinition of 'TclassW' giving the location of the previous definition.
#endif

// type_to_gp
//     &in - const reference to object to convert into generic payload
//     bitlen - bit length of 'in'
//     BB - BaseBit position in Catapult memory map
//     *gp - pointer to pre-allocated tlm generic payload (allocated with N bytes where N=ceil(0.5+bitlen/8))

#if defined(__AC_INT_H) && !defined(CS_GP_TYPECONV_H_AC_INT)
#define CCS_GP_TYPECONV_H_AC_INT
   // ---------------------------------------------------------
   // --------------------------------- AC_INT
   // AC_INT => GenericPayload
   template<int Twidth, bool Tsigned>
   inline void type_to_gp(const ac_int<Twidth,Tsigned> &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert(Twidth==bitlen);
     assert (gp->get_data_length()>0);
     unsigned char *ptr = gp->get_data_ptr();

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char s_mask = pow(2,(sbit))-1;   // sbit=2,  s_mask is 00000011
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < gp->get_data_length());

     ac_int<Twidth+8,Tsigned> tmp_in = in; // make it 1 byte longer than needed
     tmp_in = tmp_in<<sbit; // pre-shift to align start bit
     unsigned char cur_byte;
     for (unsigned int B=sbyte; B<= ebyte; B++) {
       if (sbit>0 && B==sbyte) {
         // must do read-modify-write
         cur_byte = ptr[B] & s_mask; // read
       } else {
         cur_byte = 0;
       }
       ac_int<8,false> tmp_byte = tmp_in.template slc<8>((B-sbyte)*8);
       cur_byte |= tmp_byte;
       if (ebit<7 && B==ebyte) cur_byte &= e_mask;
       ptr[B] = cur_byte; // write
     }
   }

   // GenericPayload => AC_INT
   template<int Twidth, bool Tsigned>
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, ac_int<Twidth,Tsigned> *result) {
     assert(bitlen==Twidth);
     *result = 0; // clear all bits
     unsigned char *    ptr = gp.get_data_ptr();
     unsigned int       len = gp.get_data_length(); // number of bytes

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < len);

     ac_int<Twidth+16,Tsigned> tmp = 0; // make an int large enough to hold two partial bytes
     for (unsigned int B=sbyte; B<=ebyte; B++) {
       ac_int<8,false> by = (unsigned)ptr[B];
       if (ebit<7 && B==ebyte) by = by & e_mask;
       tmp.set_slc((B-sbyte)*8,by);
     }
     tmp = tmp>>sbit; // post-shift to undo sbit
     *result = tmp; // upper bits are truncated
   }
#endif

#if defined(__AC_FIXED_H) && !defined(CS_GP_TYPECONV_H_AC_FIXED)
#define CCS_GP_TYPECONV_H_AC_FIXED

// #include <ac_sc.h>

   // ---------------------------------------------------------
   // --------------------------------- AC_FIXED
   // AC_FIXED => GenericPayload
   template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
   inline void type_to_gp(const ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<Twidth,Signed> tmp_in;
     tmp_in = in.template slc<Twidth>(0);
     type_to_gp(tmp_in,bitlen,BB,gp);
   }

   // GenericPayload => AC_FIXED
   template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> *result) {
     ac_int<Twidth,Signed> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     result->set_slc(0,tmp_result);
   }
#endif

#if defined(__AC_FLOAT_H) && !defined(CS_GP_TYPECONV_H_AC_FLOAT)
#define CCS_GP_TYPECONV_H_AC_FLOAT

   #include <ac_sc.h>

   // ---------------------------------------------------------
   // --------------------------------- AC_FLOAT
   // AC_FLOAT => GenericPayload
   template<int Twidth, int MTbits, int MIbits, int Ebits, ac_q_mode Qmode>
   inline void type_to_gp(const ac_float<MTbits,MIbits,Ebits,Qmode> &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<MTbits+Ebits,false> tmp_in;
     tmp_in.set_slc(0, in.mantissa().template slc<MTbits>(0));
     tmp_in.set_slc(MTbits, in.exp().template slc<Ebits>(0));
     tmp_in = in.template slc<Twidth>(0);
     type_to_gp(tmp_in,bitlen,BB,gp);
   }

   // GenericPayload => AC_FLOAT
   template<int Twidth, int MTbits, int MIbits, int Ebits, ac_q_mode Qmode>
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, ac_float<MTbits,MIbits,Ebits,Qmode> *result) {
     ac_int<MTbits+Ebits,false> tmp;
     ac_int<Ebits,false> tmp_exp;
     ac_fixed<MTbits,MIbits,false> tmp_mantissa;
     gp_to_type(gp, bitlen, BB, &tmp);
     tmp_exp.set_slc(0, tmp.template slc<Ebits>(MIbits));
     tmp_mantissa.set_slc(0, tmp.template slc<MIbits>(0));
     result->set_mantissa(tmp_mantissa);
     result->set_exp(tmp_exp);
   }
#endif

#if !defined(CCS_GP_TYPECONV_H)
#define CCS_GP_TYPECONV_H

   // ---------------------------------------------------------
   // utility function to allocate space in generic payload
   inline void ccs_allocate_gp(unsigned int bitlen, tlm_generic_payload *gp) {
     //std::cout << "ccs_allocate_gp(" << bitlen << ",gp) - CALLED" << std::endl;
     // Allocate data ptr
     unsigned int data_length = ceil(bitlen/8.0); // number of bytes in "bitlen"
     unsigned char *data = new unsigned char[data_length];
     gp->set_data_length(data_length);
     gp->set_data_ptr(data);
     //std::cout << "ccs_allocate_gp(" << bitlen << ",gp) - RETURNING gp data_length=" << data_length << std::endl;
   }

   // ---------------------------------------------------------
   // --------------------------------- SC_UINT (assumes systemc.h included)
   // SC_UINT => GenericPayload
   template<int Twidth>
   inline void type_to_gp(const sc_uint<Twidth> &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert(Twidth==bitlen);
     assert (gp->get_data_length()>0);
     unsigned char *ptr = gp->get_data_ptr();
     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char s_mask = pow(2,(sbit))-1;   // sbit=2,  s_mask is 00000011
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < gp->get_data_length());

     sc_uint<Twidth+8> tmp_in = in; // make it 1 byte longer than needed
     tmp_in = tmp_in<<sbit; // pre-shift to align start bit
     unsigned int B = sbyte;
     unsigned char cur_byte;
     unsigned int index = 0;
     while (B <= ebyte) {
       if (sbit>0 && B==sbyte) {
         // must do read-modify-write
         cur_byte = ptr[B] & s_mask; // read
       } else {
         cur_byte = 0;
       }
       cur_byte |= tmp_in.range( (B-sbyte+1)*8, (B-sbyte)*8 );
       if (ebit<7 && B==ebyte) cur_byte &= e_mask;
       ptr[B] = cur_byte; // write
       B++; index++;
     }
   }
   
   // GenericPayload => SC_UINT
   template<int Twidth>
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, sc_uint<Twidth> *result) {
     assert(bitlen==Twidth);
     *result = 0; // clear all bits
     unsigned char *    ptr = gp.get_data_ptr();
     unsigned int       len = gp.get_data_length();

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < len);

     sc_uint<Twidth+16> tmp = 0; // make an int large enough to hold two partial bytes
     for (unsigned int B=sbyte; B<=ebyte; B++) {
       sc_uint<8> by = (unsigned)ptr[B];
       if (ebit<7 && B==ebyte) by = by & e_mask;
       tmp.range( (B-sbyte+1)*8 , (B-sbyte)*8 ) = by;
     }
     tmp = tmp>>sbit; // post-shift to undo sbit
     *result = tmp; // upper bits are truncated
   }
   
   // ---------------------------------------------------------
   // --------------------------------- SC_INT (assumes systemc.h included)
   // SC_INT => GenericPayload
   template<int Twidth>
   inline void type_to_gp(const sc_int<Twidth> &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert(Twidth==bitlen);
     assert (gp->get_data_length()>0);
     unsigned char *ptr = gp->get_data_ptr();
     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char s_mask = pow(2,(sbit))-1;   // sbit=2,  s_mask is 00000011
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < gp->get_data_length());

     sc_int<Twidth+8> tmp_in = in; // make it 1 byte longer than needed
     tmp_in = tmp_in<<sbit; // pre-shift to align start bit
     unsigned int B = sbyte;
     unsigned char cur_byte;
     unsigned int index = 0;
     while (B <= ebyte) {
       if (sbit>0 && B==sbyte) {
         // must do read-modify-write
         cur_byte = ptr[B] & s_mask; // read
       } else {
         cur_byte = 0;
       }
       cur_byte |= tmp_in.range( (B-sbyte+1)*8, (B-sbyte)*8 );
       if (ebit<7 && B==ebyte) cur_byte &= e_mask;
       ptr[B] = cur_byte; // write
       B++; index++;
     }
   }
   
   // GenericPayload => SC_INT
   template<int Twidth>
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, sc_int<Twidth> *result) {
     assert(bitlen==Twidth);
     *result = 0; // clear all bits
     unsigned char *    ptr = gp.get_data_ptr();
     unsigned int       len = gp.get_data_length();

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned int ebyte = (BB+bitlen-1)/8;
     unsigned int ebit  = (BB+bitlen-1)%8;
     unsigned char e_mask = pow(2,(ebit+1))-1; // ebit=3,  e_mask is 00001111
     assert(ebyte < len);

     sc_int<Twidth+16> tmp = 0; // make an int large enough to hold two partial bytes
     for (unsigned int B=sbyte; B<=ebyte; B++) {
       sc_int<8> by = (unsigned)ptr[B];
       if (ebit<7 && B==ebyte) by = by & e_mask;
       tmp.range( (B-sbyte+1)*8 , (B-sbyte)*8 ) = by;
     }
     tmp = tmp>>sbit; // post-shift to undo sbit
     *result = tmp; // upper bits are truncated
   }


   // ---------------------------------------------------------
   // --------------------------------- UNSIGNED LONG LONG
   // UNSIGNED LONG LONG => GenericPayload
   inline void type_to_gp(const unsigned long long &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<64,false> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => UNSIGNED LONG LONG
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, unsigned long long *result) {
     assert(bitlen==64);
     *result = 0; // clear all bits
     ac_int<64,false> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- LONG LONG
   // LONG LONG => GenericPayload
   inline void type_to_gp(const long long &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<64,true> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => LONG LONG
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, long long *result) {
     assert(bitlen==64);
     *result = 0; // clear all bits
     ac_int<64,true> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- UNSIGNED LONG
   // UNSIGNED LONG => GenericPayload
   inline void type_to_gp(const unsigned long &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert(sizeof(long)==4); // assume 32bit compilation
     ac_int<32,false> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => UNSIGNED LONG
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, unsigned long *result) {
     assert(sizeof(long)==4); // assume 32bit compilation
     assert(bitlen==32);
     *result = 0; // clear all bits
     ac_int<32,false> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- LONG
   // LONG => GenericPayload
   inline void type_to_gp(const long &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert(sizeof(long)==4); // assume 32bit compilation
     ac_int<32,true> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => LONG
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, long *result) {
     assert(sizeof(long)==4); // assume 32bit compilation
     assert(bitlen==32);
     *result = 0; // clear all bits
     ac_int<32,true> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- UNSIGNED INT
   // UNSIGNED INT => GenericPayload
   inline void type_to_gp(const unsigned int &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<32,false> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => UNSIGNED INT
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, unsigned int *result) {
     assert(bitlen==32);
     *result = 0; // clear all bits
     ac_int<32,false> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- INT
   // INT => GenericPayload
   inline void type_to_gp(const int &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<32,true> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => INT
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, int *result) {
     assert(bitlen==32);
     *result = 0; // clear all bits
     ac_int<32,true> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- UNSIGNED SHORT
   // UNSIGNED SHORT => GenericPayload
   inline void type_to_gp(const unsigned short &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<16,false> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => UNSIGNED SHORT
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, unsigned short *result) {
     assert(bitlen==16);
     *result = 0; // clear all bits
     ac_int<16,false> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- SHORT
   // SHORT => GenericPayload
   inline void type_to_gp(const short &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<16,true> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => SHORT
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, short *result) {
     assert(bitlen==16);
     *result = 0; // clear all bits
     ac_int<16,true> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- UNSIGNED CHAR
   // UNSIGNED CHAR => GenericPayload
   inline void type_to_gp(const unsigned char &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<8,false> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => UNSIGNED CHAR
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, unsigned char *result) {
     assert(bitlen==8);
     *result = 0; // clear all bits
     ac_int<8,false> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- CHAR
   // CHAR => GenericPayload
   inline void type_to_gp(const char &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     ac_int<8,true> tmp_in = in;
     type_to_gp(tmp_in,bitlen,BB,gp);
   }
   
   // GenericPayload => CHAR
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, char *result) {
     assert(bitlen==8);
     *result = 0; // clear all bits
     ac_int<8,true> tmp_result;
     gp_to_type(gp,bitlen,BB,&tmp_result);
     *result = tmp_result;
   }

   // ---------------------------------------------------------
   // --------------------------------- BOOL
   // BOOL => GenericPayload
   inline void type_to_gp(const bool &in, unsigned int bitlen, unsigned int BB, tlm_generic_payload *gp) {
     assert (gp->get_data_length()>0);
     unsigned char *ptr = gp->get_data_ptr();

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     unsigned char s_mask = pow(2,(sbit))-1;   // sbit=2,  s_mask is 00000011
     assert(sbyte < gp->get_data_length());

     unsigned char tmp_byte = (in?1:0)<<sbit; 
     unsigned char cur_byte = ptr[sbyte]&s_mask;
     cur_byte |= tmp_byte;
     ptr[sbyte] = cur_byte; // write
   }
   
   // GenericPayload => BOOL
   inline void gp_to_type(const tlm_generic_payload &gp, unsigned int bitlen, unsigned int BB, bool *result) {
     assert(bitlen==1);
     unsigned char *    ptr = gp.get_data_ptr();
     unsigned int       len = gp.get_data_length();

     unsigned int sbyte = BB/8;
     unsigned int sbit  = BB%8;
     assert(sbyte < len);

     *result = (bool)ptr[sbyte]&(1<<sbit);
   }

#endif

