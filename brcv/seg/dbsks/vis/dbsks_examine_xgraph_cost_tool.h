// This is contrib/ntrinh/dbsks/vis/dbsks_examine_xgraph_cost_tool.h
#ifndef dbsks_examine_xgraph_cost_tool_h_
#define dbsks_examine_xgraph_cost_tool_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 16, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <dbsks/dbsks_biarc_sampler.h>
//#include <dbsks/dbsks_gray_ocm.h>
//#include <dbsks/dbsks_ccm.h>
//#include <dbsks/dbsks_wcm.h>
//#include <dbsks/dbsks_xshock_wcm_likelihood.h>
#include <dbsks/dbsks_xshock_ccm_likelihood.h>

#include <dbsksp/dbsksp_xshock_edge_sptr.h>

//#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
//#include <dbsksp/pro/dbsksp_shock_storage.h>
//#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
//#include <dbsksp/vis/dbsksp_shock_tableau.h>

#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau.h>

#include <dbdet/vis/dbdet_edgemap_tableau_sptr.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsks_examine_xgraph_cost_tool
// ============================================================================
//: 
class dbsks_examine_xgraph_cost_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsks_examine_xgraph_cost_tool();

  //: Destructor
  virtual ~dbsks_examine_xgraph_cost_tool();
  
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // DATA ACESS ---------------------------------------------------------------

  //: Get the tableau this tool is working with
  dbsksp_xgraph_tableau_sptr tableau() const {return this->tableau_; }


  //: Get the storage associated with the active tableau
  dbsksp_xgraph_storage_sptr xgraph_storage() const { return this->xgraph_storage_; }
;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Display cost of the clicked component (boundary arc or fragment)
  bool handle_display_part_cost();

  //: Display cost of the clicked component (boundary arc or fragment)
  bool handle_display_graph_cost();

  //: Display cost of a fragment
  bool handle_display_xfrag_cost(const dbsksp_xshock_edge_sptr& xe);

  //: Handle overlay redraw
  bool handle_overlay_redraw();
  

  //: compute vertex depths for the xshock graph using active_xnode as root
  void compute_xgraph_vertex_depths();

  //: Prepare image cost functions
  void prepare_image_cost_fnt();

  ////: Prepare gray ocm cost function
  //void prepare_gray_ocm_cost_fnt();

  ////: Prepare contour ocm cost function
  //void prepare_ccm_cost_fnt();

  ////: Prepare CCM model cost function
  //void prepare_wcm_model_cost_fnt();

  //: Prepare biarc sampler
  void prepare_biarc_sampler_fnt();


  //: Prepare subpix-ccm cost function
  void prepare_subpix_ccm_cost_fnt();

protected:
  
  dbsksp_xgraph_tableau_sptr tableau_;
  dbsksp_xgraph_storage_sptr xgraph_storage_;

  //: Biarc sampler
  dbsks_biarc_sampler biarc_sampler_;
  bpro1_parameters_sptr biarc_sampler_params_;

  // Cost functions

  ////: Gray OCM
  //bool examine_gray_ocm_; // true when examining gray ocm
  //dbsks_gray_ocm gray_ocm_;
  //bpro1_parameters_sptr gray_ocm_params_;
  //vidpro1_image_storage_sptr gray_ocm_image_storage_; // image
  //vidpro1_image_storage_sptr gray_ocm_edgemap_storage_; // edgemap

  ////: Contour OCM
  //bool examine_ccm_;
  //dbsks_ccm ccm_;
  //bpro1_parameters_sptr ccm_params_;
  //vidpro1_image_storage_sptr ccm_edgemap_storage_;
  //vcl_string ccm_edgeorient_file_;

  ////: WCM model
  //bool examine_wcm_model_;
  ////
  //bpro1_parameters_sptr wcm_model_params_;
  //dbsks_wcm wcm_;
  //dbsks_xshock_wcm_likelihood wcm_like_;


  //: Subpix CCM
  bool examine_subpix_ccm_;
  bpro1_parameters_sptr subpix_ccm_params_;
  dbsks_xshock_ccm_likelihood subpix_ccm_like_;
  dbdet_edgemap_tableau_sptr edgemap_tab_;


  // sample points used to compute cost
  vcl_vector<float > sample_x_[2], sample_y_[2], sample_angle_[2]; // 0: left, 1: right
  vcl_vector<float > edgel_x_[2], edgel_y_[2], edgel_angle_[2]; // 0: left, 1: right
  

  // Gesture lists
  vgui_event_condition gesture_display_part_cost_;
  vgui_event_condition gesture_display_graph_cost_;
};


//: Wrapper to call the "compute_xgraph_vertex_depths_wrapper" from outside
void dbsks_exgc_compute_xgraph_vertex_depths_wrapper(const void* toolref);

//: Wrapper to call the "compute_xgraph_vertex_depths_wrapper" from outside
void dbsks_exgc_prepare_image_cost_fnt_wrapper(const void* toolref);



// ============================================================================
// dbsks_exgc_set_image_command
// ============================================================================

//: A vgui command to an image for segmentation
class dbsks_exgc_set_storage_command: public vgui_command
{
public:  
  dbsks_exgc_set_storage_command(vcl_string storage_type, bpro1_storage_sptr* storage): 
    storage_type_(storage_type), storage_(storage){}
  virtual ~dbsks_exgc_set_storage_command(){}
  void execute();
protected:
  vcl_string storage_type_;
  bpro1_storage_sptr* storage_;
};



// ============================================================================
// dbsks_exgc_set_params_command
// ============================================================================

//: A gui command to set parameters in a bro1_parameters
class dbsks_exgc_set_params_command: public vgui_command
{
public:  
  dbsks_exgc_set_params_command(const bpro1_parameters_sptr& params):
      params_(params){}
  virtual ~dbsks_exgc_set_params_command(){}
  void execute();
protected:
  bpro1_parameters_sptr params_;
};


#endif //dbsks_examine_xgraph_cost_tool



