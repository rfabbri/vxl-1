#include "Curve.h"
#define ZERO_TOLERANCE 1E-3
#define kENOMaxOrder 3
#define kENODataLength (kENOMaxOrder + 3)
#define kZeroOrderIndex   0
#define kFirstOrderIndex  (kZeroOrderIndex+1)
#define kSecondOrderIndex (kFirstOrderIndex+1)
#define kThirdOrderIndex (kSecondOrderIndex+1)

struct ENO_Polynomial_t{
  double coeff[kENOMaxOrder + 1];
  int order;
};

struct ENO_Interpolant_t{
  double start,end;
  int forward;
  ENO_Polynomial_t poly;
  double start_tangent,end_tangent;
  double start_curvature,end_curvature;
  double total_curvature;
};

int enointerp_compute_polynomial(ENO_Interpolant_t *interp,
                                 double const pos[kENODataLength], 
                                 double const data[kENODataLength])
{
  double
    a2,		/* 2nd order coefficient of forward polynomial */
    b2,		/* 2nd order coefficient of backward polynomial */
    c2;		/* 2nd order coefficient in choosen direction */
  double a3,b3,c3;
  int off = 2;		/* offset in data array of x_loc entry */

  /* compute leading coefficient of forward and backward polynomials */
  a2 = ((data[4]-data[3])/(pos[4]-pos[3])-(data[3]-data[2])/(pos[3]-pos[2]))/(pos[4]-pos[2]);
  b2 = ((data[3]-data[2])/(pos[3]-pos[2])-(data[2]-data[1])/(pos[2]-pos[1]))/(pos[3]-pos[1]);
  /* determine which direction to use for interpolation */
  interp->forward = fabs(a2) < fabs(b2);
  if(!interp->forward) off=off-1;
  /* choose polynomial with smaller variation, where variation is
  measured as absolute value of leading polynomial coefficient.*/
  c2 = interp->forward ? a2:b2;
  if(off==2)
  {
    a2 = ((data[5]-data[4])/(pos[5]-pos[4])-(data[4]-data[3])/(pos[4]-pos[3]))/(pos[5]-pos[3]);
    b2 = ((data[4]-data[3])/(pos[4]-pos[3])-(data[3]-data[2])/(pos[3]-pos[2]))/(pos[4]-pos[2]);
    a3 = (a2 - b2)/(pos[5]-pos[2]);
    a2 = ((data[4]-data[3])/(pos[4]-pos[3])-(data[3]-data[2])/(pos[3]-pos[2]))/(pos[4]-pos[2]);
    b2 = ((data[3]-data[2])/(pos[3]-pos[2])-(data[2]-data[1])/(pos[2]-pos[1]))/(pos[3]-pos[1]);
    b3 = (a2 - b2)/(pos[4]-pos[1]);
    interp->forward = fabs(a3) < fabs(b3);
    c3 = interp->forward ? a3:b3;
    interp->poly.coeff[kThirdOrderIndex] = c3;

    if(interp->forward) 
    {
      interp->poly.coeff[kSecondOrderIndex] =c2 -c3*(pos[2]+pos[3]+pos[4]);
      interp->poly.coeff[kFirstOrderIndex] =(data[3]-data[2])/(pos[3]-pos[2])-c2*(pos[2]+pos[3])+c3*(pos[2]*
        pos[3]+pos[3]*pos[4]+pos[4]*pos[2]);
    }
    else
    {
      interp->poly.coeff[kSecondOrderIndex] =c2 -c3*(pos[1]+pos[2]+pos[3]);
      interp->poly.coeff[kFirstOrderIndex] = (data[2]-data[1])/(pos[2]-pos[1])-c2*(pos[2]+pos[3])+c3*(pos[1]*
        pos[2]+pos[2]*pos[3]+pos[3]*pos[1]);
    };
  }

  if(off==1)
  {
    a2 = ((data[4]-data[3])/(pos[4]-pos[3])-(data[3]-data[2])/(pos[3]-pos[2]))/(pos[4]-pos[2]);
    b2 = ((data[3]-data[2])/(pos[3]-pos[2])-(data[2]-data[1])/(pos[2]-pos[1]))/(pos[3]-pos[1]);
    a3 = (a2 - b2)/(pos[4]-pos[1]);
    a2 = ((data[3]-data[2])/(pos[3]-pos[2])-(data[2]-data[1])/(pos[2]-pos[1]))/(pos[3]-pos[1]);
    b2 = ((data[2]-data[1])/(pos[2]-pos[1])-(data[1]-data[0])/(pos[1]-pos[0]))/(pos[2]-pos[0]);
    b3 = (a2 - b2)/(pos[3]-pos[0]); 

    interp->forward = fabs(a3) < fabs(b3);
    c3 = interp->forward ? a3:b3;
    interp->poly.coeff[kThirdOrderIndex] = c3;

    if(interp->forward) 
    {
      interp->poly.coeff[kSecondOrderIndex] =c2 -c3*(pos[1]+pos[2]+pos[3]);
      interp->poly.coeff[kFirstOrderIndex] =(data[3]-data[2])/(pos[3]-pos[2])-c2*(pos[1]+pos[2])+c3*(pos[1]*
        pos[2]+pos[2]*pos[3]+pos[3]*pos[1]);
    }
    else
    {
      interp->poly.coeff[kSecondOrderIndex] =c2 -c3*(pos[0]+pos[1]+pos[2]);
      interp->poly.coeff[kFirstOrderIndex] = (data[2]-data[1])/(pos[2]-pos[1])-c2*(pos[1]+pos[2])+c3*(pos[0]*
        pos[1]+pos[1]*pos[2]+pos[2]*pos[0]);
    };
  }
  return 0;
}

