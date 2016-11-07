#include <vnl/vnl_math.h>

class pairwiseMatch;

#define JONAH_FORMULAS 1
#define CAN_FORMULAS 0

#define M_PI vnl_math::pi
#define DP_VERY_LARGE_COST 1E12
#define CLOSE_ANGLE_THRESH M_PI/10.0
#define DISPLAY_INFORMATION 0
#define COST_ADJUSTMENT /*3*/0 

// matching parameters
//extern double ANGLE_SIM_THRESH;
extern double COARSE_RESAMPLE_DS;
extern double DIST_STEP;
extern double EDGE_COEF;
extern double FINE_RESAMPLE_DS;
extern unsigned FINE_SAMPLE_SKIP;
extern double LAM;
extern double LENGTH_THRESH_LOW;
extern unsigned LOCAL_REG_ITER_PAIR;
extern double MAX_FINE_COST;
extern double MIN_D;
extern unsigned MIN_MAP_SIZE_CRSE;
extern unsigned MIN_MAP_SIZE_FINE;
extern double MIN_TAN_TURN;
extern unsigned NUM_COARSE_OUT;
extern unsigned NUM_FINE_OUT;
extern unsigned NUM_TIMES;
extern double OLAP_THRESH_HIGH;
extern double PSI;
extern double TH_A;
extern double TH_D;
extern double TH_L;
extern double TH_LA;
//extern unsigned TRANSITION_STEEPNESS;
extern unsigned VICINITY;

// puzzle solving parameters
extern double CLOSED_JUNCTION_BONUS;
extern double CULLING_RANGE;
extern double DIAG_THRESH_HIGH;
extern double DIST_THRESH_HIGH;
extern double DIST_THRESH_LOW;
extern double IDENTICAL_BONUS;
extern double LENGTH_THRESH_HIGH;
extern double LINEUP_DIST_THRESH;
extern unsigned LOCAL_REG_ITER_PUZ;
extern unsigned NUMBER_OF_ITERATION_STEPS;
extern unsigned NUM_STATES_ITER;
extern unsigned NUM_TOP_STATES_SAVED;
extern unsigned NUM_TOP_STATES_PROCESSED;
extern double OLAP_THRESH_LOW;
extern unsigned TOP_MATCHES_TO_KEEP;

// common parameters
extern double DIAG_COEF;
extern double DIAG_THRESH_LOW;
extern double DISTANCE_THRESH_1;
extern double DISTANCE_THRESH_2;
extern double DIST_COEF;
extern double LENGTH_COEF;
extern double LOCAL_REG_THRESH_HIGH;
extern double LOCAL_REG_THRESH_LOW;
extern double MOVE_BACK;
extern double NUM_POINTS_AVERAGED;
extern double SMOOTH_EDGE_BONUS;

extern unsigned CLOSEST_POINT_NEIGHBORHOOD;
extern bool APPLY_COMBO_MATCH;
// global matches, all states use this
extern vcl_vector< pairwiseMatch > _matches;//vector of points.
// global rotated curves, used for search states
extern vcl_vector<bfrag_curve> _cList;
// original contours
extern vcl_vector<bfrag_curve> _Contours;
extern vcl_vector<bfrag_curve> _BotContours;
