/*********************************************************
 *
 *  globalDefs.h
 *
 *  Contains definitions gathered from all over Jonahs code
 *
 ************************************************************/

#include <vnl/vnl_math.h>

#ifndef __GLOBAL_DEFS_H__
#define __GLOBAL_DEFS_H__


//#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
//#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

class intGreaterThan 
{
public:
  bool operator() (int a, int b) { return a>b; }
};


//got as many constants as possible from vision::\projects\kimia\newfragment-assembly\documents\config.txt
//when not defined in config.txt i used the default values from PuzzleSolving.cpp
//those that i made up are followed by "//?"

#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI vnl_math::pi
#endif                      // no need to add this to params.xml

#define DP_VERY_LARGE_COST  1E12 // no need to add this to params.xml
#define CLOSE_ANGLE_THRESH              M_PI/10.0 // no need to add this to params.xml
 // no need to add this to params.xml
//NEW
#define COST_ADJUSTMENT                 /*3*/0  //how much to add or subtract from the new first or last pair to differentiate from prev. 1st or last

// REFINED PARAMETERS (BY CAN ARAS)

/*******smoothing parameters*******/
#define PSI 1.0 // used for discrete curvature-based smoothing
#define NUM_TIMES 30 // used for discrete curvature-based smoothing

/*******re-sampling parameters*******/
#define FINE_RESAMPLE_DS 1.0 // used for fine resampling
#define COARSE_RESAMPLE_DS 25.0 // used for coarse resampling (analog to COARSE_SAMPLE_SIZE)

/*******cost computation parameters*******/
#define TRANSITION_STEEPNESS 8 // the value of p in the modified energy function (refer to the paper)

/*******corner finding parameters*******/
// determines the neighborhood size for the average tangent computation
// note that it does not directly tell the neighborhood size to use
#define VICINITY 35
// tangent change difference threshold for determining the extremal points, in terms of radians
#define MIN_TAN_TURN 0.5236
// determines the points used for the average tangent computation, in terms of arclength
#define DIST_STEP 0.5

/*******matching parameters*******/
// number of top fine matches picked for every pair of fragments; the matches go in the pairwise matches pool
#define NUM_FINE_OUT 5
// number of top coarse matches picked to apply fine matching for
#define NUM_COARSE_OUT 50
// minimum size allowed for a coarse matching map to be considered for fine matching
#define MIN_MAP_SIZE_CRSE 2

#define OLAP_THRESH_HIGH      25.0
#define TH_D                  0.2    // Merge Angle Difference Threshold
#define TH_L                  0.2    // Stretching Cost Threshold
#define TH_A                  0.2    // Bending Cost Threshold
#define TH_LA                 100.0  // Bending Cost Relaxation Length Threshold
#define MIN_D                 30.0   // Merge Angle Difference Relaxation Length Threshold
#define LAM                   1      // Lambda
#define MAX_FINE_COST         100.0
#define SMOOTH_EDGE_BONUS     10.0
#define LOCAL_REG_THRESH_HIGH 10.0
#define LOCAL_REG_THRESH_LOW  1.0
#define NUM_POINTS_AVERAGED   15.0
#define MOVE_BACK             25.0
#define DISTANCE_THRESH_1     10
#define DISTANCE_THRESH_2     15
#define FINE_SAMPLE_SKIP      5
#define LOCAL_REG_ITER_PAIR   10
#define DIAG_THRESH_LOW       2.0
#define LENGTH_THRESH_LOW     25.0
#define DIAG_COEF             -5.0
#define EDGE_COEF             -5.0
#define LENGTH_COEF           -1
#define DIST_COEF             20
#define MIN_MAP_SIZE_FINE     25
#define ANGLE_SIM_THRESH      0.2

/*******only puzzle solving parameters*******/
// used to determine if adding the next piece will line up with at least one junction already present in the partial solution
#define LINEUP_DIST_THRESH    15.0
#define OLAP_THRESH_LOW       10.0 //Overlap threshold.  Lower means more options eliminated
#define IDENTICAL_BONUS       20
#define CULLING_RANGE         100
#define NUM_TOP_STATES        1
#define LOCAL_REG_ITER_PUZ    25
#define CLOSED_JUNCTION_BONUS 25
#define LENGTH_THRESH_HIGH    50.0
#define DIST_THRESH_LOW       3.0
#define DIST_THRESH_HIGH      10.0
#define DIAG_THRESH_HIGH      5.0
#define NUM_STATES_ITER       12

#endif

