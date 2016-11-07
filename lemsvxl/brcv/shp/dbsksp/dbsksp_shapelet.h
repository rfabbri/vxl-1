// This is shp/dbsksp/dbsksp_shapelet.h
#ifndef dbsksp_shapelet_h_
#define dbsksp_shapelet_h_

//:
// \file
// \brief A class for the geometry of a shape fragment, or a shapelet
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Nov 6, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector_fixed.h>
#include <dbgl/algo/dbgl_conic_arc.h>
#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

class dbsksp_xshock_node_descriptor;

//: A class to represent a shock fragment, i.e. both sides of the boundary are
// circular arcs
class dbsksp_shapelet : public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ----------------------------------
  //: constructor
  dbsksp_shapelet(){};

  //
  dbsksp_shapelet(double x0, double y0, double theta0, double r0, 
    double phi0, double phi1, double m0, double len)
  {
    this->set(x0, y0, theta0, r0, phi0, phi1, m0, len);
  }

  //: construction from a length-8 vector
  dbsksp_shapelet(const vnl_vector<double >& params) : params_(params){}

  

  //: destructor
  virtual ~dbsksp_shapelet(){};

  
  // DATA ACCESS --------------------------------------------------------------
  
  //: Get internal params
  double x0() const {return this->params_[0]; }
  double y0() const {return this->params_[1]; }
  double theta0() const {return params_[2]; }
  double r0() const {return params_[3]; }
  double phi0() const {return this->params_[4]; }
  double phi1() const {return this->params_[5]; }
  double m0() const {return this->params_[6]; }
  double len() const {return this->params_[7]; }

  vnl_vector<double > params() const
  {
    return this->params_.as_vector(); 
  }

  //: Set the member variables
  void set(double x0, double y0, double theta0, double r0, 
    double phi0, double phi1, double m0, double len)
  {
    this->params_[0] = x0;
    this->params_[1] = y0;
    this->params_[2] = theta0;
    this->params_[3] = r0;
    this->params_[4] = phi0;
    this->params_[5] = phi1;
    this->params_[6] = m0;
    this->params_[7] = len;  
  }

  //: 
  void set(const vnl_vector<double >& params)
  {
    assert(params.size() == this->params_.size());
    this->params_.copy_in(params.data_block());
  }

  //: set the shapelet's parameters from starting xnode and (len, m, phi1)
  bool set_from(const vgl_point_2d<double >& pt0, double r0, 
    const vgl_vector_2d<double >& tangent0, double phi0, double m0, 
    double len, double phi1);

  // Shock geometry -----------------------------------------------------------

  //: coordinate of starting node
  vgl_point_2d<double > start() const
  {
    return vgl_point_2d<double >(this->x0(), this->y0()); 
  }

  //: coordinate of ending node
  vgl_point_2d<double > end() const
  {
    return vgl_point_2d<double >(this->x0()+this->len()*vcl_cos(this->theta0()),
      this->y0() + this->len()*vcl_sin(this->theta0()));
  }

  //: direction of the chord
  vgl_vector_2d<double > chord_dir() const
  {
    return vgl_vector_2d<double >(vcl_cos(this->theta0()), vcl_sin(this->theta0()));  }

  //:
  double chord_length() const {return this->len(); }

  //: tangent at the start and end point
  vgl_vector_2d<double> tangent_start() const;
  vgl_vector_2d<double > tangent_end() const;

  //:
  double phi_start() const {return this->phi0(); }

  //:
  double phi_end() const { return this->params_[5]; }

  //:
  double alpha_start() const
  {
    return vcl_asin( this->m0() * vcl_sin(this->phi0()) ); 
  }

  //:
  double alpha_end() const
  {
    return vcl_asin( -this->m0() * vcl_sin(this->phi_end()) );
  }

  //: normalized radius increment
  // ( = radius increment / chord_length)
  double normalized_radius_increment() const;

  //: difference in radius between the end node and start node
  double radius_increment() const;

  //: radius at starting node
  double radius_start() const {return this->r0(); }

  //: radius at ending node
  double radius_end() const{ return this->r0() + this->radius_increment(); }

  //: parameter m
  double m_start() const {return this->m0(); }
  double m_end() const {return -this->m0(); }

  //: Return the start one boundary arc: 0: left boundary, 1: right boundary
  vgl_point_2d<double > bnd_start(int side) const;

  //: Return the end one boundary arc: 0: left boundary, 1: right boundary
  vgl_point_2d<double > bnd_end(int side) const;
  
  //: Return boundary arcs: 0: left boundary, 1: right boundary
  dbgl_circ_arc bnd_arc(int side) const;

  //: Return left boundary
  dbgl_circ_arc bnd_arc_left() const {return this->bnd_arc(0); }

  //: Return right boundary
  dbgl_circ_arc bnd_arc_right() const {return this->bnd_arc(1); }

  //: Return the shock curve (conic)
  dbgl_conic_arc shock_geom() const;

  //: Return area bounded by the shapelet
  double area() const;

  //: Return the area of the ``left'' region, bounded by the left boundary,
  // shock chord, and the contact shocks
  double area_left() const;

  //: Return the area of the ``right'' region, bounded by the left boundary,
  // shock chord, and the contact shocks
  double area_right() const;

  //: Return the area of the front arc of the shapelet
  double area_front_arc() const;

  //: Return the area of the rear arc of the shapelet
  double area_rear_arc() const;


  //: Return angle phi at a shock point
  double phi_at(double t) const;

  //: Return radius at a shock point
  double radius_at(double t) const;


  //: Return the quadrilateral connecting the boundary end points
  vsol_polygon_2d_sptr bounding_quad() const;


  //: Compute extrinsic shock samples given a list of parameter t, t \in [0, 1]
  // Return false if computation fails, e.g. t < 0 or t > 1
  bool compute_xshock_samples(const vcl_vector<double >& ts,
    vcl_vector<dbsksp_xshock_node_descriptor >& list_xdesc);



  
  ////: Compute point samples of the shapelet, given sampling rate "ds"
  //vcl_vector<vgl_point_2d<double > > compute_samples(double ds) const;

  ////: Compute point-tangent samples of the shapelet, given sampling rate "ds"
  //void compute_samples(double ds, vcl_vector<vgl_point_2d<double > >& pts,
  //  vcl_vector<vgl_vector_2d<double > >& tangents) const;

  
  // UTILITY FUNCTIONS---------------------------------------------------------

  //: Return a shapelet of reversed direction
  dbsksp_shapelet_sptr reversed_dir() const;

  //: Create a scaled-up version of ``this'' shapelet. When
  // scale > 1, the resulting shapelet is bigger
  // scale < 1, the resulting shapelet is smaller
  dbsksp_shapelet_sptr scale_up(double scale) const;

  //: Return the front terminal shapelet (A_infty shock point)
  dbsksp_shapelet_sptr terminal_shapelet_front() const;

  //: Return the front terminal shapelet (A_infty shock point)
  dbsksp_shapelet_sptr terminal_shapelet_rear() const;

  //: Check legality of this shapelet
  bool is_legal() const;

  //: illegal cost. >0 is good. < 0 is illegal
  vnl_vector<double > legality_measure() const;
  

  // MISCELLANEOUS-------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os);

  
  // MEMBER VARIABLES ---------------------------------------------------------
