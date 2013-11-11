// This is dbsk2d/dbsk2d_bnd_edge.h
#ifndef dbsk2d_bnd_edge_h
#define dbsk2d_bnd_edge_h

//:
// \file
// \brief This file contains data structure for topological boundary edge of shock class
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 6/21/2005
//
// \verbatim
//  Modifications
//    Nhon Trinh 6/21/2005 Initial version
// \endverbatim

#include <vtol/vtol_edge.h>
#include <vsol/vsol_box_2d.h>

#include <dbsk2d/dbsk2d_ishock_bcurve.h>
#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>
#include <dbsk2d/dbsk2d_bnd_cell_sptr.h>

// Forward declaration
class dbsk2d_bnd_cell;


// =============================================================================
// dbsk2d_bnd_edge
// =============================================================================
//: Topological boundary edge segment that can be described parametrically.
class dbsk2d_bnd_edge: public vtol_edge
{
protected:
  dbsk2d_ishock_bcurve* left_bcurve_;
  dbsk2d_ishock_bcurve* right_bcurve_;

  //: list of cells `this' edge is contained in
  mutable vcl_list<dbsk2d_bnd_cell* > cells_;

public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZERS -------------------------------------

  //: Constructor - a real edge 
  dbsk2d_bnd_edge( dbsk2d_bnd_vertex_sptr v1,
                   dbsk2d_bnd_vertex_sptr v2,
                   dbsk2d_ishock_bcurve* left_bcurve, 
                   dbsk2d_ishock_bcurve* right_bcurve,
                   int id = -1);

  //: Constructor - a degenerate edge, i.e. a point
  dbsk2d_bnd_edge( dbsk2d_bnd_vertex_sptr v, int id = -1);

  // Destructor
  virtual ~dbsk2d_bnd_edge();

  
  // DATA ACCESS ---------------------------------------------------------------
  
  
  //: Return a platform independent string identifying the class
  // inherited
  virtual vcl_string is_a() const { return vcl_string("dbsk2d_bnd_edge"); }

  //: Return true if `this' is a degenerate edge, i.e. it is a point
  virtual bool is_a_point() const { return this->v1_ == this->v2_; };

  //: Return true if the argument matches the string 
  // identifying the class or any parent class
  // inherited
  virtual bool is_class(const vcl_string& cls) const
  { return cls==is_a() || vtol_edge::is_class(cls); }

  //: Return the left half dbsk2d_ishock_bcurve
  dbsk2d_ishock_bcurve* left_bcurve() { return this->left_bcurve_; };
  const dbsk2d_ishock_bcurve* left_bcurve() const { return this->left_bcurve_; };

  //: Return the right half dbsk2d_ishock_bcurve
  dbsk2d_ishock_bcurve* right_bcurve() { return this->right_bcurve_; };
  const dbsk2d_ishock_bcurve* right_bcurve() const { return this->right_bcurve_; };

  //: Set the two half curves dbsk2d_ishock_bcurve
  // First check if `left' and `right' are twin. Then set.
  // Return false if `left' and `right' are not twins.
  virtual bool set_bcurves( dbsk2d_ishock_bcurve* left_bcurve, 
    dbsk2d_ishock_bcurve* right_bcurve);

  //: Delete the bcurve objects of `this'
  void clear_bcurves();
  

  //: Return starting vertex
  dbsk2d_bnd_vertex_sptr bnd_v1() const
  { return static_cast<dbsk2d_bnd_vertex*>(this->v1().ptr()); }

  //: Return ending vertex
  dbsk2d_bnd_vertex_sptr bnd_v2() const
  { return static_cast<dbsk2d_bnd_vertex*>(this->v2().ptr());}

  //: Return geometry point of starting vertex
  vgl_point_2d<double > point1() const
  { return this->bnd_v1()->point(); }

  //: Return geometry point of ending vertex
  vgl_point_2d<double > point2() const
  { return this->bnd_v2()->point(); }

  //: return length of the edge
  virtual double len() const;

  
  
  // OPERATORS -----------------------------------------------------------------
  
  
  //: Equality operator. Shallow comparison.
  // inherited from vtol_edge. need rewrite.
  virtual bool operator==(const vtol_edge &other) const;
  bool operator==(const dbsk2d_bnd_edge& other) const;

  // BASIC FUNCTIONS -----------------------------------------------------------
  
  //: Merge `this' edge with `other' edge
  // all linkages in `other' will be transfered to `this'
  // require: `this' and `other' share both vertices
  void merge_with(dbsk2d_bnd_edge& other);

  //: comparison of geometry
  // inherited. Need rewrite.
  virtual bool compare_geometry(const vtol_edge &other) const;

  //: reverse the edge direction, i.e. exchange starting and ending vertices
  bool reverse_direction();



  //======= cell related functions ---------------------------------------------

  //: Return referece to list of cells `this' edge is contained in
  const vcl_list<dbsk2d_bnd_cell* >& cells() const { return this->cells_; }

  //: Return number of cells `this' edge belongs to
  int num_cells() const {return this->cells_.size(); }
  
  //: Add a cell to `this' edge's cell list
  void add_cell(const dbsk2d_bnd_cell_sptr& cell);

  //: Remove a cell from `this' edge's cell list
  void remove_cell(const dbsk2d_bnd_cell_sptr& cell);

  //: Return false if `this' edge is guaranteed not to intersect `cell'
  bool intersect_cell(const dbsk2d_bnd_cell_sptr& cell, double tol = B_EPSILON);

  //: Disconnect `this' from all the cells it belongs to
  void unlink_from_cells();



  // I/O -----------------------------------------------------------------------

  //: print
  // inherited. Need rewrite.
  virtual void print(vcl_ostream &os=vcl_cout) const;

  void print_cell_info(vcl_ostream &os = vcl_cout) const;

  //
  ////: describe
  //// inherited. Need rewrite.
  //virtual void describe(vcl_ostream &strm=vcl_cout,
  //                       int blanking=0) const;

  //: Compute bounding box.
  // Inherited from vsol_spatial_object_2d
  virtual void compute_bounding_box() const;

private:
  // depreciated functions

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization.
  // inherited. Depreciated.
  // Need rewrite
  virtual vsol_spatial_object_2d* clone() const{ return 0; };

  //:  copy the geometry
  // inherited. Depreciated. Need rewrite.
  virtual void copy_geometry(const vtol_edge &other){};
};


#endif // dbsk2d_bnd_edge_h
