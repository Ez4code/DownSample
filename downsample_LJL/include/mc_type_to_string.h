//-------------------------------------------------
// Conversion functions from various types to sc_lv
//-------------------------------------------------

#include <string>
#include <sstream>
#include <iostream>
using std::string;
using std::stringstream;
using std::istringstream;


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

#if !defined(MC_TYPE_TO_STRING_H)
#define MC_TYPE_TO_STRING_H
// built-in types and sc_types:


// GENERIC => STRiNG for trace file
template <class T>
bool type_to_string(const T &in, string& s) {
	long long tmp;
	tmp = (long long)in;
	stringstream out;
	if((out << tmp).fail()){
		return false;
	}
    s.assign(out.str());
	return true;
}

// this one for messaging
template<class T>
struct tr_data_printer {
	inline static bool type_to_print(T &in, string& str) {
		stringstream out;
		if((out << in).fail()){
			return false;
		}
		str.assign(out.str());
		return true;
	}
	
	static bool print_number(int in, string& str){
		stringstream out;
		out << in;
		str.assign(out.str());
		return true;
	}
};

// avoid outputing value as an ascii char
template<> inline bool tr_data_printer<char>::type_to_print(char &in, string& str) { return print_number((int)in,str); }
template<> inline bool tr_data_printer<unsigned char>::type_to_print(unsigned char &in, string& str) { return print_number((int)in,str); }
template<> inline bool tr_data_printer<signed char>::type_to_print(signed char &in, string& str) { return print_number((int)in,str); }

// SC_INT => STRING
  // (uses GENERIC type_to_string)

// STRING => SC_INT
// template<int Twidth>
// bool string_to_type(const string &in, bool issigned, sc_int<Twidth> *result) {
  // *result = in;
// }

// SC_UINT => STRING
  // (uses GENERIC type_to_string)

// STRING => SC_UINT
// template<int Twidth>
// bool string_to_type(const string &in, bool issigned, sc_uint<Twidth> *result) {
  // *result = in;
// }

// SC_BIGINT => STRING
  // (uses GENERIC type_to_string)

// STRING => SC_BIGINT
// template<int Twidth>
// bool string_to_type(const string &in, bool issigned, sc_bigint<Twidth> *result) {
  // *result = in;
// }

// SC_BIGUINT => STRING
  // (uses GENERIC type_to_string)


// STRING => SC_BIGUINT
// template<int Twidth>
// bool string_to_type(const string &in, bool issigned, sc_biguint<Twidth> *result) {
  // *result = in;
// }

// BOOL => STRING 
  // (uses GENERIC type_to_string)

// STRING => BOOL
template<class T>
bool string_to_type(const string &in, bool &result) {
  result = in.compare("0") ? true : false;
  return true;
}


template<class T>
bool string_to_type_builtin(const string &in, T &result) {
	std::istringstream sstr(in);
	return !(sstr >> std::dec >> result).fail();
}

// template<int Twidth, class T>
// bool string_to_type_builtin_64(const string &in, bool issigned, T *result) {
 // sc_assert(sizeof(T) * CHAR_BIT <= 64);
  // if (issigned) {
    // static sc_int<Twidth < 64 ? Twidth : 64> i;
    // i = in;
    // *result = i.to_int64();
  // } else {
    // static sc_uint<Twidth < 64 ? Twidth : 64> u;
    // u = in;
    // *result = u.to_uint64();
  // }
// }


// CHAR => STRING
  // (uses GENERIC type_to_string)

// STRING => CHAR
template<int Twidth>
bool string_to_type(const string &in, char &result) {
  return string_to_type_builtin(in, result);
}

// UNSIGNED CHAR => STRING
  // (uses GENERIC type_to_string)

// STRING => UNSIGNED CHAR
template<int Twidth>
bool string_to_type(const string &in, unsigned char &result) {
  return string_to_type_builtin(in, result);
}

// STRING => SIGNED CHAR
template<int Twidth>
bool string_to_type(const string &in, signed char &result) {
  return string_to_type_builtin(in, result);
}

// SHORT => STRING
  // (uses GENERIC type_to_string)

// STRING => SHORT
template<int Twidth>
bool string_to_type(const string &in, short &result) {
  return string_to_type_builtin(in, result);
}

// UNSIGNED SHORT => STRING
  // (uses GENERIC type_to_string)

// STRING => UNSIGNED SHORT
template<int Twidth>
bool string_to_type(const string &in, unsigned short &result) {
  return string_to_type_builtin(in, result);
}

// INT => STRING
  // (uses GENERIC type_to_string)

// STRING => INT
template<int Twidth>
bool string_to_type(const string &in, int &result) {
  return string_to_type_builtin(in, result);
}

// UNSIGNED INT => STRING
  // (uses GENERIC type_to_string)

// STRING => UNSIGNED INT
template<int Twidth>
bool string_to_type(const string &in, unsigned int &result) {
  return string_to_type_builtin(in, result);
}

// LONG => STRING
  // (uses GENERIC type_to_string)

// STRING => LONG
template<int Twidth>
bool string_to_type(const string &in, long &result) {
  return string_to_type_builtin(in, result);
}

// UNSIGNED LONG => STRING
  // (uses GENERIC type_to_string)

// STRING => UNSIGNED LONG
template<int Twidth>
bool string_to_type(const string &in, unsigned long &result) {
  return string_to_type_builtin(in, result);
}

// LONG LONG => STRING
  // (uses GENERIC type_to_string)

