
// ----------------------------------------------------------------------------
// Type conversion functions between Algorithmic datatypes and Matlab/Simulink
// ----------------------------------------------------------------------------

#ifndef __INCLUDED_MATLAB_TYPECONV_H__
#define __INCLUDED_MATLAB_TYPECONV_H__

#include <ac_complex.h>

// ----------------------------------------------------------------------------
// Required header for Matlab Simulink data structures
#include "simstruc.h"
#include "fixedpoint.h"

// Type conversion functions (specialized for ac_int and ac_fixed)
#if defined(__AC_INT_H)
template<int Twidth, bool Tsigned>
void simtype_to_origtype(SimStruct *S, int loc, const void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, ac_int<Twidth,Tsigned> &target, bool is_fxp)
{
   switch(dt) {
      case SS_BOOLEAN: {
         if (Tsigned) ssSetErrorStatus(S,"Assignment of SS_BOOLEAN to signed AC datatype");
         const boolean_T *typed_p = static_cast<const boolean_T *>(untyped_vp);
         target = typed_p[loc];
         break;
      }
      default: {
         switch(sc) {
            case FXP_STORAGE_UINT8: {
               // AC datatype width must be atleast 8 bits and be unsigned
               const uint8_T *typed_p = static_cast<const uint8_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT8: {
               // AC datatype width must be atleast 8 bits and be unsigned
               const int8_T *typed_p = static_cast<const int8_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UINT16: {
               // AC datatype width must be atleast 16 bits and be unsigned
               const uint16_T *typed_p = static_cast<const uint16_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT16: {
               // AC datatype width must be atleast 16 bits and be unsigned
               const int16_T *typed_p = static_cast<const int16_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UINT32: {
               // AC datatype width must be atleast 32 bits and be unsigned
               const uint32_T *typed_p = static_cast<const uint32_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT32: {
               // AC datatype width must be atleast 32 bits and be unsigned
               const int32_T *typed_p = static_cast<const int32_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UNKNOWN: {
               ssSetErrorStatus(S,"FXP_STORAGE_UNKNOWN not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_DOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_DOUBLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_SINGLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SINGLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_CHUNKARRAY: {
               ssSetErrorStatus(S,"FXP_STORAGE_CHUNKARRAY not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_SCALEDDOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SCALEDDOUBLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_OTHER_SINGLE_WORD:
            case FXP_STORAGE_MULTIWORD: {
               uint32_T  nBits = (unsigned int)ssGetDataTypeFxpContainWordLen(S,dt);
               unsigned int nWords = (nBits+31U) / 32U;
               if (nWords > 4) {
                 ssSetErrorStatus(S,"FXP_STORAGE_MULTIWORD must be less than 128 bit limit in simtype_to_origtype");
                 break;
               }
               int totBits = 0;  // copied
               target=0;
               // Adjust vp here so that index stays in range...
               if (loc > 0) {
                 // Numerically the same as using index (nWords*loc)+i but the api treats it differently
                 int dtSz = ssGetDataTypeSize(S, dt );
                 untyped_vp = (void*) ((char*)untyped_vp + (loc*dtSz));
               }

               for (int i=0;i<nWords;i++) {
                 uint32_T rawData = ssFxpGetU32BitRegion(S, untyped_vp, dt, i);
                 int nextLoc = i*32;

                 if ((totBits + (Twidth%32)) == Twidth) {
                   // Last bits need to sign-extend properly
                   // Cant use set_slc because correct construction of 
                   // intLast is problematic when TWidth%32 is 0
                   ac_int<32, Tsigned> intLast(rawData);
                   for (int b=0;b<(Twidth%32);b++) {
                     target[nextLoc+b] = intLast[b];
                   }
                   break;
                 } else {
                   ac_int<32, false> int32(rawData);
                   target.set_slc(nextLoc, int32);
                 }
                 totBits += 32;
               }
               break;
            }
            default: {
               ssSetErrorStatus(S,"Unimplemented case in simtype_to_origtype");
               break;
            }
         }
         break;
      }
   }
}

template<int Twidth, bool Tsigned>
void origtype_to_simtype(SimStruct *S, int loc, void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, const ac_int<Twidth,Tsigned> &src, bool is_fxp)
{
   switch(dt) {
      case SS_BOOLEAN: {
         boolean_T *typed_p = static_cast<boolean_T*>(untyped_vp);
         typed_p[loc] = src.to_int();
         break;
      }
      default: {
         switch(sc) {
            case FXP_STORAGE_UINT8: {
               uint8_T *typed_p = static_cast<uint8_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_INT8: {
               int8_T *typed_p = static_cast<int8_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_UINT16: {
               uint16_T *typed_p = static_cast<uint16_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_INT16: {
               int16_T *typed_p = static_cast<int16_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_UINT32: {
               uint32_T *typed_p = static_cast<uint32_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_INT32: {
               int32_T *typed_p = static_cast<int32_T*>(untyped_vp);
               typed_p[loc] = src.to_int();
               break;
            }
            case FXP_STORAGE_UNKNOWN: {
               ssSetErrorStatus(S,"FXP_STORAGE_UNKNOWN not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_DOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_DOUBLE not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_SINGLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SINGLE not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_CHUNKARRAY: {
               ssSetErrorStatus(S,"FXP_STORAGE_CHUNKARRAY not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_SCALEDDOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SCALEDDOUBLE not supported in origtype_to_simtype");
               break;
            }
              //         case FXP_STORAGE_OTHER_SINGLE_WORD: {  // 64-bit long
              //               if (Tsigned) {
              //                 int64_T *typed_p = static_cast<int64_T*>(untyped_vp);
              //                 typed_p[loc] = src.to_int64();
              //               } else {
              //                 uint64_T *typed_p = static_cast<uint64_T*>(untyped_vp);
              //                 typed_p[loc] = src.to_uint64();
              //               }
              //               break;
              //            }
            case FXP_STORAGE_OTHER_SINGLE_WORD:
            case FXP_STORAGE_MULTIWORD: {
               uint32_T rawData;
               uint32_T  nBits = (unsigned int)ssGetDataTypeFxpContainWordLen(S,dt);
               unsigned int nWords = (nBits+31U) / 32U;
               if (nWords > 4) {
                 ssSetErrorStatus(S,"FXP_STORAGE_MULTIWORD must be less than 128 bit limit in origtype_to_simtype");
                 break;
               }
               if (loc > 0) {
                 // Numerically the same as using index (nWords*loc)+i but the api treats it differently
                 int dtSz = ssGetDataTypeSize(S, dt );
                 untyped_vp = (void*) ((char*)untyped_vp + (loc*dtSz));
               }

               int totBits = 0;  // copied
               for (int i=0;i<nWords;i++) {
                 int nextLoc = i*32;
                 uint32_T rawData = 0;

                 if ((totBits + (Twidth%32)) == Twidth) {
                   ac_int<32, Tsigned> intDone;
                   if (Tsigned && (src < 0)) {
                     intDone = 0xffffffff;
                   } else {
                     intDone = 0;
                   }
                   for (int b=0;b<(Twidth%32);b++) {
                     intDone[b] = src[nextLoc+b];
                   }
                   rawData = intDone.to_uint();
                  } else if (totBits > Twidth) {
                   rawData = 0;
                   if (Tsigned && (src < 0)) {
                     rawData = 0xffffffff;
                   }
                 } else {
                   //intDone = src.slc< Twidth%32 >(nextLoc);
                   ac_int<32, true> data32;
                   data32.set_slc(0, src.template slc<32 >(nextLoc));
                   rawData = data32.to_uint();
                 }
                 totBits+=32;
                 ssFxpSetU32BitRegion(S, untyped_vp, dt, rawData, i);
               }
               break;
            }
            default: {
               ssSetErrorStatus(S,"Unimplemented default case in origtype_to_simtype");
               break;
            }
         }
      }
   }
}
#endif

#if defined(__AC_FIXED_H)
template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
void simtype_to_origtype(SimStruct *S, int loc, const void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &target, bool is_fxp)
{
   if (ssGetDataTypeFxpWordLength(S,dt) != Twidth) {
      ssSetErrorStatus(S,"Assignment of fixed point value to ac_fixed of different word length");
      return;
   }
   if (ssGetDataTypeFractionLength(S,dt) != Twidth-Ibits) {
      ssSetErrorStatus(S,"Assignment of fixed point value to ac_fixed of different fraction length");
      return;
   }
   if (( (bool)ssGetDataTypeFxpIsSigned(S,dt) ) != Signed ) {
      ssSetErrorStatus(S,"Assignment of fixed point value to ac_fixed with different sign bits");
      return;
   }
   ac_int<Twidth,Signed> temp;
   simtype_to_origtype(S,loc,untyped_vp,dt,sc,temp,true);
   target.set_slc(0,temp);
}

template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
void origtype_to_simtype(SimStruct *S, int loc, void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, const ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &src, bool is_fxp)
{
   if (ssGetDataTypeFxpWordLength(S,dt) != Twidth) {
      ssSetErrorStatus(S,"Assignment of ac_fixed to a fixed point value of different word length");
      return;
   }
   if (ssGetDataTypeFractionLength(S,dt) != Twidth-Ibits) {
      ssSetErrorStatus(S,"Assignment of ac_fixed to a fixed point value of different fraction length");
      return;
   }
   if (( (bool)ssGetDataTypeFxpIsSigned(S,dt) ) != Signed ) {
      ssSetErrorStatus(S,"Assignment of ac_fixed to a fixed point value with different sign bits");
      return;
   }
   ac_int<Twidth,Signed> temp = src.template slc<Twidth>(0);
   origtype_to_simtype(S,loc,untyped_vp,dt,sc,temp,true);
}
#endif

// Catch-all conversion
template <class Tclass>
void simtype_to_origtype(SimStruct *S, int loc, const void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, Tclass &target, bool is_fxp)
{
   switch(dt) {
      case SS_BOOLEAN: {
         const boolean_T *typed_p = static_cast<const boolean_T *>(untyped_vp);
         target = typed_p[loc];
         break;
      }
      default: {
         switch(sc) {
            case FXP_STORAGE_UINT8: {
               // AC datatype width must be atleast 8 bits and be unsigned
               const uint8_T *typed_p = static_cast<const uint8_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT8: {
               // AC datatype width must be atleast 8 bits and be unsigned
               const int8_T *typed_p = static_cast<const int8_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UINT16: {
               // AC datatype width must be atleast 16 bits and be unsigned
               const uint16_T *typed_p = static_cast<const uint16_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT16: {
               // AC datatype width must be atleast 16 bits and be unsigned
               const int16_T *typed_p = static_cast<const int16_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UINT32: {
               // AC datatype width must be atleast 32 bits and be unsigned
               const uint32_T *typed_p = static_cast<const uint32_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_INT32: {
               // AC datatype width must be atleast 32 bits and be unsigned
               const int32_T *typed_p = static_cast<const int32_T*>(untyped_vp);
               target = typed_p[loc];
               break;
            }
            case FXP_STORAGE_UNKNOWN: {
               ssSetErrorStatus(S,"FXP_STORAGE_UNKNOWN not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_DOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_DOUBLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_SINGLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SINGLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_CHUNKARRAY: {
               ssSetErrorStatus(S,"FXP_STORAGE_CHUNKARRAY not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_SCALEDDOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SCALEDDOUBLE not supported in simtype_to_origtype");
               break;
            }
            case FXP_STORAGE_OTHER_SINGLE_WORD: {
               ssSetErrorStatus(S,"FXP_STORAGE_OTHER_SINGLE_WORD not supported in simtype_to_origtype(2)");
               break;
            }
            case FXP_STORAGE_MULTIWORD: {
              ssSetErrorStatus(S,"FXP_STORAGE_MULTIWORD not supported in simtype_to_origtype");
               break;
            }
            default: {
               ssSetErrorStatus(S,"Unimplemented case in simtype_to_origtype");
               break;
            }
         }
         break;
      }
   }

}

template <class Tclass>
void origtype_to_simtype(SimStruct *S, int loc, void *untyped_vp, const DTypeId dt, const fxpStorageContainerCategory sc, const Tclass &src, bool is_fxp)
{
   switch(dt) {
      case SS_BOOLEAN: {
         boolean_T *typed_p = static_cast<boolean_T*>(untyped_vp);
         typed_p[loc] = src;
         break;
      }
      default: {
         switch(sc) {
            case FXP_STORAGE_UINT8: {
               uint8_T *typed_p = static_cast<uint8_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_INT8: {
               int8_T *typed_p = static_cast<int8_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_UINT16: {
               uint16_T *typed_p = static_cast<uint16_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_INT16: {
               int16_T *typed_p = static_cast<int16_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_UINT32: {
               uint32_T *typed_p = static_cast<uint32_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_INT32: {
               int32_T *typed_p = static_cast<int32_T*>(untyped_vp);
               typed_p[loc] = src;
               break;
            }
            case FXP_STORAGE_UNKNOWN: {
               ssSetErrorStatus(S,"FXP_STORAGE_UNKNOWN not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_DOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_DOUBLE not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_SINGLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SINGLE not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_CHUNKARRAY: {
               ssSetErrorStatus(S,"FXP_STORAGE_CHUNKARRAY not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_SCALEDDOUBLE: {
               ssSetErrorStatus(S,"FXP_STORAGE_SCALEDDOUBLE not supported in origtype_to_simtype");
               break;
            }
            case FXP_STORAGE_OTHER_SINGLE_WORD: {
               ssSetErrorStatus(S,"FXP_STORAGE_OTHER_SINGLE_WORD not supported in origtype_to_simtype(3)");
               break;
            }
            case FXP_STORAGE_MULTIWORD: {
               ssSetErrorStatus(S,"FXP_STORAGE_MULTIWORD not supported in origtype_to_simtype");
               break;
            }
            default: {
               ssSetErrorStatus(S,"Unimplemented default case in origtype_to_simtype");
               break;
            }
         }
      }
   }
}

template<class Tclass>
void convert_input(
   SimStruct *S,
   int loc,
   int PortIdx,
   Tclass &target)
{
   // Create untyped void ptr to input data port
   const void *untyped_vp = ssGetInputPortSignal  (S, PortIdx);
   // get input data port type
   DTypeId input_dt = ssGetInputPortDataType  (S, PortIdx);
   // get input data storage category
   fxpStorageContainerCategory input_sc = ssGetDataTypeStorageContainCat (S, input_dt);
   // now copy data

   simtype_to_origtype(S,loc,untyped_vp, input_dt, input_sc, target, false);
}

template<class Tclass>
void convert_input(
   SimStruct *S,
   int portIdx,
   int fieldIdx,
   int loc,
   DTypeId busT,
   Tclass &target)
{
  // Create untyped void ptr to input data port
  const char *untyped_vp = (const char*)ssGetInputPortSignal  (S, portIdx);

  // Now offset into it for our struct field.  Careful with the ptr math
  int bOffset = ssGetBusElementOffset(S, busT, fieldIdx);
  const char *fieldP = untyped_vp+bOffset;

  // get input bus field data type
  DTypeId input_dt = (DTypeId) ssGetBusElementDataType(S, busT, fieldIdx);

  // get input data storage category
  fxpStorageContainerCategory input_sc = ssGetDataTypeStorageContainCat (S, input_dt);

  // now copy data
  simtype_to_origtype(S, loc, (void*)fieldP, input_dt, input_sc, target, false);
    
  return;
}

template<typename Ttype>
void convert_input(
   SimStruct *S,
   int loc,
   int PortIdx,
   ac_complex<Ttype> &target)
{
   // Verify that port is COMPLEX
   if (ssGetInputPortComplexSignal(S,PortIdx) != COMPLEX_YES) {
      ssPrintf("Input port %d was not marked COMPLEX_YES\n", PortIdx);
      return;
   }
   const void *untyped_vp = ssGetInputPortSignal  (S, PortIdx);
   // get input data port type
   DTypeId input_dt = ssGetInputPortDataType  (S, PortIdx);
   // get input data storage category
   fxpStorageContainerCategory input_sc = ssGetDataTypeStorageContainCat (S, input_dt);

   // now copy REAL data
   Ttype tmp_real;
   simtype_to_origtype(S,loc,untyped_vp, input_dt, input_sc, tmp_real, false);
   target.set_r(tmp_real);
   // now copy IMAG data
   Ttype tmp_imag;
   simtype_to_origtype(S,loc+1,untyped_vp, input_dt, input_sc, tmp_imag, false);
   target.set_i(tmp_imag);
}

template<typename Ttype>
void convert_input(
   SimStruct *S,
   int portIdx,
   int fieldIdx,
   int loc,
   DTypeId busT,
   ac_complex<Ttype> &target)
{
  // Verify that port is COMPLEX
  if (ssGetBusElementComplexSignal(S, busT, fieldIdx) != COMPLEX_YES) {
    ssPrintf("Input port %d and struct field %d was not marked COMPLEX_YES\n", portIdx, fieldIdx);
    return;
  }
  
  // Create untyped void ptr to input data port
  const char *untyped_vp = (const char*)ssGetInputPortSignal  (S, portIdx);

  // Now offset into it for our struct field.  Careful with the ptr math
  int bOffset = ssGetBusElementOffset(S, busT, fieldIdx);
  const char *fieldP = untyped_vp+bOffset;

  // get input bus field data type
  DTypeId input_dt = (DTypeId) ssGetBusElementDataType(S, busT, fieldIdx);

  // get input data storage category
  fxpStorageContainerCategory input_sc = ssGetDataTypeStorageContainCat (S, input_dt);

  Ttype tmp_real;
  Ttype tmp_imag;
  simtype_to_origtype(S, loc, (void*)fieldP, input_dt, input_sc, tmp_real, false);
  simtype_to_origtype(S, loc+1, (void*)fieldP, input_dt, input_sc, tmp_imag, false);
  target.set_r(tmp_real);
  target.set_i(tmp_imag);

  return;
}

template<class Tclass>
void convert_output(
   SimStruct *S,
   int loc,
   int PortIdx,
   Tclass &target)
{
   // Create untyped void ptr to output data port
   void *untyped_vp = ssGetOutputPortSignal  (S, PortIdx);
   // get output data port type
   DTypeId output_dt = ssGetOutputPortDataType  (S, PortIdx);
   // get output data storage category
   fxpStorageContainerCategory output_sc = ssGetDataTypeStorageContainCat (S, output_dt);
   // now copy data

   origtype_to_simtype(S,loc,untyped_vp, output_dt, output_sc, target, false);
}

template<class Tclass>
void convert_output(
   SimStruct *S,
   int portIdx,
   int fieldIdx,
   int loc,
   DTypeId busT,
   Tclass &target)
{
  // Create untyped void ptr to output data port
  const char *untyped_vp = (const char*)ssGetOutputPortSignal  (S, portIdx);
  
  // Now offset into it for our struct field
  int bOffset = ssGetBusElementOffset(S, busT, fieldIdx);
  const char *fieldP = untyped_vp+bOffset;
  
  // get output field data type
  DTypeId output_dt = (DTypeId) ssGetBusElementDataType(S, busT, fieldIdx);

  // get output data storage category
  fxpStorageContainerCategory output_sc = ssGetDataTypeStorageContainCat (S, output_dt);

  // now copy data
  origtype_to_simtype(S ,loc,(void*)fieldP, output_dt, output_sc, target, false);

  return;
}

template<typename Ttype>
void convert_output(
   SimStruct *S,
   int loc,
   int PortIdx,
   ac_complex<Ttype> &target)
{
   // Verify that port is COMPLEX
   if (ssGetOutputPortComplexSignal(S,PortIdx) != COMPLEX_YES) {
      ssPrintf("Output port %d was not marked COMPLEX_YES\n", PortIdx);
      return;
   }
   // Create untyped void ptr to output data port
   void *untyped_vp = ssGetOutputPortSignal  (S, PortIdx);
   // get output data port type
   DTypeId output_dt = ssGetOutputPortDataType  (S, PortIdx);
   // get output data storage category
   fxpStorageContainerCategory output_sc = ssGetDataTypeStorageContainCat (S, output_dt);

   // now copy REAL data
   Ttype tmp_real = target.r();
   origtype_to_simtype(S,loc,untyped_vp, output_dt, output_sc, tmp_real, false);
   // now copy IMAG data
   Ttype tmp_imag = target.i();
   origtype_to_simtype(S,loc+1,untyped_vp, output_dt, output_sc, tmp_imag, false);
}

template<typename Ttype>
void convert_output(
   SimStruct *S,
   int portIdx,
   int fieldIdx,
   int loc,
   DTypeId busT,
   ac_complex<Ttype> &target)
{
  // Verify that port is COMPLEX
  if (ssGetBusElementComplexSignal(S, busT, fieldIdx) != COMPLEX_YES) {
    ssPrintf("Output port %d struct field %d was not marked COMPLEX_YES\n", portIdx, fieldIdx);
    return;
  }

  // Create untyped void ptr to output data port
  const char *untyped_vp = (const char*)ssGetOutputPortSignal  (S, portIdx);
  
  // Now offset into it for our struct field
  int bOffset = ssGetBusElementOffset(S, busT, fieldIdx);
  const char *fieldP = untyped_vp+bOffset;
  
  // get output field data type
  DTypeId output_dt = (DTypeId) ssGetBusElementDataType(S, busT, fieldIdx);

  // get output data storage category
  fxpStorageContainerCategory output_sc = ssGetDataTypeStorageContainCat (S, output_dt);

  // now copy data
  Ttype tmp_real = target.r();
  Ttype tmp_imag = target.i();
  origtype_to_simtype(S,loc,(void*)fieldP, output_dt, output_sc, tmp_real, false);
  origtype_to_simtype(S,loc+1,(void*)fieldP, output_dt, output_sc, tmp_imag, false);
  return;
}


template<typename Ttype>
void  matlab_to_catapult(void      *uTp, 
                         mxClassID vType, 
                         int       idx, 
                         Ttype     &target)
{
  switch (vType)  {
  case mxLOGICAL_CLASS : {
    // typedef bool mxLogical;
    mxLogical *p = (mxLogical *) uTp;
    target = p[idx];
  } break;
  case mxCHAR_CLASS : {
    // typedef CHAR16_T mxChar;
    mxChar *p = (mxChar *) uTp;
    target = p[idx];
  } break;
  case mxINT8_CLASS:  {
    mxInt8 *p = (mxInt8 *) uTp;
    target = p[idx];
  } break; 
  case mxUINT8_CLASS:  {
    mxUint8 *p = (mxUint8 *) uTp;
    target = p[idx];
  } break; 
  case mxINT16_CLASS:  {
    mxInt16 *p = (mxInt16 *) uTp;
    target = p[idx];
  } break; 
  case mxUINT16_CLASS: {
    mxUint16 *p = (mxUint16 *) uTp;
    target = p[idx];
  } break; 
  case mxINT32_CLASS:  {
    mxInt32 *p = (mxInt32 *) uTp;
    target = p[idx];
  } break; 
  case mxUINT32_CLASS: {
    mxUint32 *p = (mxUint32 *) uTp;
    target = p[idx];
  } break; 
  case mxINT64_CLASS:  {
    mxInt64 *p = (mxInt64 *) uTp;
    target = p[idx];
  } break; 
  case mxUINT64_CLASS: {
    mxUint64 *p = (mxUint64 *) uTp;
    target = p[idx];
  } break; 
  case mxSINGLE_CLASS: {
    mxSingle *p = (mxSingle *) uTp;
    target = p[idx];
  } break; 
  case mxDOUBLE_CLASS: {
    mxDouble *p = (mxDouble *) uTp;
    target = p[idx];
  } break; 
  default:
    mexErrMsgTxt("Error: Unsupported input data type.  Must be basic C data types.");
    target = 0;
    break;
  }

  return;
}

// ac_int 
template<int Twidth, bool Tsigned>
void catapult_to_matlab(void *uTp, 
                        mxClassID vType, 
                        int idx, 
                        const ac_int<Twidth,Tsigned> &src)
{
  // mexPrintf("::catapult_to_matlab 1\n");
  switch (vType)  {
  case mxLOGICAL_CLASS : {
    // mexPrintf("::catapult_to_matlab mxLOGICAL\n");
    mxLogical *p = (mxLogical *) uTp;
    p[idx] = (mxLogical) src.to_uint();
  } break;
  case mxCHAR_CLASS : {
    // mexPrintf("::catapult_to_matlab mxCHAR\n");
    mxChar *p = (mxChar *) uTp;
    p[idx] = (mxChar) src.to_uint();
  } break;
  case mxINT8_CLASS:  {
    // mexPrintf("::catapult_to_matlab mxINT8\n");
    mxInt8 *p = (mxInt8 *) uTp;
    p[idx] = (mxInt8) src.to_int();
  } break; 
  case mxUINT8_CLASS:  {
    // mexPrintf("::catapult_to_matlab mxUINT \n");
    mxUint8 *p = (mxUint8 *) uTp;
    p[idx] = ( mxUint8) src.to_uint();
  } break; 
  case mxINT16_CLASS:  {
    // mexPrintf("::catapult_to_matlab mxINT16\n");
    mxInt16 *p = (mxInt16 *) uTp;
    p[idx] = (mxInt16) src.to_int();
  } break; 
  case mxUINT16_CLASS: {
    // mexPrintf("::catapult_to_matlab mxUINT16 \n");
    mxUint16 *p = (mxUint16 *) uTp;
    p[idx] = (mxUint16) src.to_uint();
  } break; 
  case mxINT32_CLASS:  {
    // mexPrintf("::catapult_to_matlab mxINT32\n");
    mxInt32 *p = (mxInt32 *) uTp;
    p[idx] = (mxInt32) src.to_int();
  } break; 
  case mxUINT32_CLASS: {
    // mexPrintf("::catapult_to_matlab mxUINT32 \n");
    mxUint32 *p = (mxUint32 *) uTp;
    p[idx] = (mxUint32) src.to_uint();
  } break; 
  case mxINT64_CLASS:  {
    // mexPrintf("::catapult_to_matlab mxINT64\n");
    mxInt64 *p = (mxInt64 *) uTp;
    p[idx] = (mxInt64) src.to_int64();
  } break; 
  case mxUINT64_CLASS: {
    // mexPrintf("::catapult_to_matlab mxUINT64\n");
    mxUint64 *p = (mxUint64 *) uTp;
    p[idx] = (mxUint64) src.to_uint64();
  } break; 
  case mxSINGLE_CLASS: {
    // mexPrintf("::catapult_to_matlab mxSINGLE\n");
    mxSingle *p = (mxSingle *) uTp;
    p[idx] = (mxSingle) src.to_double();
  } break; 
  case mxDOUBLE_CLASS: {
    // mexPrintf("::catapult_to_matlab mxDOUBLE\n");
    mxDouble *p = (mxDouble *) uTp;
    p[idx] = (mxDouble) src.to_double();
  } break; 
  default: {
    // mexPrintf("::catapult_to_matlab default\n");
    mexErrMsgTxt("Error: Unsupported output data type.  Must be basic C data types.");
  } break;
  }
  return;
}

// ac_fixed - hand off to ac_int
template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
void catapult_to_matlab(void *uTp, 
                        mxClassID vType, 
                        int idx, 
                        const ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &src)
{
  // mexPrintf("::catapult_to_matlab 2\n");
  switch(vType) {
  case mxSINGLE_CLASS: {
    // mexPrintf("::catapult_to_matlab mxSINGLE\n");
    mxSingle *p = (mxSingle *) uTp;
    p[idx] = (mxSingle) src.to_double();
    return;
  } break; 
  case mxDOUBLE_CLASS: {
    // mexPrintf("::catapult_to_matlab mxDOUBLE\n");
    mxDouble *p = (mxDouble *) uTp;
    p[idx] = (mxDouble) src.to_double();
    return;
  } break; 
  default:
    break;
  }

  ac_int<Twidth,Signed> temp = src.template slc<Twidth>(0);
  catapult_to_matlab(uTp, vType, idx, temp);
  return;
}

// non-ac_class types
template <typename Ttype>
void catapult_to_matlab(void *uTp, 
                        mxClassID vType, 
                        int idx, 
                        const Ttype &src)
{
  // mexPrintf("::catapult_to_matlab 3\n");

  switch (vType)  {
  case mxLOGICAL_CLASS : {
    mxLogical *p = (mxLogical *) uTp;
    p[idx] = (mxLogical) src;
  } break;
  case mxCHAR_CLASS : {
    mxChar *p = (mxChar *) uTp;
    p[idx] = (mxChar) src;
  } break;
  case mxINT8_CLASS:  {
    mxInt8 *p = (mxInt8 *) uTp;
    p[idx] = (mxInt8)src;
  } break; 
  case mxUINT8_CLASS:  {
    mxUint8 *p = (mxUint8 *) uTp;
    p[idx] = ( mxUint8)src;
  } break; 
  case mxINT16_CLASS:  {
    mxInt16 *p = (mxInt16 *) uTp;
    p[idx] = (mxInt16)src;
  } break; 
  case mxUINT16_CLASS: {
    mxUint16 *p = (mxUint16 *) uTp;
    p[idx] = (mxUint16)src;
  } break; 
  case mxINT32_CLASS:  {
    mxInt32 *p = (mxInt32 *) uTp;
    p[idx] = (mxInt32) src;
  } break; 
  case mxUINT32_CLASS: {
    mxUint32 *p = (mxUint32 *) uTp;
    p[idx] = ( mxUint32) src;
  } break; 
  case mxINT64_CLASS:  {
    mxInt64 *p = (mxInt64 *) uTp;
    p[idx] = (mxInt64) src;
  } break; 
  case mxUINT64_CLASS: {
    mxUint64 *p = (mxUint64 *) uTp;
    p[idx] = (mxUint64) src;
  } break; 
  case mxSINGLE_CLASS: {
    mxSingle *p = (mxSingle *) uTp;
    p[idx] = (mxSingle) src;
  } break; 
  case mxDOUBLE_CLASS: {
    mxDouble *p = (mxDouble *) uTp;
    p[idx] = (mxDouble)  src;
  } break; 
  default: {
    mexErrMsgTxt("Error: Unsupported output data type.  Must be basic C data types.");
  } break;
  }
  
  return;
}


template<class Tclass>
void get_matlab_input(const mxArray *mxArray,
                 mxClassID     vType,
                 int           idx,
                 Tclass        &target)
{
  void *utP;

  utP = mxGetData(mxArray);
  matlab_to_catapult(utP, vType, idx, target);
  return;
}


template<typename Ttype>
void get_matlab_input(const mxArray     *mxArray,
                 mxClassID         vType,
                 int               idx,
                 ac_complex<Ttype> &target)
{
  void *uTp;
  Ttype rTmp(0);
  Ttype iTmp(0);

  if (!mxIsComplex(mxArray)) {
    target.set_i(iTmp);
    target.set_r(rTmp);
    mexErrMsgTxt("Error: Cannot assign non-complex input to ac_complex type.");
    return;
  }

  uTp = mxGetData(mxArray);
  matlab_to_catapult(uTp, vType, idx, rTmp);
  
  uTp = mxGetImagData(mxArray);
  matlab_to_catapult(uTp, vType, idx, iTmp);

  target.set_r(rTmp);
  target.set_i(iTmp);
  return;
}

//Output_c_p[i] = c;
template<class Tclass>
void set_matlab_output(const mxArray     *mxArray,
                       mxClassID         vType,
                       int               idx,
                       Tclass            &target)
{
  void *uTp;
  // mexPrintf("::set_matlab_output 1\n");
  uTp = mxGetData(mxArray);
  catapult_to_matlab(uTp, vType, idx, target);
  return;
}

template<typename Ttype>
void set_matlab_output(const mxArray     *mxArray,
                       mxClassID         vType,
                       int               idx,
                       ac_complex<Ttype> &target)
{
  void *uTp;
  Ttype rTmp = target.real();
  Ttype iTmp = target.imag();
  // mexPrintf("::set_matlab_output 2\n");

  if (!mxIsComplex(mxArray)) {
    mexErrMsgTxt("Error: Cannot assign ac_complex type to non-complex matlab output.");
    return;
  }

  uTp = mxGetData(mxArray);
  catapult_to_matlab(uTp, vType, idx, rTmp);
  
  uTp = mxGetImagData(mxArray);
  catapult_to_matlab(uTp, vType, idx, iTmp);
  return;
}

#endif

