// This is brcv/shp/dbsk2d/dbsk2d_ishock_bcurve.h
#ifndef dbsk2d_ishock_bcurve_h_
#define dbsk2d_ishock_bcurve_h_
//:
// \file
// \brief Boundary curve class for intrinsic shock boundary
// This is the parent class for other parametric boundary class such as bline, barc, etc.
// \author Nhon Trinh
// \date 6/21/2005
//
// \verbatim
//  Modifications
//   Nhon Trinh 06/21/2005       Initial version. 
// \endverbatim

#include <vbl/vbl_bounding_box.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>

class dbsk2d_bnd_edge;

//: Boundary curve class for intrinsic shock boundary
// Boundary curve are boundary segment that can be represent parametrically.
// Examples include line, arc. Each boundary curve segment used for shock 
// computation consists of four subelements (2 end points and 2 half curves): 
// The endpoints are dbsk2d_ishock_bpoints and the two half curves are dbsk2d_ishock_bcurve. 
// By half curves, we mean curves which only exert their influence on the half 
// plane on one side of it. 
class dbsk2d_ishock_bcurve : public dbsk2d_ishock_belm
{
  
protected:
  //: The boundary points attached to the ends of this curve
  dbsk2d_ishock_bpoint *start_bpoint_;
  dbsk2d_ishock_bpoint *end_bpoint_;   
  
  //: The other half curve
  dbsk2d_ishock_bcurve *twin_bcurve_;

  //: The topological edge containing `this' curve
  dbsk2d_bnd_edge *bnd_edge_;

public:

  //**********************************************************
  // Constructors/Destructors/Initializers
  //***********************************************************
  
  //: Constructor
  dbsk2d_ishock_bcurve(dbsk2d_ishock_bpoint* startpt, 
    dbsk2d_ishock_bpoint* endpt, 
    int id=-1, 
    bool bGUI=false);

  //: Destructor
  virtual ~dbsk2d_ishock_bcurve();

  
  //**************************************************************
  // Data access
  //**************************************************************
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a () const { return vcl_string("dbsk2d_ishock_bcurve"); }  

  //: starting boundary point of this curve segment. inherited from belm
  virtual dbsk2d_ishock_bpoint* s_pt() { return this->start_bpoint_; }
  virtual const dbsk2d_ishock_bpoint* s_pt() const{return this->start_bpoint_;}

  //: set starting bpoint of this curve segment
  void set_s_pt( dbsk2d_ishock_bpoint* new_s_pt )
  { this->start_bpoint_ = new_s_pt; }
  
  //: ending boundary point of this curve segment. inherited from belm
  virtual dbsk2d_ishock_bpoint* e_pt() { return this->end_bpoint_; }
  virtual const dbsk2d_ishock_bpoint* e_pt() const{return this->start_bpoint_;}


  //: set ending bpoint of this curve segment
  void set_e_pt( dbsk2d_ishock_bpoint* new_e_pt )
  { this->end_bpoint_ = new_e_pt; }

  //: extrinsic starting point of `this' curve segment. inherited from belm
  virtual vgl_point_2d<double> start() const 
  { return this->start_bpoint_->pt(); }
  
  //: extrinsic ending point of `this' curve segment. inherited from belm
  virtual vgl_point_2d<double> end() const 
  { return this->end_bpoint_->pt(); }

  //: Return itself if it is a GUI element, else, return its twin.
  virtual dbsk2d_ishock_belm* GUIelm();
  
  //: Return the twin dbsk2d_ishock_bcurve of `this'
  dbsk2d_ishock_bcurve* twin_bcurve() { return this->twin_bcurve_; }
  const dbsk2d_ishock_bcurve* twin_bcurve() const { return this->twin_bcurve_;}


  //: Set the twin dbsk2d_ishock_bcurve of `this'
  // Return false if setting fails.
  bool set_twin_bcurve(dbsk2d_ishock_bcurve* other);


  //: Return true if `this' and `other' are twin of each other
  bool is_twin_bcurve_with( const dbsk2d_ishock_bcurve* other) const;

  //: left contact shock
  dbsk2d_ishock_contact* lContact();

  //: right contact shock
  dbsk2d_ishock_contact* rContact();

  //: left-most shock
  dbsk2d_ishock_edge* leftmost_shock();

  //: right-most shock
  dbsk2d_ishock_edge* rightmost_shock();
  
  //: Return the topological edge containing `this' curve
  dbsk2d_bnd_edge * bnd_edge() { return this->bnd_edge_; }

  //: Set the topological edge containing `this' curve
  void set_bnd_edge(dbsk2d_bnd_edge* bnd_edge){ this->bnd_edge_ = bnd_edge; }

  //********************************************************
  // FUNCTIONS TO BE REDEFINED BY DERIVED CLASS
  //********************************************************

  // compute local copies of commonly used parameters
  virtual void compute_cached_params(){}

  //: Return the length of the boundary curve segment 
  virtual double len() const = 0;

  //: Return tangent angle [0, 2pi) of curve given arc-length
  virtual VECTOR_TYPE tangent_at(double s) const =0;
  virtual VECTOR_TYPE reverse_tangent_at(double s) const =0;

  //: Compute the bounding box of this curve
  virtual void compute_bounding_box(vbl_bounding_box<double, 2 >& box ) const = 0;

};

#endif // dbsk2d_ishock_bcurve_h_
