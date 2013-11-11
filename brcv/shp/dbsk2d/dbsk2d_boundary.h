// This is brcv/shp/dbsk2d/dbsk2d_boundary.h
#ifndef dbsk2d_boundary_h_
#define dbsk2d_boundary_h_
//:
// \file
// \brief Shock boundary class
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 06/23/2005
//
// 
// \verbatim
//  Modifications
//   Nhon Trinh    06/23/2005    Initial version.
//                               Modified from dbsk2d_ishock_boundary.h
// \endverbatim


#include <vbl/vbl_array_2d.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbsol/dbsol_interp_curve_2d_sptr.h>

#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <dbsk2d/dbsk2d_bnd_cell.h>
#include <dbsk2d/dbsk2d_bnd_cell_sptr.h>


//: Some useful type definitions
typedef vcl_list<dbsk2d_bnd_contour_sptr > bnd_contour_list;
typedef vcl_list<dbsk2d_bnd_edge_sptr > bnd_edge_list;
typedef vcl_list<dbsk2d_bnd_vertex_sptr > bnd_vertex_list;

typedef vcl_vector<dbsk2d_bnd_contour_sptr > bnd_contour_vector;
typedef vcl_vector<dbsk2d_bnd_edge_sptr > bnd_edge_vector;
typedef vcl_vector<dbsk2d_bnd_vertex_sptr > bnd_vertex_vector;



//: shock boundary class
//
// This class store a list of boundary elements.
// It handles the file I/O of these boundary elements and is also
// responsible for insuring that the boundary elements form a valid
// planar topology (this is not strictly checked at the moment)
// Since the boundary elements are linked to the shocks that they
// form, add and delete operations to modify the shock graph too.
//
// \todo The boundary class is not treated like a graph even though 
// it needs to be topologically correct. That is because there can exist
// free points and line segments that are not connected to one another.
// It is better suited to be modeled as a vtol object.
class dbsk2d_boundary : public vsol_spatial_object_2d
{
protected:
  //********************************************
  // DATA MEMBERS
  //********************************************

  //: The next available unique id for a boundary element
  mutable int  _nextAvailableID;

  //: list of contours in the boundary that are already preprocessed
  bnd_contour_list preproc_contours_;

  //: list of contours in the boundary that need preprocessing
  bnd_contour_list scratch_contours_;

  //: intrinsic shock graph formed by 'this' dbsk2d_boundary object
  dbsk2d_ishock_graph_sptr ishock_graph_;

  //: shock graph formed by `this' dbsk2d_boundary object
  dbsk2d_shock_graph_sptr shock_graph_;

  //: a list of bnd_edges in the boundary
  mutable vcl_list<dbsk2d_bnd_edge_sptr > all_edges_;

  //: a list of belm in the boundary. This list is for fast accessing
  // purpose only and is created from 
  // preproc_contours and scratch_contours_
  mutable vcl_vector<dbsk2d_ishock_belm* > belm_list_;

  //: a list of all gaps in the boundary
  mutable vcl_vector<vcl_vector<dbsk2d_ishock_belm*> > gaps_;

  //: a set of all belms that are off by loops
  mutable vcl_set<int> belms_off_;

  //: 2d array of cells, rectangular partition of the boundary
  // All cells have identical width and height
  vbl_array_2d<dbsk2d_bnd_cell_sptr > cells_;
  double cell_h_;
  double cell_w_;
  double xmin_;
  double ymin_;


public:
  //***********************************************
  // Constructors/Destructors/Initialize
  //***********************************************
   
  //: Constructor
  dbsk2d_boundary ();

  //: Destructor
  virtual ~dbsk2d_boundary ();

  //***********************************************
  // Data access
  //***********************************************

  //: get the spatial type
  virtual vsol_spatial_object_2d_type spatial_type() const
  { return vsol_spatial_object_2d::SPATIALGROUP; }

  //: returns the next available ID 
  int nextAvailableID() const {return ++(this->_nextAvailableID); }

