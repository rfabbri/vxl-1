// This is brcv/shp/dbsk2d/algo/dbsk2d_xshock_graph_fileio.h
#ifndef dbsk2d_xshock_graph_fileio_h_
#define dbsk2d_xshock_graph_fileio_h_
//:
// \file
// \brief Load and save extrinsic shock graphs from/to .esf files
// \author Amir Tamrakar
// \date 02/23/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 07/07/2005    Moved to dbsk2d/algo
//   Ozge C Ozcanli Jan 08, 2007 Change in write_xshock_sample() method:
//                               In older .esf files "boundarypoints" is written as "boundaryPoints" with capital P 
//                               changed to boundaryPoints to be competible with old files
//   
//                               Called form_shock_fragments() method for the nodes 
//               
//   Ozge C Ozcanli Apr 08, 2008 Fixed a bug in loader
//                               edges with same source and target id's were treated as same edges while creating adjacency list of nodes
//                               used the sample id information in the node description to break the ambiguity
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>

//: class to load and save extrinsic shock graph files
class dbsk2d_xshock_graph_fileio
{
protected:
  char buffer[2000];
  vcl_ifstream fp_in;
  vcl_ofstream fp_out;

  dbsk2d_shock_graph_sptr shock;
  int num_nodes;
  int num_edges;
  int num_samples;

  int version; ///> version of the extrinsic shock file

  vcl_map<int, dbsk2d_xshock_sample_sptr> samples_map;
  vcl_map<int, dbsk2d_shock_node_sptr> nodes_map;
  
  //: ozge made into the following vector type //vcl_map<int, vcl_vector<int> > node_adjacency_map;
  vcl_map<int, vcl_vector<vcl_pair<int, int> > > node_adjacency_map;
  //: ozge made into the following vector type //vcl_map<vcl_pair<int, int>, dbsk2d_shock_edge_sptr> edges_map;
  vcl_map<vcl_pair<int, vcl_pair<int, int> >, dbsk2d_shock_edge_sptr> edges_map;
  vcl_map<dbsk2d_shock_edge_sptr, vcl_vector<int> > edge_samples_map;

public:

  //: Constructor
  dbsk2d_xshock_graph_fileio();

  //:Destructor
  ~dbsk2d_xshock_graph_fileio();

  //: clear all the cached information
  void clear_all();

  //: Load an extrinsic shock graph from an .esf file.
  // \relates dbsk2d_shock_graph
  dbsk2d_shock_graph_sptr load_xshock_graph(vcl_string filename);

  void load_xshock_header();
  void load_xshock_node_description();
  void load_xshock_node_samples();
  dbsk2d_xshock_sample_sptr load_xshock_sample();
  void load_xshock_edge_description();
  void load_xshock_edge_samples();
  void assign_edge_samples();
  void setup_connectivity_between_nodes_and_edges();
  void finish_load();

  //: Save an extrinsic shock graph to disk (.esf file)
  // \relates dbsk2d_shock_graph
  bool save_xshock_graph(dbsk2d_shock_graph_sptr shock_graph, vcl_string filename);

  void write_xshock_header();
  void write_xshock_node_description();
  void write_xshock_node_samples();
  void write_xshock_sample (dbsk2d_xshock_sample_sptr sample);
  void write_xshock_edge_description();
  void write_xshock_edge_samples();
};

#endif //dbsk2d_xshock_graph_fileio_h_
