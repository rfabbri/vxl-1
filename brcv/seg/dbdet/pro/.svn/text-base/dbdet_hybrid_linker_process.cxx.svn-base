// This is brcv/seg/dbdet/pro/dbdet_hybrid_linker_process.cxx

//:
// \file

#include "dbdet_hybrid_linker_process.h"

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <dbdet/algo/dbdet_postprocess_contours.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curve_model.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource.h>

//: Constructor
dbdet_hybrid_linker_process::dbdet_hybrid_linker_process()
{
  vcl_vector<vcl_string> curve_model_choices;
  curve_model_choices.push_back("Simple Linear model");                   //0
  curve_model_choices.push_back("Linear model");                          //1
  curve_model_choices.push_back("Circular Arc model w/o perturbations");  //2
  curve_model_choices.push_back("Circular Arc model w k classes");        //3
  curve_model_choices.push_back("Circular Arc model with perturbations"); //4
  curve_model_choices.push_back("Circular Arc 3d Bundle");                //5
  curve_model_choices.push_back("Euler Spiral model w/o perturbations");  //6
  curve_model_choices.push_back("Euler Spiral model with perturbations"); //7

  //Curvelet type choices
  vcl_vector<vcl_string> curvelet_type_choices;
  curvelet_type_choices.push_back("Anchor Centered");                      //0
  curvelet_type_choices.push_back("Anchor Centered/Bidirectional");        //1
  curvelet_type_choices.push_back("Anchor Leading/Bidirectional");         //2
  curvelet_type_choices.push_back("ENO Style around Anchor");              //3

  vcl_vector<vcl_string> cvlet_formation_choices;
  cvlet_formation_choices.push_back("From existing contour Only");        //0
  cvlet_formation_choices.push_back("Between existing contours Only");    //1
  cvlet_formation_choices.push_back("Both");                              //2

  vcl_vector<vcl_string> post_processing_choices;
  post_processing_choices.push_back("Do Not Link");                       //0
  post_processing_choices.push_back("Using the Link graph");              //1
  post_processing_choices.push_back("Breaking contours where curve bundles do not form"); //2

  if ( 
      //grouping parameters
      !parameters()->add( "Get Uncertainty from edges" , "-badap_uncer" , false ) ||
      !parameters()->add( "Position uncertainty" , "-dx" , 0.2 ) ||
      !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 15.0 ) ||
      !parameters()->add( "Radius of Neighborhood" , "-nrad" , 7.0 ) ||
      !parameters()->add( "# of edgels per group" , "-max_size_to_group", (unsigned) 7 ) ||

      //curve model
      !parameters()->add( "Curve Model"   , "-curve_model" , curve_model_choices, 5) ||

      //curve model parameters
      !parameters()->add( "  - Maximum Curvature" , "-max_k" , 0.2 ) ||
      !parameters()->add( "  - Maximum Curvature Derivative" , "-max_gamma" , 0.05 ) ||
      
      !parameters()->add( "  - Curvelet Type"   , "-cvlet_type" , curvelet_type_choices, 3) ||
      
      //Hybrid options
      !parameters()->add( "Form curvelets from :"   , "-cvlet_form" , cvlet_formation_choices, 2) ||

      //post processing options
      !parameters()->add( "Filter Contours by :"   , "-post_process" , post_processing_choices, 1))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_hybrid_linker_process::~dbdet_hybrid_linker_process()
{
}


//: Clone the process
bpro1_process*
dbdet_hybrid_linker_process::clone() const
{
  return new dbdet_hybrid_linker_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_hybrid_linker_process::name()
{
  return "Hybrid Edge Linker";
}


//: Return the number of input frame for this process
int
dbdet_hybrid_linker_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_hybrid_linker_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_hybrid_linker_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_hybrid_linker_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}


