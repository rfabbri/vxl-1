// This is dbsks/algo/dbsks_detect_xgraph.cxx

//:
// \file

#include "dbsks_detect_xgraph.h"


#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_nms.h>
#include <dbsks/dbsks_shotton_ocm.h>
#include <dbsks/dbsks_gray_ocm.h>
#include <dbsks/dbsks_ccm.h>
#include <dbsks/dbsks_wcm.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_detector.h>
#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/dbsks_xshock_wcm_likelihood.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsks/xio/dbsks_xio_xgraph_ccm_model.h>

#include <dbsks/dbsks_xshock_ccm_likelihood.h>
#include <dbsks/algo/dbsks_load.h>
#include <dbsks/algo/dbsks_algos.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>


#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbsol/dbsol_file_io.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>

#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_read.h>
#include <vnl/vnl_file_matrix.h>
#include <vul/vul_timer.h>
#include <vcl_utility.h>












// -----------------------------------------------------------------------------
//: Detect an object, represented as a shock graph, in an image
bool dbsks_detect_xgraph(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         float ocm_edge_threshold,
                         //const vcl_string& cfg_file,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_vector<double >& xgraph_scales,
                         int root_vid,
                         int major_child_eid,
                         float ocm_lambda,
                         float ocm_gamma,
                         float ocm_tol_near_zero,
                         float ocm_distance_threshold,
                         vil_image_resource_sptr& image_resource, 
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& list_det)
{
  // load the image
  vcl_cout << "Loading image: " << image_file << " ... ";
  vil_image_view<float > image_src = *vil_convert_cast(float(), vil_load(image_file.c_str()));
  
  if (!image_src) 
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else 
  {
    vcl_cout << "Succeeded.\n";
  }

  // \TODO we probably won't need this later /////////////////////////////
  image_resource = vil_load_image_resource(image_file.c_str());
  /////////////////////////////////////////////////////////////////////////

  //>>> load the edge map
  vcl_cout << "Loading edgemap: " << edgemap_file << " ... ";
  vil_image_view<float > edgemap = *vil_convert_cast(float(), vil_load(edgemap_file.c_str()));
  if (!edgemap) 
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else 
  {
    vcl_cout << "Succeeded.\n";
  }

  //>>> Load edge orientation file
  vnl_file_matrix<double > theta(edgeorient_file.c_str());
  bool loaded_edgeorient = !theta.empty();



  if (!loaded_edgeorient)
  {
    vcl_cout << "\nERROR: couldn't load edge orientation file: " << edgeorient_file << vcl_endl;
    return false;
  }

  if (edgemap.nj() != theta.rows() || edgemap.ni() != theta.cols())
  {
    vcl_cout << "\nERROR: edgemap and edge orientation map do not have the same dimension." << vcl_endl;
    return false;
  }


  // create edge angle image view from a matrix
  vil_image_view<float > edge_angle(edgemap.ni(), edgemap.nj());
  for (unsigned i =0; i < edge_angle.ni(); ++i)
  {
    for (unsigned j =0; j < edge_angle.nj(); ++j)
    {
      edge_angle(i, j) = float(theta(j, i));
    }
  }

  



  ///////////////////////////////////////////////////////////////////////////

  //// load the CFG file
  //vcl_cout << "\nLoading curve fragment graph (.cem): " << cfg_file << "...";
  //dbdet_curve_fragment_graph CFG;
  //dbdet_edgemap_sptr EM = dbdet_load_cem(cfg_file, CFG);
  //if (!EM) 
  //{
  //  vcl_cout << "Failed.\n";
  //  return false;
  //}
  //else
  //{
  //  vcl_cout << "Done.\n";
  //}

  // Load the shock graph
  dbsksp_xshock_graph_sptr xgraph = 0;

  vcl_cout << "Loading xshock_graph XML file: " << xgraph_file << "...";
  if ( x_read(xgraph_file, xgraph) )
  {
    vcl_cout << "Succeeded.\n";
  }
  else
  {
    vcl_cout << "Failed.\n";
    return false;
  } 


  // Load the xgraph geometric model
  dbsks_xgraph_geom_model_sptr xgraph_geom = 0;
  if (!x_read(xgraph_geom_file, xgraph_geom))
  {
    vcl_cout << "ERROR: Couldn't load xgraph geometric model from file " << xgraph_geom_file << vcl_endl;
    return false;
  }


  // \todo Make sure the geometric model is compatible with the shock graph
  bool compatible = true;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator mit = 
      xgraph_geom->map_edge2geom().find((*eit)->id());
    if (mit == xgraph_geom->map_edge2geom().end())
    {
      compatible = false;
      break;
    }
  }



  if (!compatible)
  {
    vcl_cout << "\nERROR !!! xfrag models are not compatible with the xgraph." << vcl_endl;
    return false;
  
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Detect xgraphs
  list_det.clear();
  double def_scale = vcl_sqrt(xgraph->area());
  if (xgraph_scales.empty())
  {
    vcl_cout << "\n>>>>> Processing default xgraph scale = " << def_scale << vcl_endl;
    dbsks_detect_xgraph(image_src, edgemap, edge_angle, 
      ocm_edge_threshold,
      //&CFG, 
      xgraph, def_scale, 
      root_vid, major_child_eid, 
      ocm_lambda, ocm_gamma, ocm_tol_near_zero, ocm_distance_threshold,
      xgraph_geom, list_det);
  }
  else
  {
    for (unsigned i =0; i < xgraph_scales.size(); ++i)
    {
      vcl_cout << "\n>>>>> Processing xgraph scale = " << xgraph_scales[i] << vcl_endl;

      vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
      dbsks_detect_xgraph(image_src, edgemap, edge_angle, 
        ocm_edge_threshold,
        //&CFG, 
        xgraph, xgraph_scales[i], root_vid, major_child_eid, 
        ocm_lambda, ocm_gamma, ocm_tol_near_zero, ocm_distance_threshold,
        xgraph_geom, dets);
      list_det.insert(list_det.end(), dets.begin(), dets.end());
    }
  }
  return true;
}









