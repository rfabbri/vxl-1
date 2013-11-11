#include "NewDPMatch.h"


template <class curveType,class floatType>
NewDPMatch<curveType,floatType>::NewDPMatch(){
//  DPMatch<curveType,floatType>::DPMatch();
  _R1 = 1.0;
  _R2 = 1.0;
}


template <class curveType,class floatType>
NewDPMatch<curveType,floatType>::NewDPMatch(curveType &c1, curveType &c2){
  //  DPMatch<curveType,floatType>::DPMatch(c1,c2);
  //cout << "In NewDPMatch constructor" << endl;
  vector< pair <int,int> > b;

  int n; 
  _curve1 = c1;
  _curve2 = c2;
  _flip = false;
  _curve1.computeProperties();
  _curve2.computeProperties();
  _n=_curve1.numPoints();
  _m=_curve2.numPoints();
  for (n=0;n<_n;n++){
    vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    pair <int,int> tmp3(0,0);
    vector< pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  _finalMap = b;
  _finalCost = DP_VERY_LARGE_COST;
  _R1 = 15.0;
  _R2 = 0.0;
//  _lambda = 1.0;
  _lambda.push_back(1.0);
}

template <class curveType,class floatType>
NewDPMatch<curveType,floatType>::NewDPMatch(curveType &c1, curveType &c2,
					    floatType R1,floatType R2, vector<floatType> lambda, 
					    int numLenElems){
  

  vector< pair <int,int> >  b;
  //cout << "In NewDPMatch Constructor" << endl;
  //fflush(stdout);
  int n; 
  _curve1 = c1;
  _curve2 = c2;
  //cout << "In NewDPMatch Constructor: Curves copied" << endl;
  //fflush(stdout);
  _flip = false;
  //_curve1.computeProperties();
  //_curve2.computeProperties();
  //cout << "In NewDPMatch Constructor: Properties computed" << endl;
  //fflush(stdout);
  _n=_curve1.numPoints();
  _m=_curve2.numPoints();
  //cout << "In NewDPMatch Constructor: N=" <<_n << " M= " << _m << endl;
  //fflush(stdout);
  for (n=0;n<_n;n++){
    vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    pair <int,int> tmp3(0,0);
    vector< pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  _finalMap = b;
  _finalCost = DP_VERY_LARGE_COST;
  _R1 = R1;
  _R2 = R2;
  _lambda =lambda;
  _numLenElems=numLenElems;
  //cout << "lambda " << _lambda << endl;
}


// Cost of matching the  interval [x(l),x(i)] to the interval [y(k),y(j)].
// Cost of matching the  interval [i,ip] to the interval [j,jp].
template <class curveType,class floatType>
floatType NewDPMatch<curveType,floatType>::computeIntervalCost(int i, int ip, int j, int jp)
{
  floatType cost,dF=0,dK=0,dTorsion=0;

  //Faster way ???
//   floatType ds11=0,ds12=0,ds13=0;
//   floatType ds21=0,ds22=0,ds23=0;
//   floatType dt11=0,dt12=0,dt13=0;
//   floatType dt21=0,dt22=0,dt23=0;
//   _curve1.stretchCost(i,ip,ds11,ds12,ds13);
//   _curve2.stretchCost(j,jp,ds21,ds22,ds23);
//   dF = fabs(ds11-ds21)+fabs(ds12-ds22)+fabs(ds13-ds23);

//   _curve1.bendCost(i,ip,dt11,dt12,dt13);
//   _curve2.bendCost(j,jp,dt21,dt22,dt23);
//   dK = fabs(dt11-dt21)+fabs(dt12-dt22)+fabs(dt13-dt23);
  //Faster way ???


  //Cleaner way ???

  vector <floatType> ds1,ds2,dt1,dt2,dtorsion1,dtorsion2;
  int k;
  _curve1.stretchCost(i,ip,ds1);
  _curve2.stretchCost(j,jp,ds2);
  for (k=0;k<ds1.size();k++)
    dF = dF+fabs(ds1[k]-_lambda[k]*ds2[k]);
  
  //if (i != ip && j != jp){
  
  _curve1.bendCost(i,ip,dt1);
  _curve2.bendCost(j,jp,dt2);
  for (k=0;k<dt1.size();k++)
    dK = dK+fabs(dt1[k]-dt2[k]);
  
  //Cleaner way ???

  _curve1.twistCost3D(i,ip,dtorsion1);
  _curve2.twistCost3D(j,jp,dtorsion2);
  
  for (k=0;k<dtorsion1.size();k++)
    dTorsion = dTorsion+fabs(dtorsion1[k]-dtorsion2[k]);

//  cost = dF+_R1*dK+_R2*dTorsion;
  cost = dF+_R1*dK;
  return cost;
} 
/*CANARAS
// Cost of matching the  interval [x(l),x(i)] to the interval [y(k),y(j)].
template <class curveType,class floatType>
floatType NewDPMatch<curveType,floatType>::computeIntervalCost1(int i, int ip, int j, int jp){
  floatType cost,dF=0,dK=0;

  //Faster way ???
//   floatType ds11=0,ds12=0,ds13=0;
//   floatType ds21=0,ds22=0,ds23=0;
//   floatType dt11=0,dt12=0,dt13=0;
//   floatType dt21=0,dt22=0,dt23=0;
//   _curve1.stretchCost(i,ip,ds11,ds12,ds13);
//   _curve2.stretchCost(j,jp,ds21,ds22,ds23);
//   dF = fabs(ds11-ds21)+fabs(ds12-ds22)+fabs(ds13-ds23);

//   _curve1.bendCost(i,ip,dt11,dt12,dt13);
//   _curve2.bendCost(j,jp,dt21,dt22,dt23);
//   dK = fabs(dt11-dt21)+fabs(dt12-dt22)+fabs(dt13-dt23);
  //Faster way ???


  //Cleaner way ???
  vector <floatType> ds1,ds2,dt1,dt2;
  int k;
  _curve1.stretchCost(i,ip,ds1);
  _curve2.stretchCost(j,jp,ds2);
  for (k=0;k<ds1.size();k++)
    dF = dF+fabs(ds1[k]-_lambda[k]*ds2[k]);
  
  //if (i != ip && j != jp){
  _curve1.bendCost1(i,ip,dt1);
  _curve2.bendCost1(j,jp,dt2);
  for (k=0;k<dt1.size();k++)
    {
      dK = dK+fabs(dt1[k]-dt2[k]);
      if((i==10)&&(ip==8)&&(j==12)&&(jp==9)) printf("%f  %f ",dt1[k],dt2[k]);
    }       

  //Cleaner way ???

  cost = dF+_R1*dK;
  return cost;
} */
