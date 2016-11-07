#ifndef __TBS_CURVE_DEF__
#define __TBS_CURVE_DEF__


#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <assert.h>

#include "Point.h"
#include "global.h"
#include "utils.h"

//foward declaration b/c bfrag2D includes this header (Curve.h)
class bfrag2D;

template <class ptType,class floatType>
class Curve{

public:
    Curve();
    Curve(PuzPoint<ptType> *pt, int size, bool isOpen);
    Curve(ptType *x, ptType *y, int size, bool isOpen);
    
    ~Curve(){};
    
    void setFrag(bfrag2D* f){ theFrag = f; };
    bfrag2D *getFrag(){ return theFrag; };
    void setTop(bool b){ isTop = b; };
    
    // Read data from Raphael contour format file
    void readDataFromFile(vcl_string fileName);
    
    // Write to Raphael contour format file
    void writeToFile(vcl_string fileName);
    
    // Eroded Curve
    Curve<ptType,floatType> *eCurve;
    
    bool operator==(Curve<ptType,floatType> c2);
    floatType operator-(Curve<ptType,floatType> c2);
    
    // Member Access functions 
    void printElems(); // Print all points
    int numPoints(){return _numPoints;}
    int numExtrema(){return _extrema.size();}
    int numCorners(){return _corners.size();}
    int coarseNumPoints(){return _coarseNumPoints;}
    int offset(){return _offset;}
    floatType length(){return _length;}
    floatType totalCurvature(){return _totalCurvature;}
    floatType totalAngleChange(){return _totalAngleChange;}
    bool isOpen(){return _isOpen;}
    bool lumCheck(){return static_cast<int>(_luminance.size())==numPoints();}
    int orient(){if(_totalAngleChange>0) return 1;else return -1;}
    
    floatType min_x(){return _box[0];}
    floatType min_y(){return _box[1];}
    floatType max_x(){return _box[2];}
    floatType max_y(){return _box[3];}
    
    // Access different members based on an index
    PuzPoint<ptType> ptInterp(floatType index);
    
    PuzPoint<ptType> point(int index){return _ptArray[index];}
    ptType x(int index){return _ptArray[index].x();}
    ptType y(int index){return _ptArray[index].y();}
    
    PuzPoint<ptType> Cpoint(int index){return _coarsePtArray[index].first;}
    ptType Cx(int index){return _coarsePtArray[index].first.x();}
    ptType Cy(int index){return _coarsePtArray[index].first.y();}
    
    floatType arcLength(int index){return _arcLength[index];}
    floatType normArcLength(int index){return _normArcLength[index];}
    floatType curvature(int index){return _curvature[index];}
    floatType angle(int index){return _angle[index];}
    floatType dx(int index){return _dx[index];}
    floatType dy(int index){return _dy[index];}
    floatType lum(int index){return _luminance[index];}
    
    floatType grad_x(int index){
      return (_gradient[index].first*cos(_angleRef)+
          _gradient[index].second*sin(_angleRef));
    }
    floatType grad_y(int index){
      return (_gradient[index].second*cos(_angleRef)-
          _gradient[index].first*sin(_angleRef));
    }
    
    int extrema(int index){return _extrema[index];}
    
    
    //Andrew
    //Added for 
    int corners(int index){return _corners[index];}
    
    int coarseRef(int index){return _coarsePtArray[index].second;}
    bool isCorner(int index){return vcl_find(_extrema.begin(),_extrema.end(),index)!=_extrema.end();}
    
    //Functions to insert to the point list
    void append(PuzPoint<ptType> pt);
    void append(ptType x, ptType y);
    
    //other functions
    void invert();
    void empty();
    void rotateTranslate(floatType angle, floatType x, floatType y);
    void translateRotate(floatType x, floatType y, floatType angle);
    void smooth(floatType win);
    
    //Functions to compute the DPCosts
    floatType mergeLength(int ip, int i);
    floatType mergeAngle(int ip, int i); 
    floatType totalLength(int i, int ip);
    
    //Computing the properties of the curve.
    void computeProperties();
    void computeExtrema();
    //im taking this out b/c it seems to be the only thing that uses
    //RGBAImage and i don't wanna include that if i don't HAVE to
    //-spinosa-
    //void imageProfile(RGBAImage* img);
    
    void computeArcLength();
    void computeDerivatives();
    void computeCurvatures();
    void computeAngles();
    void computeTangents();
    void computeAveTangents();
    floatType aveTangent(int start, int count);
    void findExtrema();
    
    //Andrew
    //Added to find corners
    void findCorners();
    
    void computeBox();
    
    //Resampling Functions
    void resample(floatType dS);
    void coarseResample(floatType dS);
    void becomeCoarse();
    