  //: Is preprocessing needed ?
  bool is_preprocessing_needed() const 
  { return !(this->scratch_contours_.empty()); }

  //: Return reference to list of contours that have been preprocesed
  const vcl_list<dbsk2d_bnd_contour_sptr >& preproc_contours() const
  { return this->preproc_contours_; }

  //: Return reference to list of contours that need preprocessing
  const vcl_list<dbsk2d_bnd_contour_sptr >& scratch_contours() const
  { return this->scratch_contours_; }

  //: Obtain list of all contours, including preprocessed and scratch
  void all_contours(bnd_contour_list& all_contours) const;

  //: Set the list of preprocessed contours
  void set_preproc_contours(const bnd_contour_list& new_contours)
  { this->preproc_contours_ = new_contours;}

  //: Set the list of scratch contours
  void set_scratch_contours(const bnd_contour_list& new_contours)
  { this->scratch_contours_ = new_contours;}

  //: Set contours that are on
  void set_belms_off(int key){belms_off_.insert(key);}

  //: Set contours that are off
  void set_belms_on(int key){belms_off_.erase(key);}

  //: Contour on/off
  bool belm_off(int key){return belms_off_.count(key);}


protected:
  //: Add a dbsk2d_bnd_contour to the proprocessed contour list
  // just add to the list. No duplication check is performed.
  void add_a_preproc_contour(const dbsk2d_bnd_contour_sptr& new_contour)
  { this->preproc_contours_.push_back(new_contour); }
  
  //: Add a dbsk2d_bnd_contour to the scratch contour list
  // just add to the list. No duplication check is performed.
  void add_a_scratch_contour(const dbsk2d_bnd_contour_sptr& new_contour)
  { this->scratch_contours_.push_back(new_contour); };
  
public:  

  //: return the intrinsic shock graph formed by 'this' boundary
  dbsk2d_ishock_graph_sptr ishock_graph() const { return ishock_graph_; }

  //: set the intrinsic shock graph formed by 'this' boundary
  void set_ishock_graph (const dbsk2d_ishock_graph_sptr new_ishock_graph) 
  { ishock_graph_ = new_ishock_graph; }

  //: Return the shock graph formed by `this' boundary
  dbsk2d_shock_graph_sptr shock_graph() {return this->shock_graph_; }

  //: Set the shock graph formed by `this' boundary
  void set_shock_graph(const dbsk2d_shock_graph_sptr new_shock_graph ) 
  { this->shock_graph_ = new_shock_graph; };

  //: Update the list of dbsk2d_ishock_belm
  // This list is created by collecting data from 
  // preproc_contours and scratch_contours
  void update_belm_list() const;

  //: Local update of belm list
  // This is a local update of belm list with new contour
  void update_belm_list(dbsk2d_bnd_contour_sptr& new_contour,
                        vcl_vector<dbsk2d_ishock_belm*>& ret_belms);

  //: Return reference to the belm list
  // Need to run update_belm_list() first before first use.
  const vcl_vector< dbsk2d_ishock_belm* >& belm_list() const
  { return (this->belm_list_); }

  //: Return reference to the all gaps
  const vcl_vector< vcl_vector<dbsk2d_ishock_belm* > >& gaps() const
  { return (this->gaps_); }

  //: Return number of belm in the boundary
  unsigned int num_belms(){ return this->belm_list().size(); }

  //: Return reference to list of all edges in the boundary
  const vcl_list<dbsk2d_bnd_edge_sptr >& all_edges() const
  { return (this->all_edges_); }

  //: compute bounding box
  virtual void compute_bounding_box() const;


  //******************************************************
  // Cell related functions
  //******************************************************
  
  //: Set parameters of cell partionting,
  void set_partition_params(double xmin, double ymin,
    int num_rows, int num_cols, 
    double cell_height, double cell_width);

  //: Return number of rows in cell partitioning
  int num_rows() const{ return this->cells_.rows(); }

