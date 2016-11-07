#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>

#ifndef __TBS_CURVE_DEF__
#define __TBS_CURVE_DEF__

#include "Point.h"
#include "utils.h"

// Curve class
template <class ptType,class floatType>
class Curve{
public:
  //Constructors
  Curve();
  Curve(Point<ptType> *pt, int size, bool isOpen);
  Curve(ptType *x, ptType *y, ptType *z, int size, bool isOpen);
  Curve(const Curve<ptType,floatType> &rhs);
  Curve(ptType *x, ptType *y, ptType *z, floatType *theta, int size, bool isOpen);
  Curve(vector< Point<ptType> > pt, vector<floatType> theta,int size, bool isOpen);
  Curve(vector< Point<ptType> > pt, vector<floatType> theta,
    vector<floatType> curvature,int size, bool isOpen);
  // Destructor
  ~Curve(){};

  // Read data from Raphael contour format file
  void readDataFromFile(string fileName);

  // Iterators to go through the point list. 
  typename vector< Point<ptType> >::iterator begin(){return _ptArray.begin();}
  typename vector< Point<ptType> >::iterator end(){return _ptArray.end();}

  // Assignment operator
  Curve<ptType,floatType>& operator=(const Curve<ptType,floatType> &rhs);

  // Member Access functions 
  // Print all points
  void printElems();
  int numPoints(){return _numPoints;}
  floatType length(){return _length;}
  floatType totalCurvature(){return _totalCurvature;}
  floatType totalAngleChange(){return _totalAngleChange;}
  bool isOpen(){return _isOpen;}
  // Access different members based on an index
  Point<ptType>   point(int index){return _ptArray[index];}
  ptType x(int index){return _ptArray[index].x();};
  ptType y(int index){return _ptArray[index].y();};
  ptType z(int index){return _ptArray[index].z();};
  floatType arcLength(int index){return _arcLength[index];};
  floatType normArcLength(int index){return _normArcLength[index];};
  floatType curvature(int index){return _curvature[index];};
  floatType torsion(int index){return _tau[index];};
  floatType angle(int index){return _angle[index];};
  floatType dx(int index){return _dx[index];};
  floatType dy(int index){return _dy[index];};
  floatType dz(int index){return _dz[index];};

  //Functions to insert to the point list
  void append(Point<ptType> pt);
  void append(ptType x, ptType y, ptType z);
  void insert(typename vector< Point<ptType> >::iterator iter,Point<ptType> pt);
  void insert(typename vector< Point<ptType> >::iterator iter,ptType x, ptType y, ptType z);

  //Functions to compute the DPCosts
  void stretchCost(int i, int ip, vector<floatType> &a)
  {
    a.push_back(_arcLength[i]-_arcLength[ip]);
  };

  /*CANARAS
  void bendCost1(int i, int ip, vector<floatType> &a)
  {
  //a.push_back(angleDiff(_angle[i],_angle[ip]));

  double temp,temp1;
  temp=0.0;

  int j;

  for(j=ip+1;j<=i;j++)
  {
  temp+=(_curvature[j-1]+_curvature[j])/2.*(_arcLength[j]-_arcLength[j-1]);
  }

  //a.push_back(temp);
  //temp=angleDiff(_theta[i],_theta[ip]);

  temp1=(_curvature[i]+_curvature[ip])/2.*(_arcLength[i]-_arcLength[ip]);//+0.1*(_tau[i]+_tau[ip])/2.*(_arcLength[i]-_arcLength[ip]);

  //temp1=sqrt((_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+sin(_phi[i])*sin(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip]));
  //temp1+=0.1*(_tau[i]+_tau[ip])/2.*(_arcLength[i]-_arcLength[ip]);
  //temp1=temp1+(2*cos(_phi[i])*(_theta[i]-_theta[ip])*(_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+sin(_phi[i])*(_phi[i]-_phi[ip])*(_theta[i]-_theta[ip])+sin(_phi[i])*(_theta[i]-_theta[ip])*(-(_dphi[i]-_dphi[ip])+sin(_phi[i])*cos(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip])))/((_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+sin(_phi[i])*sin(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip]));

  a.push_back(temp);

  //printf("%d %d %f %f\n",i,ip,temp,temp1);
  };*/