    //yeah i made it public, so be careful
    bool                   _isOpen;     // true - open, false - closed

protected:
    //Data
    vcl_vector<PuzPoint<ptType> > _ptArray;
    int                    _numPoints;
    floatType              _length;
    floatType              _angleRef;
    vcl_vector<floatType>      _arcLength;
    vcl_vector<floatType>      _normArcLength;
    vcl_vector<floatType>      _dx;
    vcl_vector<floatType>      _dy;
    vcl_vector<floatType>      _curvature;
    floatType              _totalCurvature;
    vcl_vector<floatType>      _angle;
    floatType              _totalAngleChange;

    // new stuff
    vcl_vector<floatType>                   _tangent;
    vcl_vector<vcl_pair<floatType,floatType> >  _aveTangent;
    vcl_vector<int>                         _extrema;

    //Andrew
    vcl_vector<int>                         _corners;

    vcl_vector<vcl_pair<PuzPoint<ptType>,int> > _coarsePtArray;
    int                                 _coarseNumPoints;
    int                                 _offset;
    vcl_vector<floatType>                   _box;
    vcl_vector<floatType>                   _luminance;
    vcl_vector<vcl_pair<floatType,floatType> >  _gradient;

private:
    //--spinner--//a pointer to the loins from whence this Curve was produced
    bfrag2D *theFrag;
    //and if it represents the top curve or the bottom
    bool isTop;

};


//################################################################################

//see Curve.cpp
//template <class ptType,class floatType> 
//void Curve<ptType,floatType>::NOop(){}

#define ZERO_TOLERANCE 1E-1

// Default Constructor:
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(){
  vcl_vector< PuzPoint<ptType> > a;
  vcl_vector<floatType> b;
  vcl_vector<int> c;

  _ptArray=a;
  _arcLength = b;
  _normArcLength = b;
  _dx = b;
  _dy = b;
  _curvature = b;
  _angle = b;
  _numPoints=0;
  _angleRef=0.0;
  _coarseNumPoints=0;
  _length=0.0;
  _totalCurvature=0.0;
  _totalAngleChange=0.0;
  _isOpen=true;
  _extrema=c;
  eCurve=0;
}

// Constructor: From an array of points.
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(PuzPoint<ptType> *pt, int size, bool isOpen){
  _numPoints=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++){
    _ptArray.push_back(pt[i]);
  }
  _angleRef=0.0;
  eCurve=0;
  computeProperties();
}

//Constructor: From an array of x and y coords.
template <class ptType,class floatType>
Curve<ptType,floatType>::Curve(ptType *x, ptType *y, int size, bool isOpen){
  _numPoints=size;
  _isOpen=isOpen;
  for (int i=0;i<size;i++){
    PuzPoint <ptType> pt(x[i],y[i]); 
    _ptArray.push_back(pt);
  }
  _angleRef=0.0;
  eCurve=0;
  computeProperties();
}

// //Copy constructor.
//template <class ptType,class floatType>
// Curve<ptType,floatType>::Curve(const Curve<ptType,floatType> &rhs){
//   if (this != &rhs){
//     _ptArray          = rhs._ptArray;    
//     _numPoints        = rhs._numPoints;
//     _isOpen           = rhs._isOpen;
//     _length           = rhs._length;
//     _arcLength        = rhs._arcLength;
//     _normArcLength    = rhs._normArcLength;
//     _dx               = rhs._dx;
//     _dy               = rhs._dy;
//     _curvature        = rhs._curvature;
//     _totalCurvature   = rhs._totalCurvature;
//     _angle            = rhs._angle;
//     _totalAngleChange = rhs._totalAngleChange;
//     _tangent          = rhs._tangent;
//     _aveTangent       = rhs._aveTangent;
//     _extrema          = rhs._extrema;
//     _coarsePtArray    = rhs._coarsePtArray;
//     _coarseNumPoints  = rhs._coarseNumPoints;
//     _box              = rhs._box;
//   }
// }

// // Assignment operator
// template <class ptType,class floatType>
// Curve<ptType,floatType> & 
// Curve<ptType,floatType>::operator=(const Curve<ptType,floatType> &rhs){
//   if (this != &rhs){
//     _ptArray          = rhs._ptArray;    
//     _numPoints        = rhs._numPoints;
//     _isOpen           = rhs._isOpen;
//     _length           = rhs._length;
//     _arcLength        = rhs._arcLength;
//     _normArcLength    = rhs._normArcLength;
//     _dx               = rhs._dx;
//     _dy               = rhs._dy;
//     _curvature        = rhs._curvature;
//     _totalCurvature   = rhs._totalCurvature;
//     _angle            = rhs._angle;
//     _totalAngleChange = rhs._totalAngleChange;
//     _tangent          = rhs._tangent;
//     _aveTangent       = rhs._aveTangent;
//     _extrema          = rhs._extrema;
//     _coarsePtArray    = rhs._coarsePtArray; 
//     _coarseNumPoints  = rhs._coarseNumPoints;
//     _box              = rhs._box;
//   }
//   return *this;
// }


