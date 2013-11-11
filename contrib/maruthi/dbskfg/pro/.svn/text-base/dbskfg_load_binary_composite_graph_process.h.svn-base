// This is brcv/shp/dbskfg/pro/dbskfg_load_binary_composite_graph_process.h
#ifndef dbskfg_load_binary_composite_graph_process_h_
#define dbskfg_load_binary_composite_graph_process_h_

//:
// \file
// \brief This process load_binarys a composite graph from a xml file 
//
// \author Maruthi Narayanan
// \date 09/26/10
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <vcl_set.h>
#include <vcl_utility.h>

class dbskfg_load_binary_composite_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_load_binary_composite_graph_process();
  
  //: Destructor
  virtual ~dbskfg_load_binary_composite_graph_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  vcl_map<unsigned int,dbskfg_composite_graph_sptr > get_cgraphs()
  {return cgraphs_;}

  vcl_vector<unsigned int> get_frags_removed(){return frags_removed_;}

  bool compute_graph(vcl_vector<vsol_spatial_object_2d_sptr>& contours,
                     vcl_set<unsigned int>& contour_ids,
                     vcl_pair<unsigned int,unsigned int>& image_size,
                     bool prune_degree_three_nodes=true);

  void get_first_graph(dbskfg_composite_graph_sptr& pointer)
  {pointer=(*(cgraphs_.begin())).second;}

private: 
  
  void read_binary_file(vcl_string input_file, 
                        vcl_map<unsigned int,
                        vcl_vector< vsol_spatial_object_2d_sptr > >& geoms,
                        vcl_map<unsigned int,vcl_set<unsigned int> >& con_ids);

  bool compute_composite_graph(vidpro1_vsol2D_storage_sptr output_vsol,
                               vcl_set<unsigned int>& cons,
                               bool prune_degree_three_nodes=true);

  unsigned int image_ni_;
  unsigned int image_nj_;

  vcl_map<unsigned int,dbskfg_composite_graph_sptr > cgraphs_;

  vcl_vector<unsigned int> frags_removed_;

};

#endif  //dbskfg_load_binary_composite_graph_process_h_
