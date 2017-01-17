// This is shp/dbsksp/dbsks_examine_xgraph_cost_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 6, 2008

#include "dbsks_examine_xgraph_cost_tool.h"


//#include <dbsks/dbsks_compute_ocm_cost.h>
//#include <dbsks/dbsks_xshock_wcm_likelihood.h>
//#include <dbsks/algo/dbsks_detect_xgraph.h>

#include <dbsks/algo/dbsks_load.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/xio/dbsks_xio_xgraph_ccm_model.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/vis/dbsksp_soview_xshock.h>

#include <dbdet/algo/dbdet_resize_edgemap.h>
#include <dbdet/vis/dbdet_edgemap_tableau.h>
#include <dbdet/algo/dbdet_convert_edgemap.h>


#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_util.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>


#include <vgui/vgui_dialog.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_file_matrix.h>
#include <vul/vul_timer.h>
#include <vcl_iostream.h>

// -----------------------------------------------------------------------------
//: Constructor
dbsks_examine_xgraph_cost_tool::
dbsks_examine_xgraph_cost_tool() : 
  tableau_(0), 
  xgraph_storage_(0)
{  
  this->gesture_display_part_cost_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  
  this->gesture_display_graph_cost_ = 
    vgui_event_condition(vgui_key('g'), vgui_MODIFIER_NULL,true);


  //// parameters for gray-ocm cost
  //this->examine_gray_ocm_ = false;
  //this->gray_ocm_params_ = new bpro1_parameters();
  //this->gray_ocm_params_->add("Edge value threshold", "-gray-ocm-edge-threshold", 30.0f);
  //this->gray_ocm_params_->add("Distance threshold", "-gray-ocm-distance-threshold", 8.0f);
  //this->gray_ocm_params_->add("Tolerance near zero", "-gray-ocm-tol-near-zero", 4.0f);
  //this->gray_ocm_params_->add("Weight of orientation cost (lambda)", "-gray-ocm-lambda", 0.5f);
  //this->gray_ocm_params_->add("Number of orientation channels to cover [0, pi] (int)", "-gray-ocm-nchannel-0topi", (int)18);


  ////: Contour OCM
  //this->examine_ccm_ = false;
  //this->ccm_params_ = new bpro1_parameters();
  //this->ccm_params_->add("Edge value threshold", "-contour-ocm-edge-threshold", 30.0f);
  //this->ccm_params_->add("Edge orientation file (.orient.txt)", "-contour-ocm-edgeorient-file", bpro1_filepath("", "*.orient.txt"));
  //this->ccm_params_->add("Distance threshold", "-contour-ocm-distance-threshold", 8.0f);
  //this->ccm_params_->add("Tolerance near zero", "-contour-ocm-tol-near-zero", 4.0f);
  //this->ccm_params_->add("Weight of orientation cost", "-contour-ocm-lambda", 0.3f);
  //this->ccm_params_->add("Weight of distance derivative cost", "-contour-ocm-gamma", 0.4f);
  //this->ccm_params_->add("Number of orientation channels to cover [0, pi] (int)", "-contour-ocm-nchannel-0topi", 18);

  ////: WCM model
  //this->examine_wcm_model_ = false;
  //this->wcm_model_params_ = new bpro1_parameters();
  //this->wcm_model_params_->add("Edgemap file (_edges.tiff)", "-edgemap-file", bpro1_filepath("", "*_edges.tiff"));
  //this->wcm_model_params_->add("Edge orientation file (.orient.txt)", "-edgeorient-file", bpro1_filepath("", "*.orient.txt"));
  //this->wcm_model_params_->add("Contour-edge map file (.cemv)", "-cemv-file", bpro1_filepath("", "*.cemv"));
  //this->wcm_model_params_->add("Xgraph CCM model file (.xml)", "-xgraph-ccm-file", bpro1_filepath("", "*.xml"));

  // parameters for biarc sampler
  this->biarc_sampler_params_ = new bpro1_parameters();
  this->biarc_sampler_params_->add("sample rate ds (float)", "-biarc-sampler-ds", 2.0f);
  this->biarc_sampler_params_->add("Number of orientation channels to cover [0, 2pi]", "-biarc-sampler-nchannel-0to2pi", (int)36);

  //: Subpix CCM
  this->examine_subpix_ccm_ = false;
  this->subpix_ccm_params_ = new bpro1_parameters();
  this->subpix_ccm_params_->add("Xgraph CCM model file (.xml)", "-xgraph-ccm-model", bpro1_filepath("", "*.xml"));

    //this->subpix_ccm_params_->add("Edge map pyramid folder: ", "-edgemap-pyramid", bpro1_filepath("", "*.png"));

  // Name of edge orientation file

  this->subpix_ccm_params_->add("Edge image file (_edges.png)", "-edge-image", bpro1_filepath("", ".png"));
  this->subpix_ccm_params_->add("Edge orient file (_orient.txt)", "-edge-orient", bpro1_filepath("", ".png"));
  this->subpix_ccm_params_->add("Log2 of scaling-up factor: ", "-log2-scale-up-factor", -0.5f);
}