// STRING => LONG LONG
// not tested!!!
template<int Twidth>
bool string_to_type(const string &in, long long &result) {
  return string_to_type_builtin(in, result);
}

// UNSIGNED LONG LONG => STRING
  // (uses GENERIC type_to_string)

// STRING => UNSIGNED LONG LONG
template<int Twidth>
bool string_to_type(const string &in, unsigned long long &result) {
  return string_to_type_builtin(in, result);
}

// CONTAINER mgc_sysc_ver_array1D => STRING
// template <class Tclass, int V, int Twidth>
// bool type_to_string(const mgc_sysc_ver_array1D<Tclass,V> &in, int length, string& rvec)
// {
 // sc_assert(Twidth == length);
//  sc_assert(Twidth % V == 0);
  // const int element_length = Twidth / V;
  // sc_lv<element_length> el_vec;
  // for (int i = 0; i < V; ++i) {
    // type_to_string(in[i], element_length, el_vec);
    // rvec.range((i + 1) * element_length - 1, i * element_length) = el_vec;
  // }
// }

// STRING => CONTAINER mgc_sysc_ver_array1D
// template <int Twidth, class Tclass, int TclassW>
// bool string_to_type(const string &in, bool issigned, mgc_sysc_ver_array1D<Tclass,TclassW> *result) 
// {
 // sc_assert(Twidth > 0 && Twidth % TclassW == 0);
  // enum { ew = Twidth/TclassW }; 
  // for (int i = 0; i < TclassW; ++i) {
    // sc_lv<ew> tmp = in.range((i + 1) * ew - 1, i * ew);
    // string_to_type(tmp, issigned, &result->operator[](i));
  // } 
// }

#endif

#if defined(SC_INCLUDE_FX) && !defined(MC_TYPE_TO_STRING_H_SC_FIXED)
#define MC_TYPE_TO_STRING_H_SC_FIXED
// SC_FIXED => STRING
// template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
// bool type_to_string(const sc_fixed<Twidth,Ibits,Qmode,Omode,Nbits> &in, int length, string& rvec) {
 // sc_assert(length == Twidth);
 // rvec = in.range(Twidth-1, 0); // via assignment from sc_bv_base not provided 
  // static sc_bigint<Twidth> tmp;
  // tmp = in.range(Twidth-1, 0);
  // type_to_string(tmp, length, rvec);
// }

// STRING => SC_FIXED
// template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
// bool string_to_type(const string &in, bool issigned, sc_fixed<Twidth,Ibits,Qmode,Omode,Nbits> *result) {
  // result->range(Twidth-1, 0) = in; // via sc_bv_base
// }

// SC_UFIXED => STRING
// template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
// bool type_to_string(const sc_ufixed<Twidth,Ibits,Qmode,Omode,Nbits> &in, int length, string& rvec) {
 // sc_assert(length == Twidth);
//  rvec = in.range(Twidth-1, 0); // via assignment from sc_bv_base not provided 
  // static sc_bigint<Twidth> tmp;
  // tmp = in.range(Twidth-1, 0);
  // type_to_string(tmp, length, rvec);
// }

// STRING => SC_UFIXED
// template<int Twidth, int Ibits, sc_q_mode Qmode, sc_o_mode Omode, int Nbits>
// bool string_to_type(const string &in, bool issigned, sc_ufixed<Twidth,Ibits,Qmode,Omode,Nbits> *result) {
  // result->range(Twidth-1, 0) = in; // via sc_bv_base
// }
#endif

#if defined(__AC_INT_H) && !defined(MC_TYPE_TO_STRING_H_AC_INT)
#define MC_TYPE_TO_STRING_H_AC_INT

//#include <ac_sc.h>

// AC_INT => STRING
template<int Twidth>
bool string_to_type(const string &in, ac_int<Twidth,true> &result) {
	// !! limit to 64 bits
  long long tmp;
  if (string_to_type_builtin(in, tmp)){
	result = tmp;
	return true;
  }
  return false;
}

template<int Twidth>
bool string_to_type(const string &in, ac_int<Twidth,false> &result) {
  long long tmp;
  if (string_to_type_builtin(in, tmp)){
	  result = tmp;
	  return true;
  }
  return false;
}

// STRING => AC_INT
 template<int Twidth>
bool type_to_string(const ac_int<Twidth,true> &in,  string& str) {
  long long int tmp;
  tmp = (long long)in;
  return type_to_string(tmp, str); 
}

template<int Twidth>
bool  type_to_string(const ac_int<Twidth,false> &in, string& str) {
  long long tmp;
  tmp = (long long)in;
  return type_to_string(tmp, str); 
}

#endif

#if defined(__AC_FIXED_H) && !defined(MC_TYPE_TO_STRING_H_AC_FIXED)
#define MC_TYPE_TO_STRING_H_AC_FIXED

//#include "ac_sc.h"

//AC_FIXED => STRING

//use this one for trace 
template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
bool type_to_string(const ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &in, string& str) {
  static ac_int<Twidth,Signed> tmp;
  tmp = in.template slc<Twidth>(0);
  return type_to_string(tmp, str);
}


// STRING => AC_FIXED
template<int Twidth, int Ibits, bool Signed, ac_q_mode Qmode, ac_o_mode Omode>
bool string_to_type(const string &in, ac_fixed<Twidth,Ibits,Signed,Qmode,Omode> &result) {
  static ac_int<Twidth,Signed> tmp;
  if (string_to_type(in, tmp)){
	result.set_slc(0, tmp);
	return true;
  }
  return false;
}

#endif