// -----------------------------------------------------------------------------
//: Detect xgraph of in an iamge at a particular scale
bool dbsks_detect_xgraph(const vil_image_view<float >& image_src,
                         const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         float ocm_edge_threshold,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         //dbdet_curve_fragment_graph* CFG,
                         double xgraph_scale,
                         int root_vid,
                         int major_child_eid,
                         float ocm_lambda,
                         float ocm_gamma,
                         float ocm_tol_near_zero,
                         float ocm_distance_threshold,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& det_list)
{
  // ///////////////////////////////////////////////////////////////////////////
  // confidence threshold to reject a detection
  double confidence_lower_threshold = 0.01;

  // if one bbox overlaps with another bbox with higher confidence more than 
  // "min_overlap_ratio_for_rejection" of its area then that bbox is rejected.
  double min_overlap_ratio_for_rejection = 0.64;

  // Shotton OCM --------------------------
  int nchannel = 18;                           
  // --------------------------------------- 


  // ///////////////////////////////////////////////////////////////////////////

  // >> Computing cost of Oriented Chamfer Maching
  dbsks_ocm_image_cost_sptr ocm = new dbsks_ocm_image_cost();

  ocm->set_edgemap(edgemap, ocm_edge_threshold);

  // standard deviations oriented chamfer matching
  float sigma_distance = 8;
  float sigma_angle = (float) vnl_math::pi/6;
  ocm->set_sigma(sigma_distance, sigma_angle);

  // ///////////////////////////////////////////////////////////////////////////

  // >> Shotton-OCM cost function
  vcl_cout << "\nConstructing Shotton-OCM cost function ...";
  vul_timer timer;
  timer.mark();

  dbsks_shotton_ocm shotton_ocm;
  shotton_ocm.set_edgemap(edgemap);
  shotton_ocm.set_ocm_params(ocm_edge_threshold, ocm_distance_threshold, ocm_tol_near_zero, nchannel);
  shotton_ocm.set_lambda(ocm_lambda);

  // pre-compute chamfer cost and orientation cost at every pixel and orientation

  // IMPORTANT: uncomment this to use shotton_ocm in image cost
  //shotton_ocm.compute(); 
  vcl_cout << "done" << vcl_endl;;
  vcl_cout << "    Time spent = ";
  timer.print(vcl_cout);

  //////////////////////////////////////////////////////////////////////////////
  // >> Gray-value OCM cost function
  vcl_cout << "\nConstructing gray-OCM cost function ...";
  timer.mark();

  dbsks_gray_ocm gray_ocm;
  gray_ocm.set_image(image_src);
  gray_ocm.set_edgemap(edgemap, ocm_edge_threshold, 255);
  gray_ocm.set_lambda(ocm_lambda);
  gray_ocm.set_ocm_params(ocm_distance_threshold, ocm_tol_near_zero, nchannel);
  
  // pre-compute chamfer cost and orientation cost at every pixel and orientation
  //gray_ocm.compute();
  vcl_cout << "done" << vcl_endl;;
  vcl_cout << "    Time spent = ";
  timer.print(vcl_cout);


  //////////////////////////////////////////////////////////////////////////////
  //>> Contour OCM cost function
  vcl_cout << "\nConstructing contour-OCM cost function ...";
  timer.mark();

  dbsks_ccm ccm;
  ccm.set_ocm_params(ocm_distance_threshold, ocm_tol_near_zero, ocm_lambda, ocm_gamma, nchannel);
  ccm.set_edge_strength(edgemap, ocm_edge_threshold, 255);
  ccm.set_edge_orient(edge_angle);
  ccm.compute();

  vcl_cout << "done" << vcl_endl;;
  vcl_cout << "    Time spent = ";
  timer.print(vcl_cout);



  ////////////////////////////////////////////////////////////////////////////////
  //// >> CFG OCM cost function
  //vcl_cout << "\nConstructing CFG OCM";
  //timer.mark();

  //dbsks_cfg_ocm cfg_ocm;


  ////////////////////////////////////////////////////////////////////////////////

  //>>> Biarc sampler
  vcl_cout << "\nConstructing a biarc sampler ...";
  timer.mark();

  // Set parameters of biarc sampler
  dbsks_biarc_sampler_params bsp;
  bsp.step_dx = 8;
  bsp.num_dx = 29;
  bsp.min_dx = -bsp.step_dx * (bsp.num_dx-1)/2;
  
  bsp.step_dy = 8;
  bsp.num_dy = 29;
  bsp.min_dy = -bsp.step_dy * (bsp.num_dy-1)/2;

  bsp.step_alpha0 = vnl_math::pi/17;
  bsp.num_alpha0 = 9;
  bsp.min_alpha0 = -bsp.step_alpha0*(bsp.num_alpha0-1)/2;
  
  bsp.step_alpha2 = vnl_math::pi/17;
  bsp.num_alpha2 = 9;
  bsp.min_alpha2 = -bsp.step_alpha2*(bsp.num_alpha2-1)/2;

  // Define a biarc sampler
  dbsks_biarc_sampler biarc_sampler;

  // compute coordinates of the grid points
  biarc_sampler.set_grid(bsp);

  // compute prototype sample points for all biarcs in the grid
  double ds = 2;
  int num_bins = 36;
  biarc_sampler.set_sampling_params(num_bins, ds);
  biarc_sampler.compute_cache_sample_points();

  vcl_cout << " done" << vcl_endl;
  vcl_cout << "    Total time spent = ";
  timer.print(vcl_cout);

  // scaled version of the xgraph
  double cur_scale = vcl_sqrt(xgraph->area());
  dbsksp_xshock_graph_sptr scaled_xgraph = new dbsksp_xshock_graph(*xgraph);
  scaled_xgraph->similarity_transform( (*scaled_xgraph->vertices_begin())->pt(), 0, 0, 0, xgraph_scale / cur_scale);
  scaled_xgraph->update_all_degree_1_nodes();


  // Use sliding window techniques for large images

  // >> Compute location of windows to detect objects in
  vcl_cout << "\n>>> Compute location of windows to detect objects in." << vcl_endl;

  vcl_vector<vgl_box_2d<int > > windows;
  int ni = image_src.ni();
  int nj = image_src.nj();
  int max_width = 512;
  int max_height = 512;

  int cur_xmin = 8;
  do
  {
    int cur_ymin = 8;
    do
    {
      // determine window dimension
      int xmax = vnl_math_min(cur_xmin + max_width-1, ni-8); // padding on right
      int ymax = vnl_math_min(cur_ymin + max_height-1, nj-8); // padding at bottom
      int xmin = vnl_math_max(xmax-max_width+1, 8);
      int ymin = vnl_math_max(ymax-max_height+1, 8);
      windows.push_back(vgl_box_2d<int >(xmin, xmax, ymin, ymax));

      // update
      cur_ymin += max_height/2;
    }
    while (cur_ymin < (nj-8-max_height/2)); // padding on top
    cur_xmin += max_width/2;
  }
  while (cur_xmin < (ni-8-max_width/2)); // padding in front

  vcl_cout << "\nTotal number of windows to run on = " << windows.size() << "\n";
  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "  window " << iw << ": xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << "\n";
  }
  vcl_cout.flush();

  // >> Detect objects within each window

  vcl_cout << "\n>>>Object detection on each of the windows" << vcl_endl;
  det_list.clear();
  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "\nWindow index = " << iw << " xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << vcl_endl;

    // >> xshock detection engine
    dbsks_xshock_detector engine;
    //engine.image_src_ = image_src;
    engine.ocm_ = ocm;
    engine.shotton_ocm_ = &shotton_ocm;
    engine.gray_ocm_ = &gray_ocm;
    engine.ccm_ = &ccm;

    engine.biarc_sampler_ = &biarc_sampler;
    engine.xgraph_geom_ = xgraph_geom;
    engine.set_xgraph(scaled_xgraph);
    engine.root_vid_ = root_vid;
    engine.major_child_eid_ = major_child_eid;
    engine.detect(window);

    // construct a vector of detection descriptor
    for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
    {
      dbsksp_xshock_graph_sptr xgraph = engine.list_solutions_[i];
      double confidence = 1-(engine.list_solution_costs_[i] / xgraph->number_of_edges());
      double real_confidence = 1-(engine.list_solution_real_costs_[i] / xgraph->number_of_edges());

      // \Debug //////////////////////////////////////////////
      if (vnl_math_abs(real_confidence - confidence) > 0.001)
      {
        vcl_cout << "\nWarning: inconsistency. sol_idx=" << i << " confidence=" << confidence 
          << " --real_confidence=" << real_confidence << vcl_endl;
      }

      if (confidence > confidence_lower_threshold)
      {
        dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(xgraph, confidence);
        det->compute_bbox();
        det_list.push_back(det);
      }
      else
      {
        vcl_cout << "\n Detection result rejected. Confidence = " << confidence << vcl_endl;
      }
    } // solution
  } // iw
  vcl_cout << "\nNumber of detection before non-max suppression based on bounding box: " 
    << det_list.size() << vcl_endl;


  //////////////////////////////////////////////////////////////////////////////
  // Non-max supression on the bounding box

  vcl_cout << "\nNon-max suppression based on bounding box: \n"
    << det_list.size() << vcl_endl;

  // first sort the detections by their confidence level
  vcl_vector<dborl_det_desc_sptr > temp_det_list;
  temp_det_list.reserve(det_list.size());
  for (unsigned i =0; i < det_list.size(); ++i)
  {
    temp_det_list.push_back(det_list[i].ptr());
  }

  // a vector of non-max indicators for the detection (= true if non-max)
  vcl_vector<bool > to_remove(temp_det_list.size(), false);
  if (temp_det_list.size() > 1)
  {
    // first sort the detection result based on their score
    vcl_sort(temp_det_list.begin(), temp_det_list.end(), dborl_decreasing_confidence);

    // starting from the detection with lowest confidence and move up. If it overlaps 
    // significantly with another detection with higher confidence then ignore this detection
    for (unsigned i = (temp_det_list.size()-1); i > 0; --i) // we will keep the one with highest confidence
    {
      dborl_det_desc_sptr det0 = temp_det_list[i];
      
      if (det0->bbox()->area() < 1) // reject too-small objects (rarely happens)
      {
        to_remove[i] = true;
      }

      for (unsigned j =0; j < i; ++j)
      {
        dborl_det_desc_sptr det1 = temp_det_list[j];

        // if det0 overlaps significantly with det1, reject det0 (since det1 has higher confidence level)
        vsol_box_2d_sptr intersect = 0;
        if (bsol_algs::intersection(det0->bbox(), det1->bbox(), intersect) &&           
          (intersect->area() / det0->bbox()->area()) > min_overlap_ratio_for_rejection)
        {
          to_remove[i] = true;
          break;
        }
      } // for j
    } // for i
  } // if det_list.size

  // Now construct a new detection list suppressing all the non-max objects
  det_list.clear();
  for (unsigned i =0; i < temp_det_list.size(); ++i)
  {
    if (!to_remove[i])
    {
      det_list.push_back(static_cast<dbsks_det_desc_xgraph* >(temp_det_list[i].ptr()));
    }
  }
  
  vcl_cout << "\nNumber of detection after non-max suppression based on bounding box= " 
    << det_list.size() << vcl_endl; 
  
  return true;
}







