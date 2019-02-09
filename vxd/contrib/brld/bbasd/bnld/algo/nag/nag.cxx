#include "NAG.hpp"
#include <time.h>
#include "lapack.hpp"
template <class Field>


// lapack solve routine (direct call)
// matrix A is transposed
bool solve_via_lapack(int size,
                      complex* A,  // size-by-size matrix of complex #s
                      int bsize,
                      complex* b,  // bsize-by-size RHS of Ax=b
                      complex* x   // solution
                      )
{
  bool ret = true;
  int info;

  int* permutation = newarray_atomic(int, size);
  complex* At = newarray(complex, size * size);
  int i, j;
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)  // transpose the matrix: lapack solves A^t x = b
      *(At + i * size + j) = *(A + j * size + i);
  double* copyA = (double*)At;
  copy_complex_array<ComplexField>(size, b, x);
  double* copyb = (double*)x;  // result is stored in copyb

  /*
  printf("-----------(solve)-----------------------------------\ncopyA:\n");
  for (i=0; i<size*size; i++)
    printf("(%lf %lf) ", *(copyA+2*i), *(copyA+2*i+1));
  printf("\nb:\n");
  for (i=0; i<size; i++)
    printf("(%lf %lf) ", *(copyb+2*i), *(copyb+2*i+1));
  */
  zgesv_(&size, &bsize, copyA, &size, permutation, copyb, &size, &info);
  /*
  printf("\nx = b:\n");
  for (i=0; i<size; i++)
    printf("(%lf %lf) ", *(copyb+2*i), *(copyb+2*i+1));
  printf("\n");
  */
  if (info > 0)
    {
      ERROR("according to zgesv, matrix is singular");
      ret = false;
    }
  else if (info < 0)
    {
      ERROR("argument passed to zgesv had an illegal value");
      ret = false;
    }

  deletearray(permutation);
  deletearray(At);

  return ret;
}

// lapack solve routine (direct call)
bool solve_via_lapack_without_transposition(
    int size,
    complex* A,  // size-by-size matrix of complex #s
    int bsize,
    complex* b,  // bsize-by-size RHS of Ax=b
    complex* x   // solution
    )
{
  bool ret = true;
  int info;

  int* permutation = newarray_atomic(int, size);
  // int i,j;
  double* copyA = (double*)A;
  copy_complex_array<ComplexField>(size, b, x);
  double* copyb = (double*)x;  // result is stored in copyb

  /*
  printf("-----------(solve)-----------------------------------\ncopyA:\n");
  for (int i=0; i<size*size; i++)
    printf("(%lf %lf) ", *(copyA+2*i), *(copyA+2*i+1));
  printf("\nb:\n");
  for (int i=0; i<size; i++)
    printf("(%lf %lf) ", *(copyb+2*i), *(copyb+2*i+1));
  */

  zgesv_(&size, &bsize, copyA, &size, permutation, copyb, &size, &info);
  /*
  printf("\nx = b:\n");
  for (i=0; i<size; i++)
    printf("(%lf %lf) ", *(copyb+2*i), *(copyb+2*i+1));
  printf("\n");
  */
  if (info > 0)
    {
      // ERROR("according to zgesv, matrix is singular");
      ret = false;
    }
  else if (info < 0)
    {
      ERROR("argument passed to zgesv had an illegal value");
      ret = false;
    }

  deletearray(permutation);

  return ret;
}

// In: A, a square matrix of size "size"
// Out: true if success, cond = condition number of A
bool cond_number_via_svd(int size, complex* A, double& cond)
{
  bool ret = true;
  char doit = 'A';  // other options are 'S' and 'O' for singular vectors only
  int rows = size;
  int cols = size;
  int info;
  int min = (rows <= cols) ? rows : cols;

  if (min == 0)
    {
      ERROR("expected a matrix with positive dimensions");
      return false;
    }

  int max = (rows >= cols) ? rows : cols;
  int wsize = 4 * min + 2 * max;
  double* workspace = newarray_atomic(double, 2 * wsize);
  double* rwork = newarray_atomic(double, 5 * max);

  double* copyA = (double*)A;
  double* u = newarray_atomic(double, 2 * rows * rows);
  double* vt = newarray_atomic(double, 2 * cols * cols);
  double* sigma = newarray_atomic(double, 2 * min);

  zgesvd_(&doit,
          &doit,
          &rows,
          &cols,
          copyA,
          &rows,
          sigma,
          u,
          &rows,
          vt,
          &cols,
          workspace,
          &wsize,
          rwork,
          &info);

  if (info < 0)
    {
      ERROR("argument passed to zgesvd had an illegal value");
      ret = false;
    }
  else if (info > 0)
    {
      ERROR("zgesvd did not converge");
      ret = false;
    }
  else
    {
      cond = sigma[0] / sigma[size - 1];
      // print_complex_matrix(size,copyA);
      // printf("(s_large=%lf, s_small=%lf)\n", sigma[0], sigma[size-1]);
    }

  deletearray(workspace);
  deletearray(rwork);
  // deletearray(copyA);
  deletearray(u);
  deletearray(vt);
  deletearray(sigma);

  return ret;
}