//Overload == operator to compare curves
template <class ptType,class floatType>
bool Curve<ptType,floatType>::operator==(Curve<ptType,floatType> c2) {
  
  if(numPoints() != c2.numPoints()) 
    return false;
  
  return (*this-c2)<10.0; 
}


//Overload - operator to subtract curves.  Will fail if they
//have a different number of points.  Returns
//the average distance between each pair of points with corresponding indices
template <class ptType,class floatType>
floatType Curve<ptType,floatType>::operator-(Curve<ptType,floatType> c2) {

  assert(numPoints()==c2.numPoints());  
  
  //Distance between two points of same index
  floatType d;

  //Average distance between each pair of points
  floatType ave_d = 0.0;

  //Maximum distance between each pair of points
  //Note: not used in current version
  floatType max_d = 0.0;
  
  //Calculate d for each pair of points, keep track of the maximum distance and average distance
  for(int i=0;i<numPoints();i++) {
    d=pointDist(x(i),y(i),c2.x(i),c2.y(i));
    if(d>max_d) max_d=d;
    ave_d+=d;
  }
  ave_d/=numPoints();
  
  return ave_d;
  //return max_d;
}

//Print x,y coordinates of all points.
template <class ptType,class floatType>
void Curve<ptType,floatType>::printElems(){
  vcl_cout << "Num Points " << _numPoints << vcl_endl;
  typename vcl_vector< PuzPoint<ptType> >::iterator iter;
  for (iter=_ptArray.begin();iter!=_ptArray.end();iter++)
    printf("%8.3f %8.3f \n",iter->x(),iter->y());
  //vcl_cout << iter->x() << " " << iter->y() <<vcl_endl;
}

//Insert a point to the list at location specified by iterator pt.
//template <class ptType,class floatType>
//void Curve<ptType,floatType>::insert(vcl_vector< PuzPoint<ptType> >::iterator iter,PuzPoint<ptType> pt){
//  _ptArray.insert(iter,pt);
//  _numPoints++;
//}

//Insert a point (x,y) to the list.
//template <class ptType,class floatType>
//void Curve<ptType,floatType>::insert(vcl_vector< PuzPoint<ptType> >::iterator iter,ptType x,ptType y){
//  PuzPoint<ptType> pt(x,y);
//  _ptArray.insert(iter,pt);
//  _numPoints++;
//}

//Add a point to the end of the list of points.
template <class ptType,class floatType>
void Curve<ptType,floatType>::append(PuzPoint<ptType> pt){
  _ptArray.push_back(pt);
  _numPoints++;
}

//Add a point  (x,y) to the end.
template <class ptType,class floatType>
void Curve<ptType,floatType>::append(ptType x,ptType y){
  PuzPoint<ptType> pt(x,y);
  _ptArray.push_back(pt);
  _numPoints++;
}

//Reverse the order of points.
template <class ptType,class floatType>
void Curve<ptType,floatType>::invert() {
  int i;
  vcl_vector<PuzPoint<ptType> > temp = _ptArray;
  vcl_vector<vcl_pair<PuzPoint<ptType>,int> > coarseTemp = _coarsePtArray;
  vcl_vector<int> extTemp = _extrema;
  
  for(i=0;i<_numPoints;i++)
    _ptArray[i]=temp[_numPoints-i-1];
  for(i=0;i<_coarseNumPoints;i++) {
    _coarsePtArray[i]=coarseTemp[_coarseNumPoints-i-1];
    _coarsePtArray[i].second=_numPoints-_coarsePtArray[i].second-1;
  }    
  int extrema_size_int = static_cast<int>(_extrema.size());
  for(i=0;i<extrema_size_int;i++)
    _extrema[i]=_coarseNumPoints-extTemp[i]-1;
  computeProperties();
}

//Remove all points from the contour
template <class ptType,class floatType>
void Curve<ptType,floatType>::empty() {

  vcl_vector<PuzPoint<ptType> > a;
  _ptArray=a;
  _numPoints=0;
}