////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_ccm(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_string& xgraph_ccm_file,
                         const vcl_vector<double >& xgraph_scales,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list)
{
  //>>> load the image
  vcl_cout << "\n>>Loading image: " << image_file << " ... ";
  vil_image_view<float > image_src = *vil_convert_cast(float(), vil_load(image_file.c_str()));
  
  if (!image_src) 
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else 
  {
    vcl_cout << "Succeeded.\n";
  }

  //>>> load the edge map
  vcl_cout << "\n>>Loading edgemap: " << edgemap_file << " ... ";
  vil_image_view<float > edgemap = *vil_convert_cast(float(), vil_load(edgemap_file.c_str()));
  if (!edgemap) 
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else 
  {
    vcl_cout << "Succeeded.\n";
  }

  //>>> Load edge orientation file
  vcl_cout << "\n>>Loading edge orientation file: " << edgeorient_file << " ... ";
  vnl_file_matrix<double > theta(edgeorient_file.c_str());
  bool loaded_edgeorient = !theta.empty();
  if (!loaded_edgeorient)
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else
  {
    vcl_cout << "Succeeded.\n";
  }

  if (edgemap.nj() != theta.rows() || edgemap.ni() != theta.cols())
  {
    vcl_cout << "\nERROR: edgemap and edge orientation map do not have the same dimension." << vcl_endl;
    return false;
  }


  // create edge angle image view from a matrix
  vil_image_view<float > edge_angle(edgemap.ni(), edgemap.nj());
  for (unsigned i =0; i < edge_angle.ni(); ++i)
  {
    for (unsigned j =0; j < edge_angle.nj(); ++j)
    {
      edge_angle(i, j) = float(theta(j, i));
    }
  }

  //>>> Load the shock graph
  vcl_cout << "\n>>Loading xshock_graph XML file: " << xgraph_file << "...";
  dbsksp_xshock_graph_sptr xgraph = 0;
  if (!x_read(xgraph_file, xgraph) )
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else
  {
    vcl_cout << "Succeeded.\n";
  } 


  //>>> Load the xgraph geometric model
  vcl_cout << "\n>>Loading xgraph geometric model file (.xml):" << xgraph_geom_file << "...";
  dbsks_xgraph_geom_model_sptr xgraph_geom = 0;
  if (!x_read(xgraph_geom_file, xgraph_geom))
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else
  {
    vcl_cout << "Succeeded.\n";
  }


  // Check compatibility between the geometric model and the shock graph (are all edges covered?)
  // \todo More thorough checks
  vcl_cout << "\n>>Checking compatibility between geometric model and xgraph...";
  bool geom_compatible = true;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator mit = 
      xgraph_geom->map_edge2geom().find((*eit)->id());
    if (mit == xgraph_geom->map_edge2geom().end())
    {
      geom_compatible = false;
      break;
    }
  }

  if (!geom_compatible)
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }





  //>>> Load the xgraph geometric model
  vcl_cout << ">>Loading xgraph Contour Chamfer Matching (ccm) model file (.xml):" << xgraph_ccm_file << "...";
  dbsks_xgraph_ccm_model_sptr xgraph_ccm = 0;
  if (!x_read(xgraph_ccm_file, xgraph_ccm))
  {
    vcl_cout << "Failed.\n";
    return false;
  }
  else
  {
    vcl_cout << "Succeeded.\n";
  }


  //>>> Check compatibility between Contour Chamfer Matching model and xgraph (are all edges covered)
  vcl_cout << ">>Checking compatibility between CCM model and xgraph...";
  bool ccm_compatible = true;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator mit = 
      xgraph_ccm->map_edge2ccm().find((*eit)->id());
    if (mit == xgraph_ccm->map_edge2ccm().end())
    {
      ccm_compatible = false;
      break;
    }
  }

  if (!ccm_compatible)
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }


  // \hack
  // Manually override the distribution of some contour fragments which have low distribution fit
  // Manually override some of the distribution
  vcl_vector<vcl_string > cfrag_desc_list;
  cfrag_desc_list.push_back("27-L");
  cfrag_desc_list.push_back("27-R");
  cfrag_desc_list.push_back("16-L");
  cfrag_desc_list.push_back("16-R");
  cfrag_desc_list.push_back("19-L");
  cfrag_desc_list.push_back("19-R");
  cfrag_desc_list.push_back("9-R");
  xgraph_ccm->override_cfrag_with_constant_distribution(cfrag_desc_list);


  //////////////////////////////////////////////////////////////////////////////////////////////////
  //>>> Detect xgraphs
  vcl_cout << ">>Detecting xgraph in image at multiple scales...";
  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_det_list;
  double def_scale = vcl_sqrt(xgraph->area());
  if (xgraph_scales.empty())
  {
    vcl_cout << "\n----ERROR: no scale was specified. Nothing was run.\n";
  }
  else
  {
    for (unsigned i =0; i < xgraph_scales.size(); ++i)
    {
      vcl_cout << "\n>>>>Processing xgraph scale = " << xgraph_scales[i] << vcl_endl;
      vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
      dbsks_detect_xgraph_using_ccm(image_src, edgemap, edge_angle, 
        xgraph, xgraph_geom, xgraph_ccm, xgraph_scales[i], dets);
      raw_det_list.insert(raw_det_list.end(), dets.begin(), dets.end());
    }
  }

  //--------------------------------------------------------------
  vcl_cout << "\n----Number of detection before non-max suppression: " 
    << raw_det_list.size() << vcl_endl; 

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //>>> Non-max suppression across detection results form different scales
  double min_overlap_ratio_for_rejection = 0.5;

  if (xgraph_scales.size() > 1)
  {
    dbsks_det_nms_using_polygon(raw_det_list, output_det_list, min_overlap_ratio_for_rejection);
  }
  else
  {
    output_det_list = raw_det_list;
  }

  vcl_cout << "\n----Number of detection after non-max suppression: " 
    << output_det_list.size() << vcl_endl; 


  return true;
}






