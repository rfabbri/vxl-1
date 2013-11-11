/**************************************************************************
 * 
 * Jonahs code did not include a Curve.cpp file, all functionality of Curve
 * was just within the header, along with some global functions.  My first try
 * at splitting things up failed (LINKer errors) so now i am trying to integrate
 * simply by copying files exactly and making the necessary minor changes.
 *
 * i guess i don't not know anything about templated classes, but from the samples 
 * i've seen (and jonahs code) the implementation of the functionality described in the
 * class description (traditionaly header) comes after the class description in the header,
 * i.e. the functions are in the .h and there is no .cpp file (this file)!
 *
 * try uncommenting this NOop function here and in the header and call it anywhere; compile
 * will be fine, but linking will not succeed.
 ***************************************************************************/


#include "Curve.h"

template <class ptType,class floatType> class Curve;

//NOop is a test to see what went wrong w/ LINKing and what not
//template <class ptType,class floatType> 
//void Curve<ptType,floatType>::NOop(){}