// -----------------------------------------------------------------------------
//: Destructor
dbsks_examine_xgraph_cost_tool::
~dbsks_examine_xgraph_cost_tool()
{
}



// -----------------------------------------------------------------------------
//: set the tableau to work with
bool dbsks_examine_xgraph_cost_tool::
set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_xgraph_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);

    ////
    //if (this->edgemap_tab_)
    //{
    //  this->tableau_->set_child(this->edgemap_tab_);
    //  this->tableau_->get_i
    //  //this->tableau_->post_redraw();
    //}

    return true;
  }
  tableau_ = 0;
  return false;
}




// -----------------------------------------------------------------------------
//: Set the storage class for the active tableau
bool dbsks_examine_xgraph_cost_tool::
set_storage ( const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
    return false;
  
  //make sure its a vsol storage class
  if (storage_sptr->type() == "dbsksp_xgraph"){
    this->xgraph_storage_.vertical_cast(storage_sptr);
    return true;
  }
  this->xgraph_storage_ = 0;
  return false;
}




// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbsks_examine_xgraph_cost_tool::
name() const 
{
  return "Examine xgraph cost";
}





// -----------------------------------------------------------------------------
//: Allow the tool to add to the popup menu as a tableau would
void dbsks_examine_xgraph_cost_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vcl_string on = "[x] ", off = "[ ] ";

  // computer vertex depths
  menu.add("Compute xgraph vertex depths", dbsks_exgc_compute_xgraph_vertex_depths_wrapper, (void*)(this));
  
  //// gray-ocm
  //menu.separator();
  //menu.add( (this->examine_gray_ocm_ ? on:off)+ "Examine gray OCM cost?", bvis1_tool_toggle, 
  //  (void*)(&this->examine_gray_ocm_) );
  //menu.add("Set image for gray ocm", 
  //  new dbsks_exgc_set_storage_command("image", &(this->gray_ocm_image_storage_))); 
  //menu.add("Set edgemap for gray ocm", 
  //  new dbsks_exgc_set_storage_command("image", &(this->gray_ocm_edgemap_storage_))); 
  //menu.add("Set parameters of gray ocm", new dbsks_exgc_set_params_command(this->gray_ocm_params_));

  //// contour ocm
  //menu.separator();
  //menu.add( (this->examine_ccm_ ? on:off)+ "Examine contour OCM cost?", bvis1_tool_toggle, 
  //  (void*)(&this->examine_ccm_) );
  //menu.add("Set edgemap for contour ocm", 
  //  new dbsks_exgc_set_storage_command("image", &(this->ccm_edgemap_storage_))); 
  //menu.add("Set parameters of contour ocm", new dbsks_exgc_set_params_command(this->ccm_params_));

  //// wcm model
  //menu.separator();
  //menu.add( (this->examine_wcm_model_ ? on:off)+ "Examine CCM model cost?", bvis1_tool_toggle, 
  //  (void*)(&this->examine_wcm_model_) );
  //menu.add("Set parameters of CCM model", new dbsks_exgc_set_params_command(this->wcm_model_params_));


  // ccm model
  menu.separator();
  menu.add( (this->examine_subpix_ccm_ ? on:off)+ "Examine sub-pixel CCM cost?", bvis1_tool_toggle, 
    (void*)(&this->examine_subpix_ccm_) );
  menu.add("Set parameters of sub-pixel CCM", new dbsks_exgc_set_params_command(this->subpix_ccm_params_));

  
  // prepare image cost
  menu.separator();
  menu.add("Prepare image cost function", dbsks_exgc_prepare_image_cost_fnt_wrapper, (void*)(this));
}




// ----------------------------------------------------------------------------
//: Handle events
bool dbsks_examine_xgraph_cost_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  // gestures to display costs
  if (this->gesture_display_part_cost_(e))
  {
    return this->handle_display_part_cost();
  }


  if (this->gesture_display_graph_cost_(e))
  {
    return this->handle_display_graph_cost();
  }

  if (e.type == vgui_OVERLAY_DRAW)
  {
    return this->handle_overlay_redraw();
  }

  return false;
}



// -----------------------------------------------------------------------------
//: compute vertex depths for the xshock graph using active_xnode as root
void dbsks_examine_xgraph_cost_tool::
compute_xgraph_vertex_depths()
{
  dbsksp_xshock_graph_sptr xgraph = this->xgraph_storage()->xgraph();
  dbsksp_xshock_node_sptr xv = 0;
  
  // for subpix_ccm, root node is encoded in the ccm_model file
  if (this->examine_subpix_ccm_)
  {
    if (this->subpix_ccm_like_.xgraph_ccm())
    {
      xv = xgraph->node_from_id(this->subpix_ccm_like_.xgraph_ccm()->root_vid());
    }
  }
  else
  {
    xv = this->xgraph_storage()->active_xnode();
  }

  if (!xv)
  {
    vcl_cout << "\nERROR: couldn't determine root node" << vcl_endl;
    return;
  }
  xgraph->compute_vertex_depths(xv->id());
  return;
}