// -----------------------------------------------------------------------------
//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_ccm(const vil_image_view<float >& image_src,
                         const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                         double xgraph_scale,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list)
{
  // ///////////////////////////////////////////////////////////////////////////
  // confidence threshold to reject a detection
  double confidence_lower_threshold = 0.0001;

  // if one bbox overlaps with another bbox with higher confidence more than 
  // "min_overlap_ratio_for_rejection" of its area then that bbox is rejected.
  double min_overlap_ratio_for_rejection = 0.3;

  vul_timer timer;
  timer.mark();

  //////////////////////////////////////////////////////////////////////////////
  //>> Contour OCM cost function
  vcl_cout << "\n>>Constructing contour-OCM cost function ...";

  float ccm_distance_threshold = 0;
  float ccm_tol_near_zero = 0;
  float ccm_lambda = 0;
  float ccm_gamma = 0;
  float ccm_edge_threshold = 15;
  int nbins_0topi = 18;
  xgraph_ccm->get_ccm_params(ccm_edge_threshold, ccm_tol_near_zero, ccm_distance_threshold, ccm_gamma, ccm_lambda, nbins_0topi);

  dbsks_ccm ccm;
  ccm.set_ocm_params(ccm_distance_threshold, ccm_tol_near_zero, ccm_lambda, ccm_gamma, nbins_0topi);
  ccm.set_edge_strength(edgemap, ccm_edge_threshold, 255);
  ccm.set_edge_orient(edge_angle);
  ccm.compute();

  vcl_cout << "done" << vcl_endl;;
  vcl_cout << "    Time spent = ";
  timer.print(vcl_cout);
  
  ////////////////////////////////////////////////////////////////////////////////




  //>>> Biarc sampler
  vcl_cout << "\n>>Constructing a biarc sampler ...";
  timer.mark();

  // Set parameters of biarc sampler
  dbsks_biarc_sampler_params bsp;
  bsp.set_to_default_values();

  // Define a biarc sampler
  dbsks_biarc_sampler biarc_sampler;

  // compute coordinates of the grid points
  biarc_sampler.set_grid(bsp);

  // compute prototype sample points for all biarcs in the grid
  double ds = 2;
  biarc_sampler.set_sampling_params(nbins_0topi * 2, ds);
  biarc_sampler.compute_cache_sample_points();

  vcl_cout << "done" << vcl_endl;
  vcl_cout << "    Total time spent = ";
  timer.print(vcl_cout);

  
  //>> Scale the shock graph to desired scale
  vcl_cout << "\n>>Scaling the xgraph to specified scale ...";
  double cur_scale = vcl_sqrt(xgraph->area());
  dbsksp_xshock_graph_sptr scaled_xgraph = new dbsksp_xshock_graph(*xgraph);
  scaled_xgraph->similarity_transform( (*scaled_xgraph->vertices_begin())->pt(), 0, 0, 0, xgraph_scale / cur_scale);
  scaled_xgraph->update_all_degree_1_nodes();
  vcl_cout << "done\n";


  //>> Compute all windows (rectangular boxes) necessary to cover the whole image
  vcl_cout << "\n>>Computing sliding (rectangular) windows to cover the whole image ...";
  
  // hard-coded maximum window size
  int max_width = 512; 
  int max_height = 512;

  // list of windows to run detection on
  vcl_vector<vgl_box_2d<int > > windows;

  int ni = image_src.ni();
  int nj = image_src.nj();

  int cur_xmin = 8;
  do
  {
    int cur_ymin = 8;
    do
    {
      // determine window dimension
      int xmax = vnl_math_min(cur_xmin + max_width-1, ni-8); // padding on right
      int ymax = vnl_math_min(cur_ymin + max_height-1, nj-8); // padding at bottom
      int xmin = vnl_math_max(xmax-max_width+1, 8);
      int ymin = vnl_math_max(ymax-max_height+1, 8);
      windows.push_back(vgl_box_2d<int >(xmin, xmax, ymin, ymax));

      // update
      cur_ymin += max_height/2;
    }
    while (cur_ymin < (nj-8-max_height/2)); // padding on top
    cur_xmin += max_width/2;
  }
  while (cur_xmin < (ni-8-max_width/2)); // padding in front

  // Print out list of windows
  vcl_cout << "\n  Total number of windows to run on = " << windows.size() << "\n";
  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "  -window " << iw << ": xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << "\n";
  }
  vcl_cout.flush();

  //>> Detect objects within each window
  vcl_cout << "\n>>Detecting objects in all computed windows ...";
  vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;
  det_list.clear();

  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "\n--Window index = " << iw << " xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << vcl_endl;

    // xshock detection engine
    dbsks_xshock_detector engine;
    //engine.image_src_ = image_src;
    engine.ccm_ = &ccm;

    engine.biarc_sampler_ = &biarc_sampler;
    engine.xgraph_geom_ = xgraph_geom;
    engine.xgraph_ccm_ = xgraph_ccm;
    engine.set_xgraph(scaled_xgraph);
    engine.root_vid_ = xgraph_geom->root_vid();
    engine.major_child_eid_ = xgraph_geom->major_child_eid();
    engine.detect(window);

    // construct a vector of detection descriptor
    for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
    {
      dbsksp_xshock_graph_sptr xgraph = engine.list_solutions_[i];
      
      //double confidence = vcl_exp(-engine.list_solution_costs_[i]);
      //double real_confidence = vcl_exp(-engine.list_solution_real_costs_[i]);

      double confidence = -engine.list_solution_costs_[i];
      double real_confidence = -engine.list_solution_real_costs_[i];
      
      // \Debug //////////////////////////////////////////////
      if (vnl_math_abs(vcl_log(real_confidence) - vcl_log(confidence)) > 0.001)
      {
        vcl_cout << "\nWarning: inconsistency. sol_idx=" << i << " confidence=" << confidence 
          << " --real_confidence=" << real_confidence << vcl_endl;
      }

      // only consider detetion with at least minimal confidence level
      if (confidence > confidence_lower_threshold)
      {
        dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(xgraph, confidence);
        det->compute_bbox();
        det_list.push_back(det);
      }
      /////////////////////////////////////////////////////
    } // solution
  } // iw
  vcl_cout << "\n----Number of detection before non-max suppression: " 
    << det_list.size() << vcl_endl;

  //>>>> Non-max supression on the boundary polygon
  vcl_cout << "\n----Non-max suppression based on boundary polygon box: ...";
  dbsks_det_nms_using_polygon(det_list, output_det_list, min_overlap_ratio_for_rejection);
  
  vcl_cout << "\n----Number of detection after non-max suppression: " 
    << output_det_list.size() << vcl_endl; 
  return true;
}








