#ifndef __TBS_CURVE_DEF__
#define __TBS_CURVE_DEF__
#include <string>
#include <cmath>
#include <utility>
#include <vgl/vgl_point_2d.h>
#include <dbctrk/dbctrk_utils.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>



class Curve
{
 public:
  //Constructors
  Curve();
  Curve(const Curve &rhs);
  // Destructor
  ~Curve(){};

  // Read data from Raphael contour format file
 void readDataFromFile(std::string fileName);
 void readDataFromVector(std::vector<std::pair<double,double> > v);
 void readDataFromVector(std::vector<vgl_point_2d<double> > v);
  // Assignment operator
  Curve& operator=(const Curve &rhs);

  // Print all points
  void printElems();
  int numPoints(){return numPoints_;}
  double length(){return length_;}
  double totalCurvature(){return totalCurvature_;}
  double totalAngleChange(){return totalAngleChange_;}
  bool isOpen(){return isOpen_;}
  // Access different members based on an index
  vgl_point_2d <double>  point(int index){return ptArray_[index];}
  double x(int index){return ptArray_[index].x();};
  double y(int index){return ptArray_[index].y();};
  double arcLength(int index){return arcLength_[index];};
  double normArcLength(int index){return normArcLength_[index];};
  double curvature(int index){return curvature_[index];};
  double Tcurvature(){return std::fabs(totalCurvature_);}
  double angle(int index){return angle_[index];};
  double dx(int index){return dx_[index];};
  double dy(int index){return dy_[index];};
  std::vector<vgl_point_2d <double> >  pointarray(){return ptArray_;}


  //Functions to insert to the point list
  void append(vgl_point_2d<double> pt);
  void append(double x, double y);
  //Functions to compute the DPCosts
  void stretchCost(int i, int ip, double &a){
    a=arcLength_[i]-arcLength_[ip];
  };
  void bendCost(int i, int ip, double &a){
  a=utils::angleDiff(angle_[i],angle_[ip]); // defined in dbctrk/utils.cxx
  };

  void computeProperties();
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;
  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(std::string const&) const;
 protected:
  //Data
  std::vector< vgl_point_2d<double> > ptArray_;
  std::vector<double> arcLength_;
  std::vector<double> normArcLength_;
  std::vector<double> dx_;
  std::vector<double> dy_;
  std::vector<double> curvature_;
  std::vector<double> angle_;
  int numPoints_;
  double length_;
  double totalCurvature_;
  double totalAngleChange_;
  bool isOpen_; // true - open, false - closed

  //Computing the properties of the curve.
  void computeArcLength();
  void computeDerivatives();
  void computeCurvatures();
  void computeAngles();
};


//: Binary save Curve to stream.
void vsl_b_write(vsl_b_ostream & os, const Curve * p);
//: Binary load Curve to stream
void vsl_b_read(vsl_b_istream &is,Curve * &p);

#endif