// Default Constructor:
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve()
{
  vector< Point<ptType> > a;
  vector<floatType> b;

  _ptArray=a;
  _arcLength = b;
  _normArcLength = b;
  _dx = b;
  _dy = b;
  _dz = b;
  _d2x = b;
  _d2y = b;
  _d2z = b;
  _curvature = b;
  _tau=b;
  _theta=b;
  _phi=b;

  _angle = b;
  _numPoints=0;
  _length=0.0;
  _totalCurvature=0.0;
  _totalAngleChange=0.0;
  _isOpen=true;
}

// Constructor: From an array of points.
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(Point<ptType> *pt, int size, bool isOpen)
{
  _numPoints=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++)
    _ptArray.push_back(pt[i]);
  computeProperties();
}

//Constructor: From an array of x y z coords.
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(ptType *x, ptType *y, ptType *z, int size, bool isOpen)
{
  _numPoints=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++)
  {
    Point <ptType> pt(x[i],y[i],z[i]); 
    _ptArray.push_back(pt);
  }
  computeProperties();
}

//Copy constructor.
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(const Curve<ptType,floatType> &rhs)
{
  if (this != &rhs)
  {
    _numPoints=rhs._numPoints;
    _isOpen=rhs._isOpen;
    _ptArray = rhs._ptArray;
    _arcLength = rhs._arcLength;
    _normArcLength = rhs._normArcLength;
    _dx = rhs._dx;
    _dy = rhs._dy;
    _dz = rhs._dz;
    _curvature = rhs._curvature;
    _angle = rhs._angle;
    _totalCurvature=  rhs._totalCurvature;

    _tau = rhs._tau;
    _theta = rhs._theta;
    _phi= rhs._phi;
  }
}

// The following constructors to initialize from shocks should go
// into a new ShockCurve class.
// Constructor from shock info arrays
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(ptType *x, ptType *y, ptType *z, floatType *theta, int size, bool isOpen)
{
  _numPoints=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++)
  {
    Point <ptType> pt(x[i],y[i],z[i]); 
    _ptArray.push_back(pt);
    _angle.push_back(theta[i]);
  }
  computeArcLength();
  computeDerivatives();
  computeCurvatures();
}

// Constructor from shock info vectors
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(vector< Point<ptType> > pt, vector<floatType> theta, int size, bool isOpen)
{
  _numPoints=size;
  _isOpen=isOpen;
  _ptArray = pt;
  _angle = theta;

  computeArcLength();
  computeDerivatives();
  computeCurvatures();
}

