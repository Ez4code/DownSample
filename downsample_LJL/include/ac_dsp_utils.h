////////////////////////////////////////////////////////////////////////////////
// Catapult Synthesis
// 
// Copyright (c) 2003-2015 Mentor Graphics Corp.
//       All Rights Reserved
// 
// This document contains information that is proprietary to Mentor Graphics
// Corp. The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.
////////////////////////////////////////////////////////////////////////////////
// NO WARRANTY. MENTOR GRAPHICS EXPRESSLY DISCLAIMS ALL WARRANTY 
// FOR THE SOFTWARE. TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE 
// LAW, THE SOFTWARE AND ANY RELATED DOCUMENTATION IS PROVIDED "AS IS"
// AND WITH ALL FAULTS AND WITHOUT WARRANTIES OR CONDITIONS OF ANY 
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE, OR NONINFRINGEMENT. THE ENTIRE RISK ARISING OUT OF USE OR 
// DISTRIBUTION OF THE SOFTWARE REMAINS WITH YOU.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  Source:         ac_dsp_utils.h
//  Description:    Various C++ classes used in testbenches for DSP designs
////////////////////////////////////////////////////////////////////////////////

#include <float.h>
#include <vector>
#include <ac_channel.h>

//====================================================================
// Function: gen_random
// Description: Calculate a random number
// Return value: A random number as a double between 0 and 1
//====================================================================
inline double gen_random()
{
   return( (double)(rand() & RAND_MAX) / RAND_MAX - 0.5 );
}

//====================================================================
// Function: chirp
// Description: Generate monuaral samples of length 'length' of a 
// sinusoidal signal frequency sweep
// Return value: The number of samples created
//====================================================================
inline int chirp(int length, ac_channel<double> &samples)
{
   double interval = 2.0 * atan(1.0)/(double)length;
   for (int i=0; i<length; i++) {
      double sample = (double) sin( (double)(i * i) * interval);
      samples.write(sample);
   }
   return(length);
}

//====================================================================
// Function: gen_wave
// Description: Generates monaural sinusoidal waveform of length 
// 'samples' at frequency 'freq' (in Hz), sample frequency 'fs'
// and with amplitude 'amp'.
// Return value: The number of samples created
//====================================================================
template <class T>
inline int gen_wave(int samples, int freq, int fs, int amp, ac_channel<T> &waveout)
{
  const double twoPI = 2.0*3.14159;
  int i=0;
  for (i=0; i < samples; i++) {
    double val = ((double)amp)*sin(twoPI * freq * ((double)i/(double)fs));
    T val2 = val;
    waveout.write(val);
  }
  return i;
}

//====================================================================
// Function: gen_modulated_wave
// Description: Generates modulated monaural sinusoidal waveform of 
// length 'samples' at frequencies 'freq1' and 'freq2' (in Hz), sample
// frequency 'fs' and with amplitude 'amp'.
// Return value: The number of samples created
//====================================================================
template <class T>
inline int gen_modulated_wave(int samples, int freq1, int freq2, int fs, int amp, ac_channel<T> &waveout)
{
  const double twoPI = 2.0*3.14159;
  int i=0;
  for (i=0; i < samples; i++) {
    double val = ((double)amp)*sin(twoPI * freq1 * ((double)i/(double)fs)) + 
                 ((double)amp)*sin(twoPI * freq2 * ((double)i/(double)fs));
    T val2 = val/2.0;
    waveout.write(val);
  }
  return i;
}

//====================================================================
// Function: uniform_noise
// Description: Generate monaural samples of length 'length' of 
// uniformly distributed random numbers between 0 and 1 (uniform noise).
// Return value: The number of samples created
//====================================================================
inline int uniform_noise(int length, ac_channel<double> &samples)
{
   for (int i=0; i<length; i++) {
      double sample = gen_random();
      samples.write(sample);
   }
   return(length);
}

//====================================================================
// Function: gaussian_noise
// Description: Generate monaural samples of length 'length' of 
// Gaussian-distributed random numbers of the specified maximum
// amplitude.
// Return value: The number of samples created
//====================================================================
inline int gaussian_noise(int length, double amplitude, ac_channel<double> &samples)
{
   double v1, v2, r; 
   for (int i=0; i<length/2; i++) {
      do {
         v1 = 2.0 * gen_random();
         v2 = 2.0 * gen_random();
         r = v1 * v1 + v2 * v2;
      } while (r > 1.0);
      double fac = sqrt( 2.0 * -log(r)/r);
      samples.write(v1 * fac * amplitude);
      samples.write(v2 * fac * amplitude);
   }
   return(length);
}

//====================================================================
// Function: get_stdev
// Description: Calculates the standard deviation of the list of
// samples.
// Return value: The standard deviation (same data type as samples).
//====================================================================
template <class T>
T get_std_dev(ac_channel<T> &samples)
{
   T sum = (T)0;
   T sum2 = (T)0;
   
   for (unsigned int i=0; i<samples.debug_size(); i++) {
      T sample = samples[i];
      sum += sample;
      sum2 += sample * sample;
   }
   T avg = sum / (T)samples.debug_size();
   T var = (sum2 - sum * avg) / ( samples.debug_size() - 1);
   return(sqrt(var));   
}