protected:

  vnl_vector_fixed<double, 8> params_;
  ////
  //double x0_;
  //double y0_;
  //double theta0_;
  //double r0_;
  //double phi0_;
  //double phi1_;
  //double m0_;
  //double len_;
  //
};



//: Create a shapelet that is a terminal fragment
// All parameters are for the start node (the non-trivial node)
dbsksp_shapelet_sptr dbsksp_new_terminal_shapelet(double x0, double y0, double theta0, 
                                                  double r0, double phi0);



// ============================================================================
// dbsksp_twoshapelet
// ============================================================================

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_twoshapelet : public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ----------------------------------
  //: constructor
  dbsksp_twoshapelet(): params_(vnl_vector<double >(11) ){};

  //
  dbsksp_twoshapelet(double x0, double y0, double theta0, double r0, 
    double phi0, double m0, double len0, 
    double phi1, double m1, double len1, 
    double phi2 ) : params_( vnl_vector<double >(11) )
  {
    this->set( x0,  y0,  theta0,  r0, 
     phi0,  m0,  len0, 
     phi1,  m1,  len1, 
     phi2 );
  }

  //: construction from a length-11 vector
  dbsksp_twoshapelet(const vnl_vector<double >& params) : params_(params){}


  //: Construct a twoshapelet by breaking an existing shapelets into two pieces
  dbsksp_twoshapelet(const dbsksp_shapelet_sptr& s, double t = 0.5);

  //: destructor
  virtual ~dbsksp_twoshapelet(){};

  
  // DATA ACCESS --------------------------------------------------------------
  
  //: Get internal params
  double x0() const {return this->params_[0]; }
  double y0() const {return this->params_[1]; }
  double theta0() const {return this->params_[2]; }
  double r0() const {return this->params_[3]; }
  double phi0() const {return this->params_[4]; }
  double m0() const {return this->params_[5]; }
  double len0() const {return this->params_[6]; }
  double phi1() const {return this->params_[7]; }
  double m1() const {return this->params_[8]; }
  double len1() const {return this->params_[9]; }
  double phi2() const {return this->params_[10]; }

  //: Set internal params
  void set_x0(double x0) {this->params_[0] = x0; }
  void set_y0(double y0) {this->params_[1] = y0; }
  void set_theta0(double theta0) {this->params_[2] = theta0; }
  void set_r0(double r0) {this->params_[3] = r0; }
  void set_phi0(double phi0) {this->params_[4] = phi0; }
  void set_m0(double m0) {this->params_[5] = m0; }
  void set_len0(double len0) {this->params_[6] = len0; }
  void set_phi1(double phi1) {this->params_[7] = phi1; }
  void set_m1(double m1) {this->params_[8] = m1; }
  void set_len1(double len1) {this->params_[9] = len1; }
  void set_phi2(double phi2) {this->params_[10] = phi2; }


  //: Get all params as a vector
  vnl_vector<double > params() const
  {
    return this->params_; 
  }

  //: Set internal parameters
  void set(double x0, double y0, double theta0, double r0, 
    double phi0, double m0, double len0, 
    double phi1, double m1, double len1, 
    double phi2 )
  {
    this->params_[0] = x0;
    this->params_[1] = y0;
    this->params_[2] = theta0;
    this->params_[3] = r0;
    this->params_[4] = phi0;
    this->params_[5] = m0;
    this->params_[6] = len0;
    this->params_[7] = phi1;
    this->params_[8] = m1;
    this->params_[9] = len1;
    this->params_[10] = phi2;
  }

  void set(const vnl_vector<double >& params)
  {
    assert(params.size() == this->params_.size());
    this->params_ = params;
  }

  // ---------------- Geometry --------------------------------
  //: Return the starting shapelet
  dbsksp_shapelet_sptr shapelet_start() const;

  //: Return the ending shapelet
  dbsksp_shapelet_sptr shapelet_end() const;

  //: Return one of the two shapelets
  // shapelet(0) == starting shapelet
  // shapelet(1) == ending shapelet
  dbsksp_shapelet_sptr shapelet(int i) const;

  // --------------------- UTILITY FUNCTIONS ---------------------

  //: Check legality of this shapelet
  bool is_legal() const;

  //: Return a twoshapelet of reversed direction
  dbsksp_twoshapelet_sptr reversed_dir() const;
  

  // --------------------- MISCELLANEOUS ---------------------

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os);

  
  // --------------------- MEMBER VARIABLES ---------------------