// In: A, a square matrix of size "size"
// Out: true if success, "norm" = operator norm of A^{-1}
bool norm_of_inverse_via_svd(int size, complex* A, double& norm)
{
  bool ret = true;
  char doit = 'A';  // other options are 'S' and 'O' for singular vectors only
  int rows = size;
  int cols = size;
  int info;
  int min = (rows <= cols) ? rows : cols;

  if (min == 0)
    {
      ERROR("expected a matrix with positive dimensions");
      return false;
    }

  int max = (rows >= cols) ? rows : cols;
  int wsize = 4 * min + 2 * max;
  double* workspace = newarray_atomic(double, 2 * wsize);
  double* rwork = newarray_atomic(double, 5 * max);

  double* copyA = (double*)A;
  double* u = newarray_atomic(double, 2 * rows * rows);
  double* vt = newarray_atomic(double, 2 * cols * cols);
  double* sigma = newarray_atomic(double, 2 * min);

  zgesvd_(&doit,
          &doit,
          &rows,
          &cols,
          copyA,
          &rows,
          sigma,
          u,
          &rows,
          vt,
          &cols,
          workspace,
          &wsize,
          rwork,
          &info);

  if (info < 0)
    {
      ERROR("argument passed to zgesvd had an illegal value");
      ret = false;
    }
  else if (info > 0)
    {
      ERROR("zgesvd did not converge");
      ret = false;
    }
  else
    {
      if (sigma[size - 1] == 0)
        {
          ERROR("invertible matrix expected");
          ret = false;
        }
      norm = 1 / sigma[size - 1];
    }

  deletearray(workspace);
  deletearray(rwork);
  // deletearray(copyA);
  deletearray(u);
  deletearray(vt);
  deletearray(sigma);

  return ret;
}

// END lapack-based routines

////////////////// PathTracker
/////////////////////////////////////////////////////////////////////////////////////////////////

int PathTracker::num_path_trackers = 0;

PathTracker::PathTracker()
{
  raw_solutions = NULL;
  solutions = NULL;
  DMforPN = NULL;
}

PathTracker::~PathTracker()
{
  for (int i = 0; i < n_sols; i++) raw_solutions[i].release();
  deletearray(raw_solutions);
  deletearray(DMforPN);
}

// creates a PathTracker object (case: is_projective), builds slps for predictor
// and corrector given start/target systems
// input: two (1-row) matrices of polynomials
// out: PathTracker*
PathTracker /* or null */* PathTracker::make(const Matrix* S,
                                             const Matrix* T,
                                             gmp_RR productST)
{
  if (S->n_rows() != 1 || T->n_rows() != 1)
    {
      ERROR("1-row matrices expected");
      return NULL;
    };
  PathTracker* p = new PathTracker;
  const PolyRing* R = p->homotopy_R = S->get_ring()->cast_to_PolyRing();
  if (R == NULL)
    {
      ERROR("polynomial ring expected");
      return NULL;
    }
  p->C = cast_to_CCC(R->getCoefficients());
  // const Ring* K = R->getCoefficients();
  // p->C = cast_to_CCC(K); // cast to ConcreteRing<ARingCCC> for now
  if (!p->C)
    {
      ERROR("complex coefficients expected");
      return NULL;
    }
  p->productST = mpfr_get_d(productST, GMP_RNDN);
  // p->bigT = asin(sqrt(1-p->productST*p->productST));
  // const double pi = 3.141592653589793238462643383279502;
  // if (p->productST < 0)
  //  p->bigT = pi - p->bigT;
  p->bigT = acos(p->productST);

  int n = S->n_cols() + 1;  // equals the number of variables
  p->maxDegreeTo3halves = 0;
  p->DMforPN = newarray(double, n);
  p->DMforPN[n - 1] = 1;
  p->S = S;
  p->slpS = NULL;
  for (int i = 0; i < n - 1; i++)
    {
      int d = degree_ring_elem(R, S->elem(0, i));
      if (d > p->maxDegreeTo3halves) p->maxDegreeTo3halves = d;
      p->DMforPN[i] = 1 / sqrt(d);
      StraightLineProgram* slp = StraightLineProgram::make(R, S->elem(0, i));
      if (p->slpS == NULL)
        p->slpS = slp;
      else
        {
          StraightLineProgram* t = p->slpS->concatenate(slp);
          delete slp;
          delete p->slpS;
          p->slpS = t;
        }
    }
  p->slpSx = p->slpS->jacobian(false, p->slpHxH /*not used*/, true, p->slpSxS);
  p->maxDegreeTo3halves = p->maxDegreeTo3halves * sqrt(p->maxDegreeTo3halves);

  p->T = T;
  p->slpT = NULL;
  for (int i = 0; i < T->n_cols(); i++)
    {
      StraightLineProgram* slp = StraightLineProgram::make(R, T->elem(0, i));
      if (p->slpT == NULL)
        p->slpT = slp;
      else
        {
          StraightLineProgram* t = p->slpT->concatenate(slp);
          delete slp;
          delete p->slpT;
          p->slpT = t;
        }
    }
  p->slpTx = p->slpT->jacobian(false, p->slpHxH /*not used*/, true, p->slpTxT);

  return p;
}


