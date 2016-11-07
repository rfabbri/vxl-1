#ifndef __TBS_UTILS_DP_DEF__
#define __TBS_UTILS_DP_DEF__

#include <vcl_vector.h>

#include "Point.h"
#include "extern_params.h"

template <class Type>
Type fixAngleMPiPi(Type a);

template <class Type>
Type fixAngleZTPi(Type a);

template <class Type>
Type angleDiff(Type a1, Type  a2);


template <class Type>
Type angleAdd(Type a1, Type  a2);

//double pointDist(vsol_point_2d a ,vsol_point_2d b);

//double pointDist(double x1, double y1, double x2, double y2);

template<class floatType>
floatType strainCost(floatType a1,floatType a2,floatType b1,floatType b2,
         floatType c1,floatType c2,floatType A1,floatType A2,
         floatType B1,floatType B2,floatType C1,floatType C2);

#define ARCLENSAMPLE 0.1

// Ensuring that the returned angle value is between -PI and PI
template <class Type>
Type fixAngleMPiPi(Type a)
{
  if (a < -M_PI)
    return a+2*M_PI;
  else if (a > M_PI)
    return a-2*M_PI;
  else
    return a;
}

template <class Type>
Type fixAngleZTPi(Type a){
  if (a < 0)
    return a+2*M_PI;
  else if (a > 2*M_PI)
    return a-2*M_PI;
  else
    return a;
}

// Computing the difference between a1 and a2, making sure
// the returned difference value is between -PI and PI
template <class Type>
Type angleDiff(Type a1, Type a2)
{
  a1=fixAngleMPiPi(a1);
  a2=fixAngleMPiPi(a2);
  if (a1 > a2)
    if (a1-a2 > M_PI)
      return a1-a2-2*M_PI;
    else
      return a1-a2;
  else if (a2 > a1)
    if (a1-a2 < -M_PI)
      return a1-a2+2*M_PI;
    else
      return a1-a2;
  return 0.0;
}


template <class Type>
Type angleAdd(Type a1, Type  a2){
  Type a=a1+a2;
  
  if (a > M_PI)
    return a-2*M_PI;
  if (a < -M_PI)
    return a+2*M_PI;
  else
    return a;
}
/*
double pointDist(vsol_point_2d a, vsol_point_2d b)
{
  return sqrt(pow(a.x()-b.x(),2.0)+pow(a.y()-b.y(),2.0));
}
*/
// finds the distance between two points given by (x1,y1) and (x2,y2)
/*
double pointDist(double x1, double y1, double x2, double y2)
{
  return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}
*/
/*
template<class floatType>
  floatType strainCost(floatType a1,floatType a2,floatType b1,floatType b2,
         floatType c1,floatType c2,floatType A1,floatType A2,
         floatType B1,floatType B2,floatType C1,floatType C2){

  floatType lambda=1E-2,mu=1E-2;
  floatType Area,U;
  floatType e11,e22,e12;

  //COUT << a1 << " " << a2 << endl;
  //COUT << b1 << " " << b2 << endl;
  //COUT << c1 << " " << c2 << endl;

  //   Area = fabs((b1*c2-b2*c1)+a1*(b2-c2)+a2*(c1-b1));
  //   e11 = fabs((b2-c2)*(A1-a1) + (c2-a2)*(B1-b1) + (a2-b2)*(C1-c1));
  //   e22 = fabs((c1-b1)*(A2-a2) + (a1-c1)*(B2-b2) + (b1-a1)*(C2-c2));
  //   e12 = fabs((c1-b1)*(A1-a1) + (a1-c1)*(B1-b1) + (b1-a1)*(C1-c1)+
  //    (b2-c2)*(A2-a2) + (c2-a2)*(B2-b2) + (a2-b2)*(C2-c2));
  Area = ((b1*c2-b2*c1)+a1*(b2-c2)+a2*(c1-b1));
  e11 = ((b2-c2)*(A1-a1) + (c2-a2)*(B1-b1) + (a2-b2)*(C1-c1));
  e22 = ((c1-b1)*(A2-a2) + (a1-c1)*(B2-b2) + (b1-a1)*(C2-c2));
  e12 = ((c1-b1)*(A1-a1) + (a1-c1)*(B1-b1) + (b1-a1)*(C1-c1)+
   (b2-c2)*(A2-a2) + (c2-a2)*(B2-b2) + (a2-b2)*(C2-c2));
  printf("A=%7.2f E11=%7.2f E2=%7.2f E12=%7.2f \n",Area,e11,e22,e12);
  printf("a: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",a1,a2,b1,b2,c1,c2,Area);
  printf("A: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",A1,A2,B1,B2,C1,C2,(B1*C2-B2*C1)+A1*(B2-C2)+A2*(C1-B1));
  if (Area == 0){
    U=0.0;
    printf("a: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",a1,a2,b1,b2,c1,c2,Area);
    printf("A: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",A1,A2,B1,B2,C1,C2,(B1*C2-B2*C1)+A1*(B2-C2)+A2*(C1-B1));
  }
  else
    U=((lambda+2*mu)*(pow(e11,2)+pow(e22,2))+2*lambda*e11*e22+4*mu*pow(e12,2))/Area;
  
  COUT << Area << " " << U << " " << endl;
  return U;
}

// template<class ptType, class floatType>
// void interpolateCurves(const Curve<ptType,floatType> &old, Curve<ptType,floatType> &interp){

//   int i;
//   int numSamples;

//   int numPoints=old.numPoints();
//   floatType ratio;

//   int interpPoints=0;
//   for (i=1;i<numPoints;i++){
//     floatType currDs = old.arcLength(i)-old.arcLength(i-1);
//     if (currDs>ARCLENSAMPLE){
//       numSamples=currDs/ARCLENSAMPLE;
//       for (j=1;j<=numSamples-1;j++){
  
//       }
//     }
//   }
// }


*/

//map<vcl_string, vcl_vector<vcl_string> > readParamters(int argc, char *argv[]);

#endif

