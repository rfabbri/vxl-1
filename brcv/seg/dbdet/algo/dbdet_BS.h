// This is brcv/seg/dbdet/algo/dbdet_BS.h
#ifndef dbdet_BS_h
#define dbdet_BS_h
//:
//\file
//\brief 
//
// This code implements the basic Binary Splitting algorithm described in 
// the 1991 IEEE Trans. on Sig. Proc. article "Color Quantization of Images"
// by Michael Orchard and Charles Bouman, pp. 2677-90.
//
// The input is a 1-D array of data points.  The #define'd constant DIM 
// holds the number of dimensions.  The output is a set of cluster centers
// and is also a 1-D array.  The number of clusters is also returned.
//
//\author Mark A. Ruzon 8 September 1998 (ported to C++ by Amir Tamrakar)
//\date 11/26/07
//
//\verbatim
//  Modifications
//\endverbatim

#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#define MAXCLUSTERS 30
#define CDIM 3

void dbdet_BS(double *, int, int, int *, double **, int *);


#endif // dbdet_BS_h