//Rigid Transformation of the contour
template <class ptType,class floatType>
void Curve<ptType,floatType>::rotateTranslate(floatType angle, floatType Tx, floatType Ty) {

  vcl_vector<PuzPoint<ptType> > newPointsFine;
  vcl_vector<PuzPoint<ptType> > newPointsCoarse;
  int i;
  floatType newX, newY;

  _angleRef+=angle;
  
  for(i=0;i<_numPoints;i++) {
    newX=  cos(angle)*x(i) + sin(angle)*y(i) + Tx;
    newY= -sin(angle)*x(i) + cos(angle)*y(i) + Ty;
    PuzPoint<ptType> newPoint(newX,newY);
    newPointsFine.push_back(newPoint);
  }
  for(i=0;i<_coarseNumPoints;i++) {
    newX=  cos(angle)*Cx(i) + sin(angle)*Cy(i) + Tx;
    newY= -sin(angle)*Cx(i) + cos(angle)*Cy(i) + Ty;
    PuzPoint<ptType> newPoint(newX,newY);
    newPointsCoarse.push_back(newPoint);
  }
 
  for(i=0;i<_numPoints;i++)
    _ptArray[i]=newPointsFine[i];
  
  for(i=0;i<_coarseNumPoints;i++)
    _coarsePtArray[i].first=newPointsCoarse[i];
  
  computeProperties();
  if(eCurve!=0)
    eCurve->rotateTranslate(angle,Tx,Ty);
}

//Different order (translate then rotate)
template <class ptType,class floatType>
void Curve<ptType,floatType>::translateRotate(floatType Tx, floatType Ty, floatType angle) {

  vcl_vector<PuzPoint<ptType> > newPointsFine;
  vcl_vector<PuzPoint<ptType> > newPointsCoarse;
  int i;
  floatType newX, newY;
  
  _angleRef+=angle;

  for(i=0;i<_numPoints;i++) {
    newX=  cos(angle)*(x(i)+Tx) + sin(angle)*(y(i)+Ty);
    newY= -sin(angle)*(x(i)+Tx) + cos(angle)*(y(i)+Ty);
    PuzPoint<ptType> newPoint(newX,newY);
    newPointsFine.push_back(newPoint);
  }
  for(i=0;i<_coarseNumPoints;i++) {
    newX=  cos(angle)*(Cx(i)+Tx) + sin(angle)*(Cy(i)+Ty);
    newY= -sin(angle)*(Cx(i)+Tx) + cos(angle)*(Cy(i)+Ty);
    PuzPoint<ptType> newPoint(newX,newY);
    newPointsCoarse.push_back(newPoint);
  }

  for(i=0;i<_numPoints;i++)
    _ptArray[i]=newPointsFine[i];
  
  for(i=0;i<_coarseNumPoints;i++)
    _coarsePtArray[i].first=newPointsCoarse[i];
  
  computeProperties();
  if(eCurve!=0)
    eCurve->translateRotate(Tx,Ty,angle);
}


//Performs smoothing on the contour
template <class ptType,class floatType>
void Curve<ptType,floatType>::smooth(floatType win) {
  
  int i,j,k,ind,num,sub_rng;
  int rng=static_cast<int>(ceil(3.0*win));
  int num_extr=_extrema.size();
  ptType xc,yc,xT,yT;
  vcl_vector<PuzPoint<ptType> > temp;
  
  if(num_extr==0) {

    for(i=0;i<_numPoints;i++) {
      num=0;xT=0;yT=0;
      for(j=-rng;j<=rng;j++) {  
  ind=(i+j+_numPoints) % _numPoints; 
  xc=x(ind);yc=y(ind);
  if(sqrt(pow(x(i)-xc,2)+pow(y(i)-yc,2)) <= win) {
    xT+=xc;yT+=yc;
    num++;
  }
      }
      xT/=num;yT/=num;
      temp.push_back(PuzPoint<ptType>(xT,yT));  
    }
    _ptArray=temp;
    computeProperties();
    resample(1.0);  
  }

  else {

    int ec,ep;
    temp=_ptArray;

    for(k=1;k<=num_extr;k++) {
      ec=_extrema[k%num_extr];
      ep=_extrema[k-1];

      for(i=ep;i%_numPoints!=ec;i++) {
  num=0;xT=0;yT=0;
  sub_rng= MIN(i-ep,MIN((ec-i+_numPoints)%_numPoints,rng));
  for(j=-sub_rng;j<=sub_rng;j++) {  
    ind=(i+j+_numPoints) % _numPoints; 
    xc=x(ind);yc=y(ind);
    if(sqrt(pow(x(i%_numPoints)-xc,2)+pow(y(i%_numPoints)-yc,2)) <= win) {
      xT+=xc;yT+=yc;
      num++;
    }
  }
  xT/=num;yT/=num;
  temp[i%_numPoints]=PuzPoint<ptType>(xT,yT);
      }
    }
    _ptArray=temp;
    computeProperties();
  }
}