  //: Return number of columns in cell partitioning
  int num_cols() const{ return this->cells_.cols(); }

  //: Return total number of cells in the boundary
  int num_cells() const{ return this->num_rows()*this->num_cols(); }

  //: Return height of each cell
  double cell_h() const{ return this->cell_h_; }

  //: Return width of each cell
  double cell_w() const{ return this->cell_w_; }

  //: Return xmin of partitioned area
  double xmin() const{ return this->xmin_;}

  // Return ymin of partitioned area
  double ymin() const{ return this->ymin_;}

  //: Return total width of partitioned area
  double width() const
  { return this->num_cols()*this->cell_w(); }

  //: Return total height of partitioned area
  double height() const
  { return this->num_rows()*this->cell_h(); }

  //: Return smart pointer to a cell given an index
  dbsk2d_bnd_cell_sptr cell(int row, int col) const
  { dbsk2d_assert (0<=row && row<this->num_rows() && 0<=col && col<this->num_cols());
  return this->cells_(row,col); }

  //: Return a reference to the cell arrary
  const vbl_array_2d<dbsk2d_bnd_cell_sptr >& cells() const
  {return this->cells_; }

  //: Return a retangular box of the region that covers all cells.
  vgl_box_2d<double > cell_region() const
  { return vgl_box_2d<double >(this->xmin(), this->xmin()+this->width(), 
    this->ymin(), this->ymin()+this->height()); }

  //: Partition the boundary, i.e. putting boundary elements (edges) into cells
  // `tol' is tolerance of fuzzy comparison
  // Require: run set_partition_params(...) before this command
  void partition_into_cells(bool tight=false, bool talkative=false, 
    double tol=B_EPSILON);


  //: Insert a line edge into cells it INTERSECTS with (fuzzily).
  void insert_line_into_cells_tight(const dbsk2d_bnd_edge_sptr& e, double tol);

  //: Insert an edge into cells using bounding box intersection
  void insert_edge_into_cells_using_bounding_box(const dbsk2d_bnd_edge_sptr& e,
    double tol);


  //: intersect a line with `this' boundary's grid lines
  // Return a sorted list of parameters of intersections points
  void intersect_line_with_cell_grids(const vgl_point_2d<double >& p1, 
    const vgl_point_2d<double >& p2, 
    vcl_list<double >& intersections) const;

  //: intersect a circular arc with `this' boundary's grid lines
  // Return a sorted list of parameters of intersections points
  void intersect_arc_with_cell_grids(const vgl_point_2d<double >& arc_p1, 
    const vgl_point_2d<double >& arc_p2, double arc_k,
    vcl_list<double >& intersections) const;

  //: Compute list of cells that a point belongs to (fuzzily)
  vcl_vector<dbsk2d_bnd_cell_sptr > compute_cell_membership_of_point(
    const vgl_point_2d<double >& pt, double tol = 0) const;


  //: Compute list of cells that a line segment intersect with (no fuzzy)
  void compute_cell_membership_of_line(const vgl_point_2d<double >& p1, 
    const vgl_point_2d<double >& p2, 
    vcl_list<dbsk2d_bnd_cell_sptr >& ret_cells) const;

  //: COmpute list of cells that a bounding box intersect with (no fuzzy)
  vcl_vector<dbsk2d_bnd_cell_sptr > compute_cell_membership_of_bbox(
    const vgl_box_2d<double >& bbox, double tol) const;

  //: Break long lines into shorter segments such that each segment does not
  // extend beyond one cell (fuzzily)
  // Required: edges have been put in the cells (use partition_into_cells())
  void break_long_line_edges(double tol);

  //: Break long arcs into shorter segments such that each segment does not
  // extend beyond one cell (fuzzily)
  // Required: edges have been put in the cells (use partition_into_cells())
  void break_long_arc_edges(double tol);

  //: Remove all edges from the cells
  void clear_cells();


