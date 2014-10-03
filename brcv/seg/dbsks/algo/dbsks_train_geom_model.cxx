// This is file seg/dbsks/algo/dbsks_train_geom_model.cxx

//:
// \file

#include "dbsks_train_geom_model.h"


#include <dbsks/dbsks_xnode_geom.h>
#include <dbsks/dbsks_xnode_geom_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>

#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>

#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <vcl_fstream.h>

#include <dbxml/dbxml_vector_xio.h>
#include <dbxml/dbxml_xio.h>
#include <dbul/dbul_parse_simple_file.h>


#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>



#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vcl_utility.h>




//=============================================================================
//  dbsks_train_geom_model
//=============================================================================

//------------------------------------------------------------------------------
//: Collect positive training data
bool dbsks_train_geom_model::
collect_data()
{
  vcl_string xshock_folder = this->params_.xshock_folder;
  vcl_string xshock_list_file = this->params_.xshock_list_file;
  int root_vid = this->params_.root_vid;
  int pseudo_parent_eid = this->params_.pseudo_parent_eid;
  vcl_string output_file = this->params_.output_file;

  //////////////////////////////////////////////////////////////////////////////
  double normalized_size = this->params_.normalized_xgraph_size;
  //////////////////////////////////////////////////////////////////////////////

  //> Load the exemplar shock graphs
  vcl_cout << "\n> Load the exemplar xshock graphs from " << xshock_folder << "\n";
  
  // parse input file containing list of xshock graphs
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // Iterate thru the names and load one by one
  vcl_vector<dbsksp_xshock_graph_sptr > list_xgraphs;
  list_xgraphs.reserve(xml_filenames.size());
  for (unsigned index =0; index < xml_filenames.size(); ++index)
  {
    // filename
    vcl_string fname = xml_filenames[index];

    vcl_cout << "\n  Loading " << fname << "...";
    
    vcl_string xml_file = xshock_folder + "/" + xml_filenames[index];
    dbsksp_xshock_graph_sptr xgraph = 0;
    if ( x_read(xml_file, xgraph) )
    {
      vcl_cout << "[ OK ]";
      xgraph->compute_vertex_depths(root_vid);
      list_xgraphs.push_back(xgraph);
    }
    else
    {
      vcl_cout << "[ Failed ].";
      continue;
    }
  }

  // Check-up
  if (list_xgraphs.empty())
  {
    vcl_cout << "ERROR: No xgraph was loaded. \n";
    return false;
  }
  int num_xgraphs = list_xgraphs.size();

  //> use the first exemplar as the prototype
  this->prototype_xgraph = new dbsksp_xshock_graph(*list_xgraphs[0]);
  this->prototype_xgraph->compute_vertex_depths(root_vid);

  if(this->params_.b_normalize)
  	this->prototype_xgraph->scale_up(0, 0, normalized_size/vcl_sqrt(this->prototype_xgraph->area()));
  else
	this->params_.normalized_xgraph_size = vcl_sqrt(this->prototype_xgraph->area());
  
  //> Collect xfragments from the shock graphs
  vcl_cout << "\n> Collect xfrags from the exemplar shock graphs\n";

  // edges
  this->map_edge2frags.clear();
  for (dbsksp_xshock_graph::edge_iterator eit = this->prototype_xgraph->edges_begin();
    eit != this->prototype_xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    map_edge2frags[xe->id()].reserve(num_xgraphs);
  }

  // nodes
  this->map_node2geom.clear();
  for (dbsksp_xshock_graph::vertex_iterator vit = this->prototype_xgraph->vertices_begin();
    vit != this->prototype_xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree()==1)
      continue;
    map_node2geom[xv->id()].reserve(num_xgraphs);
  }

  // Iterate thru the xgraph list and collect edge and node info
  for (unsigned index =0; index < list_xgraphs.size(); ++index)
  {
    dbsksp_xshock_graph_sptr xgraph = list_xgraphs[index];

    // Normalize the exemplar xgraph to the standard size
    double cur_size = vcl_sqrt(xgraph->area());
	if(this->params_.b_normalize)
    	xgraph->scale_up(0, 0, normalized_size / cur_size);
 
    // Compute depths of every node
    xgraph->compute_vertex_depths(root_vid);

    // Collect edge info
    for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
      xgraph->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;

      // build an xfrag flowing from parent to child node
      dbsksp_xshock_node_sptr parent = xe->parent_node();
      dbsksp_xshock_node_sptr child = xe->child_node();

      dbsksp_xshock_node_descriptor xdesc0 = *parent->descriptor(xe);
      dbsksp_xshock_node_descriptor xdesc1 = child->descriptor(xe)->opposite_xnode();
      dbsksp_xshock_fragment_sptr xfrag = new dbsksp_xshock_fragment(xdesc0, xdesc1);

      map_edge2frags[xe->id()].push_back(xfrag);
    } // edge iterator

    // Collect all xnode information
    for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
      xgraph->vertices_end(); ++vit)
    {
      dbsksp_xshock_node_sptr xv = *vit;

      if (xv->degree() == 1)
        continue;

      // Geometric attributes at a node is directional, so we need to determine
      // the parent edge (or pseudo parent edge for root node)
      dbsksp_xshock_edge_sptr parent_edge = 0;
      if (xv->id() == root_vid)
      {
        parent_edge = xgraph->edge_from_id(pseudo_parent_eid);
      }
      else
      {
        parent_edge = xv->parent_edge();
      }
      // just to make sure things are all right ...
      assert(parent_edge->is_vertex(xv));

      // a descriptor for the parent node
      dbsksp_xshock_node_descriptor xdesc = xv->descriptor(parent_edge)->opposite_xnode();

      // retrieve xnode geometry
      dbsks_xnode_geom_sptr xnode_geom = new dbsks_xnode_geom();
      xnode_geom->x_ = xdesc.pt_.x();
      xnode_geom->y_ = xdesc.pt_.y();
      xnode_geom->psi_ = xdesc.psi_;
      xnode_geom->phi_ = xdesc.phi_;
      xnode_geom->radius_ = xdesc.radius_;
      if (xv->degree() == 2)
      {
        xnode_geom->phi_diff_  = xnode_geom->phi_;
      }
      else if (xv->degree() == 3)
      {
        // determine the phi's of the two children
        dbsksp_xshock_edge_sptr child1 = xgraph->cyclic_adj_succ(parent_edge, xv);
        dbsksp_xshock_edge_sptr child2 = xgraph->cyclic_adj_succ(child1, xv);

        double phi_child1 = xv->descriptor(child1)->phi_;
        double phi_child2 = xv->descriptor(child2)->phi_;
        xnode_geom->phi_diff_ = phi_child1 - phi_child2;
      }
      else // we don't know how to handle this
      {
        vcl_cout << "\nERROR: Can't handle node with degree = " << xv->degree() << "\n";
        assert(false);
      }
      map_node2geom[xv->id()].push_back(xnode_geom);
    } // edge iterator
  } // index of xgraphs


  
  return true;
}









