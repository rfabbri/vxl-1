// This is brcv/seg/dbdet/pro/dbdet_generic_linker_process.cxx

//:
// \file

#include "dbdet_generic_linker_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/algo/dbdet_generic_linker.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

//: Constructor
dbdet_generic_linker_process::dbdet_generic_linker_process()
{
  if ( 
      //parameters
      !parameters()->add( "Edge Strength Threshold" , "-thresh" , 1.0 ) ||      
      !parameters()->add( "Require Appearance Consistency"    , "-req_app_con", false ) ||
      !parameters()->add( "  Appearance Threshold" , "-app_thresh" , 8.0 ) ||

      //!parameters()->add( "Require Edge Orientation Consistency"    , "-req_ori_con", true ) ||
      //!parameters()->add( "Require Smooth Continuation"    , "-req_smooth_con", true ) ||
      //!parameters()->add( "Require Low Curvatures"    , "-req_low_ks", false ) ||
      
      !parameters()->add( "Post Process to Break Contours"    , "-bpost_process", false ) ||
      !parameters()->add( "  Window size" , "-win_len" , (unsigned) 7 ) ||
      !parameters()->add( "  Adaptive Threshold" , "-adap_thresh" , 2.0 ) ||
      !parameters()->add( "  Curvature Threshold" , "-k_thresh" , 1.0 ) ||

      !parameters()->add( "Prune Contours"    , "-bprune_cons", false ) ||
      !parameters()->add( "  Length threshold" , "-len_thresh", (unsigned) 3 ) ||
      !parameters()->add( "  Edge Strength Threshold" , "-strength_thresh", (double) 4.0 ) ||
      !parameters()->add( "  Mean Contrast Threshold" , "-contrast_thresh", (double) 4.0 ) ||
      !parameters()->add( "  Adaptive Threshold factor" , "-adap_thresh_fac", (double) 2.0 ) ||
      !parameters()->add( "  d2f Threshold" , "-d2f_thresh", (double) 2.0 ) ||
      !parameters()->add( "  Curvature Threshold", "-avg_k_thresh", (double) 0.2 ) ||
      
      !parameters()->add( "Output contours as vsol", "-output_vsol", false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_generic_linker_process::~dbdet_generic_linker_process()
{
}


//: Clone the process
bpro1_process*
dbdet_generic_linker_process::clone() const
{
  return new dbdet_generic_linker_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_generic_linker_process::name()
{
  return "Generic Edge Linker";
}


//: Return the number of input frame for this process
int
dbdet_generic_linker_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_generic_linker_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_generic_linker_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_generic_linker_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
 
  //get the parameters
  bool output_vsol;
  parameters()->get_value( "-output_vsol", output_vsol );

  //if desired, output the vsol (but this takes too long)
  if (output_vsol)
    to_return.push_back( "vsol2D" );
  else
    to_return.push_back( "sel" );

  return to_return;
}


//: Execute the process
bool dbdet_generic_linker_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_generic_linker_process::execute() - not exactly one input \n";
    return false;
  }
  clear_output();

  //get the parameters
  get_parameters();

  //get the input storage class
  dbdet_edgemap_storage_sptr input_edgemap;
  input_edgemap.vertical_cast(input_data_[0][0]);
  dbdet_edgemap_sptr EM = input_edgemap->get_edgemap();

  // create the sel storage class
  dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();
  output_sel->set_EM(EM);

  //get pointers to the data structures in it
  dbdet_curve_fragment_graph &CFG = output_sel->CFG();
  vbl_array_2d<bool> &EULM = output_sel->EULM(); 
  
  //start the timer
  vul_timer t;

  //construct a generic linker
  dbdet_generic_linker edge_linker(EM, CFG, EULM, threshold);

  //set various flags
  edge_linker.require_appearance_consistency(req_app_con);//orientation consistency
  edge_linker.set_appearance_threshold(app_threshold);

  edge_linker.require_orientation_consistency(req_ori_con);//orientation consistency
  edge_linker.require_smooth_continuation(req_smooth_con);//smooth continuation
  edge_linker.require_low_curvatures(req_low_ks);//low curvatures

  //Extract the contours
  edge_linker.extract_image_contours_by_tracing();

  //post process to break contours
  if (bpost_process){
    appearance_based_post_processing(CFG, win_len, adap_thresh);
    post_process_based_on_curvature(CFG, k_thresh);
  }

  //prune contours
  if (bprune_cons)
    prune_contours(CFG, len_thresh, strength_thresh, contrast_thresh, adap_thresh_fac, d2f_thresh, avg_k_thresh);

  double link_time = t.real() / 1000.0;
  vcl_cout << "Time taken to link: " << link_time << " sec" << vcl_endl;

  //output the linked contours as vsol
  if (output_vsol)
  {
    //construct vsol objects from the linked contours
    vcl_vector< vsol_spatial_object_2d_sptr > image_curves;
  
    dbdet_edgel_chain_list_iter f_it = CFG.frags.begin();
    for (; f_it != CFG.frags.end(); f_it++)
    {
      dbdet_edgel_chain* chain = (*f_it);

      vcl_vector<vsol_point_2d_sptr> pts;
      for (unsigned j=0; j<chain->edgels.size(); j++)
        pts.push_back(new vsol_point_2d(chain->edgels[j]->pt));
      vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);

      image_curves.push_back(new_curve->cast_to_spatial_object());
    }

    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(image_curves, "contours");
    output_data_[0].push_back(output_vsol);
  }
  else {
    // output the sel storage class
    output_data_[0].push_back(output_sel);
  }

  return true;
}

bool
dbdet_generic_linker_process::finish()
{
  return true;
}

void
dbdet_generic_linker_process::get_parameters()
{
  //parameters
  parameters()->get_value( "-thresh", threshold);

  parameters()->get_value( "-req_app_con", req_app_con);
  parameters()->get_value( "-app_thresh", app_threshold);

  //temp
  req_ori_con = false;
  req_smooth_con = false;
  req_low_ks = false;

  //parameters()->get_value( "-req_ori_con", req_ori_con);
  //parameters()->get_value( "-req_smooth_con", req_smooth_con);
  //parameters()->get_value( "-req_low_ks", req_low_ks);
      
  parameters()->get_value( "-bpost_process", bpost_process);
  parameters()->get_value( "-win_len", win_len);
  parameters()->get_value( "-adap_thresh", adap_thresh);
  parameters()->get_value( "-k_thresh", k_thresh);

  parameters()->get_value( "-bprune_cons", bprune_cons );
  parameters()->get_value( "-len_thresh", len_thresh );
  parameters()->get_value( "-strength_thresh", strength_thresh );
  parameters()->get_value( "-contrast_thresh", contrast_thresh );
  parameters()->get_value( "-adap_thresh_fac", adap_thresh_fac );
  parameters()->get_value( "-d2f_thresh", d2f_thresh );
  parameters()->get_value( "-avg_k_thresh", avg_k_thresh);
  
  //output type
  parameters()->get_value( "-output_vsol", output_vsol );
}