// Constructor from shock info vectors
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(vector< Point<ptType> > pt, vector<floatType> theta,
                               vector<floatType> curvature, int  size, bool isOpen)
{
  _numPoints=size;
  _isOpen=isOpen;
  _ptArray = pt;
  _angle = theta;
  _curvature = curvature;
  computeArcLength();
  computeDerivatives();
}

// Assignment operator
template <class ptType,class floatType>
Curve<ptType,floatType> & 
Curve<ptType,floatType>::operator=(const Curve<ptType,floatType> &rhs)
{
  if (this != &rhs)
  {
    _numPoints = rhs._numPoints;
    _isOpen = rhs._isOpen;
    _length = rhs._length;
    _ptArray = rhs._ptArray;
    _arcLength = rhs._arcLength;
    _normArcLength = rhs._normArcLength;
    _dx = rhs._dx;
    _dy = rhs._dy;
    _dz = rhs._dz;
    _curvature = rhs._curvature;
    _tau=rhs._tau;
    _theta=rhs._theta;
    _phi=rhs._phi;
    _dphi=rhs._dphi;
    _dtheta=rhs._dtheta;
    _angle = rhs._angle;
  }
  return *this;
}

//Print x y z coordinates of all points.
template <class ptType,class floatType>
void Curve<ptType,floatType>::printElems()
{
  cout << "Num Points " << _numPoints << endl;
  vector< Point<ptType> >::iterator iter;
  for (iter=_ptArray.begin();iter!=_ptArray.end();iter++)
    printf("%8.3f %8.3f %8.3f\n",iter->x(),iter->y(),iter->z());
  //cout << iter->x() << " " << iter->y() <<endl;
}

//Insert a point to the list at location specified by iterator pt.
template <class ptType,class floatType>
void Curve<ptType,floatType>::insert(typename vector< Point<ptType> >::iterator iter,Point<ptType> pt)
{
  _ptArray.insert(iter,pt);
  _numPoints++;
}

//Add a point to the end of the list of points.
template <class ptType,class floatType>
void Curve<ptType,floatType>::append(Point<ptType> pt)
{
  _ptArray.push_back(pt);
  _numPoints++;
}

//Insert a point (x,y,z) to the list.
template <class ptType,class floatType>
void Curve<ptType,floatType>::insert(typename vector< Point<ptType> >::iterator iter,ptType x,ptType y,ptType z)
{
  Point<ptType> pt(x,y,z);
  _ptArray.insert(iter,pt);
  _numPoints++;
}

//Add a point  (x,y,z) to the end.
template <class ptType,class floatType>
void Curve<ptType,floatType>::append(ptType x,ptType y,ptType z)
{
  Point<ptType> pt(x,y,z);
  _ptArray.push_back(pt);
  _numPoints++;
}

//Compute arclength and normalized arc length
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeArcLength()
{
  floatType px,py,pz;
  floatType cx,cy,cz;
  floatType dL;
  int i;

  //Compute arc length
  _arcLength.clear();
  _length=0;
  _arcLength.push_back(0.0);

  px=_ptArray[0].x();
  py=_ptArray[0].y();
  pz=_ptArray[0].z();
  for (i=1;i<_numPoints;i++)
  {
    cx=_ptArray[i].x();
    cy=_ptArray[i].y();
    cz=_ptArray[i].z();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2)+pow(cz-pz,2));
    //cout<<cx<<" "<<cy<<" "<<cz<<" "<<dL<<" "<<endl;
    _length += dL;
    _arcLength.push_back(_length);
    px=cx;
    py=cy;
    pz=cz;
  }

  //Deal with the last point for a closed curve separately.
  if (!_isOpen)
  {
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    pz=_ptArray[_numPoints-1].z();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    cz=_ptArray[0].z();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2)+pow(cz-pz,2));
    _length += dL;
    _arcLength[0]=_length;
  }

  //Compute normalized arc length 
  _normArcLength.clear();
  for (i=0;i<_numPoints;i++)
    _normArcLength.push_back(_arcLength[i]/_length);
}