// Returns an interpolated contour point using a non-integer index
template <class ptType,class floatType>
PuzPoint<ptType> Curve<ptType,floatType>::ptInterp(floatType index) {
  
  floatType x1,y1,x2,y2,x,y,interp;
  PuzPoint<ptType> prev,next,nullpt;
  int p,n;

  interp=index-(floatType)floor(index);

  p=floor(index);
  if(p>=_numPoints) {
    vcl_cout << "Warning! Index out of Bounds" << vcl_endl;
    return nullpt;
  }
  n=p+1; 
  if(n==_numPoints) 
    n=0;
  
  prev=point(p);
  next=point(n);
  
  x1=prev.x();
  y1=prev.y();
  x2=next.x();
  y2=next.y();

  x=x1+interp*(x2-x1);
  y=y1+interp*(y2-y1);

  PuzPoint<ptType> newPt(x,y);
    
  return newPt;
}

//Returns total length along curve between ip and i
template <class ptType,class floatType>
floatType Curve<ptType,floatType>::totalLength(int ip, int i) {
    
  floatType l=0.0;
  int s;
  
  if(ip<=i){
    for(s=ip+1;s<=i;s++)
      //arcLength returns the arclength of the segment ending in s
      l+=_arcLength[s];
  }
  else{
    for(s=ip+1;s<_numPoints;s++)
      l+=_arcLength[s];
    for(s=0;s<=i;s++)
      l+=_arcLength[s];
  }
  return l;
}



//Finds the Euclidean distance between point index ip and index i
template <class ptType,class floatType>
floatType Curve<ptType,floatType>::mergeLength(int ip, int i) {
    
  floatType lx=0;
  floatType ly=0;
  floatType l;
  
  if(i-ip==1)
    l=_arcLength[i];

  else {
    lx=_ptArray[i].x()-_ptArray[ip].x();
    ly=_ptArray[i].y()-_ptArray[ip].y(); 
    l=sqrt(lx*lx+ly*ly);
  }
  return l;
}  

//Calculates the angle from horizontal from point of
//index ip to point of index i.
template <class ptType,class floatType>
floatType Curve<ptType,floatType>::mergeAngle(int ip, int i) {
  
  floatType a=0;
  floatType lx=0;
  floatType ly=0;
  
  if(i-ip==1) 
    a=_angle[i];
  else {
    lx=_ptArray[i].x()-_ptArray[ip].x();
    ly=_ptArray[i].y()-_ptArray[ip].y(); 
    a=atan2(ly,lx);
  }  
  return a;
}

/*
//Profiles Image (?)
template <class ptType,class floatType>
void Curve<ptType,floatType>::imageProfile(RGBAImage* img) {

  int i,xi,yi,ref;
  vcl_vector<floatType> ex,ey,lum;
  
  imSmooth(img,config::val("GAUSS_WIN_SIZE",10),&ex,&ey,&lum);
  int num=numPoints();
  
  for(i=0;i<num;i++) {
    xi=floor(x(i));
    yi=floor(y(i));
    ref=yi*img->width()+xi;
    _gradient.push_back(vcl_pair<floatType,floatType>(ex[ref],ey[ref]));
    _luminance.push_back(lum[ref]); 
  }
}
*/

//Compute the box containing the contour
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeBox() {

  int i;
  floatType mn_x,mn_y,mx_x,mx_y;
  _box.clear();
  
  mn_x=x(0);
  mn_y=y(0);
  mx_x=x(0);
  mx_y=y(0);  
  
  for(i=1;i<numPoints();i++) {
    if(x(i)<mn_x) mn_x=x(i);
    if(y(i)<mn_y) mn_y=y(i);
    if(x(i)>mx_x) mx_x=x(i);
    if(y(i)>mx_y) mx_y=y(i);
  }
  
  _box.push_back(mn_x);
  _box.push_back(mn_y);
  _box.push_back(mx_x);
  _box.push_back(mx_y);
}

//Compute arclength and normalized arc length
//Normalized arclength means total length is 1
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeArcLength() {

  floatType px,py;
  floatType cx,cy;
  floatType dL;
  int i;

  //Compute arc length
  _arcLength.clear();
  _length=0;
  _arcLength.push_back(0.0);
  
  px=_ptArray[0].x();
  py=_ptArray[0].y();
  for (i=1;i<_numPoints;i++){
    cx=_ptArray[i].x();
    cy=_ptArray[i].y();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2));
    _length += dL;
    _arcLength.push_back(dL);
    px=cx;
    py=cy;
  }
  
  //Deal with the last point for a closed curve separately.
  if (!_isOpen){
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2));
    _length += dL;
    _arcLength[0]=dL;
  }

  //Compute normalized arc length 
  _normArcLength.clear();
  for (i=0;i<_numPoints;i++)
    _normArcLength.push_back(_arcLength[i]/_length);
}


