#include "dbdet_compute_linked_curves_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curve_model.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbgl/algo/dbgl_arc_algo.h>
#include <vgl/algo/vgl_conic_2d_regression.h>

#include <mbl/mbl_stats_1d.h>


dbdet_compute_linked_curves_process::dbdet_compute_linked_curves_process()
{
}


//: Destructor
dbdet_compute_linked_curves_process::~dbdet_compute_linked_curves_process()
{
}


//: Clone the process
bpro1_process*
dbdet_compute_linked_curves_process::clone() const
{
  return new dbdet_compute_linked_curves_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_compute_linked_curves_process::name()
{
  return "All in one Edge > Sel > Vsol";
}


//: Return the number of input frame for this process
int
dbdet_compute_linked_curves_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_compute_linked_curves_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_compute_linked_curves_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_compute_linked_curves_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );

  return to_return;
}

//: Execute the process
bool dbdet_compute_linked_curves_process::execute()
{
  if ( input_data_.size() != 1 ) {
    vcl_cout << "In dbdet_compute_linked_curves_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }

  clear_output();

  // ----------------------------------------------------------------------
  vcl_cout << "Third_order edge detection...";
  vcl_cout.flush();
  dbdet_edgemap_sptr EM;

  {
  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = 
    image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  // set the parameters
  unsigned grad_op = 0,  // Gradient Operator
           conv_algo = 0, // Convolution Algo
           parabola_fit = 0;
  double sigma = 1.0, 
         thresh = 2.0; // Gradient Magnitude Threshold
  int N = 1; // Interpolation Factor
  bool badap_thresh = false, // Apply adaptive threshold
       reduce_tokens = false,
       binterp_grid = false; // Output on Interp. Grid 

  // perfrom third-order edge detection with these parameters
  EM = dbdet_detect_third_order_edges(
      image_view, sigma, thresh, N, parabola_fit, grad_op, conv_algo, 
      badap_thresh, binterp_grid, reduce_tokens);
  }

  vcl_cout << "done!" << vcl_endl;
  vcl_cout << "#edgels = " << EM->num_edgels() << vcl_endl;

  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  vcl_cout << "Symbolic edge linker started.\n";

  dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

  {
  // get params. See dbet_sel_process for an explanation
  double nrad = 2.0, 
         dx = 0.4, 
         dt = 15.0;
  bool badap_uncer = true;
  unsigned curve_model_type = 5;
  double token_len = 1.0, 
         max_k = 0.2, 
         max_gamma = 0.05;

  unsigned grouping_algo = 2;
  unsigned max_size_to_group = 7;
//  unsigned cvlet_type = 0;
  bool bCentered_grouping = true;
  bool bBidirectional_grouping = false;

  bool bFormCompleteCvletMap = false;
  bool b_use_all_cvlets = false;
  unsigned app_usage = 0;
  double app_thresh = 0.2;
  
  unsigned linkgraph_algo = 0;
  unsigned min_size_to_link = 4;
  
  unsigned linking_algo = 2; // Extract regular contours.
  unsigned num_link_iters = 7;

  // create the sel storage class
  output_sel->set_EM(EM);


  //get pointers to the data structures in it
  dbdet_curvelet_map& CM = output_sel->CM();
  dbdet_edgel_link_graph& ELG = output_sel->ELG();
  dbdet_curve_fragment_graph &CFG = output_sel->CFG();

  //different types of linkers depending on the curve model
  typedef dbdet_sel<dbdet_simple_linear_curve_model> dbdet_sel_simple_linear;
  typedef dbdet_sel<dbdet_linear_curve_model> dbdet_sel_linear;
  typedef dbdet_sel<dbdet_CC_curve_model> dbdet_sel_CC;
  typedef dbdet_sel<dbdet_CC_curve_model_new> dbdet_sel_CC_new;
  typedef dbdet_sel<dbdet_CC_curve_model_perturbed> dbdet_sel_CC_perturbed;
  typedef dbdet_sel<dbdet_CC_curve_model_3d> dbdet_sel_CC_3d;
  typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
  typedef dbdet_sel<dbdet_ES_curve_model_perturbed> dbdet_sel_ES_perturbed;

  //start the timer
  vul_timer t;

  //construct the linker
  dbdet_sel_sptr edge_linker;

  //The curvelet formation parameters
  dbdet_curvelet_params cvlet_params(dbdet_curve_model::CC, 
                                     nrad, dt, dx, badap_uncer, 
                                     token_len, max_k, max_gamma,
                                     bCentered_grouping,
                                     bBidirectional_grouping);

  switch (curve_model_type)
  {
  case 0: //simple linear_model
    cvlet_params.C_type = dbdet_curve_model::LINEAR;
    edge_linker = new dbdet_sel_simple_linear(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 1: //linear_model
    cvlet_params.C_type = dbdet_curve_model::LINEAR;
    edge_linker = new dbdet_sel_linear(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 2: //CC_model
    cvlet_params.C_type = dbdet_curve_model::CC;
    edge_linker = new dbdet_sel_CC(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 3: //CC_model new
    cvlet_params.C_type = dbdet_curve_model::CC2;
    edge_linker = new dbdet_sel_CC_new(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 4: //CC_model with discrete perturbations
    cvlet_params.C_type = dbdet_curve_model::CC;
    edge_linker = new dbdet_sel_CC_perturbed(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 5: //CC_model 3d bundle
    cvlet_params.C_type = dbdet_curve_model::CC3d;
    edge_linker = new dbdet_sel_CC_3d(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 6: //ES_model
    cvlet_params.C_type = dbdet_curve_model::ES;
    edge_linker = new dbdet_sel_ES(EM, CM, ELG, CFG, cvlet_params);
    break;
  case 7: //ES_model with discrete perturbations
    cvlet_params.C_type = dbdet_curve_model::ES;
    edge_linker = new dbdet_sel_ES_perturbed(EM, CM, ELG, CFG, cvlet_params);
    break;
  }
  
  //set appearance usage flags
  edge_linker->set_appearance_usage(app_usage);
  edge_linker->set_appearance_threshold(app_thresh);

  //perform local edgel grouping
  switch (grouping_algo)
  {
  case 0: //combinatorial grouping
    edge_linker->build_curvelets_using_combination_rules();
    break;
  case 1: //hierarchical grouping (breadth-first grouping)
    edge_linker->build_curvelets_hierarchically();
    break;
  case 2: //greedy (depth first grouping)
    edge_linker->build_curvelets_greedy(max_size_to_group, false);
    break;
  case 3: //extra greedy (depth first grouping)
    edge_linker->build_curvelets_greedy(max_size_to_group, true);
    break;
  }
  
  if (bFormCompleteCvletMap)
    edge_linker->form_full_cvlet_map();
  
  double group_time = t.real() / 1000.0;
  t.mark();
  vcl_cout << "Time taken to form groups: " << group_time << " sec" << vcl_endl;

  //form a link graph
  if (b_use_all_cvlets)
    edge_linker->use_all_curvelets();
  else
    edge_linker->use_anchored_curvelets_only();

  //form the link graph
  edge_linker->construct_the_link_graph(min_size_to_link, linkgraph_algo);
  
  //extract contours
  switch (linking_algo) {
    case 1: // iteratively
      edge_linker->extract_image_contours_from_the_link_graph(num_link_iters);
      break;
    case 2: // regular contours
      edge_linker->extract_regular_contours_from_the_link_graph();
      break;
  }

  double link_time = t.real() / 1000.0;
  vcl_cout << "Time taken to link: " << link_time << " sec" << vcl_endl;
  } // end block
  vcl_cout << "Done with edge linker!" << vcl_endl;



  //----------------------------------------------------------------------
  dbdet_curve_fragment_graph& CFG = output_sel->CFG();


  vcl_vector< vsol_spatial_object_2d_sptr > image_curves;

  {
  // get the parameters
  unsigned len_thresh = 7; ///< length threshold

  bool apply_strength_thresh = false; ///< apply avg. strength threshold flag
  double strength_thresh = 4.0;     ///< avg. strength threshold
  bool apply_contrast_thresh = false; ///< apply mean contrast threshold flag
  double contrast_thresh = 4.0;     ///< mean contrast threshold
  bool apply_adap_thresh = false;     ///< apply adaptive threshold flag
  double adap_thresh_fac = 2.0;     ///< adaptive threshold factor
  bool apply_d2f_thresh = false;      ///< apply d2f threshold
  double d2f_thresh = 2.0;          ///< d2f threshold

  bool smooth_con = true;     ///< smooth contour flag
  float psi = 1.0;           ///< smoothing parameter
  unsigned num_times = 10;  ///< # times to smooth
  bool apply_k_thresh = false; ///< apply curvature threshold
  float k_thresh = 0.2f;      ///< curvature threshold for breaking contours



  //form vsol curves from the edgel chains in the linker

  dbdet_edgel_chain_list_iter f_it = CFG.frags.begin();
  for (; f_it != CFG.frags.end(); f_it++) {
    dbdet_edgel_chain* chain = (*f_it);
  
    //A) Apply the length threshold
    if (chain->edgels.size() < len_thresh)
      continue;

    //collect other statistics for the contours
    mbl_stats_1d Ldata, Rdata;

    // 1) compute the average edge strength of an edgel chain
    for (unsigned j=0; j<chain->edgels.size(); j++)
      Ldata.obs(chain->edgels[j]->strength);

    double avg_strength = Ldata.mean();

    // B) Apply avg. strength threshold
    if (apply_strength_thresh && avg_strength<strength_thresh)
      continue;

    // 2) compute mean and std intensity of the left and right sides of the contour  
    Ldata.clear();  Rdata.clear();
    for (unsigned k=0; k<chain->edgels.size(); k++){
      Ldata.obs(chain->edgels[k]->left_app->value());
      Rdata.obs(chain->edgels[k]->right_app->value());
    }

    double Lmean = Ldata.mean(); double Rmean = Rdata.mean();
    double Lstd = Ldata.sd(); double Rstd = Rdata.sd(); 

    // C) Apply mean contrast threshold
    if (apply_contrast_thresh && vcl_fabs(Lmean-Rmean)<contrast_thresh)
      continue;

    // D) apply adaptive contrast threshold
    if (apply_adap_thresh && vcl_fabs(Lmean-Rmean)<adap_thresh_fac*(Lstd+Rstd)) //saliency test
        continue;

    // 3) compute average d2f for the contour fragment
    Ldata.clear();  Rdata.clear();
    for (unsigned j=0; j<chain->edgels.size(); j++)
      Ldata.obs(chain->edgels[j]->deriv);

    double d2f_mean = Ldata.mean();

    // E) apply peakiness threshold
    if (apply_d2f_thresh && vcl_fabs(d2f_mean)<d2f_thresh)
        continue;

    // ==========
    //create a polyline out of the edgel chain
    vcl_vector<vgl_point_2d<double> > pts;
    pts.reserve(chain->edgels.size());
    for (unsigned j=0; j<chain->edgels.size(); j++)
      pts.push_back(chain->edgels[j]->pt);

    // smooth this contour
    if (smooth_con)
      dbgl_csm(pts, psi, num_times);

    // F) Apply curvature threshold
    if (apply_k_thresh)
    {
      vcl_vector<double> ks;
      ks.resize(pts.size());

      for (unsigned j=0; j<pts.size(); j++)
      {
        unsigned k1; //index of the first of 3 pts
        if (j<1)                  k1=0;
        else if  (j>pts.size()-2) k1=pts.size()-3;
        else                      k1 = j-1;

        //vgl_conic_2d_regression<double> reg;
        //for (unsigned k=k1; k<k2; k++)
        //  reg.add_point(pts[k]);
        //reg.fit();

        //compute curvature
        ks[j] = 1./dbgl_arc_algo::compute_arc_radius_from_three_points(pts[k1], pts[k1+1], pts[k1+2]);
      }

      //TODO: smooth the curvature function slightly

      Ldata.clear();
      for (unsigned j=0; j<ks.size(); j++)
        Ldata.obs(ks[j]);

      if (vcl_fabs(Ldata.mean())<k_thresh) 
        continue;
    }

    //finally construct vsol curves out of the remaining segments
    vcl_vector<vsol_point_2d_sptr> vsol_pts;
    for (unsigned i=0; i<pts.size(); ++i)
      vsol_pts.push_back(new vsol_point_2d(pts[i]));

    vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(vsol_pts);
    
    //add this contour to the storage class
    image_curves.push_back(new_curve->cast_to_spatial_object());
  }
  } // end block

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(image_curves, "contours");
  output_data_[0].push_back(output_vsol);
  return true;
}

bool
dbdet_compute_linked_curves_process::finish()
{
  return true;
}