  // ======= FUNCTIONS TO ADD ELEMENTS TO THE BOUNDARY
public:

  //: Add a stand-alone point, return smart ptr to the new dbsk2d_bnd_contour
  // if `preproc_needed' is true, the point will be listed to be preprocessed
  dbsk2d_bnd_contour_sptr add_a_point(const vgl_point_2d<double >& pt,
    bool preproc_needed = true);

  //: Add a stand-alone point, return smart ptr to the new dbsk2d_bnd_contour
  dbsk2d_bnd_contour_sptr add_a_point(const vsol_point_2d_sptr & point,
      bool preproc_needed = true);

  //: Add a line segment, return smart ptr to the new the dbsk2d_bnd_contour
  dbsk2d_bnd_contour_sptr add_a_line(const vgl_point_2d<double >& p1, 
    const vgl_point_2d<double >& p2, bool preproc_needed = true);
  
  //: Add a line segment, return smart ptr to the new the dbsk2d_bnd_contour
  dbsk2d_bnd_contour_sptr add_a_line(const vsol_line_2d_sptr & line,
      bool preproc_needed = true);

  //: Add a polyline, return smart ptr to the new dbsk2d_bnd_contour
  dbsk2d_bnd_contour_sptr add_a_polyline(const vsol_polyline_2d_sptr & polyline,
     bool preproc_needed = true);

  //: Add a polygon, return smart ptr to the new dbsk2d_bnd_contour
  dbsk2d_bnd_contour_sptr add_a_polygon(const vsol_polygon_2d_sptr & polygon,
     bool preproc_needed = true);

  //: Add a LEMS curve, return smart ptr to dbsk2d_bnd_contour object
  // For now, just perform a linear interpolation on the LEMS curve and
  // add in as a polyline
  // \TODO: use more advanced interpolation algorithm to interpolate as a polyarcs
  dbsk2d_bnd_contour_sptr add_an_interp_curve_2d(
    const dbsol_interp_curve_2d_sptr& curve, bool preproc_needed = true);

  //: Add a set of connected lines to the boundary. 
  // This is a common function for add_a_polyline and add_a_polygon
  dbsk2d_bnd_contour_sptr add_connected_lines(
    const vcl_vector<vgl_point_2d<double > > & vertices, 
    bool closed = false, bool preproc_needed = true );


  //: Add an arc to the boundary. Return smart poiter to the new contour
  dbsk2d_bnd_contour_sptr add_an_arc( vgl_point_2d<double > arc_p1,
    vgl_point_2d<double > arc_p2, double arc_k, bool preproc_needed = true );

  //: Add a set of connected arcs to the boundary
  dbsk2d_bnd_contour_sptr add_connected_arcs(
    const vcl_vector<vgl_point_2d<double > > &vertices,
    const vcl_vector<double >& curvatures,
    bool closed = false, bool preproc_needed = true );

  //: Add a boundary contour to the boundary.
  void add_a_bnd_contour(const dbsk2d_bnd_contour_sptr& new_bnd_contour,
    bool preproc_needed = true);

  //: Remove a boundary contour from the boundary.
  void remove_a_bnd_contour(dbsk2d_bnd_contour_sptr& bnd_contour)
  {preproc_contours_.remove(bnd_contour);}

  //: Remove empty contours
  void remove_empty_contours(); 
  
public:

  //-------------------------------------------------------------------
  // Utilities
  //-------------------------------------------------------------------
  
  //: Clone `this': creation of a new object and initialization
  // inherited. Need rewrite.
  virtual vsol_spatial_object_2d* clone() const {return 0; }

  //: Print the belm list
  void print_belm_list(vcl_ostream& os = vcl_cout );

