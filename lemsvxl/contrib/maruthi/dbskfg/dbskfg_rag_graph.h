// This is shp/dbsksp/dbskfg_rag_graph.h
#ifndef dbskfg_rag_graph_h_
#define dbskfg_rag_graph_h_

//:
// \file
// \brief A class for a region adjancency graph
//        
// \author Maruthi Narayanan ( mn@lems.brown.edu)
// \date July 07, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_graph.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_rag_link.h>
#include <vcl_string.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>

//: 
class dbskfg_rag_graph : 
public dbgrl_graph<dbskfg_rag_node, dbskfg_rag_link>
{
public:

  // --------------------------------------------------------------------------
  // Constructors / Destructors
  // --------------------------------------------------------------------------
  //: Constructor
  dbskfg_rag_graph();
  
  //: Destructor
  /* virtual */ ~dbskfg_rag_graph(){destroy_map();}

  //: insert node
  
  // Get next available id
  //: increment the id counter and return new id
  unsigned int next_available_id();

  // Get current id
  unsigned int current_id(){ return next_available_id_;}

  // set curent id
  void set_current_id(unsigned int current_id){
      next_available_id_ = current_id;}

  // Return the rag node for this id
  dbskfg_rag_node_sptr rag_node(unsigned int id);

  // Return the rag node for this id
  dbskfg_rag_node_sptr rag_node(vcl_set<vcl_string>& wavefront);

  // Return the rag node for this id
  dbskfg_rag_node_sptr rag_node(unsigned int id,
                                vcl_set<unsigned int>& rag_con_ids);

  // Return the rag node for this id
  void rag_node(vcl_set<unsigned int> rag_con_ids,
                dbskfg_transform_descriptor_sptr transform,
                vcl_vector<dbskfg_rag_node_sptr>& rag_nodes);

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Return the type identifier string
  vcl_string is_a() const {return "dbskfg_rag_graph"; }

  //: Keep track of rag ids being destroyed 
  void rag_ids_map(unsigned int deleted_id,unsigned int merged_region)
  {region_tree_[deleted_id][merged_region]="temp";}

  //: Keep track of contour and region id 
  void contour_region_map(unsigned int contour_id,unsigned int frag_id)
  {contour_to_region_[contour_id].insert(frag_id);}

  //: Destroy map
  void destroy_map(){region_tree_.clear();}

  //: Find rag id based on outer shock nodes 
  dbskfg_rag_node_sptr find_region(
      const vcl_map<unsigned int, vgl_point_2d<double> >& wavefront);

  //: return number of regular fragments()
  unsigned int numb_regular_fragments();
  
  //: create copy of rag graph
  void create_copy(const dbskfg_composite_graph_sptr& cgraph,
                   dbskfg_rag_graph& that);

protected:
  

private:
  
  vcl_map<unsigned int, vcl_map<unsigned int,vcl_string> > region_tree_;
  vcl_map<unsigned int, vcl_set<unsigned int> > contour_to_region_;

  unsigned int next_available_id_;

};

#endif // brcv/shp/dbskfg/dbskfg_rag_graph.h