//------------------------------------------------------------------------------
//: Construct model from collected data
bool dbsks_train_geom_model::
construct_geom_model()
{
  // identify id of major child
  dbsksp_xshock_node_sptr xv_root = this->prototype_xgraph->node_from_id(this->params_.root_vid);
  dbsksp_xshock_edge_sptr xe_parent = this->prototype_xgraph->edge_from_id(this->params_.pseudo_parent_eid);
  dbsksp_xshock_edge_sptr xe_major_child = this->prototype_xgraph->cyclic_adj_succ(xe_parent, xv_root);
  unsigned major_child_eid = xe_major_child->id();

  // construct a geometric model
  this->xgraph_geom = new dbsks_xgraph_geom_model();
  xgraph_geom->build(this->prototype_xgraph, map_edge2frags, map_node2geom, 
    this->params_.root_vid, major_child_eid, 
    this->params_.normalized_xgraph_size);  

  return true;
}



//------------------------------------------------------------------------------
//: Save geom model to file
bool dbsks_train_geom_model::
save_geom_model_to_file()
{
  //> open output file for writing
  vcl_ofstream os(this->params_.output_file.c_str());
  if (!os)
  {
    vcl_cout << " ERROR: Couldn't open for writing file: " 
      << this->params_.output_file << ".\n";
    return false;
  }

  //> write out geom model
  x_write(os, xgraph_geom);
  

  //> close the file
  os.close();

  return true;
}



//------------------------------------------------------------------------------
//: Collect list of xgraph sizes in the exemplar xgraphs
bool dbsks_train_geom_model::
save_list_of_xgraph_sizes()
{
  vcl_string xshock_folder = this->params_.xshock_folder;
  vcl_string xshock_list_file = this->params_.xshock_list_file;
  vcl_string output_file = this->params_.output_file + ".scales.txt";

  //> Load the exemplar shock graphs
  vcl_cout << "\n> Load the exemplar xshock graphs from " << xshock_folder << "\n";
  
  // parse input file containing list of xshock graphs
  vcl_vector<vcl_string > xml_filenames;
  dbul_parse_string_list(xshock_list_file, xml_filenames);

  // Iterate thru the names and load one by one
  vcl_vector<dbsksp_xshock_graph_sptr > list_xgraph;
  list_xgraph.reserve(xml_filenames.size());
  for (unsigned index =0; index < xml_filenames.size(); ++index)
  {
    // filename
    vcl_string fname = xml_filenames[index];

    vcl_cout << "\n  Loading " << fname << "...";
    
    vcl_string xml_file = xshock_folder + "/" + xml_filenames[index];
    dbsksp_xshock_graph_sptr xgraph = 0;
    if ( x_read(xml_file, xgraph) )
    {
      vcl_cout << "[ OK ]";
      list_xgraph.push_back(xgraph);
    }
    else
    {
      vcl_cout << "[ Failed ].";
      continue;
    }
  }

  // Check-up
  if (list_xgraph.empty())
  {
    vcl_cout << "ERROR: No xgraph was loaded. \n";
    return false;
  }
  int num_xgraphs = list_xgraph.size();

  // Compute the xgraph scales and save to a vector
  vcl_vector<double > xgraph_sizes;
  xgraph_sizes.reserve(list_xgraph.size());
  for (unsigned i =0; i < list_xgraph.size(); ++i)
  {
    dbsksp_xshock_graph_sptr xgraph = list_xgraph[i];
    xgraph_sizes.push_back(vcl_sqrt(xgraph->area()));
  }
  vcl_cout << "\n>> Saving list of xgraph scales to file: " 
    << output_file << "\n";


  vcl_ofstream os(output_file.c_str(), vcl_ios_out);
  for (unsigned i =0; i < xgraph_sizes.size(); ++i)
  {
    os << xgraph_sizes[i] << "\n";
  }
  os.close();

  return true;
}