// THE MEAT ///////////////////////////////////////////////////////////////////
int PathTracker::track(const Matrix* start_sols)
{
  double the_smallest_number = 1e-13;
  double epsilon2 = mpfr_get_d(epsilon, GMP_RNDN);
  epsilon2 *= epsilon2;                           // epsilon^2
  double t_step = mpfr_get_d(init_dt, GMP_RNDN);  // initial step
  double dt_min_dbl = mpfr_get_d(min_dt, GMP_RNDN);
  double dt_increase_factor_dbl = mpfr_get_d(dt_increase_factor, GMP_RNDN);
  double dt_decrease_factor_dbl = mpfr_get_d(dt_decrease_factor, GMP_RNDN);
  double infinity_threshold2 = mpfr_get_d(infinity_threshold, GMP_RNDN);
  infinity_threshold2 *= infinity_threshold2;
  double end_zone_factor_dbl = mpfr_get_d(end_zone_factor, GMP_RNDN);

  int n = n_coords = start_sols->n_cols();
  n_sols = start_sols->n_rows();

  if (M2_numericalAlgebraicGeometryTrace > 1)
    printf(
        "epsilon2 = %e, t_step = %lf, dt_min_dbl = %lf, dt_increase_factor_dbl "
        "= %lf, dt_decrease_factor_dbl = %lf\n",
        epsilon2,
        t_step,
        dt_min_dbl,
        dt_increase_factor_dbl,
        dt_decrease_factor_dbl);

  // memory distribution for arrays
  complex* s_sols = newarray(complex, n * n_sols);
  raw_solutions = newarray(Solution, n_sols);
  complex* x0t0 = newarray(complex, n + 1);
  complex* x0 = x0t0;
  complex* t0 = x0t0 + n;
  complex* x1t1 = newarray(complex, n + 1);
  //  complex* x1 =  x1t1;
  //  complex* t1 = x1t1+n;
  complex* dxdt = newarray(complex, n + 1);
  complex* dx = dxdt;
  complex* dt = dxdt + n;
  complex* Hxt = newarray_atomic(complex, (n + 1) * n);
  complex* HxtH = newarray_atomic(complex, (n + 2) * n);
  complex* HxH = newarray_atomic(complex, (n + 1) * n);
  complex *LHS, *RHS;
  complex one_half(0.5, 0);
  complex* xt = newarray_atomic(complex, n + 1);
  complex* dx1 = newarray_atomic(complex, n);
  complex* dx2 = newarray_atomic(complex, n);
  complex* dx3 = newarray_atomic(complex, n);
  complex* dx4 = newarray_atomic(complex, n);

  // read solutions: rows are solutions
  int i, j;
  complex* c = s_sols;
  for (i = 0; i < n_sols; i++)
    for (j = 0; j < n; j++, c++)
      *c = complex(toBigComplex(C, start_sols->elem(i, j)));

  Solution* t_s = raw_solutions;  // current target solution
  complex* s_s = s_sols;          // current start solution

  for (int sol_n = 0; sol_n < n_sols; sol_n++, s_s += n, t_s++)
    {
      t_s->make(n, s_s);  // cook a Solution
      t_s->status = PROCESSING;
      bool end_zone = false;
      double tol2 =
          epsilon2;  // current tolerance squared, will change in end zone
      copy_complex_array<ComplexField>(n, s_s, x0);
      *t0 = complex(0, 0);

      *dt = complex(t_step);
      int predictor_successes = 0;
      int count = 0;  // number of steps
      while (t_s->status == PROCESSING &&
             1 - t0->getreal() > the_smallest_number)
        {
          if (1 - t0->getreal() <= end_zone_factor_dbl + the_smallest_number &&
              !end_zone)
            {
              end_zone = true;
              // to do: see if this path coinsides with any other path on entry
              // to the end zone
            }
          if (end_zone)
            {
              if (dt->getreal() > 1 - t0->getreal())
                *dt = complex(1 - t0->getreal());
            }
          else
            {
              if (dt->getreal() > 1 - end_zone_factor_dbl - t0->getreal())
                *dt = complex(1 - end_zone_factor_dbl - t0->getreal());
            }

          bool LAPACK_success = false;

          if (is_projective)  // normalize
            multiply_complex_array_scalar<ComplexField>(
                n, x0, 1 / sqrt(norm2_complex_array<ComplexField>(n, x0)));

          // PREDICTOR in: x0t0,dt,pred_type
          //           out: dx
          switch (pred_type)
            {
              // case RUNGE_KUTTA:
                  copy_complex_array<ComplexField>(n + 1, x0t0, xt);

                  // dx1
                  evaluate_slpHxt(n, xt, Hxt);
                  LHS = Hxt;
                  RHS = Hxt + n * n;
                  //
                  negate_complex_array<ComplexField>(n, RHS);
                  solve_via_lapack_without_transposition(n, LHS, 1, RHS, dx1);

                  // dx2
                  multiply_complex_array_scalar<ComplexField>(
                      n, dx1, one_half * (*dt));
                  add_to_complex_array<ComplexField>(
                      n, xt, dx1);            // x0+.5dx1*dt
                  xt[n] += one_half * (*dt);  // t0+.5dt
                  //
                  evaluate_slpHxt(n, xt, Hxt);
                  LHS = Hxt;
                  RHS = Hxt + n * n;
                  //
                  negate_complex_array<ComplexField>(n, RHS);
                  LAPACK_success = solve_via_lapack_without_transposition(
                      n, LHS, 1, RHS, dx2);

                  // dx3
                  multiply_complex_array_scalar<ComplexField>(
                      n, dx2, one_half * (*dt));
                  copy_complex_array<ComplexField>(n, x0t0, xt);  // spare t
                  add_to_complex_array<ComplexField>(
                      n, xt, dx2);  // x0+.5dx2*dt
                  // xt[n] += one_half*(*dt); // t0+.5dt (SAME)
                  //
                  evaluate_slpHxt(n, xt, Hxt);
                  LHS = Hxt;
                  RHS = Hxt + n * n;
                  //
                  negate_complex_array<ComplexField>(n, RHS);
                  LAPACK_success =
                      LAPACK_success && solve_via_lapack_without_transposition(
                                            n, LHS, 1, RHS, dx3);

                  // dx4
                  multiply_complex_array_scalar<ComplexField>(n, dx3, *dt);
                  copy_complex_array<ComplexField>(n + 1, x0t0, xt);
                  add_to_complex_array<ComplexField>(n, xt, dx3);  // x0+dx3*dt
                  xt[n] += *dt;                                    // t0+dt
                  //
                  evaluate_slpHxt(n, xt, Hxt);
                  LHS = Hxt;
                  RHS = Hxt + n * n;
                  //
                  negate_complex_array<ComplexField>(n, RHS);
                  LAPACK_success =
                      LAPACK_success && solve_via_lapack_without_transposition(
                                            n, LHS, 1, RHS, dx4);

                  // "dx1" = .5*dx1*dt, "dx2" = .5*dx2*dt, "dx3" = dx3*dt
                  multiply_complex_array_scalar<ComplexField>(n, dx4, *dt);
                  multiply_complex_array_scalar<ComplexField>(n, dx1, 2);
                  multiply_complex_array_scalar<ComplexField>(n, dx2, 4);
                  multiply_complex_array_scalar<ComplexField>(n, dx3, 2);
                  add_to_complex_array<ComplexField>(n, dx4, dx1);
                  add_to_complex_array<ComplexField>(n, dx4, dx2);
                  add_to_complex_array<ComplexField>(n, dx4, dx3);
                  multiply_complex_array_scalar<ComplexField>(n, dx4, 1.0 / 6);
                  copy_complex_array<ComplexField>(n, dx4, dx);
                break;
              default:
                ERROR("unknown predictor");
            };

          // make prediction
          copy_complex_array<ComplexField>(n + 1, x0t0, x1t1);
          add_to_complex_array<ComplexField>(n + 1, x1t1, dxdt);

          // CORRECTOR
          int n_corr_steps = 0;
          bool is_successful;
          do
            {
              n_corr_steps++;
              //
              evaluate_slpHxH(n, x1t1, HxH);
              LHS = HxH;
              RHS = HxH + n * n;  // i.e., H
              //
              negate_complex_array<ComplexField>(n, RHS);
              LAPACK_success =
                  LAPACK_success &&
                  solve_via_lapack_without_transposition(n, LHS, 1, RHS, dx);
              add_to_complex_array<ComplexField>(n, x1t1, dx);
              is_successful =
                (norm2_complex_array<ComplexField>(n, dx) <
                   tol2 * norm2_complex_array<ComplexField>(n, x1t1));
              // printf("c: |dx|^2 = %lf\n",
              // norm2_complex_array<ComplexField>(n,dx));
            }
          while (!is_successful and n_corr_steps < max_corr_steps);

          if (!is_successful)
            {
              // predictor failure
              predictor_successes = 0;
              *dt = complex(dt_decrease_factor_dbl) * (*dt);
              if (dt->getreal() < dt_min_dbl) t_s->status = MIN_STEP_FAILED;
            }
          else
            {
              // predictor success
              predictor_successes = predictor_successes + 1;
              copy_complex_array<ComplexField>(n + 1, x1t1, x0t0);
              count++;
              if (is_successful &&
                  predictor_successes >= num_successes_before_increase)
                {
                  predictor_successes = 0;
                  *dt = complex(dt_increase_factor_dbl) * (*dt);
                }
            }
          if (norm2_complex_array<ComplexField>(n, x0) > infinity_threshold2)
            t_s->status = INFINITY_FAILED;
          if (!LAPACK_success) t_s->status = SINGULAR;
        }
      // record the solution
      copy_complex_array<ComplexField>(n, x0, t_s->x);
      t_s->t = t0->getreal();
      if (t_s->status == PROCESSING) t_s->status = REGULAR;
      evaluate_slpHxH(n, x0t0, HxH);
      cond_number_via_svd(n, HxH /*Hx*/, t_s->cond);
      t_s->num_steps = count;
      if (M2_numericalAlgebraicGeometryTrace > 0)
        {
          if (sol_n % 50 == 0) printf("\n");
          switch (t_s->status)
            {
              case REGULAR:
                printf(".");
                break;
              case INFINITY_FAILED:
                printf("I");
                break;
              case MIN_STEP_FAILED:
                printf("M");
                break;
              case SINGULAR:
                printf("S");
                break;
              default:
                printf("-");
            }
          fflush(stdout);
        }
    }
  if (M2_numericalAlgebraicGeometryTrace > 0) printf("\n");

  // clear arrays
  // deletearray(t_sols); // do not delete (same as raw_solutions)
  deletearray(s_sols);
  deletearray(x0t0);
  deletearray(x1t1);
  deletearray(dxdt);
  deletearray(xt);
  deletearray(dx1);
  deletearray(dx2);
  deletearray(dx3);
  deletearray(dx4);
  deletearray(Hxt);
  deletearray(HxtH);
  deletearray(HxH);

  return n_sols;
}


// ------------------------- service functions -------------------------
void Solution::make(int m, const complex* s_s)
{
  this->n = m;
  x = newarray(complex, m);
  start_x = newarray(complex, m);
  copy_complex_array<ComplexField>(m, s_s, start_x);
}

void Solution::make(int m, const complexAP* s_s)
{
  this->n = m;
  x = newarray(complex, m);
  start_x = newarray(complex, m);
  for (int i = 0; i < n; i++) start_x[i] = s_s[i].to_complex();
}

int degree_ring_elem(const PolyRing* R, ring_elem re)
{
  RingElement* RE = RingElement::make_raw(R, re);
  M2_arrayint d_array = RE->multi_degree();
  delete RE;
  return d_array->array[0];
}
