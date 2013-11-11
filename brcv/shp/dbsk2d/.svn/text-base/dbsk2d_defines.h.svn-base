// This is brcv/shp/dbsk2d/dbsk2d_defines.h
#ifndef dbsk2d_defines_h_
#define dbsk2d_defines_h_
//:
// \file
// \brief This file contains various symbols and function 
// definitions for use internal to this library.
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

//##########################################################
// IMPORTANT SYSTEM NUMERICAL PARAMETERS
//##########################################################

//Note that the input geometry is assumed to be single precision.

#define  INPUT_TYPE       float
#define  VECTOR_TYPE      double

const double  ISHOCK_DIST_HUGE  =  1E28;
const double  DOUBLE_PRECISION  =  1E-14;      //Trustable Double Precision
const double  EI                =  1E-3;       //Relative Grid/World of Ei

const double  BND_WORLD_SIZE    =  2000.0;     //dbsk2d_ishock_boundary World Size [0-BND_WORLD_SIZE) 1000
const double  MAX_RADIUS        =  5000.0;     //IGNORE all shock propagations if over this range

const double  B_EPSILON         =  EI;         //EPSILON for input geometry (preprocessing)
const double  D_EPSILON         =  B_EPSILON;  //EPSILON to detect near degeneracy
                                               //needs to be a free parameter but larger than either
                                               //A_EPSILON or L_EPSILON
                                               //(may need to be made into two epsilons for angles and lengths)

const double  R_EPSILON         =  D_EPSILON;  //EPSILON for radius(time) comparison (larger than D_EPSILON)

const double  TO_EPSILON        =  1e-7;       //EPSILON for formation of LL-TO shocks (parallel lines)
                                                   //This can be small because the L-L representation is pretty accurate
                                                   //even for perfectly parallel lines
                                                   // The L-L TO shocks exist to deal with the radius issue

const double  CONTACT_EPSILON   =  0.25*TO_EPSILON; //EPSILON for forming contacts instead of shocks (colinearity)
                                                        //Needs to be smaller than TO_EPSILON otherwise no sources will be created between
                                                        //approximately parallel polylines

const double  A_EPSILON         =  1E-7;       //EPSILON for angle comparison (single precision)
const double  L_EPSILON         =  1E-7;       //EPSILON for length comparison (single precision)

//Infinity point
const double  INFINITY_POINT_X  =  MAX_RADIUS+1;
const double  INFINITY_POINT_Y  =  MAX_RADIUS+1;
const double  INVALID_POINT_X   =  MAX_RADIUS+2;
const double  INVALID_POINT_Y   =  MAX_RADIUS+2;
const double  INVALID_COORD     =  MAX_RADIUS+3;

// min curvature of a circular arc
const double MIN_ARC_CURVATURE  = 1e-10;
const double  W_THRESHOLD       =  0.01;      //width threshold to determine if an arc is replacable by a line

//##########################################################
// OTHER SYMBOLS
//##########################################################

//inside and outside for sampled files
#define    INSIDE          1
#define    OUTSIDE          0
#define    BOTHSIDE          2
#define    ESF_SELECTION      3

const double  LARGE_DRAWLENGTH  =  100.0;      // For Parabola and Hyperbola drawing
const double  POINT_TANGENT_DRAWLENGTH  =  0.3;

enum DIRECTION
{
   BOGUS_DIRECTION,
   LEFT, RIGHT,
  BOTH_DIRECTION
};

//for parameter conversions
#define CONSTRAINED true
#define UNCONSTRAINED false

#endif 
