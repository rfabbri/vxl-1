#ifdef __cplusplus
extern "C" {
#endif

/* constants & macros */

#define EPSILON 1.0e-8
#define EPSILON_ERROR 1.0e-5
#define ERROR_PRECISION 5
#define INITIAL_ERROR_STEP 1.0e-2
#define ERROR_STEP_ADJUSTMENT 0.5
#define NUM_ITERATIONS 1000
#define NUM_DIRECTIONS 4
#define INTERVAL_STEP_SIZE 1.0e-2
#define TRUE 1
#define FALSE 0
#define PS_WIDTH 40
#define PS_HEIGHT 40
#define PS_CANVAS_SIZE 720
#define PS_PT_SIZE 5

/* data structures */

typedef struct{
  double term1;
  double term2;
}vector_2;


/* function declarations */

static vector_2* integrate_f(double);
static vector_2* compute_f(double,double,double,double,double,double);
static double compute_error(double,double,double,double,double,double,double,double);
static void compute_initial_estimates(double,double,double,double,double,double,double*,double*);
static vector_2** solve(double,double,double,double,double*,double*);
vector_2** find_iterative_solution(double,double,double,double,double,double,double*,double*);

static double minimum(double[],int,int*); 
static int within_epsilon_zero(double);

#ifdef __cplusplus
}
#endif