//Compute curvature. Assumes derivative computation has been done.
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeCurvatures()
{
  floatType cdx,cdy,cdz;
  floatType d2x,d2y,d2z;
  floatType cd2x,cd2y,cd2z;
  floatType d3x,d3y,d3z;

  floatType K,tau;
  int i;
  double temp;

  //Compute curvature
  _curvature.clear();
  _totalCurvature=0.0;
  _tau.clear();

  for (i=0;i<_numPoints;i++)
  {
    cdx=_dx[i];
    cdy=_dy[i];
    cdz=_dz[i];
    d2x=_d2x[i];
    d2y=_d2y[i];
    d2z=_d2z[i];

    if (fabs(cdx) < ZERO_TOLERANCE && fabs(cdy) < ZERO_TOLERANCE && fabs(cdz) <  ZERO_TOLERANCE)
      K=0;
    else
    { 
      temp=(cdx*cdx+cdy*cdy+cdz*cdz)*(d2x*d2x+d2y*d2y+d2z*d2z)-(cdx*d2x+cdy*d2y+cdz*d2z)*(cdx*d2x+cdy*d2y+cdz*d2z);
      temp=sqrt(temp);
      K=temp/pow((pow(cdx,2)+pow(cdy,2)+pow(cdz,2)),3/2);  
      //K=(d2y*cdx-d2x*cdy)/pow((pow(cdx,2)+pow(cdy,2)),3/2);   
    }
    //cout << dL << " " << cdx << " " << cdy <<" "<<K << endl;
    //printf("%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n",cdx,cdy,cdz,d2x,d2y,d2z,K);
    _curvature.push_back(K);
    _totalCurvature+=K;

    cd2x=_d2x[i];
    cd2y=_d2y[i];
    cd2z=_d2z[i];
    cdx=_dx[i];
    cdy=_dy[i];
    cdz=_dz[i];

    d3x=_d3x[i];
    d3y=_d3y[i];
    d3z=_d3z[i];

    temp=(cdx*cdx+cdy*cdy+cdz*cdz)*(cd2x*cd2x+cd2y*cd2y+cd2z*cd2z)-(cdx*cd2x+cdy*cd2y+cdz*cd2z)*(cdx*cd2x+cdy*cd2y+cdz*cd2z);

    if (temp<ZERO_TOLERANCE)
      tau=0;
    else
    { 
      tau=cdx*cd2y*d3z+cdy*cd2z*d3x+cd2x*d3y*cdz-cdz*cd2y*d3x-cdx*cd2z*d3y-cd2x*cdy*d3z;
      tau=tau/temp;
    }
    printf(" %6.3f %6.3f %6.3f\n",_arcLength[i],K,tau);
    _tau.push_back(tau);
  }
}

