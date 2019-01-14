// This is brcv/shp/dbsk2d/algo/dbsk2d_xshock_graph_loader_from_esg_sh_file.h

#ifndef dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
#define dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
//:
// \file
// \brief Load and save extrinsic shock graphs from .esf .sh file pairs
// \author Fatih Calakli
// \date 05/21/08
//

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
class Group{
public:
  int group_id;
  char group_type;
  char group_label[25];
  std::vector<int> ordered_shocks;
  std::vector<int> incoming_groups;
  std::vector<int> outgoing_groups;
  std::vector<float> incoming_speeds;
  std::vector<float> incoming_thetas;
  std::vector<float> outgoing_speeds;
  std::vector<float> outgoing_thetas;
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
  std::vector<float> theta;
  std::vector<float> speed;
};
//: class to load and save extrinsic shock graph files
class dbsk2d_xshock_graph_loader_from_esg_sh_file
{
protected:
  char buffer[2000];
  std::ifstream fp_in;

  long last_sample_id;

  dbsk2d_shock_graph_sptr shock;

  std::vector<Group> group_list;
  
  std::map<int, Shock> shocks_map;
  std::map<int, dbsk2d_shock_node_sptr> nodes_map;
  std::map<int, std::pair<std::vector<int>,std::vector<int> > > edge_adjacency_map;
  std::map<int, dbsk2d_shock_edge_sptr > edges_map;
  std::map<dbsk2d_shock_edge_sptr, std::vector<int> > edge_samples_map;
  std::map<std::pair<int, int>, dbsk2d_xshock_sample_sptr> samples_map;
 
public:
  //: Constructor
  dbsk2d_xshock_graph_loader_from_esg_sh_file(); 
  //:Destructor
  ~dbsk2d_xshock_graph_loader_from_esg_sh_file();

  //: clear all the cached information
  void clear_all();

  //: Load an extrinsic shock graph from an .esf file.
  // \relates dbsk2d_shock_graph
  dbsk2d_shock_graph_sptr load_xshock_graph(std::string esg_filename, std::string sh_filename);
  void load_esg_file_info(std::string filename);
  void load_sh_file_info(std::string filename);
  void load_xshock_nodes_and_edges();
  dbsk2d_xshock_sample_sptr load_xshock_sample(int sample_id, int edge_id, float speed, float theta);
  dbsk2d_xshock_sample_sptr load_xshock_sample(int sample_id, int edge_id);
  void assign_edge_samples();
  void setup_connectivity_between_nodes_and_edges();
  void finish_load();
};

#endif //dbsk2d_xshock_graph_loader_from_esg_sh_file_h_