//: Execute the process
bool dbdet_hybrid_linker_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_hybrid_linker_process::execute() - not exactly one input \n";
    return false;
  }
  clear_output();

  //get the parameters
  get_parameters();

  //get the input sel storage class
  dbdet_sel_storage_sptr input_sel;
  input_sel.vertical_cast(input_data_[0][0]);

  //get pointers to the data structures in it
  dbdet_edgemap_sptr EM = input_sel->EM();
  dbdet_curvelet_map& CM = input_sel->CM();
  dbdet_edgel_link_graph& ELG = input_sel->ELG();
  dbdet_curve_fragment_graph &CFG = input_sel->CFG();

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
  double token_len = 0.7;
  

  //The curvelet formation parameters
  dbdet_curvelet_params cvlet_params(dbdet_curve_model::CC, 
                                     nrad, dt, dx, badap_uncer, 
                                     token_len, max_k, max_gamma,
                                     bCentered_grouping,
                                     bBidirectional_grouping);

  dbdet_sel_sptr edge_linker;
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

  t.mark();

  //This process assumes that contours have already been computed by the generic linker

  //filter contours using curvelets
  vcl_cout << "Forming curvelets from contours..." << vcl_endl;
  
  edge_linker->set_appearance_usage(0);//do not use appearance

  if (cvlet_form==0 || cvlet_form==2)
  {
    edge_linker->use_hybrid_ = false;
    //edge_linker->form_curvelets_from_contours(max_size_to_group);
    edge_linker->form_curvelets_from_contours(true); //use the neighborhood size
  }

  if (cvlet_form==1 || cvlet_form==2)
  {
    //compile the edge -> contour mapping
    edge_linker->compile_edge_to_contour_mapping();

    //use this mapping to avoid forming groupings with edgel skips in them
    edge_linker->use_hybrid_ = true;

    //now construct the curvelet map 
    edge_linker->build_curvelets_greedy(max_size_to_group, false, false);
  }

  if (post_process==1){
    //form the link graph from the curvelets
    edge_linker->use_anchored_curvelets_only();
    edge_linker->construct_the_link_graph(min_size_to_link, linkgraph_algo); //use all curvelets to form links

    edge_linker->extract_regular_contours_from_the_link_graph();
    //edge_linker->extract_image_contours_from_the_link_graph(num_link_iters);
  }
  else if (post_process==2){
    //Now break contours at illegal cuurve bundles
    edge_linker->post_process_to_break_contours();
  }

  double link_time = t.real() / 1000.0;
  vcl_cout << "Time taken to link: " << link_time << " sec" << vcl_endl;

  //report stats
  //edge_linker->report_stats();
  //edge_linker->determine_accuracy_of_measurements();

  return true;
}

bool
dbdet_hybrid_linker_process::finish()
{
  return true;
}

void
dbdet_hybrid_linker_process::get_parameters()
{
  //default parameter values
  min_size_to_link = 4;
  grouping_algo = 0;
  linkgraph_algo = 0;
  num_link_iters = 7;

  //grouping parameters
  parameters()->get_value( "-nrad", nrad);
  parameters()->get_value( "-badap_uncer", badap_uncer);
  parameters()->get_value( "-dx", dx);
  parameters()->get_value( "-dt", dt);
  parameters()->get_value( "-max_size_to_group", max_size_to_group);

  //curve model
  parameters()->get_value( "-curve_model" , curve_model_type);

  //curve model parameters
  parameters()->get_value( "-max_k" , max_k );
  parameters()->get_value( "-max_gamma" , max_gamma );

  parameters()->get_value( "-cvlet_type" , cvlet_type);
  switch(cvlet_type) //set the grouping flags from the choice of cvlet type
  {
  case 0: //Anchor Centered
    bCentered_grouping = true;
    bBidirectional_grouping = false;
    break;
  case 1: //Anchor Centered/Bidirectional
    bCentered_grouping = true;
    bBidirectional_grouping = true;
    break;
  case 2: //Anchor Leading/Bidirectional
    bCentered_grouping = false;
    bBidirectional_grouping = true;
    break;
  case 3: //ENO Style around Anchor
    bCentered_grouping = false;
    bBidirectional_grouping = false;
    break;
  }

  //Hybrid options
  parameters()->get_value( "-cvlet_form" , cvlet_form);

  // post-processing option
  parameters()->get_value( "-post_process" , post_process);

}

