// This is dbsks/cmdline/dbsks_xfrag_cost_map.cxx

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 18, 2010


//#include "dbsks_cmdline_utils.h"
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vcl_cstdlib.h>
#include <dbsks/algo/dbsks_load.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

#include <dbsks/dbsks_xshock_ccm_likelihood.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_biarc_sampler_sptr.h>
#include <dbsks/dbsks_subpix_ccm.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>


//: Command line programs for the sks class
int main(int argc, char *argv[]) 
{
  bool is_ccm = true;

  // Arguments
  vul_arg_info_list arg_list;
  
  // Path to xgraph xml file
  vul_arg<vcl_string > xgraph_file(arg_list,"-xgraph-file", "Path to xgraph.xml file", 
    "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph/giraffes_one.xgraph.0.xml");

  // CCM model file
  vul_arg<vcl_string > xgraph_ccm_file(arg_list,"-xgraph-ccm-file", "Path to xgraph ccm model file", 
    "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/swans/swans_prototype_0-xshock_ccm_model-2010apr08.xml");

  
  // Id of root node of xgraph
  vul_arg<unsigned > root_vid(arg_list, "-root-vid", "Id of root node", 17);

  // Id of the edge / fragment to compute cost
  vul_arg<unsigned > xfrag_eid(arg_list, "-xfrag-eid", "Id of edge to compute cost", 21);

  // Path to edge image file
  vul_arg<vcl_string > edge_image(arg_list,"-edge-image", "Path to edge image file (_edges.png)", 
    "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4/giraffes_one/giraffes_one_00_pb_edges.png");
  
  // Path to edge orientation file
  vul_arg<vcl_string > edge_orient(arg_list,"-edge-orient", "Path to edge orient file (_orient.txt)", 
    "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4/giraffes_one/giraffes_one_00_pb_orient.txt");

  // Prefix to output file(s)
  vcl_string prefix_name = "";
  if (is_ccm)
  {
    prefix_name = "two-edge-ccm-two-side-2010may26";
  }
  else
  {
    prefix_name = "two-edge-ocm-two-side-2010may26";
  }
  vul_arg<vcl_string > output_prefix(arg_list,"-output-prefix", "Prefix to all output files", 
    "D:/vision/projects/symseg/xshock/" + prefix_name);

  // print help
  vul_arg<bool > print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool > print_help(arg_list,"-help", "print usage info and exit",false);

  // parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  //----------------------------------------------------------------------------
  if (print_help() || print_usage_only()) 
  {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Process

  // start the timer
  vul_timer timer;
  timer.mark();


  bool load_ok = true;

  //1) Load xgraph
  dbsksp_xshock_graph_sptr xgraph = 0;
  load_ok &= dbsks_load_xgraph(xgraph_file(), xgraph);

  double xgraph_size = vcl_sqrt(xgraph->area());
  vcl_cout << "Xgraph size = " << xgraph_size << "\n";

  //2) Load xgraph ccm model
  dbsks_xgraph_ccm_model_sptr xgraph_ccm = 0;
  load_ok &= dbsks_load_xgraph_ccm_model(xgraph_ccm_file(), xgraph_ccm);

  //3) Computer vertex depth
  load_ok &= xgraph->compute_vertex_depths(root_vid());

  

  //5) Load edgemap
  dbdet_edgemap_sptr edgemap = dbsks_load_subpix_edgemap(edge_image(), edge_orient());
  load_ok &= (!!edgemap);

  if (!load_ok)
  {
    vcl_cout << "\nERROR: Failed to load necessary data.\n";
    return EXIT_FAILURE;
  }

  ////4) Determine targeting shock edge
  //dbsksp_xshock_edge_sptr xe = xgraph->edge_from_id(xfrag_eid());
  //load_ok &= (!!xe);

  ////5) Determine geometry of the target edge
  //dbsksp_xshock_node_descriptor start = *xe->parent_node()->descriptor(xe);
  //dbsksp_xshock_node_descriptor end   = xe->child_node()->descriptor(xe)->opposite_xnode();
  //dbsksp_xshock_fragment xfrag(start, end);

  


  //> Construct a biarc sampler
  dbsks_biarc_sampler_sptr biarc_sampler = new dbsks_biarc_sampler();
  xgraph_ccm->build_biarc_sampler(*biarc_sampler);
  biarc_sampler->compute_cache_sample_points();

  // coordinates of the biarc points
  vcl_vector<int > x_vec[2], y_vec[2], angle_vec[2];
 
  vcl_vector<unsigned > edge_ids;
  edge_ids.push_back(21);
  edge_ids.push_back(22);

  for (unsigned k =0; k < edge_ids.size(); ++k)
  {
    unsigned eid = edge_ids[k];
    dbsksp_xshock_edge_sptr temp_xe = xgraph->edge_from_id(eid);
    
    dbsksp_xshock_node_descriptor temp_start = *temp_xe->parent_node()->descriptor(temp_xe);
    dbsksp_xshock_node_descriptor temp_end   = temp_xe->child_node()->descriptor(temp_xe)->opposite_xnode();

    dbsksp_xshock_fragment xfrag(temp_start, temp_end);
    xfrag.rotate(centre(temp_start.pt(), temp_end.pt()), -vnl_math::pi / 12);
    temp_start = xfrag.start();
    temp_end = xfrag.end();

    vcl_vector<int > temp_x_vec[2], temp_y_vec[2], temp_angle_vec[2];

    // left boundary
    biarc_sampler->compute_samples_using_cache(temp_start.bnd_pt_left(), temp_start.bnd_tangent_left(),
      temp_end.bnd_pt_left()  , temp_end.bnd_tangent_left(), 
      temp_x_vec[0], temp_y_vec[0], temp_angle_vec[0]);

    // right boundary
    biarc_sampler->compute_samples_using_cache(temp_start.bnd_pt_right(), temp_start.bnd_tangent_right(), 
      temp_end.bnd_pt_right(),   temp_end.bnd_tangent_right(),
      temp_x_vec[1], temp_y_vec[1], temp_angle_vec[1]);

    x_vec[0].insert(x_vec[0].end(), temp_x_vec[0].begin(), temp_x_vec[0].end());
    x_vec[1].insert(x_vec[1].end(), temp_x_vec[1].begin(), temp_x_vec[1].end());

    y_vec[0].insert(y_vec[0].end(), temp_y_vec[0].begin(), temp_y_vec[0].end());
    y_vec[1].insert(y_vec[1].end(), temp_y_vec[1].begin(), temp_y_vec[1].end());

    angle_vec[0].insert(angle_vec[0].end(), temp_angle_vec[0].begin(), temp_angle_vec[0].end());
    angle_vec[1].insert(angle_vec[1].end(), temp_angle_vec[1].begin(), temp_angle_vec[1].end());
  }

  //) Construct a ccm cost calculator
  
  // extract param values from ccm model
  dbsks_ccm_params params = xgraph_ccm->ccm_params();

  //
  params.distance_threshold_ = 6;
  params.distance_tol_near_zero_ = 0;
  
  params.orient_tol_near_zero_ = vnl_math::pi / 2;
  params.orient_tol_near_zero_ = 0;
  

  if (is_ccm)
  {
    params.weight_chamfer_ = 0.2;
    params.weight_edge_orient_ = 0.1;
    params.weight_contour_orient_ = 0.7;
  }
  else
  {
    params.weight_chamfer_ = 0.667;
    params.weight_edge_orient_ = 0.333;
    params.weight_contour_orient_ = 0;
  }

  

  
  // apply the params to ccm
  dbsks_subpix_ccm ccm;
  ccm.set_distance_params(params.distance_threshold_, params.distance_tol_near_zero_);
  ccm.set_orient_params(params.orient_threshold_, params.orient_tol_near_zero_);
  ccm.set_nbins_0topi(params.nbins_0topi_);
  ccm.set_local_window_width(params.local_window_width_);
  ccm.set_weights(params.weight_chamfer_, params.weight_edge_orient_, params.weight_contour_orient_);

  // set data source to ccm
  ccm.set_edgemap(edgemap);

  // compute cost map of ccm for the region-of-interest
  vgl_box_2d<int > bbox(0, edgemap->width()-1, 0, edgemap->height()-1);
  ccm.compute(bbox);

  float sum =0;
  for (int side =0; side < 2; ++side)
  {
    unsigned num_pts = x_vec[side].size();
    
    float ccm_cost = ccm.f(x_vec[side], y_vec[side],  angle_vec[side]);
    sum += ccm_cost;
  }

  vcl_cout << "\nOriginal xgraph: sum = " << sum << "\n";
  


  // Move the fragment around and compute the costs

  // First compute the bounding box of the fragment
  vgl_box_2d<int > xfrag_bbox;
  for (int side =0; side < 2; ++side)
  {
    unsigned num_pts = x_vec[side].size();
    for (unsigned i =0; i < num_pts; ++i)
    {
      vgl_point_2d<int > pt(x_vec[side][i], y_vec[side][i]);
      xfrag_bbox.add(pt);
    }
  }


  // Move the fragment so that its top-left corner is at (10, 10) (avoid the edge)
  int cx = xfrag_bbox.centroid_x();
  int cy = xfrag_bbox.centroid_y();

  //int tx = -xfrag_bbox.min_x() + 10;
  //int ty = -xfrag_bbox.min_y() + 10;
  
  int tx = -cx;
  int ty = -cy;

  for (int side =0; side < 2; ++side)
  {
    unsigned num_pts = x_vec[side].size();
    for (unsigned i =0; i < num_pts; ++i)
    {
      x_vec[side][i] += tx;
      y_vec[side][i] += ty;
    }
  }
  cx += tx;
  cy += ty;
  

  // Compute the allowed range to move the fragment
  //int max_dx = bbox.width() - 20 - xfrag_bbox.width();
  //int max_dy = bbox.height() - 20 - xfrag_bbox.height();

  int max_dx = bbox.width();
  int max_dy = bbox.height();


  // translate the points, keep the angles
  vcl_vector<int > cur_x_vec[2], cur_y_vec[2];
  cur_x_vec[0] = x_vec[0];
  cur_x_vec[1] = x_vec[1];

  cur_y_vec[0] = y_vec[0];
  cur_y_vec[1] = y_vec[1];


  vnl_matrix<float > xfrag_ccm_cost(vnl_math_ceil(bbox.width()/4.0), vnl_math_ceil(bbox.height()/4.0), 2.0f);

  for (int dx = 0; dx < max_dx; dx = dx + 4)
  {
    for (int dy = 0; dy < max_dy; dy = dy + 4)
    {
      // translate the points, keep the angles
      float sum_cost = 0;
      for (int side =0; side < 2; ++side)
      {
        unsigned num_pts = x_vec[side].size();
        for (unsigned i =0; i < num_pts; ++i)
        {
          cur_x_vec[side][i] = x_vec[side][i] + dx;
          cur_y_vec[side][i] = y_vec[side][i] + dy;
        }

        float ccm_cost = ccm.f(cur_x_vec[side], cur_y_vec[side],  angle_vec[side]);
        sum_cost += ccm_cost;
      }

      xfrag_ccm_cost(cx + dx/4, cy + dy/4) = sum_cost; 
    }
  }

  vcl_string out_file = output_prefix() + "-cost.txt";
  vcl_ofstream ofs(out_file.c_str(), vcl_ios_out);
  xfrag_ccm_cost.print(ofs);
  ofs.close();


  return EXIT_SUCCESS;
}





