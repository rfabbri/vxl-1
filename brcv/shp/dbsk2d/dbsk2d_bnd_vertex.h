// This is dbsk2d/dbsk2d_bnd_vertex.h
#ifndef dbsk2d_bnd_vertex_h
#define dbsk2d_bnd_vertex_h

//:
// \file
// \brief This file contains data structure for topological boundary vertex of shock class
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 6/21/2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <vtol/vtol_vertex.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>

#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>



//: a class to represent a topological vertex of shock boundary.
class dbsk2d_bnd_vertex : public vtol_vertex
{
  //***********************************************************
  // Data members
  //***********************************************************
protected:
  //: the actual point with geometry and shock links.
  dbsk2d_ishock_bpoint* bpoint_;

public:
  //***********************************************************
  // Constructors/Destructors/Initializers
  //***********************************************************

  //: Constructor
  dbsk2d_bnd_vertex(dbsk2d_ishock_bpoint* bpoint, int id = -1);

  //: Destructor
  virtual ~dbsk2d_bnd_vertex();


public:

  //***********************************************************
  // Data access
  //***********************************************************

  //: Return a platform independent string identifying the class
  // inherited. 
  virtual vcl_string is_a() const { return vcl_string("dbsk2d_bnd_vertex"); }

  //: Return true if the argument matches the string identifying the class 
  // or any parent class
  // inherited.
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a() || 
    vtol_vertex::is_class(cls); }

  //: Return the pointer to dbsk2d_ishock_bpoint
  const dbsk2d_ishock_bpoint* bpoint() const { return this->bpoint_;}
  dbsk2d_ishock_bpoint* bpoint() { return this->bpoint_; }

  //: Set the dbsk2d_ishock_bpoint variable
  void set_bpoint(dbsk2d_ishock_bpoint* new_bpoint)
  { this->bpoint_ = new_bpoint; };

  //: Return the geometry point
  vgl_point_2d< double > point() const { return this->bpoint_->pt(); }

  //: Clone `this': creation of a new object and initialization
  // inherited.
  virtual vsol_spatial_object_2d* clone() const;

  //***********************************************************
  // Relation with other vtol objects
  //***********************************************************
  
  
  //***********************************************************
  // Operators
  //***********************************************************
  
  //: Is `this' has the same coordinates for its point than `other' ?
  // inherited.
  virtual bool operator==(const vtol_vertex &other) const;


  //**************************************************
  // Utilities
  //**************************************************

  //:  copy the geometry
  // inherited.
  virtual void copy_geometry(const vtol_vertex &other);

  //: compare the geometry with fuzzy boolean
  // inherited.
  virtual bool compare_geometry(const vtol_vertex &other) const;

  //: Merge superior linkages of `this' with 'other' vertex
  // All superior linkage in `other' will be transfered to 'this'
  // Need rewrite - remove superiors list of `other'
  void merge_superiors_with( dbsk2d_bnd_vertex_sptr other );

  //: Merge `this' vertex with `other' vertex
  // usually because they are too close to each other
  // all linkages in `other' will be transfered to `this'
  void merge_with(dbsk2d_bnd_vertex_sptr other);

  //: print
  // inherited. Need rewrite.
  virtual void print(vcl_ostream & os=vcl_cout) const;

  //  void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  //: Compute bounding box of the vertex. A local implementation
  // inherited.
  virtual void compute_bounding_box() const; 

  //: touch `this' vertex and edges and contours containing `this' vertex
  void touch_and_propagate();



  // depreciated
private:
  //: Create a line edge from `this' and `other' only if this edge does not exist.
  //  Otherwise it just returns the existing edge
  //  REQUIRE: other!=*this
  // inherited. Depreciated. Simply return 0.
  virtual vtol_edge_sptr new_edge(vtol_vertex_sptr const& other);
};


#endif // dbsk2d_bnd_vertex_h
