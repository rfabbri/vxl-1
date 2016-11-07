// matching parameters
//double ANGLE_SIM_THRESH;
double COARSE_RESAMPLE_DS;
double DIST_STEP;
double EDGE_COEF;
double FINE_RESAMPLE_DS;
unsigned FINE_SAMPLE_SKIP;
double LAM;
double LENGTH_THRESH_LOW;
unsigned LOCAL_REG_ITER_PAIR;
double MAX_FINE_COST;
double MIN_D;
unsigned MIN_MAP_SIZE_CRSE;
unsigned MIN_MAP_SIZE_FINE;
double MIN_TAN_TURN;
unsigned NUM_COARSE_OUT;
unsigned NUM_FINE_OUT;
unsigned NUM_TIMES;
double OLAP_THRESH_HIGH;
double PSI;
double TH_A;
double TH_D;
double TH_L;
double TH_LA;
//unsigned TRANSITION_STEEPNESS;
unsigned VICINITY;

// puzzle solving parameters
double CLOSED_JUNCTION_BONUS;
double CULLING_RANGE;
double DIAG_THRESH_HIGH;
double DIST_THRESH_HIGH;
double DIST_THRESH_LOW;
double IDENTICAL_BONUS;
double LENGTH_THRESH_HIGH;
double LINEUP_DIST_THRESH;
unsigned LOCAL_REG_ITER_PUZ;
unsigned NUMBER_OF_ITERATION_STEPS;
unsigned NUM_STATES_ITER;
unsigned NUM_TOP_STATES_SAVED;
unsigned NUM_TOP_STATES_PROCESSED;
double OLAP_THRESH_LOW;
unsigned TOP_MATCHES_TO_KEEP;

// common parameters
double DIAG_COEF;
double DIAG_THRESH_LOW;
double DISTANCE_THRESH_1;
double DISTANCE_THRESH_2;
double DIST_COEF;
double LENGTH_COEF;
double LOCAL_REG_THRESH_HIGH;
double LOCAL_REG_THRESH_LOW;
double MOVE_BACK;
double NUM_POINTS_AVERAGED;
double SMOOTH_EDGE_BONUS;

unsigned CLOSEST_POINT_NEIGHBORHOOD;
bool APPLY_COMBO_MATCH;
// global matches, all states use this
vcl_vector< pairwiseMatch > _matches;
// global rotated curves, used for search states
vcl_vector<bfrag_curve> _cList;
// original contours
vcl_vector<bfrag_curve> _Contours;