protected:

  //: Internal parameters, ordered as follows:
  //double x0_;
  //double y0_;
  //double theta0_;
  //double r0_;
  //double phi0_;
  //double m0_;
  //double len0_;
  //double phi1_;
  //double m1_;
  //double len1_;
  //double phi2_;
  vnl_vector_fixed<double, 11> params_;
  //vnl_vector<double > params_;
};









// ============================================================================
// dbsksp_terminal_shapelet
// ============================================================================

//: A class to represent a terminal shock fragment
class dbsksp_terminal_shapelet : public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS -------------------------------------------------
  
  //: constructor
  dbsksp_terminal_shapelet(){};

  //: constructor
  dbsksp_terminal_shapelet(double x0, double y0, double theta0, double r0, 
    double phi0)
  { this->set(x0, y0, theta0, r0, phi0); }

  //: construction from a length-5 vector
  dbsksp_terminal_shapelet(const vnl_vector<double >& params) : params_(params){}

  //: destructor
  virtual ~dbsksp_terminal_shapelet(){};

  
  // DATA ACCESS --------------------------------------------------------------
  
  //: Get internal params
  double x0() const {return this->params_[0]; }
  double y0() const {return this->params_[1]; }
  double theta0() const {return params_[2]; }
  double r0() const {return params_[3]; }
  double phi0() const {return this->params_[4]; }
  vnl_vector<double > params() const
  { return this->params_.as_vector(); }

  //: Set the member variables
  void set(double x0, double y0, double theta0, double r0, 
    double phi0);

  void set(const vnl_vector<double >& params);
  

  // Shock geometry -----------------------------------------------------------

  //: coordinate of the shock point
  vgl_point_2d<double > shock_point() const
  { return vgl_point_2d<double >(this->x0(), this->y0()); }

  //: Tangent at the shock point
  vgl_vector_2d<double > shock_dir() const
  { return vgl_vector_2d<double >(vcl_cos(this->theta0()), vcl_sin(this->theta0()));}
    
  //: Return boundary arcs: 0: left boundary, 1: right boundary
  dbgl_circ_arc bnd_arc(int i) const;

  //: Return left boundary
  dbgl_circ_arc bnd_arc_left() const {return this->bnd_arc(0); }

  //: Return right boundary
  dbgl_circ_arc bnd_arc_right() const {return this->bnd_arc(1); }

  //: Return area bounded by the shapelet
  double area() const;

  
  // UTILITY FUNCTIONS --------------------------------------------------------

  //: Check legality of this shapelet
  bool is_legal() const;

  //: illegal cost. >0 is good. < 0 is illegal
  vnl_vector<double > legality_measure() const;
  

  // MISCELLANEOUS ------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  void print(vcl_ostream & os);

  
  // MEMBER VARIABLES ---------------------------------------------------------
protected:

  vnl_vector_fixed<double, 5> params_;
  //double x0_;
  //double y0_;
  //double theta0_;
  //double r0_;
  //double phi0_;
};











#endif // shp/dbsksp/dbsksp_shapelet.h