// -----------------------------------------------------------------------------
//: Detect an object using both geometric model and Whole-Contour-Matching cost model
bool dbsks_detect_xgraph_using_wcm(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         const vcl_string& cemv_file,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_string& xgraph_ccm_file,
                         const vcl_vector<vcl_string >& cfrag_list_to_ignore,
                         float wcm_weight_unmatched,
                         const vcl_vector<double >& xgraph_scales,
                         int det_window_width,
                         int det_window_height,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list)
{
  vcl_cout << "\n>>>>Xgraph detection using Whole-Contour-Matching.<<<<<\n\n";

  //>>> Preliminary checks
  output_det_list.clear();
  if (xgraph_scales.empty())
  {
    vcl_cout << "\nERROR: no scale was specified. Nothing was run.\n";
    return false;
  }


  // Load data from the files
  vil_image_view<float > edgemap;
  vil_image_view<float > edge_angle;
  vcl_vector<vsol_polyline_2d_sptr > polyline_list;
  dbsksp_xshock_graph_sptr xgraph;
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_xgraph_ccm_model_sptr xgraph_ccm;

  //: Load data from a list of file names
  if (!dbsks_load_data(image_file, edgemap_file, edgeorient_file, cemv_file, 
    xgraph_file, xgraph_geom_file, xgraph_ccm_file, cfrag_list_to_ignore,
    edgemap, edge_angle, polyline_list, xgraph, xgraph_geom, xgraph_ccm))
  {
    vcl_cout << "\nERROR: Could not load all data.\n";
    return false;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //>>> Detect xgraphs
  vcl_cout << ">> Detecting xgraph in image at multiple scales...";
  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_det_list;
  double def_scale = vcl_sqrt(xgraph->area());
  
  for (unsigned i =0; i < xgraph_scales.size(); ++i)
  {
    vcl_cout << "\n>>>> Processing xgraph scale = " << xgraph_scales[i] << vcl_endl;

    double xgraph_scale = xgraph_scales[i];
    vil_image_view<float > new_edgemap;
    vcl_vector<vsol_polyline_2d_sptr > new_polyline_list;
    vil_image_view<float > new_edge_angle;

    //////////////////////////////////////////////////////////////////////////////
    // scale the edgemap up so that the xgraph size is "standard", i.e. 130
    double standard_scale = 130; //160 for mugs because it occupies more space. For giraffes, swans, and bottles: 130;
    double scaled_up_factor = 1.0;
    dbsks_adjust_to_standard_scale(standard_scale, xgraph_scale,
      edgemap, polyline_list, edge_angle,
      scaled_up_factor, new_edgemap, new_polyline_list, new_edge_angle);

    vcl_cout << "\n     Scaling up = " << scaled_up_factor << vcl_endl;
    vcl_cout << "\n     new_ni = " << new_edgemap.ni() << "  new_nj = " << new_edgemap.nj() << vcl_endl;

    vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
    dbsks_detect_xgraph_using_wcm(new_edgemap, new_edge_angle, new_polyline_list,
      xgraph, xgraph_geom, xgraph_ccm, 
      wcm_weight_unmatched,
      xgraph_scale * scaled_up_factor, 
      det_window_width, det_window_height, dets);

    // scale the detection down to original size of the image
    for (unsigned m =0; m < dets.size(); ++m)
    {
      dets[m]->xgraph()->scale_up(0, 0, 1.0/scaled_up_factor);

      // recompute bounding box as a result of scaling the xgraph
      dets[m]->compute_bbox();
    }

    raw_det_list.insert(raw_det_list.end(), dets.begin(), dets.end());
  }
  vcl_cout << "\n    Number of detection before non-max suppression: " 
    << raw_det_list.size() << vcl_endl;

  // \debug temporary
  //>>> Non-max suppression across detection results form different scales
  double min_overlap_ratio_for_rejection = 0.3;
  dbsks_det_nms_using_polygon(raw_det_list, output_det_list, min_overlap_ratio_for_rejection);
  

  vcl_cout << "\n----Number of detection after non-max suppression: " 
    << output_det_list.size() << vcl_endl; 


  return true;
}






// -----------------------------------------------------------------------------
//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_wcm(const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         const vcl_vector<vsol_polyline_2d_sptr >& polyline_list,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                         float wcm_weight_unmatched,
                         double xgraph_scale,
                         int det_window_width,
                         int det_window_height,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list)
{
  // ///////////////////////////////////////////////////////////////////////////
  // confidence threshold to reject a detection
  double confidence_lower_threshold = -1000;

  // if one bbox overlaps with another bbox with higher confidence more than 
  // "min_overlap_ratio_for_rejection" of its area then that bbox is rejected.
  double min_overlap_ratio_for_rejection = 0.3;

  vul_timer timer;
  timer.mark();

  //////////////////////////////////////////////////////////////////////////////
  //>> Whole-Contour-Matching
  vcl_cout << "\n>>Constructing Whole-Contour-Matching cost function ...";

  dbsks_wcm wcm;
  dbsks_prepare_ccm(&wcm, xgraph_ccm, edgemap, edge_angle);

  // set the edge label image
  wcm.set_edge_labels(polyline_list);
  wcm.set_weight_unmatched(wcm_weight_unmatched);
  

  vcl_cout << "done" << vcl_endl;;
  vcl_cout << "    Time spent = ";
  timer.print(vcl_cout);
  
  ////////////////////////////////////////////////////////////////////////////////
  //>>> Biarc sampler
  
  // Define a biarc sampler
  dbsks_biarc_sampler& biarc_sampler = dbsks_biarc_sampler::default_instance();

  /////////////////////////////////////////////////////////////////////
  //>> Build a likelihood calculator  
  dbsks_xshock_wcm_likelihood wcm_like(xgraph_ccm, &wcm, &biarc_sampler);

  
  //>> Scale the shock graph to desired scale
  vcl_cout << "\n>>Scaling the xgraph to specified scale ...";
  double cur_scale = vcl_sqrt(xgraph->area());
  dbsksp_xshock_graph_sptr scaled_xgraph = new dbsksp_xshock_graph(*xgraph);
  scaled_xgraph->similarity_transform( (*scaled_xgraph->vertices_begin())->pt(), 0, 0, 0, xgraph_scale / cur_scale);
  scaled_xgraph->update_all_degree_1_nodes();
  vcl_cout << "done\n";


  //>> Compute all windows (rectangular boxes) necessary to cover the whole image
  vcl_cout << "\n>>Computing sliding (rectangular) windows to cover the whole image ...";
  
  // maximum size for a detection window
  int max_width = det_window_width; //512; 
  int max_height = det_window_height; //512;

  // list of windows to run detection on
  vcl_vector<vgl_box_2d<int > > windows;
  dbsks_algos::compute_detection_windows(max_width, max_height, edgemap.ni(), edgemap.nj(), windows);


  // Print out list of windows
  vcl_cout << "\n  Total number of windows to run on = " << windows.size() << "\n";
  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "  window " << iw << ": xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << "\n";
  }
  vcl_cout.flush();

  //>> Detect objects within each window
  vcl_cout << "\n>> Detecting objects in all computed windows ...";
  vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;
  det_list.clear();

  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout << "\n>>>> Window index = " << iw 
      << " xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << vcl_endl;
    
    // Compute wcm for a region of interest only, intead of the whole image
    wcm.compute(window, dbsks_ccm::USE_CLOSEST_ORIENTED_EDGE);


    // xshock detection engine
    dbsks_xshock_detector engine;
    engine.xshock_likelihood_ = &wcm_like;
    engine.xgraph_geom_ = xgraph_geom;
    engine.set_xgraph(scaled_xgraph);

    ///////////////////////
    engine.detect(window);
    ///////////////////////

    // construct a vector of detection descriptor
    for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
    {
      dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];
      
      double confidence = -engine.list_solution_costs_[i];
      double real_confidence = -engine.list_solution_real_costs_[i];

      // only consider detetion with at least minimal confidence level
      if (real_confidence > confidence_lower_threshold)
      {
        dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence);
        det->compute_bbox();
        det_list.push_back(det);
      }
      /////////////////////////////////////////////////////
    } // solution
  } // iw
  vcl_cout << "\n    Number of detection before non-max suppression: " << det_list.size() << vcl_endl;

  //>> Non-max supression on the boundary polygon
  vcl_cout << "\n    Non-max suppression based on boundary polygon box: ...";
  dbsks_det_nms_using_polygon(det_list, output_det_list, min_overlap_ratio_for_rejection);

  // print out cost for each final detection
  for (unsigned i =0; i < output_det_list.size(); ++i)
  {
    double wcm_cost = wcm_like.f_whole_contour(output_det_list[i]->xgraph(), vcl_vector<unsigned >(), true);
    vcl_cout << "\nSolution i = " << i << " WCM cost = " << wcm_cost << vcl_endl;
  }
  
  vcl_cout << "\n    Number of detection after non-max suppression: " 
    << output_det_list.size() << vcl_endl; 
  return true;
}





