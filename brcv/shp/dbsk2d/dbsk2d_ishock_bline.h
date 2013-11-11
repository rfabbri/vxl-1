// This is brcv/shp/dbsk2d/dbsk2d_ishock_bline.h
#ifndef dbsk2d_ishock_bline_h_
#define dbsk2d_ishock_bline_h_
//:
// \file
// \brief Boundary line class for intrinsic shock boundary
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//   Amir Tamrakar 05/02/2005    Merged with dbsk2d_ishock_bline since there was no 
//                               need to keep the heirarchy
//   Nhon Trinh 6/22/2005        Derive this class from dbsk2d_ishock_bcurve, which
//                               is the parent for both bline and barc.
//   Amir Tamrakar 01/06/2006     Removed a bunch of defunct methods from this class
//
// \endverbatim


#include <dbsk2d/dbsk2d_ishock_bcurve.h>

//: Boundary line class for intrinsic shock boundary
// Boundary lines are special geometry elements. Each boundary line
// segment used for shock computation consists of four subelements 
// (2 end points and 2 half lines): The endpoints are dbsk2d_ishock_bpoints and 
// the two half lines are dbsk2d_ishock_bline. By half lines, we mean lines which 
// only exert their influence on the half plane on one side of it. 
class dbsk2d_ishock_bline : public dbsk2d_ishock_bcurve
{
protected:
  //: Direction vector of this line
  VECTOR_TYPE _u; 
  
  //: Normal vector to this line, =u+vnl_math::pi_over_2
  VECTOR_TYPE _n; 
  
  //: Length of this line
  double  _l; 

public:

  //: Constructor
  dbsk2d_ishock_bline(dbsk2d_ishock_bpoint* startpt, dbsk2d_ishock_bpoint* endpt, int id=-1, bool bGUI=false);

  //: Destructor
  virtual ~dbsk2d_ishock_bline(){};

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a () const { return vcl_string("dbsk2d_ishock_bline"); }

  double l() const { return _l; }
  
  //: Return the length of the boundary curve segment 
  // inherited from bcurve.
  virtual double len() const { return this->_l;};

  //: compute local copies of commonly used parameters
  virtual void compute_cached_params();

  //: Compute the bounding box of this bline
  // inherited from bcurve
  virtual void compute_bounding_box(vbl_bounding_box<double, 2 >& box ) const;

  VECTOR_TYPE u() const { return _u; }
  VECTOR_TYPE n() const { return _n; }

  //: Return the other half line
  dbsk2d_ishock_bline* twinLine()
  { return static_cast<dbsk2d_ishock_bline*>(this->twin_bcurve()); }

  const dbsk2d_ishock_bline* twinLine() const
  { return static_cast<const dbsk2d_ishock_bline*>(this->twin_bcurve()); }

  //: set the other half line
  void set_twinLine(dbsk2d_ishock_bline* twinLine)
  { this->set_twin_bcurve((dbsk2d_ishock_bcurve*)twinLine); }

  //boundary parameter functions
  virtual double min_eta() const { return 0; }
  virtual double max_eta() const { return _l; }

  //: Return tangent angle [0, 2pi) of curve given arc-length 
  virtual VECTOR_TYPE tangent_at(double s) const {return this->_u;}
  virtual VECTOR_TYPE reverse_tangent_at(double s) const {return angle0To2Pi(_u+vnl_math::pi);}

  virtual void reconnect(dbsk2d_ishock_bpoint* oldPt, dbsk2d_ishock_bpoint* newPt);

  //: Return information about the object
  virtual void getInfo (vcl_ostream& ostrm);

  //: extrinsic points for drawing purposes
  virtual void compute_extrinsic_locus();

  /*virtual*/ int get_contour_id();

};

#endif // dbsk2d_ishock_bline_h_
