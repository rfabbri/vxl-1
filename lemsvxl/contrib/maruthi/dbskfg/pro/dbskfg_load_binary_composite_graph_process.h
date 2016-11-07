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

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <vcl_set.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_2d.h>

class dbsk2d_ishock_edge;

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

  vcl_map<unsigned int,double > get_cgraph_area()
  {return area_cgraphs_;}

  vcl_map<unsigned int,double > get_cgraph_length()
  {return arc_length_cgraphs_;}

  vcl_map<unsigned int,vgl_polygon<double> > get_polygons()
  {return polygons_;}

  vcl_vector<unsigned int> get_frags_removed(){return frags_removed_;}

  bool compute_graph(vcl_vector<vsol_spatial_object_2d_sptr>& contours,
                     vcl_set<unsigned int>& contour_ids,
                     vcl_pair<unsigned int,unsigned int>& image_size,
                     bool prune_degree_three_nodes=true);
  
  bool compute_outer_shock(vidpro1_vsol2D_storage_sptr& input_vsol);

  void get_first_graph(dbskfg_composite_graph_sptr& pointer)
  {pointer=(*(cgraphs_.begin())).second;}

private: 
  
  void read_binary_file(vcl_string input_file, 
                        vcl_map<unsigned int,
                        vcl_vector< vsol_spatial_object_2d_sptr > >& geoms,
                        vcl_map<unsigned int,vcl_set<unsigned int> >& con_ids);

  bool compute_composite_graph(vidpro1_vsol2D_storage_sptr output_vsol,
                               vcl_set<unsigned int>& cons,
                               bool prune_degree_three_nodes=true,
                               bool outside_shock=false);

  void sample_outside_shock(dbsk2d_shock_graph_sptr shock_graph);

  void sample_shock_link(
      dbsk2d_ishock_edge* edge,
      vcl_vector<vcl_pair<vgl_point_2d<double>,double > >& left_curve,
      vcl_vector<vcl_pair<vgl_point_2d<double>,double > >& right_curve,
      dbsk2d_shock_graph_sptr pruned_graph,
      bool reverse);

  unsigned int image_ni_;
  unsigned int image_nj_;

  vcl_map<unsigned int,dbskfg_composite_graph_sptr > cgraphs_;
  vcl_map<unsigned int,double> area_cgraphs_;
  vcl_map<unsigned int,double> arc_length_cgraphs_;
  vcl_vector<unsigned int> frags_removed_;
  vcl_map<int,vcl_string> key_map_;
  vcl_map<vcl_pair<double,double>,double> kd_points_;
  vcl_map<unsigned int,vgl_polygon<double> > polygons_;

};

#endif  //dbskfg_load_binary_composite_graph_process_h_
