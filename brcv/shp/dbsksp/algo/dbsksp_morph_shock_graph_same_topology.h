// This is shp/dbsksp/dbsksp_morph_shock_graph_same_topology.h
#ifndef dbsksp_morph_shock_graph_same_topology_h_
#define dbsksp_morph_shock_graph_same_topology_h_

//:
// \file
// \brief A class to morph between two shock graphs
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_map.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <vnl/vnl_vector.h>


// ============================================================================
// dbsksp_morph_shock_graph_same_topology
// ============================================================================

//: A class to morph between two shock graphs with same topology
class dbsksp_morph_shock_graph_same_topology
{
public:

  // --------------------------------------------------------------------------
  // CONSTRUCTORS / DESTRUCTORS
  // --------------------------------------------------------------------------

  //: Constructor
  dbsksp_morph_shock_graph_same_topology(){};
  
  //: Destructor
  virtual ~dbsksp_morph_shock_graph_same_topology(){};

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Get and set the source shock graph
  dbsksp_shock_graph_sptr source_graph() const {return this->source_graph_; }
  void set_source_graph(const dbsksp_shock_graph_sptr& source_graph )
  { this->source_graph_ = source_graph; }

  //: Get and set the target shock graph
  dbsksp_shock_graph_sptr target_graph() const {return this->target_graph_; }
  void set_target_graph(const dbsksp_shock_graph_sptr& target_graph )
  { this->target_graph_ = target_graph; }


  // --------------------------------------------------------------------------
  // UTILITIES
  // --------------------------------------------------------------------------

  //: Establish correspondences between two shock graph using the ref_node's and
  // ref_edge's as the starting point and perform euler tours on both graphs.
  // Correspondence are in terms of <edge_id edge_id>
  // Return false if failed to establish
  bool compute_correspondence();

  //: Compute all the parameters necessary to produce intermediate shapes
  // Return false if intermediate shapes cannot be generated
  bool morph();

  //: Get an intermediate graph given a time `t', 0<= t <= 1
  dbsksp_shock_graph_sptr get_intermediate_graph(double t);

  //: print info of the morpher to an output stream
  virtual vcl_ostream& print(vcl_ostream & os) const;

  //: Get the 1-1 correpondence mapping between edges of the two graphs
  const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& edge_map()
  {return this->edge_map_; };
  
  //: Get the 1-1 correpondence mapping between nodes of the two graphs
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_shock_node_sptr>& node_map()
  {return this->node_map_; };
  



protected:
  dbsksp_shock_graph_sptr source_graph_;
  dbsksp_shock_graph_sptr target_graph_;

  

  // edge correspondence map<source_edge, target_edge>
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> edge_map_;

  // vertex correspondence map<source_node_id, target_node_id>
  vcl_map<dbsksp_shock_node_sptr, dbsksp_shock_node_sptr> node_map_;


  //: ingredients for the morphing
  // the toppology graph
  dbsksp_shock_graph_sptr morphing_graph_;
  
  //: starting parameter vector
  vnl_vector<double > source_params_;

  //: ending parameter vector
  vnl_vector<double > target_params_;


};

#endif // shp/dbsksp/dbsksp_morph_shock_graph_same_topology.h