//Compute curvature. Assumes derivative computation has been done.
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeCurvatures(){

  floatType pdx,pdy;
  floatType cdx,cdy;
  floatType d2x,d2y;
  floatType dL,K;
  int i;

  //Compute curvature
  _curvature.clear();
  _curvature.push_back(0.0);
  _totalCurvature=0.0;

  for (i=1;i<_numPoints;i++){
    pdx=_dx[i-1];
    pdy=_dy[i-1];
    cdx=_dx[i];
    cdy=_dy[i];
    dL=_arcLength[i]-_arcLength[i-1];
    if (dL > ZERO_TOLERANCE){
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    if (fabs(cdx) < ZERO_TOLERANCE && fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/pow((pow(cdx,2)+pow(cdy,2)),3/2);
    //vcl_cout << d2x << " " << d2y << " " << dL << " " << cdx << " " << cdy << " " << K << vcl_endl;
    //printf("%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f\n",d2x,d2y,dL,cdx,cdy,K);
    _curvature.push_back(K);
    _totalCurvature+=K;
  }

  //Deal with the last point for a closed curve separately.
  if (!_isOpen){
    pdx=_dx[_numPoints-1];
    pdy=_dy[_numPoints-1];
    cdx=_dx[0];
    cdy=_dy[0];
    dL=_arcLength[0]-_arcLength[_numPoints-1];
    if (dL > ZERO_TOLERANCE ){
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    if (fabs(cdx) < ZERO_TOLERANCE && fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/pow((pow(cdx,2)+pow(cdy,2)),3/2);
    _curvature[0]=K;
    
  }
}

//Compute derivatives
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeDerivatives(){

  floatType px,py;
  floatType cx,cy;
  floatType dL;
  int i;

  //Compute derivatives
  _dx.clear();
  _dx.push_back(0.0);
  _dy.clear();
  _dy.push_back(0.0);
  
  px=_ptArray[0].x();
  py=_ptArray[0].y();
  for (i=1;i<_numPoints;i++){
    cx=_ptArray[i].x();
    cy=_ptArray[i].y();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2));
    if (dL > ZERO_TOLERANCE){
      _dx.push_back((cx-px)/dL);
      _dy.push_back((cy-py)/dL);
    }
    else{
      _dx.push_back(0.0);
      _dy.push_back(0.0);
    }
    px=cx;
    py=cy;
  }
  
    //Deal with the last point for a closed curve separately.
  if (!_isOpen){
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    dL=sqrt(pow(cx-px,2)+pow(cy-py,2));
    _dx[0]=(cx-px)/dL;
    _dy[0]=(cy-py)/dL;
  }
}

//Compute angles rotated through from one point
//to the next
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeAngles(){

  floatType px,py;
  floatType cx,cy;
  floatType theta;
  int i;
  
  _angle.clear();
  _angle.push_back(0.0);
  _totalAngleChange=0.0;
  
  px=_ptArray[0].x();
  py=_ptArray[0].y();
  for (i=1;i<_numPoints;i++) {
    cx=_ptArray[i].x();
    cy=_ptArray[i].y();
    theta=atan2(cy-py,cx-px);
    _angle.push_back(theta);
    px=cx;
    py=cy;
  }

  //Deal with the last point for a closed curve separately.
  if (!_isOpen){
    px=_ptArray[_numPoints-1].x();
    py=_ptArray[_numPoints-1].y();
    cx=_ptArray[0].x();
    cy=_ptArray[0].y();
    theta=atan2(cy-py,cx-px);
    _angle[0]=theta;
  }
  
  _totalAngleChange+=angleDiff(_angle[0],_angle[_angle.size()-1]);
  int angle_size_int = static_cast<int>(_angle.size());
  for (i=1;i<angle_size_int;i++) {
    _totalAngleChange+=angleDiff(_angle[i],_angle[i-1]);
  }
}

  
//Public function that calls the private functions to
//compute the various curve properties.
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeProperties(){
  computeArcLength();
  computeAngles();
  computeBox();
  //  computeDerivatives();
  //  computeCurvatures();
}


//Finds extrema
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeExtrema(){
  computeTangents();
  computeAveTangents();
  findExtrema();
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
void Curve<ptType,floatType>::readDataFromFile(vcl_string fileName){

  vcl_ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (strncmp(magicNum,"CONTOUR",7)){
    vcl_cout << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cout << "Should be CONTOUR " << magicNum << vcl_endl;
    exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!strncmp(openFlag,"OPEN",4))
    _isOpen = true;
  else if (!strncmp(openFlag,"CLOSE",5))
    _isOpen = false;
  else{
    vcl_cout << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cout << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    exit(1);
  }
  
  int i,numOfPoints;
  infp >> numOfPoints;
  
  ptType x,y;
  for (i=0;i<numOfPoints;i++){
    infp >> x >> y;
    append(x,y);
  }
  infp.close();
  computeProperties();
}

// Write the contour to a file using the Raphael Contour format
template <class ptType,class floatType>
void Curve<ptType,floatType>::writeToFile(vcl_string fileName) {

  if(_numPoints<1) {
    vcl_cout << "Error! Null Contour - No File Written" << vcl_endl;
    return;
  }
  
  vcl_ofstream outfp(fileName.c_str());
  
  outfp << "CONTOUR" << vcl_endl;
  if(_isOpen==true)
    outfp << "OPEN" << vcl_endl;
  else
    outfp << "CLOSE" << vcl_endl;

  outfp << _numPoints << vcl_endl;
  
  for(int i=0;i<_numPoints;i++)
    outfp << point(i).x() << " " << point(i).y() << " ";
  
  outfp.close();  
}

//
// The following functions are used to calculate the 
// curvature extrema of the contour
// 

template <class ptType,class floatType>      
void Curve<ptType,floatType>::computeTangents() {
  
  int size = _numPoints;
  floatType theta = 0;
     
  for (int i=0;i<size;i++) {
    theta = atan2(_ptArray[(i+1)%size].y()-_ptArray[i].y(),
      _ptArray[(i+1)%size].x()-_ptArray[i].x());
    if (theta<0)
      theta +=2*M_PI; 
    _tangent.push_back(theta);
  }
}


//Compute average tangents
template <class ptType,class floatType>
void Curve<ptType,floatType>::computeAveTangents() {
  
  vcl_pair<floatType,floatType> temp;
  int tangent_size_int = static_cast<int>(_tangent.size());
  for (int i=0;i<tangent_size_int;i++) {
    temp.first  = aveTangent(i,                     -1*VICINITY);
    temp.second = aveTangent((i+1)%_tangent.size(),    VICINITY);
    
    _aveTangent.push_back(temp);
  }
}



template <class ptType,class floatType> 
floatType Curve<ptType,floatType>::aveTangent(int start, int count) {
   
  int size=_numPoints;

  if ((start<0)||(start>=size))
    {
      vcl_cout<<" Error:<aveTangent> Parameters out of range "<<vcl_endl;
      return 0;
    }
  
  int     count_mag=abs(count);
  floatType  prev_tangent=0, tangent_sum=0;
  
  vcl_vector<floatType> tangent_list;
  int index         = 0;
  floatType tangent    = 0;
  floatType av_tangent = 0;
  
  floatType arclength  = 0;
  int    tan_count  = 0;
  int    net_tan_count = 0;
  
  while(1) {
    if (count<0)
      index = (start-tan_count-1);
    else
      index = (start+tan_count);
    
    index %=size; 
    if (index<0)
      index+=size;
    
    tangent = _tangent[index];
    tangent = fmod(tangent, 2*M_PI);
    
    if (tangent<0){
      tangent +=2*M_PI;
    }
    tangent_list.push_back(tangent); 
    
    // Need to handle arcLength
    
    if (_arcLength[index]<(DIST_STEP+arclength)) {
      tan_count++;
      arclength = 0;
    }
    else
      arclength +=_arcLength[index];
    
    net_tan_count++;

    if (net_tan_count>=count_mag)
      break;
  }

  vcl_sort(tangent_list.begin(), tangent_list.end());
  
  for (unsigned i=0;i<tangent_list.size();i++) {
    tangent = tangent_list[i];
    if (i==0)
      prev_tangent = tangent;
    
    if (fabs(tangent-prev_tangent)>M_PI) {
      if (tangent>0)
  tangent = tangent-2*M_PI;
      else
  tangent = tangent +2*M_PI;
    }
          
    if (fabs(tangent-prev_tangent)>M_PI) {
      vcl_cout << " Warning : Angle diff > M_PI. Prev " << prev_tangent*(180/M_PI) 
     << " Tangent = " << tangent*(180/M_PI) << vcl_endl;
    }
    
    tangent_sum  += tangent;
    prev_tangent  = tangent;
  }
    
  av_tangent = (tangent_sum/(tangent_list.size()*(1.0)));
  av_tangent = fmod(av_tangent, 2*M_PI);
    
  if (av_tangent<0)
    av_tangent +=2*M_PI;
  
  return (av_tangent);
}
  
template <class ptType,class floatType>
void Curve<ptType,floatType>::findExtrema() {

  floatType diff=0;
  vcl_vector<floatType> angle_diff;
  unsigned i;
  for (i=0;i<_aveTangent.size();i++) {
    diff = (_aveTangent[i].first)-(_aveTangent[i].second);
    
    if (diff<0)
      diff+=2*M_PI;
    
    angle_diff.push_back(diff);
  }

  int prev, next;
  int size = _aveTangent.size();
  for (i=0;i<(_aveTangent.size());i++) {
    prev = i-1;
    
    if (prev<0)
      prev +=size;
    
    next = (i+1)%size;
        
    if ((angle_diff[i]>angle_diff[prev])&&(angle_diff[i]>=angle_diff[next])
  &&(angle_diff[i]>MIN_TAN_TURN)) {
      if (angle_diff[i]<M_PI)
  _extrema.push_back(i);
    }
       
    if ((angle_diff[i]<angle_diff[prev])&&(angle_diff[i]<=angle_diff[next])) {
      if ((angle_diff[i]>(M_PI))&&((angle_diff[i]<(2*M_PI-MIN_TAN_TURN))))
      _extrema.push_back(i); 
    } 
  }
}


//Creates a list of the indices of corners in _corners.
template <class ptType,class floatType>
void Curve<ptType,floatType>::findCorners() {

  _corners.clear();
  for (int i =0; i < numExtrema(); i++){
    int index = coarseRef(extrema(i));
    _corners.push_back(index);
    }

}
//
// Functions for generating resampled versions of the contour
//



//Resamples by moving along the contour keeping track of the arclength
//traversed, sampling at every point where the traversed arclength
//is greater than dS.
template <class ptType,class floatType> 
void Curve<ptType,floatType>::resample(floatType dS) {
 
  floatType length=0;
  vcl_vector<PuzPoint<ptType> > new_points;

  new_points.push_back(point(0));

  for(int i=1;i<_numPoints;i++) {
    length+=_arcLength[i];
    if(length>=dS) {
      new_points.push_back(point(i));
      length=0;
    }
  }
     
  empty();
  for(unsigned i=0;i<new_points.size();i++)
    append(new_points[i]);
  
  computeProperties();
}


//Does a coarse resampling
template <class ptType,class floatType> 
void Curve<ptType,floatType>::coarseResample(floatType dS) {

  int i,j,c,cp,num_seg;
  floatType length,sub_len,dS2;
  Curve<ptType,floatType> temp;

  computeExtrema();
 
  // shift points so that the first point is the first extrema
  vcl_vector<PuzPoint<ptType> > temp_pts;
  int temp_size = _numPoints;

  for(i=_extrema[0];i<_numPoints;i++)
    temp_pts.push_back(point(i));
  for(i=0;i<_extrema[0];i++)
    temp_pts.push_back(point(i));
  empty();
  for(i=0;i<temp_size;i++)
    append(temp_pts[i]);
  append(point(0));

  _offset=_extrema[0];
  int extrema_size_int = static_cast<int>(_extrema.size());
  for(i=0;i<extrema_size_int;i++)
    _extrema[i]-=_offset;
  _extrema.push_back(_numPoints-1);
  computeProperties();

  temp=*this;
  temp.smooth(dS); 

  // Resample
  vcl_pair<PuzPoint<ptType>,int> cPoint;
  _coarseNumPoints=0;
  
  extrema_size_int = static_cast<int>(_extrema.size());
  for(i=1;i<extrema_size_int;i++) {

    c=_extrema[i];
    cp=_extrema[i-1];
   
    cPoint.first=temp.point(cp);
    cPoint.second=cp;
    _coarsePtArray.push_back(cPoint);
    _coarseNumPoints++;
    _extrema[i-1]=_coarseNumPoints-1;
  
    length=temp.totalLength(cp,c);
    if(length>0) {
      num_seg=static_cast<int>(ceil(length/dS));    
      dS2=length/(floatType)num_seg;
    
      if(num_seg>1) {
  sub_len=0;
  for(j=cp+1;j<c;j++) {
    sub_len+=temp.arcLength(j);
    if(sub_len>=dS2) {
      cPoint.first=temp.point(j);
      cPoint.second=j;
      _coarsePtArray.push_back(cPoint); 
      _coarseNumPoints++;
      sub_len-=dS2;
    }
  }
      }
    }
  }
  
  _ptArray.pop_back();
  _extrema.pop_back();
  _numPoints--;
  computeProperties();
}

template <class ptType,class floatType> 
void Curve<ptType,floatType>::becomeCoarse() {
 
  int i;
  empty();

  for(i=0;i<_coarseNumPoints;i++){
    append(_coarsePtArray[i].first);
  }
  _coarseNumPoints=0;
  computeProperties();
}



#endif

