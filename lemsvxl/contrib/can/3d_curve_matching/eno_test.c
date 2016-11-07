#define kENOMaxOrder 3
#define kENODataLength (kENOMaxOrder + 3)
#define kZeroOrderIndex 0
#define kFirstOrderIndex (kZeroOrderIndex+1)
#define kSecondOrderIndex (kFirstOrderIndex+1)
#define kThirdOrderIndex (kSecondOrderIndex+1)

struct ENO_Polynomial_t
{
  double coeff[kENOMaxOrder + 1];
  int order;
};

struct ENO_Interpolant_t
{
  double start,end;
  int forward;
  struct ENO_Polynomial_t poly;
  double start_tangent,end_tangent;
  double start_curvature,end_curvature;
  double total_curvature;
};

int enointerp_compute_polynomial(struct ENO_Interpolant_t *interp,
                                 double const pos[kENODataLength], 
                                 double const data[kENODataLength])
{
  double a2,a3, /* 2nd order coefficient of forward polynomial */
    b2,b3, /* 2nd order coefficient of backward polynomial */
    c2,c3; /* 2nd order coefficient in choosen direction */
  int const off = 2; /* offset in data array of x_loc entry */
  int kmin=off;
  /* compute leading coefficient of forward and backward polynomials */
  a2 = ((data[off+2]-data[off+1])/(pos[off+2]-pos[off+1])-(data[off+1]-data[off])/(pos[off+1]-pos[off]))/(pos[off+2]-pos[off]);
  b2 = ((data[off+1]-data[off])/(pos[off+1]-pos[off])-(data[off]-data[off-1])/(pos[off]-pos[off-1]))/(pos[off+1]-pos[off-1]);
  /* determine which direction to use for interpolation */
  interp->forward = fabs(a2) < fabs(b2);
  /* choose polynomial with smaller variation, where variation is
  measured as absolute value of leading polynomial coefficient.*/
  c2 = interp->forward ? a2:b2;
  /* compute and store all polynomial coefficients for this interpolant */
  interp->poly.coeff[kSecondOrderIndex] = c2;
  if(interp->forward)
    interp->poly.coeff[kFirstOrderIndex] = (data[off+1]-data[off])/(pos[off+1]-pos[off])+c*(pos[off+1]-pos[off]);
  else
    interp->poly.coeff[kFirstOrderIndex] = (data[off+1]-data[off])/(pos[off+1]-pos[off])+c*(pos[off]-pos[off-1]);

  return 0;
}

