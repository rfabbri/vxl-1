/*********************************************************
 *
 *  globalDefs.h
 *
 *  Contains definitions gathered from all over Jonahs code
 *
 ************************************************************/

#ifndef __GLOBAL_DEFS_H__
#define __GLOBAL_DEFS_H__


#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))


//got as many constants as possible from vision::\projects\kimia\newfragment-assembly\documents\config.txt
//when not defined in config.txt i used the default values from PuzzleSolving.cpp
//those that i made up are followed by "//?"

#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI        3.1415926535897932384626433832795
#endif

#define TWICE_PI    2*M_PI

#define ZERO_TOLERANCE      1E-1
#define DP_VERY_LARGE_COST  1E12
#define TIMING_ON           0
#define COARSE_SAMPLE_SIZE  25.0
#define MIN_MAP_SIZE_CRSE   2
#define OLAP_THRESH_HIGH    25.0

//from threshold.h
#define TH_D        0.2    // Merge Angle Difference Threshold
#define TH_L        0.2    // Stretching Cost Threshold
#define TH_A        0.2    // Bending Cost Threshold
#define TH_LA       100.0  // Bending Cost Relaxation Length Threshold
#define MIN_D       30.0   // Merge Angle Difference Relaxation Length Threshold

#define OVERLAP     10     // Overlap Threshold
#define LAM         1      // Lambda
#define NUM_OUT     10     // Top K Coarse Matches To Return
#define ___SIZE     2      // Minimum Number Of Alignment Points In Map 

#define C_DIS_M     20.0   // Distance Measure Constant
#define C_LEN_M     -0.02  // Length Measure Constant
#define C_DGN_M     -1.0   // Diagnostic Measure Constant

#define MAX_FINE_COST                   100.0
#define NUM_FINE_OUT                    5
#define NUM_COARSE_OUT                  50
#define SMOOTH_EDGE_BONUS               10.0
#define OLAP_THRESH_LOW                 10.0
#define LOCAL_REG_THRESH_HIGH           10.0
#define LOCAL_REG_THRESH_LOW            1.0
#define NUM_POINTS_AVERAGED             15.0
#define MOVE_BACK                       25.0
#define CLOSE_ANGLE_THRESH              M_PI/10.0
#define DISTANCE_THRESH_1               10
#define DISTANCE_THRESH_2               15
#define DISTANCE_THRESH_3               10
#define MAX_ACCEPTABLE_AVERAGE_OVERLAP  0.25
#define FINE_SAMPLE_SKIP                5
#define LOCAL_REG_ITER_PAIR             10
#define DIAG_THRESH_LOW                 2.0
#define LENGTH_THRESH_LOW               25.0
#define DIAG_COEF                       -5.0
#define EDGE_COEF                       -5.0
#define LENGTH_COEF                      -1
#define DIST_COEF                       20
#define MIN_MAP_SIZE_FINE               25
#define ANGLE_SIM_THRESH                0.2

//more impt?
#define INITIAL_SMOOTH                  5.0
#define CONTINUOUS_SEARCH               0
#define IDENTICAL_BONUS                 20
#define CULLING_RANGE                   100
#define NUM_TOP_STATES                  9
#define LOCAL_REG_ITER_PUZ              25
#define CLOSED_JUNCTION_BONUS           25
#define DISPLAY_INFORMATION             0
#define LENGTH_THRESH_HIGH              50.0
#define DIST_THRESH_LOW                 3.0
#define DIST_THRESH_HIGH                10.0
#define DIAG_THRESH_HIGH                5.0
#define NUM_STATES_ITER                 12
#define VICINITY                        20
#define MIN_TAN_TURN                    0.5236
#define DIST_STEP                       0.5

//NEW
#define COST_ADJUSTMENT                 3  //how much to add or subtract from the new first or last pair to differentiate from prev. 1st or last

class intGreaterThan {
 public:
  bool operator() (int a, int b){ 
      return a>b; 
  }
};


#endif