//====================================================================
// Function: get_stats
// Description: Calculates various statistical measures for the specified
// samples including:
//   min - minimum value
//   max - maximum value
//   avg - average value
//   variance - variance
//   stdev - standard deviation
// Calculated values are returned in referenced arguments.
// Return value: None
//====================================================================
template <class T>
void get_stats(ac_channel<T> &samples, T& min, T& max, T& avg, T&variance, T&stdev)
{
   if (!samples.available(1)) {
      std::cerr << "get_stats: 'samples' is empty" << std::endl;
      return;
   }
   min = max = samples[0];
   T sum = (T)0;
   T sum2 = (T)0;
   
   for (unsigned int i=0; i<samples.debug_size(); i++) {
      T sample = samples[i];
      if (sample < min) min = sample;
      if (sample > max) max = sample;
      sum += sample;
      sum2 += sample * sample;
   }
   avg = sum / (T)samples.debug_size();
   variance = (sum2 - sum * avg) / ( samples.debug_size() - 1);
	stdev = sqrt(variance);
}


//====================================================================
// Function: get_stats 
// https://en.wikipedia.org/wiki/Relative_change_and_difference
// 
// Description: This variant returns max percent difference between the 
// paired samples.  Difference is:
//
//   diff = (abs(a - b)/max(abs(a), abs(b)))*100
// 
// Taken from the referenced wiki page - to quote:
//   "This approach is especially useful when comparing floating point values in 
//    programming languages for equality with a certain tolerance. Another 
//    application is in the computation of approximation errors when the 
//    relative error of a measurement is required."
//  
// We modify this for the case of zero denominator -
//   Consider - a=0, b=.00001
//   The above then becomes   |a - b]/b = 100% which isnt very satisfying
//   So...If either abs(a) or abs(b)) is less than the input parameter "zeroFuzz",
//   we look at the last two returned percentages and extrapolate from them.
//   The assumption is that the samples are evenly spaced.
//
//   If aSamples or bSamples are less than 2 in size, or if the zero is encountered
//   in the first two samples, we simply return zero.  In these cases, the user 
//   may optionally provide this "history" as a parameter for the function call.
//   If the input channel data is >2, then this input history is ignored and 
//   data computed here is used.  Note that history is in reverse order - 
//   Most recent datapoint is in history[0]
//
//   Default comapre to zero is 10*DBL_EPSILON and can be over-ridden
//   DBL_EPSILON was 2.22045e-16 today and 1/(10*DBL_EPSILON) is 2.22045e-15
//
// Returned data:
//   > MaxDiffPercent - Always positive
//   > maxIdx - Index into the channels where max-difference happened.  -1 if error
//
// Notes:
//   - Calculated values are returned in referenced arguments.
//   - T must be able to be cast to double - where the calculations occur.
//       So you must live within the precision of a double here
//   - The channels *are not* drained upon return.
//
// Return value: None
//====================================================================
template <class T>
void get_stats(ac_channel<T> &aSamples, 
               ac_channel<T> &bSamples,
               double &maxDifferentPercent, 
               int &maxIdx,
               std::vector<double> *historyP = 0,
               double zeroFuzz = 10*DBL_EPSILON)
{
  maxDifferentPercent = 0;
  maxIdx = -1;
  std::vector<double> history;  // reverse history

  if (historyP) {
    if (historyP->size() > 2) {
      // protect from a huge input historyP
      history[0] = (*historyP)[0];
      history[1] = (*historyP)[1];
    } else {
      history= *historyP;
    }
  }
  if (!aSamples.available(1)) {
    std::cerr << "get_stats: 'aSamples' is empty" << std::endl;
    return;
  }
  if (!bSamples.available(1)) {
    std::cerr << "get_stats: 'bSamples' is empty" << std::endl;
    return;
  }
  if (aSamples.size() != bSamples.size()) {
    std::cerr << "get_stats: Channels must be same size.  A=" << aSamples.size() << "  B=" << bSamples.size() << std::endl;
    return;
  }
  double thisErr;
  int cnt = aSamples.size();
  for (int i=0; i<cnt;i++) {
    T aVar = aSamples[i];
    T bVar = bSamples[i];

    double aDoub = (double)aVar;
    double bDoub = (double)bVar;
    
    thisErr = 0;
    if ((fabs(aDoub) <= zeroFuzz) || (fabs(bDoub) <= zeroFuzz)) {
      if (history.size() < 2) {
        thisErr = 0;
      } else {
        // extrapolate
        double y1 = history[1];
        double y2 = history[0];
        thisErr = y1 + 2*(y2-y1);
      }
    } else {
      double num = fabs(aDoub - bDoub);
      double denom = fabs(aDoub);
      if (fabs(bDoub) > denom) {
        denom = fabs(bDoub);
      }
      if (denom <= zeroFuzz) {  // pretty close to zero
        thisErr = 0;
      } else {
        thisErr = (num / denom) * 100;
      }
    }
    if (thisErr > maxDifferentPercent) {
      maxDifferentPercent = thisErr;
      maxIdx = i;
    }     
    std::vector<double>::iterator it = history.begin();
    history.insert(it, thisErr);
    if (history.size() > 2) {
      history.resize(2);
    }
  }
  return;
}