//Compute derivatives
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeDerivatives()
{
  floatType px,py,pz;
  floatType cx,cy,cz;
  floatType dL;
  int i;
  double ax[6],ay[6],az[6],pos[6];
  ENO_Interpolant_t interp;
  //Compute derivatives
  /*_dx.clear();
  _dx.push_back(0.0);
  _dy.clear();
  _dy.push_back(0.0);
  _dz.clear();
  _dz.push_back(0.0);

  px=_ptArray[0].x();
  py=_ptArray[0].y();
  pz=_ptArray[0].z();

  for (i=1;i<_numPoints;i++){
  cx=_ptArray[i].x();
  cy=_ptArray[i].y();
  cz=_ptArray[i].z();
  dL=sqrt(pow(cx-px,2)+pow(cy-py,2)+pow(cz-pz,2));

  //cout<<cx<<" "<<px<<" "<<endl;

  if (dL > ZERO_TOLERANCE){
  _dx.push_back((cx-px)/dL);
  _dy.push_back((cy-py)/dL);
  _dz.push_back((cz-pz)/dL);
  }
  else{
  _dx.push_back(0.0);
  _dy.push_back(0.0);
  _dz.push_back(0.0);
  }
  px=cx;
  py=cy;
  pz=cz;
  }*/
  _dx.clear();
  _dy.clear();
  _dz.clear();
  _dtheta.clear();
  _dphi.clear();

  _d2x.clear();
  _d2y.clear();
  _d2z.clear();  

  _d3x.clear();
  _d3y.clear();
  _d3z.clear();  

  _dx.push_back(0.0);
  _d2x.push_back(0.0);
  _d3x.push_back(0.0);
  _dtheta.push_back(0.0);
  _dphi.push_back(0.0);

  _dy.push_back(0.0);
  _d2y.push_back(0.0);
  _d3y.push_back(0.0);

  _dz.push_back(0.0);
  _d2z.push_back(0.0);
  _d3z.push_back(0.0);

  _dx.push_back(0.0);
  _d2x.push_back(0.0);
  _d3x.push_back(0.0);
  _dtheta.push_back(0.0);
  _dphi.push_back(0.0);

  _dy.push_back(0.0);
  _d2y.push_back(0.0);
  _d3y.push_back(0.0);

  _dz.push_back(0.0);
  _d2z.push_back(0.0);
  _d3z.push_back(0.0);

  for(i=2;i<_numPoints-3;i++)
  {    
    ax[0]=_ptArray[i-2].x();       
    ax[1]=_ptArray[i-1].x();
    ax[2]=_ptArray[i].x();
    ax[3]=_ptArray[i+1].x();
    ax[4]=_ptArray[i+2].x();
    ax[5]=_ptArray[i+3].x();

    pos[0]=_arcLength[i-2];
    pos[1]=_arcLength[i-1];
    pos[2]=_arcLength[i];
    pos[3]=_arcLength[i+1];
    pos[4]=_arcLength[i+2];
    pos[5]=_arcLength[i+3];

    enointerp_compute_polynomial(&interp,pos, ax);

    double a3=interp.poly.coeff[kThirdOrderIndex];
    double a2=interp.poly.coeff[kSecondOrderIndex];
    double a1=interp.poly.coeff[kFirstOrderIndex];

    _dx.push_back(2*a2*pos[2]+a1+3*a3*pos[2]*pos[2]);
    _d2x.push_back(2*a2+6*a3*pos[2]);
    _d3x.push_back(6*a3);

    ay[0]=_ptArray[i-2].y();       
    ay[1]=_ptArray[i-1].y();
    ay[2]=_ptArray[i].y();
    ay[3]=_ptArray[i+1].y();
    ay[4]=_ptArray[i+2].y();
    ay[5]=_ptArray[i+3].y();

    enointerp_compute_polynomial(&interp,pos, ay);

    a3=interp.poly.coeff[kThirdOrderIndex];
    a2=interp.poly.coeff[kSecondOrderIndex];
    a1=interp.poly.coeff[kFirstOrderIndex];

    _dy.push_back(2*a2*pos[2]+a1+3*a3*pos[2]*pos[2]);
    _d2y.push_back(2*a2+6*a3*pos[2]);
    _d3y.push_back(6*a3);

    az[0]=_ptArray[i-2].z();       
    az[1]=_ptArray[i-1].z();
    az[2]=_ptArray[i].z();
    az[3]=_ptArray[i+1].z();
    az[4]=_ptArray[i+2].z();
    az[5]=_ptArray[i+3].z();

    enointerp_compute_polynomial(&interp,pos, az);

    a3=interp.poly.coeff[kThirdOrderIndex];
    a2=interp.poly.coeff[kSecondOrderIndex];
    a1=interp.poly.coeff[kFirstOrderIndex];

    _dz.push_back(2*a2*pos[2]+a1+3*a3*pos[2]*pos[2]);
    _d2z.push_back(2*a2+6*a3*pos[2]);
    _d3z.push_back(6*a3);

    az[0]=_theta[i-2];       
    az[1]=_theta[i-1];
    az[2]=_theta[i];
    az[3]=_theta[i+1];
    az[4]=_theta[i+2];
    az[5]=_theta[i+3];

    enointerp_compute_polynomial(&interp,pos, az);

    a3=interp.poly.coeff[kThirdOrderIndex];
    a2=interp.poly.coeff[kSecondOrderIndex];
    a1=interp.poly.coeff[kFirstOrderIndex];

    _dtheta.push_back(2*a2*pos[2]+a1+3*a3*pos[2]*pos[2]);

    az[0]=_phi[i-2];       
    az[1]=_phi[i-1];
    az[2]=_phi[i];
    az[3]=_phi[i+1];
    az[4]=_phi[i+2];
    az[5]=_phi[i+3];

    enointerp_compute_polynomial(&interp,pos, az);

    a3=interp.poly.coeff[kThirdOrderIndex];
    a2=interp.poly.coeff[kSecondOrderIndex];
    a1=interp.poly.coeff[kFirstOrderIndex];

    _dphi.push_back(2*a2*pos[2]+a1+3*a3*pos[2]*pos[2]);
  }

  _dx.push_back(0.0);
  _d2x.push_back(0.0);
  _d3x.push_back(0.0);

  _dy.push_back(0.0);
  _d2y.push_back(0.0);
  _d3y.push_back(0.0);

  _dz.push_back(0.0);
  _d2z.push_back(0.0);
  _d3z.push_back(0.0);

  _dx.push_back(0.0);
  _d2x.push_back(0.0);
  _d3x.push_back(0.0);

  _dy.push_back(0.0);
  _d2y.push_back(0.0);
  _d3y.push_back(0.0);

  _dz.push_back(0.0);
  _d2z.push_back(0.0);
  _d3z.push_back(0.0);

  _dx.push_back(0.0);
  _d2x.push_back(0.0);
  _d3x.push_back(0.0);

  _dy.push_back(0.0);
  _d2y.push_back(0.0);
  _d3y.push_back(0.0);

  _dz.push_back(0.0);
  _d2z.push_back(0.0);
  _d3z.push_back(0.0);

  // ADDED BY CAN
  _dphi.push_back(0.0);
  _dphi.push_back(0.0);
  _dphi.push_back(0.0);

  _dtheta.push_back(0.0);
  _dtheta.push_back(0.0);
  _dtheta.push_back(0.0);
  ///////

  //Deal with the last point for a closed curve separately.
  if (!_isOpen)
  {
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    pz=_ptArray[_numPoints-1].z();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    cz=_ptArray[0].z();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2)+pow(cz-pz,2));
    _dx[0]=(cx-px)/dL;
    _dy[0]=(cy-py)/dL;
    _dz[0]=(cz-pz)/dL;
  }
  /*_d2x.clear();
  _d2x.push_back(0.0);
  _d2x.push_back(0.0);
  _d2y.clear();
  _d2y.push_back(0.0);
  _d2y.push_back(0.0);
  _d2z.clear();
  _d2z.push_back(0.0);
  _d2z.push_back(0.0);

  pdx=_dx[1];
  pdy=_dy[1];
  pdz=_dz[1];
  for (i=2;i<_numPoints;i++){
  cdx=_dx[i];
  cdy=_dy[i];
  cdz=_dz[i];
  dL=_arcLength[i]-_arcLength[i-1];
  if (dL > ZERO_TOLERANCE){
  _d2x.push_back((cdx-pdx)/dL);
  _d2y.push_back((cdy-pdy)/dL);
  _d2z.push_back((cdz-pdz)/dL);
  }
  else{
  _d2x.push_back(0.0);
  _d2y.push_back(0.0);
  _d2z.push_back(0.0);
  }
  pdx=cdx;
  pdy=cdy;
  pdz=cdz;
  }

  //Deal with the last point for a closed curve separately.
  if (!_isOpen){
  pdx=_dx[_numPoints-1];
  pdy=_dy[_numPoints-1];
  pdz=_dz[_numPoints-1];
  cdx=_dx[0];
  cdy=_dy[0];
  cdz=_dz[0];
  dL=_arcLength[i]-_arcLength[i-1];
  _d2x[0]=(cdx-pdx)/dL;
  _d2y[0]=(cdy-pdy)/dL;
  _d2z[0]=(cdz-pdz)/dL;
  }

  _d3x.clear();
  _d3x.push_back(0.0);
  _d3x.push_back(0.0);
  _d3y.clear();
  _d3y.push_back(0.0);
  _d3y.push_back(0.0);
  _d3z.clear();
  _d3z.push_back(0.0);
  _d3z.push_back(0.0);

  pdx=_d2x[1];
  pdy=_d2y[1];
  pdz=_d2z[1];
  for (i=2;i<_numPoints;i++){
  cdx=_d2x[i];
  cdy=_d2y[i];
  cdz=_d2z[i];
  dL=_arcLength[i]-_arcLength[i-1];
  if (dL > ZERO_TOLERANCE){
  _d3x.push_back((cdx-pdx)/dL);
  _d3y.push_back((cdy-pdy)/dL);
  _d3z.push_back((cdz-pdz)/dL);
  }
  else{
  _d3x.push_back(0.0);
  _d3y.push_back(0.0);
  _d3z.push_back(0.0);
  }
  pdx=cdx;
  pdy=cdy;
  pdz=cdz;
  }
  //Deal with the last point for a closed curve separately.
  if (!_isOpen){
  pdx=_dx[_numPoints-1];
  pdy=_dy[_numPoints-1];
  pdz=_dz[_numPoints-1];
  cdx=_dx[0];
  cdy=_dy[0];
  cdz=_dz[0];
  dL=_arcLength[i]-_arcLength[i-1];
  _d2x[0]=(cdx-pdx)/dL;
  _d2y[0]=(cdy-pdy)/dL;
  _d2z[0]=(cdz-pdz)/dL;
  }*/
}

