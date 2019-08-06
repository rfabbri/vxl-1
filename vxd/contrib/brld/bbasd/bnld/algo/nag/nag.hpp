// see ../packages/NAG.m2 for the description of the structure of SLPs

// enum SolutionStatus { ... defined in SLP-imp.hpp ... };
struct Solution
{
  int n;             // number of coordinates
  complex* x;        // array of n coordinates
  double t;          // last value of parameter t used
  complex* start_x;  // start of the path that produced x
  double cond;       // reverse condition number of Hx
  SolutionStatus status;
  int num_steps;  // number of steps taken along the path

  Solution() { status = UNDETERMINED; }
  void make(int m, const complex* s_s);
  void make(int m, const complexAP* s_s);
  ~Solution() { release(); }
  void release()
  {
    deletearray(x);
    deletearray(start_x);
  }
};

class PathTracker : public MutableEngineObject
{
  static int num_path_trackers;

  int number;  // trackers are enumerated

  Matrix* target;
  const Matrix *H, *S, *T;  // homotopy, start, target
  StraightLineProgram *slpH, *slpHxt, *slpHxtH,
      *slpHxH,  // slps for evaluating H, H_{x,t}, H_{x,t}|H, H_{x}|H
      *slpS, *slpSx, *slpSxS, *slpT, *slpTx,
      *slpTxT;       // slps for S and T, needed if is_projective
  double productST,  // real part of the Bombieri-Weyl (hermitian) product <S,T>
      bigT;          // length of arc between S and T
  double* DMforPN;   // multipliers used in ProjectiveNewton
  double maxDegreeTo3halves;  // max(degree of equation)^{3/2}
  // inline functions needed by track
  /* THIS SHOULD BE REPLACED BY COMPILED EVALUATIONS (of Chicago)
     void evaluate_slpHxt(int n, const complex* x0t0, complex* Hxt)
  void evaluate_slpHxtH(int n, const complex* x0t0, complex* HxtH)
  void evaluate_slpHxH(int n, const complex* x0t0, complex* HxH)
  */
  const CCC* C;                // coefficient field (complex numbers)
  const PolyRing* homotopy_R;  // polynomial ring where homotopy lives (does not
                               // include t if is_projective)
  int n_coords;
  int n_sols;
  Solution* raw_solutions;  // solutions + stats
  Matrix* solutions;        // Matrix of solutions passed to top level

  // parameters
  M2_bool is_projective;
  gmp_RR init_dt, min_dt;
  gmp_RR dt_increase_factor, dt_decrease_factor;
  int num_successes_before_increase;
  gmp_RR epsilon;
  int max_corr_steps;
  gmp_RR end_zone_factor;
  gmp_RR infinity_threshold;
  int pred_type;
  PathTracker();

 public:
  static PathTracker /* or null */* make(const Matrix*);  // from homotopy
  static PathTracker /* or null */* make(
      const Matrix* S,
      const Matrix* T,
      gmp_RR productST);  // from start/target systems
  static PathTracker /* or null */* make(
      StraightLineProgram* slp_pred,
      StraightLineProgram* slp_corr);  // precookedSLPs
  virtual ~PathTracker();

  void text_out(buffer& o) const;
  Matrix /* or null */* getSolution(int);
  Matrix /* or null */* getAllSolutions();
  int getSolutionStatus(int);
  int getSolutionSteps(int);
  gmp_RRorNull getSolutionLastT(int);
  gmp_RRorNull getSolutionRcond(int);
  int track(const Matrix*);
  Matrix /* or null */* refine(
      const Matrix* sols,
      gmp_RR tolerance,
      int max_corr_steps_refine = 100);  // refine solutions such that (error
                                         // estimate)/norm(solution) < tolerance

};
