// This is brcv/shp/dbsk2d/dbsk2d_ishock_barc.h
#ifndef dbsk2d_ishock_barc_h_
#define dbsk2d_ishock_barc_h_
//:
// \file
// \brief Boundary arc class for intrinsic shock boundary
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//   Amir Tamrakar 05/02/2005    Merged with dbsk2d_ishock_barc since there was no 
//                               need to keep the heirarchy
//   Nhon Trinh  06/22/2005      Derive this class from dbsk2d_ishock_bcurve
//                               instead of dbsk2d_ishock_belm
//   Amir Tamrakar 01/06/2006     Removed a bunch of defunct methods from this class
//
// \endverbatim


#include <dbsk2d/dbsk2d_ishock_bcurve.h>

//: Boundary arc class for intrinsic shock boundary
// Like the boundary line element (dbsk2d_ishock_bline), this element is also
// a half element. An arc segment consists of 2 end points and 2 of these
// arc elements. Its area of influence is not half the plane, just the 
// sectors of a circle.
class dbsk2d_ishock_barc : public dbsk2d_ishock_bcurve
{
protected:
  //: Center of the circle
  vgl_point_2d<double>  _center;

  //: Radius of the arc
  double _R;

  //: Direction of the arc (CW:+1 (outer arc), CCW:-1 (inner arc))
  ARC_NUD _nud; 
  
  //: \todo what is this for?
  bool        _bAcross;

  //: Useful vectors
  VECTOR_TYPE _StartVector;
  VECTOR_TYPE _EndVector;
  VECTOR_TYPE _InTangent;
  VECTOR_TYPE _OutTangent;
  VECTOR_TYPE _CCWStartVector;
  VECTOR_TYPE _CCWEndVector;
  
public:
  //: Constructor
  dbsk2d_ishock_barc(dbsk2d_ishock_bpoint* startpt, dbsk2d_ishock_bpoint* endpt, int id, bool bGUI,
                     vgl_point_2d<double> center, double r, ARC_NUD nud);

  //: Destructor
  virtual ~dbsk2d_ishock_barc(){}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a () const { return vcl_string("dbsk2d_ishock_barc"); }

  //: is this an inner arc? (inner arcs are always CCW)
  bool is_inner_arc() { return _nud==-1; }

  //: Return the center of the circle
  vgl_point_2d<double> center() const { return _center; }

  double R() const { return _R; }
  ARC_NUD nud() const { return _nud; }

  double curvature() const {return (_nud==-1)? (1/_R) : (-1/_R); }; 

  //: Return the other half arc segment
  virtual dbsk2d_ishock_barc*  twinArc() 
  { return (dbsk2d_ishock_barc*)(this->twin_bcurve());}

  //: Set pointer to the other half arc segment
  virtual void set_twinArc(dbsk2d_ishock_barc* twinArc) 
  { this->set_twin_bcurve((dbsk2d_ishock_bcurve*)twinArc); };
  
  //: length of the arc
  double l() const { return _R*CCW(_CCWStartVector,_CCWEndVector); }

  //: Return the length of the boundary curve segment 
  // inherited from dbsk2d_ishock_bcurve
  virtual double len() const { return this->l();};

  //: compute local copies of commonly used parameters
  virtual void compute_cached_params();

  //: Compute the bounding box of this arc
  virtual void compute_bounding_box(vbl_bounding_box<double, 2 >& box ) const;

  //: return the range of eta values for the wavefronts from the arc
  virtual double min_eta() const { return 0; }
  virtual double max_eta() const { return CCW(_CCWStartVector,_CCWEndVector); }

  //: convert a vector to an eta value
  double vec_to_eta(VECTOR_TYPE vec);

  //: convert an eta value to a vector
  VECTOR_TYPE eta_to_vec(double eta);

  //: tangent going into the arc at the startpt, etc.
  VECTOR_TYPE InTangentAtStartPt() const 
  { return _InTangent; }
  
  VECTOR_TYPE OutTangentAtStartPt() const 
  { return angle0To2Pi (_InTangent + vnl_math::pi); }
  
  VECTOR_TYPE InTangentAtEndPt() const 
  { return angle0To2Pi (_OutTangent + vnl_math::pi); }
  
  VECTOR_TYPE OutTangentAtEndPt() const 
  { return _OutTangent; }

  VECTOR_TYPE StartVector()    const { return _StartVector; }
  VECTOR_TYPE EndVector()      const { return _EndVector; }
  VECTOR_TYPE CCWStartVector() const { return _CCWStartVector; }
  VECTOR_TYPE CCWEndVector()   const { return _CCWEndVector; }

  VECTOR_TYPE StartNormalVector() const { return (_nud==1) ? _StartVector : angle0To2Pi(_StartVector + vnl_math::pi); }
  VECTOR_TYPE EndNormalVector()   const { return (_nud==1) ? _EndVector   : angle0To2Pi(_EndVector   + vnl_math::pi);; }

  vgl_point_2d<double>  CCWStartPoint() const 
  { return (_nud==1) ? this->end() : this->start(); }

  vgl_point_2d<double>  CCWEndPoint() const
  { return (_nud==1) ? this->start() : this->end(); }
  
  //: Return tangent angle [0, 2pi) of curve given arc-length
  // \TODO rewrite this
  virtual VECTOR_TYPE tangent_at(double s) const;
  virtual VECTOR_TYPE reverse_tangent_at(double s) const;

  virtual void reconnect(dbsk2d_ishock_bpoint* oldPt, dbsk2d_ishock_bpoint* newPt);

  virtual void getInfo (vcl_ostream& ostrm);
  virtual void compute_extrinsic_locus();
};

#endif // dbsk2d_ishock_barc_h_