//------------------------------------------------------------------------------
//: Load data from a list of file names
bool dbsks_load_data(const vcl_string& image_file,
                     const vcl_string& edgemap_file,
                     const vcl_string& edgeorient_file,
                     const vcl_string& cemv_file,
                     const vcl_string& xgraph_file,
                     const vcl_string& xgraph_geom_file,
                     const vcl_string& xgraph_ccm_file,
                     const vcl_vector<vcl_string >& cfrag_list_to_ignore,
                     vil_image_view<float >& edgemap,
                     vil_image_view<float >& edge_angle,
                     vcl_vector<vsol_polyline_2d_sptr >& polyline_list,
                     dbsksp_xshock_graph_sptr& xgraph,
                     dbsks_xgraph_geom_model_sptr& xgraph_geom,
                     dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  vil_image_resource_sptr image_resource = 0;
  if (!dbsks_load_image_resource(image_file, image_resource))
  {
    return false;
  }

  if (!dbsks_load_edgemap(edgemap_file, edgemap))
  {
    return false;
  }

  if (!dbsks_load_edge_angle(edgeorient_file, edge_angle))
  {
    return false;
  }
 
  // check size
  if (edgemap.nj() != edge_angle.nj() || edgemap.ni() != edge_angle.ni())
  {
    vcl_cout << "\n  ERROR: edgemap and edge orientation map do not have the same dimension." << vcl_endl;
    return false;
  }

  if (!dbsks_load_polyline_list(cemv_file, polyline_list))
  {
    return false;
  }

  if (!dbsks_load_xgraph(xgraph_file, xgraph))
  {
    return false;
  }

  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom))
  {
    return false;
  }

  // Check compatibility between the geometric model and the shock graph (are all edges covered?)
  vcl_cout << "\n>> Checking compatibility between geometric model and xgraph...";
  if (!xgraph_geom->is_compatible(xgraph))
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }

  dbsks_load_xgraph_ccm_model(xgraph_ccm_file, xgraph_ccm);

  //>>> Check compatibility between Contour Chamfer Matching model and xgraph (are all edges covered)
  vcl_cout << ">> Checking compatibility between CCM model and xgraph...";
  if (!xgraph_ccm->is_compatible(xgraph))
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }

  // Set distributions of user-selected boundary fragments to constant
  vcl_cout << ">> Overriding 'ignored' edges with constant distribution...";
  if (!xgraph_ccm->override_cfrag_with_constant_distribution(cfrag_list_to_ignore))
  {
    vcl_cout << "[ Failed ]\n\n";
    return false;
  }
  else
  {
    vcl_cout << "[   OK   ]\n\n";
  }

  return true;
}




//------------------------------------------------------------------------------
//: Prepare a CCM calculator from a CCM model and data sources
bool dbsks_prepare_ccm(dbsks_ccm* ccm, 
                       const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                       const vil_image_view<float >& edgemap,
                       const vil_image_view<float >& edge_angle)
{
  if (!ccm)
    return false;
  
  // extract param values from ccm model
  float ccm_distance_threshold = 8;
  float ccm_tol_near_zero = 4;
  float ccm_lambda = 0.4f;
  float ccm_gamma = 0.3f;
  float ccm_edge_threshold = 15;
  int nbins_0topi = 18;
  xgraph_ccm->get_ccm_params(ccm_edge_threshold, ccm_tol_near_zero, ccm_distance_threshold, ccm_gamma, ccm_lambda, nbins_0topi);
  
  // apply the params to ccm
  ccm->set_ocm_params(ccm_distance_threshold, ccm_tol_near_zero, ccm_lambda, ccm_gamma, nbins_0topi);
  ccm->set_edge_strength(edgemap, ccm_edge_threshold, 255);
  ccm->set_edge_orient(edge_angle);
  return true;
}










