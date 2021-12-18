/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using mt19937_init_genrand(seed)  
   or mt19937_init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

//==================================================================
// MGC Modifications (12/11/07):
//    Prefixed all identifiers with 'mt19937_'.
//    Disabled portions of code not needed.
// Functions used:
//     void mt19937_init_genrand(unsigned long s);  // set seed
//     double mt19937_genrand_real1(void);          // get rand num
//
//==================================================================

#include <stdio.h>

/* Period parameters */  
#define mt19937_N 624
#define mt19937_M 397
#define mt19937_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define mt19937_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define mt19937_LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt19937_mt[mt19937_N]; /* the array for the state vector  */
static int mt19937_mti=mt19937_N+1; /* mt19937_mti==mt19937_N+1 means mt19937_mt[mt19937_N] is not initialized */

/* initializes mt19937_mt[mt19937_N] with a seed */
void mt19937_init_genrand(unsigned long s)
{
    mt19937_mt[0]= s & 0xffffffffUL;
    for (mt19937_mti=1; mt19937_mti<mt19937_N; mt19937_mti++) {
        mt19937_mt[mt19937_mti] = 
	    (1812433253UL * (mt19937_mt[mt19937_mti-1] ^ (mt19937_mt[mt19937_mti-1] >> 30)) + mt19937_mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt19937_mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt19937_mt[mt19937_mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

#ifdef ENABLE_ALL_MT19937_CODE
/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void mt19937_init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    mt19937_init_genrand(19650218UL);
    i=1; j=0;
    k = (mt19937_N>key_length ? mt19937_N : key_length);
    for (; k; k--) {
        mt19937_mt[i] = (mt19937_mt[i] ^ ((mt19937_mt[i-1] ^ (mt19937_mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt19937_mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=mt19937_N) { mt19937_mt[0] = mt19937_mt[mt19937_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=mt19937_N-1; k; k--) {
        mt19937_mt[i] = (mt19937_mt[i] ^ ((mt19937_mt[i-1] ^ (mt19937_mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt19937_mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=mt19937_N) { mt19937_mt[0] = mt19937_mt[mt19937_N-1]; i=1; }
    }

    mt19937_mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}
#endif

/* generates a random number on [0,0xffffffff]-interval */
unsigned long mt19937_genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, mt19937_MATRIX_A};
    /* mag01[x] = x * mt19937_MATRIX_A  for x=0,1 */

    if (mt19937_mti >= mt19937_N) { /* generate mt19937_N words at one time */
        int kk;

        if (mt19937_mti == mt19937_N+1)   /* if mt19937_init_genrand() has not been called, */
            mt19937_init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<mt19937_N-mt19937_M;kk++) {
            y = (mt19937_mt[kk]&mt19937_UPPER_MASK)|(mt19937_mt[kk+1]&mt19937_LOWER_MASK);
            mt19937_mt[kk] = mt19937_mt[kk+mt19937_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<mt19937_N-1;kk++) {
            y = (mt19937_mt[kk]&mt19937_UPPER_MASK)|(mt19937_mt[kk+1]&mt19937_LOWER_MASK);
            mt19937_mt[kk] = mt19937_mt[kk+(mt19937_M-mt19937_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt19937_mt[mt19937_N-1]&mt19937_UPPER_MASK)|(mt19937_mt[0]&mt19937_LOWER_MASK);
        mt19937_mt[mt19937_N-1] = mt19937_mt[mt19937_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mt19937_mti = 0;
    }
  
    y = mt19937_mt[mt19937_mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

#ifdef ENABLE_ALL_MT19937_CODE
/* generates a random number on [0,0x7fffffff]-interval */
long mt19937_genrand_int31(void)
{
    return (long)(mt19937_genrand_int32()>>1);
}
#endif

/* generates a random number on [0,1]-real-interval */
double mt19937_genrand_real1(void)
{
    return mt19937_genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

#ifdef ENABLE_ALL_MT19937_CODE
/* generates a random number on [0,1)-real-interval */
double mt19937_genrand_real2(void)
{
    return mt19937_genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double mt19937_genrand_real3(void)
{
    return (((double)mt19937_genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double mt19937_genrand_res53(void) 
{ 
    unsigned long a=mt19937_genrand_int32()>>5, b=mt19937_genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

int main(void)
{
    int i;
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    mt19937_init_by_array(init, length);
    printf("1000 outputs of mt19937_genrand_int32()\n");
    for (i=0; i<1000; i++) {
      printf("%10lu ", mt19937_genrand_int32());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of mt19937_genrand_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", mt19937_genrand_real2());
      if (i%5==4) printf("\n");
    }
    return 0;
}
#endif