// -----------------------------------------------------------------------------
//: Display cost of the clicked component (boundary arc or fragment)
bool dbsks_examine_xgraph_cost_tool::
handle_display_part_cost()
{
  unsigned int highlighted_id = this->tableau()->get_highlighted();
  if (highlighted_id)
  {
    vgui_soview* so = vgui_soview::id_to_object(highlighted_id);
    if (so->type_name() == "dbsksp_soview_xshock_chord")
    {
      dbsksp_soview_xshock_chord* chord = 
        static_cast<dbsksp_soview_xshock_chord* >(so);

      // Selected xedge
      dbsksp_xshock_edge_sptr xe = chord->xedge();

      // Retrieve edge id
      unsigned edge_id = xe->id();

      // handler of fragment cost
      return this->handle_display_xfrag_cost(xe);
    }
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Display cost of a fragment
bool dbsks_examine_xgraph_cost_tool::
handle_display_xfrag_cost(const dbsksp_xshock_edge_sptr& xe)
{
  vcl_cout << "\nEdge id= " << xe->id() << "\n";
  
  // parent and child nodes
  dbsksp_xshock_node_sptr xv_parent = xe->parent_node();
  dbsksp_xshock_node_sptr xv_child = xe->child_node();

  if (!xv_parent || !xv_child)
  {
    vcl_cout << "\nERROR: couldn't determine child and parent of edge id=" << xe->id() << vcl_endl;
    return true;
  }

  // form fragment following tree-order
  dbsksp_xshock_node_descriptor start = *(xv_parent->descriptor(xe));
  dbsksp_xshock_node_descriptor end = xv_child->descriptor(xe)->opposite_xnode();
  dbsksp_xshock_fragment xfrag(start, end);


  ////>>> display various types of cost
  //if (this->examine_gray_ocm_)
  //{
  //  // Compute gray OCM cost
  //  float gray_ocm_cost = 0;
  //  dbsks_compute_ocm_cost(&this->gray_ocm_, &this->biarc_sampler_, xfrag, gray_ocm_cost);
  //  vcl_cout << "  Gray OCM cost= " << gray_ocm_cost << vcl_endl;
  //}

  //else if (this->examine_ccm_)
  //{
  //  float ccm_cost = -1.0f;
  //  dbsks_compute_ocm_cost(&this->ccm_, &this->biarc_sampler_, xfrag, ccm_cost);
  //  vcl_cout << "  Contour OCM cost= " << ccm_cost << vcl_endl;
  //}

  //else if (this->examine_wcm_model_)
  //{
  //  float ccm_cost = vnl_numeric_traits<float >::maxval;
  //  dbsks_compute_ocm_cost(&this->wcm_, &this->biarc_sampler_, xfrag, ccm_cost);
  //  vcl_cout << "\n  CCM cost = " << ccm_cost << ".\n";

  //  float wcm_likelihood = vnl_numeric_traits<float >::maxval;
  //  wcm_likelihood = this->wcm_like_.loglike(xe->id(), xfrag);
  //  vcl_cout << "  WCM likelihood= " << wcm_likelihood << ".\n";
  //}

  //else 
  if (this->examine_subpix_ccm_)
  {
    float subpix_ccm_loglike = vnl_numeric_traits<float >::maxval;

    vcl_vector<int > x_vec[2], y_vec[2], angle_vec[2];
    

    subpix_ccm_loglike = this->subpix_ccm_like_.loglike(xe->id(), xfrag,
      x_vec[0], y_vec[0], angle_vec[0],
      x_vec[1], y_vec[1], angle_vec[1]);

    float radians_per_bin = this->subpix_ccm_like_.biarc_sampler()->radians_per_bin();    
    
    // save the sample points for drawing
    for (int k = 0; k < 2; ++k)
    {
      unsigned num_pts = x_vec[k].size();
      this->sample_x_[k].resize(num_pts);
      this->sample_y_[k].resize(num_pts);
      this->sample_angle_[k].resize(num_pts);
    
      for (unsigned i =0; i < num_pts; ++i)
      {
        this->sample_x_[k][i] = x_vec[k][i];
        this->sample_y_[k][i] = y_vec[k][i];
        this->sample_angle_[k][i] = radians_per_bin * angle_vec[k][i];
      }
    }

    // save the corresponding edgels
    for (int k =0; k < 2; ++k)
    {
      unsigned num_pts = x_vec[k].size();
      this->edgel_x_[k].clear();
      this->edgel_y_[k].clear();
      this->edgel_angle_[k].clear();

      for (unsigned i =0; i < num_pts; ++i)
      {
        int ix = x_vec[k][i];
        int iy = y_vec[k][i];
        int ibin = angle_vec[k][i];
        dbdet_edgel* edgel = this->subpix_ccm_like_.ccm()->matched_edgel(ix, iy, ibin);

        if (edgel)
        {
          this->edgel_x_[k].push_back(edgel->pt.x());
          this->edgel_y_[k].push_back(edgel->pt.y());
          this->edgel_angle_[k].push_back(edgel->tangent);
        }
      
      }

    
    }
    
    vcl_cout << "  Subpix CCM likelihood= " << subpix_ccm_loglike << ".\n";
    this->tableau()->post_overlay_redraw();
  }
  return true;
}






// -----------------------------------------------------------------------------
//:
bool dbsks_examine_xgraph_cost_tool::
handle_display_graph_cost()
{
  dbsksp_xshock_graph_sptr xgraph = this->xgraph_storage()->xgraph();
  if (!xgraph)
  {
    vcl_cout << "\nERROR: couldn't find xshock graph." << vcl_endl;
    return true;
  }

  vcl_cout << "\nGraph cost - #edges= " << xgraph->number_of_edges() << vcl_endl;
  

  
  // determine the ignored edge
  vcl_vector<unsigned int > ignored_edges;
  ignored_edges.clear();

  //// compute each cost type separately
  //if (this->examine_gray_ocm_)
  //{
  //  // Compute gray OCM cost
  //  float gray_ocm_cost = 0;
  //  
  //  dbsks_compute_ocm_cost(&this->gray_ocm_, &this->biarc_sampler_, xgraph, gray_ocm_cost, 
  //    ignored_edges, false);
  //  vcl_cout << "  Gray OCM cost= " << gray_ocm_cost 
  //    << " - Confidence= " << 1-(gray_ocm_cost / xgraph->number_of_edges()) << vcl_endl;
  //}

  //if (this->examine_ccm_)
  //{
  //  // compute contour ocm cost
  //  float ccm_cost = 0;
  //  dbsks_compute_ocm_cost(&this->ccm_, &this->biarc_sampler_, xgraph, ccm_cost,
  //    ignored_edges, false);
  //  vcl_cout << "  Contour OCM cost= " << ccm_cost 
  //    << " - Confidence= " << 1-(ccm_cost/xgraph->number_of_edges()) << vcl_endl;
  //}

  //if (this->examine_wcm_model_)
  //{
  //  // compute contour ocm cost
  //  float ccm_cost = 0;
  //  dbsks_compute_ocm_cost(&this->wcm_, &this->biarc_sampler_, xgraph, ccm_cost,
  //    ignored_edges, false);
  //  vcl_cout << "  CCM cost= " << ccm_cost << "\n";

  //  float wcm_like = 0;
  //  this->wcm_like_.loglike_xgraph(xgraph, ignored_edges, false);
  //  vcl_cout << "  WCM likelihood= " << wcm_like << "\n";    
  //}


  if (this->examine_subpix_ccm_)
  {
    float subpix_ccm_loglike = vnl_numeric_traits<float >::maxval;

    vcl_vector<int > x_vec[2], y_vec[2], angle_vec[2];
    

    subpix_ccm_loglike = this->subpix_ccm_like_.loglike_xgraph(this->xgraph_storage()->xgraph(), 
      vcl_vector<unsigned >(0), true);

    vcl_cout << "\n subpix_ccm_loglike = " << subpix_ccm_loglike << "\n";
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Prepare image cost functions
void dbsks_examine_xgraph_cost_tool::
prepare_image_cost_fnt()
{
  ////
  //if (this->examine_gray_ocm_)
  //{
  //  this->prepare_gray_ocm_cost_fnt();
  //  this->prepare_biarc_sampler_fnt();
  //  return;
  //}

  //else if (this->examine_ccm_)
  //{
  //  this->prepare_ccm_cost_fnt();
  //  this->prepare_biarc_sampler_fnt();
  //  return;
  //}

  //else if (this->examine_wcm_model_)
  //{
  //  this->prepare_wcm_model_cost_fnt();
  //  this->prepare_biarc_sampler_fnt();
  //  return;
  //}

  //else 
  if (this->examine_subpix_ccm_)
  {
    this->prepare_subpix_ccm_cost_fnt();
    return;
  
  }
  return;
}








//// -----------------------------------------------------------------------------
////: Prepare gray ocm cost function
//void dbsks_examine_xgraph_cost_tool::
//prepare_gray_ocm_cost_fnt()
//{
//  vcl_cout << "\n>>>Preparing cost function for Gray-OCM...\n";
//  // get view of the image
//  if (!this->gray_ocm_image_storage_ || !(this->gray_ocm_image_storage_->get_image()))
//  {
//    vcl_cout << "\nERROR: image storage has not been set up correctly.\n";
//    return;
//  }
//  vil_image_view<float > image_view = *vil_convert_cast(float(), 
//    this->gray_ocm_image_storage_->get_image()->get_view());
//
//  // get view of the edgemap
//  if (!this->gray_ocm_edgemap_storage_ || !(this->gray_ocm_edgemap_storage_->get_image()))
//  {
//    vcl_cout << "\nERROR: edgemap storage has not been set up correctly.\n";
//    return;
//  }
//  vil_image_view<float > edgemap_view = *vil_convert_cast(float(),
//    this->gray_ocm_edgemap_storage_->get_image()->get_view());
//
//  dbsks_gray_ocm* gray_ocm = &(this->gray_ocm_);
//
//  // Retrieve cost parameters
//  float edge_threshold;
//  float distance_threshold;
//  float tol_near_zero;
//  int nchannel;
//  float lambda;
//
//  if (!this->gray_ocm_params_->get_value("-gray-ocm-edge-threshold", edge_threshold) ||
//    !this->gray_ocm_params_->get_value("-gray-ocm-distance-threshold", distance_threshold) ||
//    !this->gray_ocm_params_->get_value("-gray-ocm-tol-near-zero", tol_near_zero) ||
//    !this->gray_ocm_params_->get_value("-gray-ocm-lambda", lambda) ||
//    !this->gray_ocm_params_->get_value("-gray-ocm-nchannel-0topi", nchannel))
//  {
//    vcl_cout << "\nERROR: couldn't retrieve values of all parameters of gray_ocm.\n";
//    return;
//  }
//
//  // set up input data
//  gray_ocm->set_image(image_view);
//  gray_ocm->set_edgemap(edgemap_view, edge_threshold, 255);
//  gray_ocm->set_lambda(lambda);
//  gray_ocm->set_ocm_params(distance_threshold, tol_near_zero, nchannel);
//
//  // pre-compute chamfer cost and orientation cost at every pixel and orientation
//  gray_ocm->compute();
//  vcl_cout << "Done.>>>" << vcl_endl;
//  return;
//}
//
//
//// -----------------------------------------------------------------------------
////: Prepare contour ocm cost function
//void dbsks_examine_xgraph_cost_tool::
//prepare_ccm_cost_fnt()
//{
//  vcl_cout << "\n>>>Preparing cost function for contour-OCM...\n";
//
//  // get view of the edgemap
//  if (!this->ccm_edgemap_storage_ || !(this->ccm_edgemap_storage_->get_image()))
//  {
//    vcl_cout << "\nERROR: edgemap storage has not been set up correctly.\n";
//    return;
//  }
//  vil_image_view<float > edgemap = *vil_convert_cast(float(),
//    this->ccm_edgemap_storage_->get_image()->get_view());
//
//  // get parameters
//  float edge_threshold, distance_threshold, tol_near_zero, lambda, gamma;
//  int nchannel_0topi;
//  bpro1_filepath edgeorient_file;
//
//  if (!this->ccm_params_->get_value("-contour-ocm-edge-threshold", edge_threshold) ||
//    !this->ccm_params_->get_value("-contour-ocm-edgeorient-file", edgeorient_file) ||
//    !this->ccm_params_->get_value("-contour-ocm-distance-threshold", distance_threshold) ||
//    !this->ccm_params_->get_value("-contour-ocm-tol-near-zero", tol_near_zero) ||
//    !this->ccm_params_->get_value("-contour-ocm-lambda", lambda) ||
//    !this->ccm_params_->get_value("-contour-ocm-gamma", gamma) ||
//    !this->ccm_params_->get_value("-contour-ocm-nchannel-0topi", nchannel_0topi))
//  {
//    vcl_cout << "\nERROR: couldn't retrieve values of all parameters of ccm.\n";
//    return;
//  }
//
//
//  // loading edge angle file and construct an image for it
//  vnl_file_matrix<double > theta(edgeorient_file.path.c_str());
//  if (theta.empty())
//  {
//    vcl_cout << "\nERROR: couldn't load edge orientation file: " << edgeorient_file << vcl_endl;
//    return;
//  }
//
//  if (edgemap.nj() != theta.rows() || edgemap.ni() != theta.cols())
//  {
//    vcl_cout << "\nERROR: edgemap and edge orientation map do not have the same dimension." << vcl_endl;
//    return;
//  }
//
//
//  // create edge angle image view from a matrix
//  vil_image_view<float > edge_angle(edgemap.ni(), edgemap.nj());
//  for (unsigned i =0; i < edge_angle.ni(); ++i)
//  {
//    for (unsigned j =0; j < edge_angle.nj(); ++j)
//    {
//      edge_angle(i, j) = float(theta(j, i));
//    }
//  }
//
//
//
//
//  //////////////////////////////////////////////////////////////////////////////
//  //>> Contour OCM cost function
//  vcl_cout << "\nConstructing contour-OCM cost function ...";
//  vul_timer timer;
//  timer.mark();
//  dbsks_ccm& ccm = (this->ccm_);
//  ccm.set_ocm_params(distance_threshold, tol_near_zero, lambda, gamma, nchannel_0topi);
//  ccm.set_edge_strength(edgemap, edge_threshold, 255);
//  ccm.set_edge_orient(edge_angle);
//  ccm.compute();
//
//  vcl_cout << "done" << vcl_endl;;
//  vcl_cout << "    Time spent = ";
//  timer.print(vcl_cout);
//
//  vcl_cout << "Done.>>>" << vcl_endl;
//  return;
//}
//
//
//
//
//
//
//// -----------------------------------------------------------------------------
////: Prepare CCM model cost function
//void dbsks_examine_xgraph_cost_tool::
//prepare_wcm_model_cost_fnt()
//{
//  //>> Whole-Contour-Matching
//  vcl_cout << "\n>> Constructing Whole-Contour-Matching cost function ...";
//
//  vul_timer timer;
//  timer.mark();
//
//
//  // edgemap
//  bpro1_filepath edgemap_file;
//  this->wcm_model_params_->get_value("-edgemap-file", edgemap_file);
//  vil_image_view<float > edgemap;
//  dbsks_load_edgemap(edgemap_file.path, edgemap);
//
//  // edge angle
//  bpro1_filepath edgeorient_file;
//  this->wcm_model_params_->get_value("-edgeorient-file", edgeorient_file);
//  vil_image_view<float > edge_angle;
//  dbsks_load_edge_angle(edgeorient_file.path, edge_angle);
//
//  // contour-edge map
//  bpro1_filepath cemv_file;
//  this->wcm_model_params_->get_value("-cemv-file", cemv_file);
//  vcl_vector<vsol_polyline_2d_sptr > polyline_list;
//  dbsks_load_polyline_list(cemv_file.path, polyline_list);
//
//  // CCM model file
//  bpro1_filepath xgraph_ccm_file;
//  this->wcm_model_params_->get_value("-xgraph-ccm-file", xgraph_ccm_file);
//  dbsks_xgraph_ccm_model_sptr xgraph_ccm = 0;
//  dbsks_load_xgraph_ccm_model(xgraph_ccm_file.path, xgraph_ccm);
//  
//  
//  
//
//  //////////////////////////////////////////////////////////////////////////////
//  // Construct CCM model
//   
//  dbsks_prepare_ccm(&this->wcm_, xgraph_ccm, edgemap, edge_angle);
//
//  // set the edge label image
//  this->wcm_.set_edge_labels(polyline_list);
//  this->wcm_.set_weight_unmatched(0.0f); // remove effect of whole-contour-matching
//  this->wcm_.compute();
//
//  ///////////////////////////////////////////////////////////////////////
//  //>> Build a likelihood calculator
//  this->wcm_like_.set(xgraph_ccm, &this->wcm_, &dbsks_biarc_sampler::default_instance());
//  
//  
//
//
//  vcl_cout << "[ OK ]\n";
//  vcl_cout << "    Time spent = ";
//  timer.print(vcl_cout);
//
//  /////////////////////////////////////////////////////////////////////
//  //>> Re-compute the vertex depths using the root node in the ccm model
//  vcl_cout << ">> Recomputing vertex depths using root vertex in the CCM mode ... ";
//  this->xgraph_storage()->xgraph()->compute_vertex_depths(xgraph_ccm->root_vid());
//  vcl_cout << "[ OK ]\n";
//
//  return;
//}
//
//



// -----------------------------------------------------------------------------
//: Prepare biarc sampler
void dbsks_examine_xgraph_cost_tool::
prepare_biarc_sampler_fnt()
{
  vcl_cout << "\n<<< Constructing a biarc sampler ...";

  dbsks_biarc_sampler* biarc_sampler = &(this->biarc_sampler_);

  // Set parameters of biarc sampler
  dbsks_biarc_sampler_params bsp;
  bsp.set_to_default_values();

  // compute coordinates of the grid points
  biarc_sampler->set_grid(bsp);

  // compute prototype sample points for all biarcs in the grid
  float ds = 2;
  int nchannel_0to2pi = 36;
  if (!this->biarc_sampler_params_->get_value("-biarc-sampler-ds", ds) ||
    !this->biarc_sampler_params_->get_value("-biarc-sampler-nchannel-0to2pi", nchannel_0to2pi))
  {
    vcl_cout << "\nERROR: couldn't retrieve all parameters of biarc_sampler.\n";
    return;
  }

  biarc_sampler->set_sampling_params(nchannel_0to2pi, ds);
  biarc_sampler->compute_cache_sample_points();
  vcl_cout << "Done.>>>" << vcl_endl;

  return;
}







//------------------------------------------------------------------------------
void dbsks_examine_xgraph_cost_tool::
prepare_subpix_ccm_cost_fnt()
{
  vcl_cout << "Preparing subpix ccm cost function ... \n";

  vul_timer timer;
  timer.mark();

  // some annoucement
  vcl_cout << "\n> Constructing a likelihood function based on CCM cost ...";


  bpro1_filepath temp;

  //1) Load edge map
  dbdet_edgemap_sptr edgemap = 0;
  
  // edge-image file
  this->subpix_ccm_params_->get_value("-edge-image", temp);
  vcl_string edge_image_file = temp.path;

  // edge-orientation file
  this->subpix_ccm_params_->get_value("-edge-orient", temp);
  vcl_string edge_orient_file = temp.path;

  float log2_scale_up_factor = 0;
  this->subpix_ccm_params_->get_value("-log2-scale-up-factor", log2_scale_up_factor);

  // Load the edgel map from two files: edge image and edge orient
  edgemap = dbsks_load_subpix_edgemap(edge_image_file, edge_orient_file, 15.0f, 255.0f);
  dbdet_resize_edgemap(edgemap, vcl_pow(2, log2_scale_up_factor));


  //2) Load CCM model
  dbsks_xgraph_ccm_model_sptr xgraph_ccm = 0;

  this->subpix_ccm_params_->get_value("-xgraph-ccm-model", temp);
  vcl_string xgraph_ccm_file = temp.path;
  x_read(xgraph_ccm_file, xgraph_ccm);

  
  //3) Load biarc sampler
  dbsks_biarc_sampler* biarc_sampler = &dbsks_biarc_sampler::default_instance();

  if (!edgemap || !xgraph_ccm || !biarc_sampler)
  {
    vcl_cout << "\nERROR: could not construct submix_ccm cost function.\n";
    return;
  }

  //4) Detection window
  vgl_box_2d<int > window(0, edgemap->width()-1, 0, edgemap->height()-1);    
  vcl_cout 
    << "\n Detection window  [xmin ymin xmax ymax] = "
    << "[" << window.min_x() 
    << " " << window.min_y() 
    << " " << window.max_x()
    << " " << window.max_y() << "]\n";


  //5) Initialize CCM likelihood calculator
  this->subpix_ccm_like_.set_edgemap(edgemap);
  this->subpix_ccm_like_.set_biarc_sampler(biarc_sampler);
  this->subpix_ccm_like_.set_ccm_model(xgraph_ccm);
  
  //6) Compute cache data
  this->subpix_ccm_like_.compute_internal_data(window);

  vcl_cout << "Compute subpix_ccm cost function completed.\n";

  //
  //this->xgraph_storage()->set_image(vil_load(edge_image_file.c_str()));
  ////

  this->edgemap_tab_ = dbdet_edgemap_tableau_new();
  this->edgemap_tab_->set_edgemap(edgemap);

  //// create an image from the edgemap
  //vil_image_view<vxl_byte > bw_image;
  //dbdet_convert_edgemap_to_image(*edgemap, bw_image);
  //this->edgemap_tab_ = vgui_image_tableau_new(bw_image);



  // create an image from edgemap
  


  this->tableau()->set_child(this->edgemap_tab_);


  //bvis1_manager::instance()->update_tableau(this->tableau(), this->xgraph_storage());

  bvis1_manager::instance()->post_redraw();
  return;
}



//------------------------------------------------------------------------------
//: Handle overlay redraw
bool dbsks_examine_xgraph_cost_tool::
handle_overlay_redraw()
{


  //
  if (this->edgemap_tab_)
  {
    this->edgemap_tab_->draw_edgels();
  }

       //START HERE






  // Draw the sample lines
  for (int k =0; k < 2; ++k)
  {
    glColor3f(0, 1, 0);
    glLineWidth (2);

    float d = 1;
    
    glBegin(GL_LINES);
    unsigned num_pts = this->sample_x_[k].size();
    for (unsigned i =0; i < num_pts; ++i)
    {
      float x = this->sample_x_[k][i];
      float y = this->sample_y_[k][i];
      float a = this->sample_angle_[k][i];

      float x0 = x - vcl_cos(a) * d;
      float y0 = y - vcl_sin(a) * d;

      float x1 = x + vcl_cos(a) * d;
      float y1 = y + vcl_sin(a) * d;

      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
    }
    glEnd();
  }


  // Draw the sample points
  GLfloat r[] = {0, 1.0f};
  GLfloat g[] = {0.3f, 0};
  GLfloat b[] = {1.0f, 0};

  
  for (int k =0; k < 2; ++k)
  {
    glColor3f(r[k], g[k], b[k]);

    glPointSize (3);
    glBegin(GL_POINTS);

    unsigned num_pts = this->sample_x_[k].size();
    for (unsigned i =0; i < num_pts; ++i)
    {
      glVertex2f(this->sample_x_[k][i], this->sample_y_[k][i]);
    }
    glEnd();
  }



  // Draw the matched edgels
  for (int k =0; k < 2; ++k)
  {
    glColor3f(1, 0, 0);
    glLineWidth (2);

    float d = 1;
    
    glBegin(GL_LINES);
    unsigned num_pts = this->edgel_x_[k].size();
    for (unsigned i =0; i < num_pts; ++i)
    {
      float x = this->edgel_x_[k][i];
      float y = this->edgel_y_[k][i];
      float a = this->edgel_angle_[k][i];

      float x0 = x - vcl_cos(a) * d;
      float y0 = y - vcl_sin(a) * d;

      float x1 = x + vcl_cos(a) * d;
      float y1 = y + vcl_sin(a) * d;

      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
    }
    glEnd();
  }



  return true;
}



//------------------------------------------------------------------------------









//==============================================================================
// Wrapper functions
//==============================================================================

//------------------------------------------------------------------------------
//: Wrapper to call the "compute_xgraph_vertex_depths_wrapper" from outside
void dbsks_exgc_compute_xgraph_vertex_depths_wrapper(const void* toolref)
{
  dbsks_examine_xgraph_cost_tool* tool = (dbsks_examine_xgraph_cost_tool*) toolref;
  tool->compute_xgraph_vertex_depths();
}



// -----------------------------------------------------------------------------
//: Wrapper to call the "compute_xgraph_vertex_depths_wrapper" from outside
void dbsks_exgc_prepare_image_cost_fnt_wrapper(const void* toolref)
{
  dbsks_examine_xgraph_cost_tool* tool = (dbsks_examine_xgraph_cost_tool*) toolref;
  tool->prepare_image_cost_fnt();
}



// ============================================================================
// dbsks_exgc_set_storage_command
// ============================================================================

// -----------------------------------------------------------------------------
//: 
void dbsks_exgc_set_storage_command::
execute()
{
  //Prompt the user to select input/output variable
  vgui_dialog io_dialog(("Select storage of type "+this->storage_type_).c_str() );

  //display input options
  vcl_string input_type = this->storage_type_;
  io_dialog.message("Select Input(s) From Available ones:");

  //store the choices
  int input_choice;
  vcl_vector <vcl_string> available_storages;
  vcl_string input_name;

  //get the repository and extract the qualified ones
  vidpro1_repository_sptr repository_sptr = bvis1_manager::instance()->repository();
  
  //for this input type allow user to select from available storage classes in the repository
  available_storages = repository_sptr->get_all_storage_class_names(input_type);

  if (available_storages.empty())
  {
    vcl_cout << "ERROR: no storage of type \"" << this->storage_type_ << "\" is available." << vcl_endl;
    return;
  }

  //Multiple choice - with the list of options.
  io_dialog.choice(input_type.c_str(), available_storages, input_choice); 

  //display dialog
  if (io_dialog.ask())
  {

    // get the names of the user-select storage
    vcl_string storage_name = available_storages.at(input_choice);
    *(this->storage_) = repository_sptr->get_data_by_name(storage_name);
    return;
  }
  else // user has selected "cancel"
  {
      return;
  }
  
}





// ============================================================================
// dbsks_exgc_set_params_command
// ============================================================================

// -----------------------------------------------------------------------------
//: 
void dbsks_exgc_set_params_command::
execute()
{
  if (!this->params_)
  {
    vcl_cout << "\nERROR: Parameter list is empty." << vcl_endl;
    return;
  }

  // Pop up a dialogue box displaying all parameters in bpro1_parameters and let
  // user modify them
  vgui_dialog param_dialog("Set parameters");
  vcl_vector< bpro1_param* > param_vector = this->params_->get_param_list();
  for( vcl_vector< bpro1_param* >::iterator it = param_vector.begin(); it != 
    param_vector.end();  ++it ) 
  {
    if( bpro1_param_type<int> * param = dynamic_cast<bpro1_param_type<int> *>(*it) ) {
      param_dialog.field( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_choice_param_type * param = dynamic_cast<bpro1_choice_param_type *>(*it) ) {
      param_dialog.choice( param->description().c_str() , param->choices(), param->temp_ref() );
    }
    else if( bpro1_param_type<unsigned int> * param = dynamic_cast<bpro1_param_type<unsigned int> *>(*it) ) {
      param_dialog.field( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_param_type<float> * param = dynamic_cast<bpro1_param_type<float> *>(*it) ) {
      param_dialog.field( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_param_type<double> * param = dynamic_cast<bpro1_param_type<double> *>(*it) ) {
      param_dialog.field( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_param_type<vcl_string> * param = dynamic_cast<bpro1_param_type<vcl_string> *>(*it) ) {
      param_dialog.field( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_param_type<bool> * param = dynamic_cast<bpro1_param_type<bool> *>(*it) ) {
      param_dialog.checkbox( param->description().c_str() , param->temp_ref() );
    }
    else if( bpro1_param_type<bpro1_filepath> * param = dynamic_cast<bpro1_param_type<bpro1_filepath> *>(*it) ) {
      param_dialog.file( param->description().c_str(), param->temp_ref().ext, param->temp_ref().path );
    }
    else{
      vcl_cerr << "No valid dialog interface for parameter: " << (*it)->name() << vcl_endl;
    }
  }

  if (!param_vector.empty())
  {
    if (param_dialog.ask()) 
    {
      for(vcl_vector< bpro1_param* >::iterator it = param_vector.begin(); it != param_vector.end(); ++it)
      {
        (*it)->set_from_temp();

        //
        if( bpro1_param_type<bpro1_filepath> * param = dynamic_cast<bpro1_param_type<bpro1_filepath> *>(*it) ) 
        {
          bpro1_filepath path(param->value());
          bvis1_util::handle_file_name_prefix(path.path);
          param->set_value(path);
        }
      }
    }
    else  // The user has selected "cancel"
    {
      return;
    }
  }
}