//// -----------------------------------------------------------------------------
////: Detect an object using both geometric model and contour chamfer matching cost model
//bool dbsks_detect_xgraph_using_ccm_subpix(const vcl_string& edgemap_folder,
//                                          const vcl_string& object_name,
//                                          const vcl_string& edgemap_ext,
//                                          const vcl_string& edge_angle_ext,
//                         const vcl_string& xgraph_file,
//                         const vcl_string& xgraph_geom_file,
//                         const vcl_string& xgraph_ccm_file,
//                         const vcl_vector<vcl_string >& cfrag_list_to_ignore,
//                         double min_xgraph_scale,
//                         double log2_scale_step,
//                         double max_xgraph_scale,
//                         int det_window_width,
//                         int det_window_height,
//                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list)
//{
//  vcl_cout << "\n========================================================"
//           << "\n   Xgraph detection using Contour-Chamfer-Matching      "
//           << "\n========================================================\n\n";
//
//  //> Preliminary checks
//  output_det_list.clear(); // clean up output results
//
//  if (log2_scale_step <= 0 || max_xgraph_scale < min_xgraph_scale) // improper scale params
//  {
//    vcl_cout << "\nERROR: Improper scale params. Nothing was run.\n";
//    return false;
//  }
//
//
//  // Load data from the files
//  dbsksp_xshock_graph_sptr xgraph;
//  dbsks_xgraph_geom_model_sptr xgraph_geom;
//  dbsks_xgraph_ccm_model_sptr xgraph_ccm;
//
//  //// edgemap
//  //vcl_string edgemap_filename = object_name + edgemap_ext;
//  //vcl_string edgemap_file = object_folder + "/" + edgemap_filename;
//  //vil_image_view<float > edgemap;
//  //if (!dbsks_load_edgemap(edgemap_file, edgemap))
//  //{
//  //  return false;
//  //}
//
//  //// edge_angle
//  //vcl_string edge_angle_filename = object_name + edge_angle_ext;
//  //vcl_string edge_angle_file = object_folder + edge_angle_filename;
//  //vil_image_view<float > edge_angle;
//  //if (!dbsks_load_edge_angle(edge_angle_file, edge_angle))
//  //{
//  //  return false;
//  //}
//
//  //// check size
//  //if (edgemap.nj() != edge_angle.nj() || edgemap.ni() != edge_angle.ni())
//  //{
//  //  vcl_cout << "\nERROR: edgemap and edge orientation map do not have the same dimension." << vcl_endl;
//  //  return false;
//  //}
//
//
//  // xgraph
//  if (!dbsks_load_xgraph(xgraph_file, xgraph))
//  {
//    return false;
//  }
//
//  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom))
//  {
//    return false;
//  }
//
//  // Check compatibility between the geometric model and the shock graph (are all edges covered?)
//  vcl_cout << "\n>> Checking compatibility between geometric model and xgraph...";
//  if (!xgraph_geom->is_compatible(xgraph))
//  {
//    vcl_cout << "Failed\n." << vcl_endl;
//    return false;
//  }
//  else
//  {
//    vcl_cout << "Passed\n." << vcl_endl;
//  }
//
//  dbsks_load_xgraph_ccm_model(xgraph_ccm_file, xgraph_ccm);
//
//  //>>> Check compatibility between Contour Chamfer Matching model and xgraph (are all edges covered)
//  vcl_cout << ">> Checking compatibility between CCM model and xgraph...";
//  if (!xgraph_ccm->is_compatible(xgraph))
//  {
//    vcl_cout << "Failed\n." << vcl_endl;
//    return false;
//  }
//  else
//  {
//    vcl_cout << "Passed\n." << vcl_endl;
//  }
//
//  // Set distributions of user-selected boundary fragments to constant
//  vcl_cout << ">> Overriding 'ignored' edges with constant distribution...";
//  if (!xgraph_ccm->override_cfrag_with_constant_distribution(cfrag_list_to_ignore))
//  {
//    vcl_cout << "[ Failed ]\n\n";
//    return false;
//  }
//  else
//  {
//    vcl_cout << "[ OK ]\n\n";
//  }
//
//  //>> Load the edgemap to get the image size
//  vcl_string edgemap00_filename = object_name + "_00" + edgemap_ext;
//  vcl_string edgemap00_file = edgemap_folder + "/" + edgemap00_filename;
//  vil_image_resource_sptr edgemap00_resource = vil_load_image_resource(edgemap00_file.c_str(), true);
//  double image_width = edgemap00_resource->ni();
//  double image_height = edgemap00_resource->nj();
//  double image_scale = vcl_sqrt(image_width * image_height);
//
//  // Maximum xgraph scale is bounded above by image size
//  max_xgraph_scale = vnl_math_min(image_scale, max_xgraph_scale);
//  vcl_vector<double > xgraph_scales;
//  for (double s = min_xgraph_scale; s <= max_xgraph_scale; s *= vcl_pow(2, log2_scale_step))
//  {
//    xgraph_scales.push_back(s);
//  }
//
//  
//  //////////////////////////////////////////////////////////////////////////////
//  //>> Detect xgraphs ..........................................................
//  vcl_cout << "\n>> Computing the xgraph scales to run.\n";
//  vcl_cout << "   -> Image size (W x H) = " << image_width << " x " << image_height 
//    << "\n   -> Image scale (sqrt of area) = " << vcl_sqrt(image_width * image_height)
//    << "\n   -> Min xgraph scale = " << min_xgraph_scale 
//    << "\n   -> Log2 of scale step = " << log2_scale_step
//    << "\n   -> Max xgraph scale = " << max_xgraph_scale
//    << "\n   -> List of scales = [ ";
//  for (unsigned i =0; i < xgraph_scales.size(); ++i)
//  {
//    vcl_cout << xgraph_scales[i] << ", ";
//  }
//  vcl_cout << "]\n";
//
//  vcl_cout << "\n>> Detecting xgraph in image at multiple scales ...\n";
//  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_det_list;
//  double def_scale = vcl_sqrt(xgraph->area());
//  
//  for (unsigned i =0; i < xgraph_scales.size(); ++i)
//  {
//    vcl_cout << "\n   -> Processing xgraph scale = " << xgraph_scales[i] << "\n";
//
//    double xgraph_scale = xgraph_scales[i];
////    vil_image_view<float > new_edgemap = edgemap;
////    vil_image_view<float > new_edge_angle = edge_angle;
////
////    vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
////    dbsks_detect_xgraph_using_ccm_subpix(new_edgemap, new_edge_angle, 
////      xgraph, xgraph_geom, xgraph_ccm, 
////      xgraph_scale, 
////      det_window_width, det_window_height, dets);
////
////    // scale the detection down to original size of the image
////    for (unsigned m =0; m < dets.size(); ++m)
////    {
////      // recompute bounding box as a result of scaling the xgraph
////      dets[m]->compute_bbox();
////    }
////
////    raw_det_list.insert(raw_det_list.end(), dets.begin(), dets.end());
////  }
////
////  vcl_cout << "\n    Number of detection before non-max suppression: " 
////    << raw_det_list.size() << vcl_endl;
////
////  //>>> Non-max suppression across detection results form different scales
////  double min_overlap_ratio_for_rejection = 0.3;
////  dbsks_det_nms_using_polygon(raw_det_list, output_det_list, min_overlap_ratio_for_rejection);
////  
////
////  vcl_cout << "\n----Number of detection after non-max suppression: " 
////    << output_det_list.size() << vcl_endl; 
//
//  return true;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//// -----------------------------------------------------------------------------
////: Detect an object using both geometric model and contour chamfer matching cost model
//bool dbsks_detect_xgraph_using_ccm_subpix(const dbdet_edgemap_sptr& edgemap,
//                         const dbsksp_xshock_graph_sptr& xgraph,
//                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
//                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
//                         int det_window_width,
//                         int det_window_height,
//                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list,
//                         double confidence_lower_threshold,
//                         bool run_nms_based_on_overlap,
//                         double min_overlap_ratio_for_rejection
//                         )
//{
//
//  //// ///////////////////////////////////////////////////////////////////////////
//  //// confidence threshold to reject a detection
//  //double confidence_lower_threshold = -1000;
//
//  //// if one bbox overlaps with another bbox with higher confidence more than 
//  //// "min_overlap_ratio_for_rejection" of its area then that bbox is rejected.
//  //double min_overlap_ratio_for_rejection = 0.3;
//
//  vul_timer timer;
//  timer.mark();
//
//  //> Contour-Chamfer-Matching cost function...................................
//  vcl_cout << "\n> Constructing a likelihood function based on CCM cost ...";
//
//  dbsks_xshock_ccm_likelihood ccm_like;
//  ccm_like.set_edgemap(edgemap);
//  ccm_like.set_biarc_sampler(&dbsks_biarc_sampler::default_instance());
//  ccm_like.set_ccm_model(xgraph_ccm);
//  vcl_cout << " [ OK ]\n";
//
//  ////> Scale the shock graph to desired scale...................................
//  //vcl_cout << "\n> Scaling the xgraph to specified scale ...";
//  //double cur_scale = vcl_sqrt(xgraph->area());
//  //dbsksp_xshock_graph_sptr scaled_xgraph = new dbsksp_xshock_graph(*xgraph);
//  //scaled_xgraph->scale_up(0, 0, xgraph_scale / cur_scale);
//  //vcl_cout << "done\n";
//
//
//  //> Compute all windows (rectangular boxes) necessary to cover the whole image
//  vcl_cout << "\n> Computing sliding (rectangular) windows to cover the whole image ...";
//  
//  vcl_vector<vgl_box_2d<int > > windows; // list of detection windows
//  dbsks_compute_detection_windows(det_window_width, det_window_height, 
//    edgemap->ncols(), edgemap->nrows(), windows);
//
//  // Print out list of windows
//  vcl_cout << "\n  >> Total #windows = " << windows.size() << "\n";
//  vcl_cout << "\n  >> List of windows: \n";
//  for (unsigned iw =0; iw < windows.size(); ++iw)
//  {
//    vgl_box_2d<int > window = windows[iw];
//    vcl_cout << "     window " << iw << ": xmin=" << window.min_x() 
//      << " ymin=" << window.min_y() 
//      << " xmax=" << window.max_x()
//      << " ymax=" << window.max_y() << "\n";
//  }
//  vcl_cout << " [ OK ]\n";
//  vcl_cout.flush();
//
//  //>> Detect objects within each window
//  vcl_cout << "\n> Detecting objects in all computed windows ...";
//  vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;
//  det_list.clear();
//
//  for (unsigned iw =0; iw < windows.size(); ++iw)
//  {
//    vgl_box_2d<int > window = windows[iw];
//    vcl_cout << "\n  >> Window index = " << iw 
//      << " xmin=" << window.min_x() 
//      << " ymin=" << window.min_y() 
//      << " xmax=" << window.max_x()
//      << " ymax=" << window.max_y() << vcl_endl;
//    
//    // Compute ccm for a region of interest only
//    ccm_like.compute_internal_data(window);
//
//
//    // xshock detection engine
//    dbsks_xshock_detector engine;
//    engine.xshock_likelihood_ = &ccm_like;
//    engine.xgraph_geom_ = xgraph_geom;
//    engine.set_xgraph(xgraph);
//
//    ///////////////////////
//    engine.detect(window);
//    ///////////////////////
//
//    // construct a vector of detection descriptor
//    for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
//    {
//      dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];
//      
//      double confidence = -engine.list_solution_costs_[i];
//      double real_confidence = -engine.list_solution_real_costs_[i];
//
//      // only consider detetion with at least minimal confidence level
//      if (real_confidence > confidence_lower_threshold)
//      {
//        dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence);
//        det->compute_bbox();
//        det_list.push_back(det);
//      }
//      /////////////////////////////////////////////////////
//    } // solution
//  } // iw
//  vcl_cout << "\n    Number of raw detections: " << det_list.size() << vcl_endl;
//
//  if (run_nms_based_on_overlap)
//  {
//    //>> Non-max supression on the boundary polygon
//    vcl_cout << "\n    Non-max suppression based on boundary polygon box: ...";
//    dbsks_det_nms_using_polygon(det_list, output_det_list, min_overlap_ratio_for_rejection);
//
//    vcl_cout << "\n    Number of detection after non-max suppression: " 
//      << output_det_list.size() << vcl_endl; 
//  }
//  else
//  {
//    output_det_list = det_list;
//  }
//  return true;
//}






//: Construct an dbdet_edgemap from an edge image and an edge angle matrix
dbdet_edgemap_sptr dbsks_make_edgemap(const vil_image_view<float >& edgemap_view,
                                      const vil_image_view<float >& edge_angle_view,
                                      float lower_threshold,
                                      float max_edge_value)
{
  if (edgemap_view.ni() != edge_angle_view.ni() || 
    edgemap_view.nj() != edge_angle_view.nj())
  {
    return 0;
  }

  // construct the edgemap with the same size as the image
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(edgemap_view.ni(), edgemap_view.nj());
  int ni = int(edgemap_view.ni());
  int nj = int(edgemap_view.nj());

  for (int j =0; j < nj; ++j)
  {
    for (int i =0; i < ni; ++i)  
    {
      if (edgemap_view(i, j) < lower_threshold)
        continue;

      double x = i;
      double y = j;
      double dir = edge_angle_view(i, j);
      double conf = edgemap_view(i, j) / max_edge_value;

      dbdet_edgel* e = new dbdet_edgel(vgl_point_2d<double>(x,y), dir, conf);
      e->id = edgemap->edgels.size();
      edgemap->insert(e);      
    }
  }
  
  return edgemap;
}




