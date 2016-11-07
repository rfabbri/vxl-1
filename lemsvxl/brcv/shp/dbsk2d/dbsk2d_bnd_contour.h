// This is dbsk2d/dbsk2d_bnd_contour.h
#ifndef dbsk2d_bnd_contour_h
#define dbsk2d_bnd_contour_h

//:
// \file
// \brief This file contains data structure for boundary contour of shock 2d  
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 6/17/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vtol/vtol_one_chain.h>

#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>


struct timed_vector_of_double: public vul_timestamp,
                              public vcl_vector<double >
{};

//: Boundary contour for shock
class dbsk2d_bnd_contour : public vtol_one_chain
{

  //***********************************************************
  // DATA MEMBERS
  //***********************************************************

protected:
  //: Cache system to cache cumulated lengths of the segments
  mutable timed_vector_of_double len_cache_;

  //: links to shocks



  //************************************************************
  // Constructor/Destructor/Initialization
  //*************************************************************

public:
  //: Constructor - default
  dbsk2d_bnd_contour(){};

  //: Constructor - from a list of edges without directions
  // require: the edges are connected
  dbsk2d_bnd_contour( const vcl_vector<dbsk2d_bnd_edge_sptr >& bnd_edges, int id = -1 );

  //: Constructor - from a list of edges with known directions
  // require: the edges are connected
  // no connectivity check
  dbsk2d_bnd_contour(const vcl_vector<dbsk2d_bnd_edge_sptr >& edges,
    const vcl_vector<signed char >& directions, int id=-1);


  //: Constructor - from an edge
  dbsk2d_bnd_contour(const dbsk2d_bnd_edge_sptr& edge, int id = -1);
  


  //: Destructor
  virtual ~dbsk2d_bnd_contour(){};

  //************************************************************
  // Data access
  //************************************************************

  //: Return a platform independent string identifying the class
  // inherited. 
  virtual vcl_string is_a() const 
  { return vcl_string("dbsk2d_bnd_contour"); };

  //: Return true if the argument matches the string identifying the class or any parent class
  // inherited.
  virtual bool is_class(const vcl_string& cls) const
  { return cls==is_a() || vtol_one_chain::is_class(cls); }
  
  //: Is `inferior' type valid for `this' ?
  // inherited.
  virtual bool valid_inferior_type(vtol_topology_object const* inferior) const
  { return inferior->is_a() == "dbsk2d_bnd_edge"; }

  //: Is `chain_inf_sup' type valid for `this' ?
  // i.e. whether chain_inf_sup can be an inferior chain of this chain. 
  virtual bool valid_chain_type(vtol_chain_sptr chain_inf_sup) const
  {return chain_inf_sup->is_a() == this->is_a();}


  //----- Geometry Properties

  //: point at arclength s
  // need rewrite
  virtual vgl_point_2d< double > point_at(double s) const;

  //: tangent at arclength s
  // need rewrite
  virtual vgl_vector_2d< double > tangent_at (double s) const;

  //: tangent angle at arclength s
  // need rewrite
  virtual double tangent_angle_at( double s) const;

  //: return index of bnd_edge at arclength s
  // if out of range, return num_of_edges
  int edge_index_at(double s) const;
  
  //: return dbsk2d_bnd_edge at index i (first edge: i = 0)
  dbsk2d_bnd_edge_sptr bnd_edge(int i) const;
  
  //: return dbsk2d_bnd_edge at arclength s (first edge: 0<= s < s1)
  dbsk2d_bnd_edge_sptr edge_at(double s) const;

  //: return arclength to the beginning of a bnd_edge
  // return -1 if `edge' is not a segment of `this' contour
  double arclength_at(int index) const;
  double arclength_at(dbsk2d_bnd_edge_sptr edge) const;

  //: Return vertex at index i of the chain of (num_edges()+1) vertices.
  dbsk2d_bnd_vertex_sptr bnd_vertex(int i) const;

  //----------------------------------------------
  // Cache-related functions
  //----------------------------------------------
  //: Return pointer to the cached cumulated-length vector 
  // if `recompute' = true, the cache vector will be cleared and then recomputed
  const vcl_vector< double >& len_cache() const;


  //: Recompute the cached cumulated-length vector
  void update_len_cache() const;
  
  //-----------------------------------------------
  // shock related functions
  //-----------------------------------------------

  //: purge all links to shocks from the contour
  //  This function is called when a new shock graph needs to be
  //  formed from the current boundary
  void clear_shock_links(){}

  //***********************************************
  // OPERATORS
  //***********************************************

  //: Equality operator
  // inherited from vtol_one_chain. Need rewrite.
  virtual bool operator==(vtol_one_chain const& other) const;

  //: Equality operator
  // Need rewrite.
  bool operator==(dbsk2d_bnd_contour const& other) const;


  //*********************************************
  // Basic functions
  //*********************************************
  
  //: Link with an dbsk2d_bnd_edge
  void link_inferior(dbsk2d_bnd_edge_sptr inf );
  
  //: Unlink an dbsk2d_bnd_edge
  void unlink_inferior(dbsk2d_bnd_edge_sptr inf);

  //: add a vtol_edge
  // `new_dir' may be overridden
  // inherited.
  virtual void add_edge(vtol_edge_sptr & new_edge, bool new_dir);

  //: add a dbsk2d_bnd_edge
  // Require: `new_edge' shares a vertex with contour's last edge
  // Return false when adding fails
  bool add_edge(const dbsk2d_bnd_edge_sptr & new_edge);

  ////: replace an edge with a vector of dbsk2d_bnd_edges
  //// Require: `new_edges' need to be connected together and to
  //// edges before and after the replace edge
  //bool replace_edge(const vtol_edge_sptr& old_edge, 
  //  const vcl_vector<dbsk2d_bnd_edge_sptr >& new_edges);

  // replace a group of edges with a group of new edges
  // Require: `new_edges' need to be connected together and to
  // edges before and after the replaced edges
  // if `end_edge' == 0, it is assumed = `start_edge'
  // No connectivity check in this function
  bool replace_edges(const vcl_vector<dbsk2d_bnd_edge_sptr >& new_edges,
    const vcl_vector<signed char >& directions,
    const dbsk2d_bnd_edge_sptr& start_edge,
    const dbsk2d_bnd_edge_sptr& end_edge = 0);





  //*********************************************
  // Binary I/O
  //*********************************************
  

public:
  //: very brief description of the class
  virtual void print(vcl_ostream &strm=vcl_cout) const;

  // virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const{}

  //*********************************************
  // DEPRECIATED FUNCTIONS
  //*********************************************
  

private:
  // Depreciated for now

  //: Clone `this': creation of a new object and initialization
  // inherited. depreciated.
  virtual vsol_spatial_object_2d* clone() const {return 0; };

  //: copy
  virtual vtol_one_chain * copy_with_arrays(topology_list &verts,
                                            topology_list &edges) const
  {return 0;};


  //: add a vtol_edge_2d
  // inherited. depreciated.
  virtual void add_edge(vtol_edge_2d_sptr const&, bool){};


  //: type checks
  //: Determine edge directions. Don't know what this does yet.
  // inherited. depreciated.
  virtual void determine_edge_directions(){};
};




#endif // dbsk2d_bnd_contour_h
