// This is brcv/shp/dbsk2d/dbsk2d_ishock_bpoint.h
#ifndef dbsk2d_ishock_bpoint_h_
#define dbsk2d_ishock_bpoint_h_
//:
// \file
// \brief Boundary point class for intrinsic shock boundary
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//   Amir Tamrakar 05/02/2005    Merged with dbsk2d_ishock_bpoint since there 
//                               was no need to keep the heirarchy
//   Nhon Trinh    07/21/2005    Added pointer back to its container, 
//                               a `dbsk2d_bnd_vertex'
//   Amir Tamrakar 01/06/2006     Removed a bunch of defunct methods from this class
//
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_belm.h>

#define TANGENT_UNDEFINED -100 //For dbsk2d_ishock_bpoint tangent

// forward declaration
class dbsk2d_bnd_vertex;

//: Boundary point class for intrinsic shock boundary
// Boundary points can be either free standing, or at endpoints of line
// and arc segments or at junctions of line and arc segments.
//
// The boundary points are therefore nodes in the boundary graph. Consequently,
// they have the responsibility of maintaining the adjacency list. LinkedBElmList 
// is the attribute holding this information.
//
// - The LinkedBElmList is CCW ordered.  
// - The ordering rules are:
//     -  BLINE: if this is the endpoint it goes first
//     -  BARC:  if this is the startpt, the CCW arc goes first
//               if this is the endpt, the CW arc goes first

class dbsk2d_ishock_bpoint : public dbsk2d_ishock_belm
{
protected:

  //: Extrinsic point
  vgl_point_2d<double> _pt;

  //: Pointer back to the container of `this'
  dbsk2d_bnd_vertex* bnd_vertex_;

  //: reference vector for boundary eta
  VECTOR_TYPE _vref;
  
  //: max eta (important if it is not a singular point)
  double _max_eta;

  //: whether this point is visible to neighboring elements
  // if this point links two colinear line segments then its not visible
  bool _is_visible;

  //: For point tangent pairs 
  // \todo This information should be removed to another class

  //: Tangent direction
  double _dir;

  //: Point (edge) confidence
  double _conf;

public:

  //: List of elements this point is connected to (ordered CCW)
  belm_list LinkedBElmList;

  //: Constructor
  dbsk2d_ishock_bpoint(double x, double y, int id=-1, bool bGUI=true, 
                       double tangent=TANGENT_UNDEFINED, double conf=0.0);

  //: Constructor
  dbsk2d_ishock_bpoint(dbsk2d_ishock_bpoint* old_bp, int new_id);


  //: Destructor
  virtual ~dbsk2d_ishock_bpoint(){}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a () const { return vcl_string("dbsk2d_ishock_bpoint"); };

  //: Return extrinsic point
  vgl_point_2d<double> pt() const { return _pt; }

  //: Set the extrinsic point's coordinate
  void set_pt(double new_x, double new_y);

  //: Return a pointer to `this' bpoint's container
  dbsk2d_bnd_vertex* bnd_vertex(){ return this->bnd_vertex_; }
  const dbsk2d_bnd_vertex* bnd_vertex() const 
  { return this->bnd_vertex_; }

  //: Set the pointer back to `this' bpoint's container
  void set_bnd_vertex( dbsk2d_bnd_vertex* v ) { this->bnd_vertex_ = v; }

  //: return the reference vector
  VECTOR_TYPE vref() const { return _vref; }

  //: set the reference vector
  void set_vref(VECTOR_TYPE vref) { _vref = vref; }

  //: if this point visible?
  bool is_visible() const { return _is_visible; }

  //: set visibility
  void set_visibility(bool state) { _is_visible = state; }

  //: Return tangent
  double tangent() const { return _dir;}
  bool has_a_tangent() const;

  //: set the tangent
  void set_tangent(double tangent){_dir = tangent;}

  //: Return the confidence
  double conf() const { return _conf;}

  //: set the confindence
  void set_conf(double conf) {_conf = conf;}

  virtual dbsk2d_ishock_belm* GUIelm() { return this; }

  virtual vgl_point_2d<double> start() const { return _pt; }
  virtual vgl_point_2d<double> end() const { return _pt; }

  virtual dbsk2d_ishock_bpoint* s_pt() { return this; }
  virtual const dbsk2d_ishock_bpoint* s_pt() const {return this;}

  virtual dbsk2d_ishock_bpoint* e_pt() { return this; }
  virtual const dbsk2d_ishock_bpoint* e_pt() const {return this;}

  //: left contact shock
  dbsk2d_ishock_contact* lContact();

  //: right contact shock
  dbsk2d_ishock_contact* rContact();

  //---------------------------------------------------------
  //Functions for working with the bnd_ishock_map (wavefront map)
  //---------------------------------------------------------
  
  //: add the shock into the shock map at the slot defined by its boundary params
  virtual void add_shock (dbsk2d_ishock_edge* shock);

  //: delete this shock from the shock map
  virtual bool delete_shock (dbsk2d_ishock_edge* sielm);

  virtual double min_eta() const { return 0; }
  virtual double max_eta() const { return _max_eta; }

  void set_max_eta(double max_eta) { _max_eta = max_eta; }

  //: convert from vector to the eta parameter for this point
  double vec_to_eta(VECTOR_TYPE v);

  //: convert from eta to vector for this point
  VECTOR_TYPE eta_to_vec(double eta);

  //: test if a particular parameter angle is still valid or burnt
  //  for a point: the test is always done with an absolute vector
  //  because the etas are not defined until the first shock is created
  virtual bool is_wavefront_alive(double vec, double time);

  //: return the shock at a given eta on the element
  virtual dbsk2d_ishock_edge* get_shock_at(double eta, bool & degen_eta);

  //-----------------------------------------
  //functions for handling the connectivity
  //-----------------------------------------

  //: number of elements this point is connected to.
  int nLinkedElms() const { return LinkedBElmList.size(); }

  bool is_a_free_point() const { return nLinkedElms()==0; }
  bool is_an_end_point() const { return nLinkedElms()==2; }
  bool is_a_junction_point() const { return nLinkedElms()>2; }
  
  //: Connect this point to a line or an arc
  // by putting the element into the appropriate slot of its LinkedBElmList
  virtual void connectTo(dbsk2d_ishock_belm* elm);
  
  //; Disconnect this point from the element
  void disconnectFrom(dbsk2d_ishock_belm* elm);

  //: Return the element that is connected to this point and is to the right of the query element
  // critical function
  dbsk2d_ishock_belm* getElmToTheRightOf(dbsk2d_ishock_belm* elm);

  //: Return the element that is connected to this point and is to the left of the query element
  // critical function
  dbsk2d_ishock_belm* getElmToTheLeftOf(dbsk2d_ishock_belm* elm);

  //-----------------------------------------
  // Preprocessing functions
  //-----------------------------------------

  //: Merge with another boundary point (usually because they are too close to be reliably resolved)
  void mergeWith (dbsk2d_ishock_bpoint* bpt);

  //-----------------------------------------
  // Useful functions
  //-----------------------------------------

  virtual void getInfo (vcl_ostream& ostrm);
  virtual void compute_extrinsic_locus();
};

#endif // dbsk2d_ishock_bpoint_h_
