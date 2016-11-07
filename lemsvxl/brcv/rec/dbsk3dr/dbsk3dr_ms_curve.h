// This is dbsk3dr/dbsk3dr_ms_curve.h
#ifndef dbsk3dr_ms_curve_h_
#define dbsk3dr_ms_curve_h_
//*****************************************************************************
//:
// \file
// \verbatim
//  Modifications
//   2008/07/10 MingChing Chang - Modified from the code of Thomas Sebastian
// \endverbatim
//*****************************************************************************

#include <vgl/vgl_fwd.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_string.h>

#include <dbsk3d/dbsk3d_ms_curve.h>

#define ZERO_TOLERANCE 1E-1

//: General intrinsic curve class

class dbsk3dr_ms_curve
{
 protected:
  //***************************************************************************
  // Data members

  //: Pointer to list of vgl_point_3d<double> smart pointers
  vcl_vector<vgl_point_3d<double>*> *storage_;
  //: First point of the curve
  vgl_point_3d<double>* p0_;
  //: Last point of the curve
  vgl_point_3d<double>* p1_;
  //: Arclength measured from the p0_
  vcl_vector<double> arcLength_;
  //: Arclength of the current segment i to i+1
  vcl_vector<double> s_;
  //: The z angle of the intrinsic curve
  vcl_vector<double> phi_;
  //: First derivative of the z angle of the intrinsic curve
  vcl_vector<double> phis_;
  //: Second derivative of the z angle of the intrinsic curve
  vcl_vector<double> phiss_;
  //: The xy angle of the intrinsic curve
  vcl_vector<double> theta_;
  //: First derivative of the xy angle of the intrinsic curve
  vcl_vector<double> thetas_;
  //: Second derivative of the xy angle of the intrinsic curve
  vcl_vector<double> thetass_;
  //: Turning angle of the intrinsic curve
  vcl_vector<double> angle_;
  //: Curvature of the intrinsic curve
  vcl_vector<double> curvature_;
  //: Torsion of the intrinsic curve
  vcl_vector<double> torsion_;
  //: Total curvature of the intrinsic curve
  double totalCurvature_;
  //: Total angle change of the intrinsic curve
  double totalAngleChange_;

  //: Tangent vector for each point
  vcl_vector<vgl_vector_3d<double>*> Tangent_;
  //: Normal vector for each point
  vcl_vector<vgl_vector_3d<double>*> Normal_;
  //: Binormal vector for each point
  vcl_vector<vgl_vector_3d<double>*> Binormal_;

  vcl_vector<double> radius_;

  bool isOpen_; // true - open, false - closed

 public:
  //***************************************************************************
  // Initialization

  //: Default Constructor
  dbsk3dr_ms_curve();

  //: Constructor from a vcl_vector of points
  dbsk3dr_ms_curve(const vcl_vector<vgl_point_3d<double>*> &new_vertices);
  
  //: Constructor from a dbmsh3d_curve
  dbsk3dr_ms_curve (dbsk3d_ms_curve* MC, const bool flip = false);

  //: Destructor
  virtual ~dbsk3dr_ms_curve();

  //***************************************************************************
  // Access

  bool isOpen(void) { return isOpen_; }
  //: Return the first point of `this';
  virtual vgl_point_3d<double>* p0() const { return p0_; }
  //: Return the last point of `this';
  virtual vgl_point_3d<double>* p1() const { return p1_; }
  //: Is `i' a valid index for the list of vertices ?
  bool valid_index(unsigned int i) const { return i<storage_->size(); }
  //: Return vertex `i'
  vgl_point_3d<double>* vertex(const int i) const {
    assert(valid_index(i));
    return (*storage_)[i];
  }
  //: Return x coord of vertex `i'
  double x (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->x();
  }
  //: Return y coord of vertex `i'
  double y (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->y();
  }
  double z (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->z();
  }

  //: Return the number of vertices
  int size(void) const { return storage_->size(); }
  //: Return the total arclength from vertex `0' to vertex `i'
  double arcLength (const int i) const {
    assert (valid_index(i));
    return arcLength_[i];
  }
  //: Return the total arclength of the current vertex `i-1' to vertex `i'
  double s (const int i) const {
    assert (valid_index(i));
    return s_[i];
  }
  //: Return the normalized arclength from vertex `0' to vertex `i'
  double normArcLength (const int i) const;

  //: Return the length of the intrinsic curve
  virtual double length (void) const { 
    if (arcLength_.empty())
      return 0;
    else
      return arcLength_[arcLength_.size()-1]; 
  }

  //: Return the dx vertex `i-1' to vertex `i'
  double dx (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->x() - (*storage_)[i-1]->x();
  }
  //: Return the dy vertex `i-1' to vertex `i'
  double dy (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->y() - (*storage_)[i-1]->y();
  }
  //: Return the curvature of vertex `i'
  double curvature (const int i) const;
  //: Return the angle of vertex `i'
  double angle (const int i) const;

  //: Return the total curvature of the intrinsic curve
  double totalCurvature (void) { return totalCurvature_; }
  //: Return the total angle change of the intrinsic curve
  double totalAngleChange (void) { return totalAngleChange_; }

  //: Return the radius of vertex `i'
  double radius (const int i) const {
    assert (valid_index(i));
    return radius_[i];
  }

protected:

  // Internal status setting functions

  //: Computing the properties of the curve.
  void computeDerivatives();
  void computeCurvatures();
  void computeArcLength();
  void computeAngles();

public:

  //***************************************************************************
  // Status setting

  void setOpen(bool flag) { isOpen_ = flag; }

  //: Set the first point of the curve
  virtual void set_p0 (const vgl_point_3d<double>* new_p0);
  //: Set the last point of the curve
  virtual void set_p1 (const vgl_point_3d<double>* new_p1);
  //: Compute intrinsic properties.
  //  Note that if you even call the other modifying function with the
  //  flag bRecomputeProperties set to false, remember to call this function
  //  to recompute intrinsic properties of this curve.
  void computeProperties();

  //: Delete all points of the intrinsic curve
  void clear();

  //: Add another point to the end of the intrinsic curve
  void add_vertex (vgl_point_3d<double>* new_p, double r, bool bRecomputeProterties=false);

  //: Add another point to the end of the intrinsic curve
  void add_vertex (double x, double y, double z, double r = 0) {
    vgl_point_3d<double>* newpoint = new vgl_point_3d<double> (x, y, z);
    add_vertex (newpoint, r, false);
  }
  //: Remove one vertex from the intrinsic curve
  void remove_vertex (const int i, bool bRecomputeProterties=false);
  //: Modify one vertex of the intrinsic curve
  void modify_vertex (const int i, double x, double y, double z, bool bRecomputeProterties=false);
  //: Insert one vertex to position `i' of the intrinsic curve
  //  Note that it insert the vertex into `i-1'
  void insert_vertex (int i, double x, double y, double z, bool bRecomputeProterties=false);

  void read_con3_file (vcl_string fileName);

  //: output description to stream
  inline void describe(vcl_ostream &strm, int blanking=0) const {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "<dbsk3dr_ms_curve "
         << "p0 = " << *p0_ << ", p1 = " << *p1_ << " >\n";
  }
};

#endif // dbsk3dr_ms_curve_3d_h_
