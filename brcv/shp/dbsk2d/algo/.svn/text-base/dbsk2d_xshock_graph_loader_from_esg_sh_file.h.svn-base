// This is brcv/shp/dbsk2d/algo/dbsk2d_xshock_graph_loader_from_esg_sh_file.h

#ifndef dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
#define dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
//:
// \file
// \brief Load and save extrinsic shock graphs from .esf .sh file pairs
// \author Fatih Calakli
// \date 05/21/08
//

#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
class Group{
public:
  int group_id;
  char group_type;
  char group_label[25];
  vcl_vector<int> ordered_shocks;
  vcl_vector<int> incoming_groups;
  vcl_vector<int> outgoing_groups;
  vcl_vector<float> incoming_speeds;
  vcl_vector<float> incoming_thetas;
  vcl_vector<float> outgoing_speeds;
  vcl_vector<float> outgoing_thetas;
};
class Shock{
public:
  int sample_id;
  float x;
  float y;
  float t;
  int order;
  int group_id;
  char label[25];
  vcl_vector<float> theta;
  vcl_vector<float> speed;
};
//: class to load and save extrinsic shock graph files
class dbsk2d_xshock_graph_loader_from_esg_sh_file
{
protected:
  char buffer[2000];
  vcl_ifstream fp_in;

  long last_sample_id;

  dbsk2d_shock_graph_sptr shock;

  vcl_vector<Group> group_list;
  
  vcl_map<int, Shock> shocks_map;
  vcl_map<int, dbsk2d_shock_node_sptr> nodes_map;
  vcl_map<int, vcl_pair<vcl_vector<int>,vcl_vector<int> > > edge_adjacency_map;
  vcl_map<int, dbsk2d_shock_edge_sptr > edges_map;
  vcl_map<dbsk2d_shock_edge_sptr, vcl_vector<int> > edge_samples_map;
  vcl_map<vcl_pair<int, int>, dbsk2d_xshock_sample_sptr> samples_map;
 
public:
  //: Constructor
  dbsk2d_xshock_graph_loader_from_esg_sh_file(); 
  //:Destructor
  ~dbsk2d_xshock_graph_loader_from_esg_sh_file();

  //: clear all the cached information
  void clear_all();

  //: Load an extrinsic shock graph from an .esf file.
  // \relates dbsk2d_shock_graph
  dbsk2d_shock_graph_sptr load_xshock_graph(vcl_string esg_filename, vcl_string sh_filename);
  void load_esg_file_info(vcl_string filename);
  void load_sh_file_info(vcl_string filename);
  void load_xshock_nodes_and_edges();
  dbsk2d_xshock_sample_sptr load_xshock_sample(int sample_id, int edge_id, float speed, float theta);
  dbsk2d_xshock_sample_sptr load_xshock_sample(int sample_id, int edge_id);
  void assign_edge_samples();
  void setup_connectivity_between_nodes_and_edges();
  void finish_load();
};

#endif //dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