  // Binary I/O-----------------------------------------------------------------

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return vcl_string("dbsk2d_boundary");}

  ////: Return IO version number;
  //short version() const;

  ////: Binary save self to stream.
  //virtual void b_write(vsl_b_ostream &os) const;
 
  ////: Binary load self from stream.
  //virtual void b_read(vsl_b_istream &is);

  //: print a quick summary
  // Need rewrite.
  virtual void print(vcl_ostream &os=vcl_cout) const;
  
  //: describe
  // Need rewrite.
  virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  //: print a summary of partition parameters
  void print_partition_summary(vcl_ostream& os=vcl_cout) const;



  //**********************************************
  // ITERATORS
  //**********************************************

public:
  //: An iterator to iterate through all dbsk2d_ishock_belm
  // elements in the boundary

  typedef vcl_vector<dbsk2d_ishock_belm* >::iterator belm_iterator;
  //: begin belm_iterator
  belm_iterator belm_begin() { return this->belm_list_.begin(); }

  //: end belm_iterator
  belm_iterator belm_end() { return this->belm_list_.end(); }
    
  



  //class belm_iterator2 : public (*belm_ishock_belm)
  //{
//  protected:
//    dbsk2d_boundary* boundary_;
//    vcl_vector< dbsk2d_ishock_belm* >::iterator belm_iter_;
//  public:
//
//    //: Constructor - defaul
//    belm_iterator(): boundary_(0){};
//
//    //: Constructor
//    // If is_begin = true, Return a begin() iterator
//    // Otherwise return an end() iterator
//    belm_iterator( dbsk2d_boundary* boundary, bool is_begin = true );
//    
//    //: Destructor
//    virtual ~belm_iterator() {}
//
//    const dbsk2d_boundary* boundary() const { return this->boundary_; }
//    dbsk2d_boundary* boundary() { return this->boundary_; }
//
//    //: Pre-Increment
//    belm_iterator& operator++ ();
//    //: Post-Increment
//    belm_iterator operator++ (int);
//
//    //: Dereference
//    dbsk2d_ishock_belm* operator -> () const;
//    //: Dereference
//    dbsk2d_ishock_belm* operator * () const;
//
//    //: Equality comparison
//    bool operator == (const belm_iterator& rhs) const;
//
//    //: Inequality comparison
//    bool operator != (const belm_iterator& rhs) const;
  //};
//
//  friend class dbsk2d_boundary::belm_iterator;
//


  

//  //**********************************************
//  // ITERATORS
//  //**********************************************
//
//public:
//  //: An iterator to iterate through all dbsk2d_ishock_belm
//  // elements in the boundary
//  class belm_iterator
//  {
//  protected:
//    dbsk2d_boundary* boundary_;
//    vcl_vector< dbsk2d_ishock_belm* >::iterator belm_iter_;
//  public:
//
//    //: Constructor - defaul
//    belm_iterator(): boundary_(0){};
//
//    //: Constructor
//    // If is_begin = true, Return a begin() iterator
//    // Otherwise return an end() iterator
//    belm_iterator( dbsk2d_boundary* boundary, bool is_begin = true );
//    
//    //: Destructor
//    virtual ~belm_iterator() {}
//
//    const dbsk2d_boundary* boundary() const { return this->boundary_; }
//    dbsk2d_boundary* boundary() { return this->boundary_; }
//
//    //: Pre-Increment
//    belm_iterator& operator++ ();
//    //: Post-Increment
//    belm_iterator operator++ (int);
//
//    //: Dereference
//    dbsk2d_ishock_belm* operator -> () const;
//    //: Dereference
//    dbsk2d_ishock_belm* operator * () const;
//
//    //: Equality comparison
//    bool operator == (const belm_iterator& rhs) const;
//
//    //: Inequality comparison
//    bool operator != (const belm_iterator& rhs) const;
//  };
//
//  friend class dbsk2d_boundary::belm_iterator;
//
//  //: begin belm_iterator
//  belm_iterator belm_begin() { return belm_iterator(this); }
//
//  //: end belm_iterator
//  belm_iterator belm_end() { return belm_iterator(this, false); }


};

#endif //#define dbsk2d_boundary_h_
