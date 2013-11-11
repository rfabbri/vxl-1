// This is shp/dbsks/dbsks_xgraph_geom_model.h
#ifndef dbsks_xgraph_geom_model_h_
#define dbsks_xgraph_geom_model_h_

//:
// \file
// \brief A generative model for xgraph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Nov 13, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xnode_geom_model_sptr.h>
#include <dbsks/dbsks_xnode_geom_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>

// =============================================================================
// dbsks_xgraph_geom_model
// =============================================================================

class dbsks_biarc_sampler;

//: A generative model for generating xshock_fragment
class dbsks_xgraph_geom_model: public vbl_ref_count
{
public:
  
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------
  
  //: constructor
  dbsks_xgraph_geom_model(): biarc_sampler_(0) {};

  //: destructor
  virtual ~dbsks_xgraph_geom_model();
  
  
  // DATA ACCESS ---------------------------------------------------------------

  unsigned root_vid() const {return this->root_vid_; }
  void set_root_vid(unsigned root_vid){ this->root_vid_ = root_vid; }

  //// not yet available
  //unsigned pseudo_parent_eid() const{return this->pseudo_parent_eid_; }
  //void set_pseudo_parent_eid(unsigned eid) {this->pseudo_parent_eid_ = eid; }

  // to be depreciated-----------------------------------------------------------
  unsigned major_child_eid() const{return this->major_child_eid_; }
  void set_major_child_eid(unsigned eid) {this->major_child_eid_ = eid; };
  // /Depreciated----------------------------------------------------------------

  //: Return normalization graph size
  double graph_size() const {return this->graph_size_; }
  void set_graph_size(double new_size){this->graph_size_ = new_size; }

  //: Return a full list of geometric models for the edges
  const vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >& map_edge2geom() const
  { return this->map_edge2geom_; };

  vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >& map_edge2geom()
  { return this->map_edge2geom_; };


  //: Return a full list of geometric models for the vertices
  const vcl_map<unsigned, dbsks_xnode_geom_model_sptr >& map_node2geom() const
  { return this->map_node2geom_; };

  vcl_map<unsigned, dbsks_xnode_geom_model_sptr >& map_node2geom()
  { return this->map_node2geom_; };



  // UTILITIES------------------------------------------------------------------

  //: Build the model
  bool build(const dbsksp_xshock_graph_sptr& xgraph,
             const vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >& map_edge2xfrag, 
             const vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >& map_node2xgeom,
             unsigned root_vid, 
             unsigned major_child_eid,
             double normalized_scale);

public:
  //: Return true if this model is compatible with a given shock graph topology
  bool is_compatible(const dbsksp_xshock_graph_sptr& xgraph);

  //: Compute distributions of geometric attributes that can be used to constrain
  // a shock graph
  void compute_attribute_constraints();




  
  // I/O -----------------------------------------------------------------------

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

  // MISC-----------------------------------------------------------------------
  
  // SUPPORTING FUNCTIONS-------------------------------------------------------
protected:
  //: Compute edge geometric attributes from training data
  void compute_edge_geom_attr_from_data(const dbsksp_xshock_graph_sptr& xgraph,
    double norm_graph_size,
    const vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >& map_eid2xfrag);

  //: Compute vertex geometric attributes from training data
  void compute_vertex_geom_attr_from_data(const dbsksp_xshock_graph_sptr& xgraph,
    double norm_graph_size,
    const vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >& map_vid2xnode);

  //: build geom models for edges from geometric attributes
  bool build_edge_geom_models();

  //: build geom models for vertices from geometric attributes
  bool build_vertex_geom_models();

  // MEMBER VARIABLES ----------------------------------------------------------
protected:
  unsigned int root_vid_;
  unsigned int pseudo_parent_eid_;
  unsigned int major_child_eid_; // to be depreciated

  // size of the modeling xgraph
  // All length parametes scale up with the graph size
  double graph_size_;
  
  vcl_map<unsigned, dbsks_xfrag_geom_model_sptr > map_edge2geom_;
  vcl_map<unsigned, dbsks_xnode_geom_model_sptr > map_node2geom_;


  //: A biarc sampler
  dbsks_biarc_sampler* biarc_sampler_;
};

#endif // shp/dbsksp/dbsks_xgraph_geom_model.h