//Compute angles
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeAngles()
{
  floatType px,py,pz;
  floatType cx,cy,cz;
  floatType theta,phi;
  int i;

  _angle.clear();
  _theta.clear();
  _phi.clear();

  _totalAngleChange=0.0;

  for (i=0;i<_numPoints-1;i++)
  {
    px=_ptArray[i].x();
    py=_ptArray[i].y();
    pz=_ptArray[i].z();

    cx=_ptArray[i+1].x();
    cy=_ptArray[i+1].y();
    cz=_ptArray[i+1].z();


    theta=atan2(cy-py,cx-px);
    _angle.push_back(theta);
    _theta.push_back(theta);

    phi=atan2(sqrt((cy-py)*(cy-py)+(cx-px)*(cx-px)),cz);
    _phi.push_back(phi);
  }
  _angle.push_back(0.0);
  _theta.push_back(0.0);
  _phi.push_back(0.0);

  _theta[0]=_theta[1];

  //Deal with the last point for a closed curve separately.
  if (!_isOpen)
  {
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    theta=atan2(cy-py,cx-px);
    _angle[0]=theta;
  }

  if (_numPoints>2){
    _angle[0]=_angle[1];
    for (i=1;i<_angle.size();i++){
      //cout << _angle[i] << " " << _angle[i-1] << endl;
      _totalAngleChange+=fabs(_angle[i]-_angle[i-1]);
    }
  }
}

//Public function that calls the private functions to
//compute the various curve properties.
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeProperties()
{
  computeArcLength();
  computeAngles();
  computeDerivatives();
  computeCurvatures();
  //computeAngles();
}

// Read the curve data from a Raph contour file. Assumes that there
// is only one contour per file.
/*
CONTOUR
OPEN (or CLOSE)
20 (numPoints)
x1 y1 x2 y2 x3 y3 ....
*/
template <class ptType,class floatType>
void Curve<ptType,floatType>::readDataFromFile(string fileName)
{
  ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (strncmp(magicNum,"CONTOUR",7)){
    cerr << "Invalid File " << fileName.c_str() << endl;
    cerr << "Should be CONTOUR " << magicNum << endl;
    exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!strncmp(openFlag,"OPEN",4))
    _isOpen = true;
  else if (!strncmp(openFlag,"CLOSE",5))
    _isOpen = false;
  else{
    cerr << "Invalid File " << fileName.c_str() << endl;
    cerr << "Should be OPEN/CLOSE " << openFlag << endl;
    exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  floatType x,y,z;
  for (i=0;i<numOfPoints;i++){
    infp >> x >> y >> z ;
    append(x,y,z);
  }
  infp.close();
}