  void bendCost3D(int i, int ip, vcl_vector<floatType> &a)
  {
    double dphi = _phi[i] - _phi[ip];
    double sinphi = vcl_sin(_phi[i]);
    double dtheta = _theta[i] - _theta[ip];
    double cost = vcl_sqrt(vcl_pow(dphi,2.0) + vcl_pow(sinphi,2.0) * vcl_pow(dtheta,2.0));
    a.push_back(cost);
  }

  void twistCost3D(int i, int ip, vcl_vector<floatType> &a)
  {
    double cosphi = vcl_cos(_phi[i]);
    double sinphi = vcl_sin(_phi[i]);
    double dtheta = _theta[i] - _theta[ip];
    double dphi = _phi[i] - _phi[ip];
    double ddphi = _dphi[i] - _dphi[ip];
    double ddtheta = _dtheta[i] - _dtheta[ip];

    double temp1 = 2 * cosphi * dtheta * vcl_pow(dphi,2.0);
    double temp2 = sinphi * dphi * ddtheta;
    double temp3 = sinphi * dtheta * (-ddphi + sinphi*cosphi*vcl_pow(dtheta,2.0));
    double temp4 = vcl_pow(dphi,2.0) + vcl_pow(sinphi,2.0) * vcl_pow(dtheta,2.0);

    double cost = 0;
    if(temp4 != 0)
      cost = (temp1 + temp2 + temp3) / temp4;

    a.push_back(cost);
  }

  void bendCost(int i, int ip, vector<floatType> &a)
  {
    //a.push_back(angleDiff(_angle[i],_angle[ip]));

    double temp,temp1;
    temp=0.0;
//    int j;

    /*CANARAS    
    for(j=ip+1;j<=i;j++)
    {
    temp+=(_curvature[j-1]+_curvature[j])/2.*(_arcLength[j]-_arcLength[j-1]);
    }
    */
    //a.push_back(temp);
    //temp=angleDiff(_theta[i],_theta[ip]);

    temp1=(_curvature[ip]+_curvature[i])/2.*(_arcLength[i]-_arcLength[ip]);//+0.1*(_tau[i]+_tau[ip])/2.*(_arcLength[i]-_arcLength[ip]);

    //temp1=sqrt((_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+sin(_phi[i])*sin(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip]));

    temp1+=0.1*(_tau[i]+_tau[ip])/2.*(_arcLength[i]-_arcLength[ip]);

    //temp1=temp1+(2*cos(_phi[i])*(_theta[i]-_theta[ip])*(_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+
    //sin(_phi[i])*(_phi[i]-_phi[ip])*(_theta[i]-_theta[ip])+
    //sin(_phi[i])*(_theta[i]-_theta[ip])*(-(_dphi[i]-_dphi[ip])+sin(_phi[i])*cos(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip])))/
    //((_phi[i]-_phi[ip])*(_phi[i]-_phi[ip])+sin(_phi[i])*sin(_phi[i])*(_theta[i]-_theta[ip])*(_theta[i]-_theta[ip]));

    a.push_back(0.7*temp1);

    //printf("%d %d %f %f\n",i,ip,temp,temp1);
  };

  //Computing the properties of the curve.
  void computeProperties();

public:
  //Data
  vector< Point<ptType> > _ptArray;
  vector<floatType> _arcLength;
  vector<floatType> _normArcLength;
  vector<floatType> _dx;
  vector<floatType> _dy;
  vector<floatType> _dz;
  vector<floatType> _d2x;
  vector<floatType> _d2y;
  vector<floatType> _d2z;
  vector<floatType> _d3x;
  vector<floatType> _d3y;
  vector<floatType> _d3z;

  vector<floatType> _curvature;
  vector<floatType> _tau; 
  vector<floatType> _angle;
  vector<floatType> _theta; 
  vector<floatType> _dtheta;
  vector<floatType> _phi;
  vector<floatType> _dphi;   

  int _numPoints;
  floatType _length;
  floatType _totalCurvature;
  floatType _totalAngleChange;
  bool _isOpen; // true - open, false - closed

  //Computing the properties of the curve.
  void computeArcLength();
  void computeDerivatives();
  void computeCurvatures();
  void computeAngles();
};

#endif
