// This is brcv/seg/dbdet/pro/dbdet_sel_extract_contours_process.cxx

//:
// \file

#include "dbdet_sel_extract_contours_process.h"

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curve_model.h>

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

//: Constructor
dbdet_sel_extract_contours_process::dbdet_sel_extract_contours_process()
{
  if ( !parameters()->add( "Length threshold" , "-len_thresh", (unsigned) 3 ) ||
       !parameters()->add( "Apply Edge Strength Threshold" , "-apply_strength_thresh", (bool) false ) ||
       !parameters()->add( "  Edge Strength Threshold" , "-strength_thresh", (double) 4.0 ) ||
       !parameters()->add( "Apply Mean Contrast Threshold" , "-apply_contrast_thresh", (bool) false ) ||
       !parameters()->add( "  Mean Contrast Threshold" , "-contrast_thresh", (double) 4.0 ) ||
       !parameters()->add( "Apply Adaptive Contrast Threshold" , "-apply_adap_thresh", (bool) false ) ||
       !parameters()->add( "  Adaptive Threshold factor" , "-adap_thresh_fac", (double) 2.0 ) ||
       !parameters()->add( "Apply d2f Threshold" , "-apply_d2f_thresh", (bool) false ) ||
       !parameters()->add( "  d2f Threshold" , "-d2f_thresh", (double) 2.0 ) ||

       !parameters()->add( "Smooth Extracted Contour" , "-smooth_con", (bool) true ) ||
       !parameters()->add( "  Smoothing Stepsize (psi) ", "-psi", 1.0f ) ||
       !parameters()->add( "  Number of times " , "-num_times" , (unsigned)1 ) ||
       !parameters()->add( "Apply curvature Threshold" , "-apply_k_thresh", (bool) false ) ||
       !parameters()->add( "  Curvature Threshold", "-k_thresh", 0.2f ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_sel_extract_contours_process::~dbdet_sel_extract_contours_process()
{
}


//: Clone the process
bpro1_process*
dbdet_sel_extract_contours_process::clone() const
{
  return new dbdet_sel_extract_contours_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_sel_extract_contours_process::name()
{
  return "Extract Linked Curves";
}


//: Return the number of input frame for this process
int
dbdet_sel_extract_contours_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_sel_extract_contours_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_sel_extract_contours_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_sel_extract_contours_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );

  return to_return;
}


//: Execute the process
bool dbdet_sel_extract_contours_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_sel_extract_contours_process::execute() - not exactly one input \n";
    return false;
  }
  clear_output();

  //get the parameters
  parameters()->get_value( "-len_thresh", len_thresh );
  parameters()->get_value( "-apply_strength_thresh", apply_strength_thresh );
  parameters()->get_value( "-strength_thresh", strength_thresh );
  parameters()->get_value( "-apply_contrast_thresh", apply_contrast_thresh );
  parameters()->get_value( "-contrast_thresh", contrast_thresh );
  parameters()->get_value( "-apply_adap_thresh", apply_adap_thresh );
  parameters()->get_value( "-adap_thresh_fac", adap_thresh_fac );
  parameters()->get_value( "-apply_d2f_thresh", apply_d2f_thresh );
  parameters()->get_value( "-d2f_thresh", d2f_thresh );
  parameters()->get_value( "-smooth_con", smooth_con );
  parameters()->get_value( "-psi", psi);
  parameters()->get_value( "-num_times", num_times);
  parameters()->get_value( "-apply_k_thresh", apply_k_thresh);
  parameters()->get_value( "-k_thresh", k_thresh);
  
  //get the input storage class
  dbdet_sel_storage_sptr input_sel;
  input_sel.vertical_cast(input_data_[0][0]);
  dbdet_curve_fragment_graph& CFG = input_sel->CFG();

  //form vsol curves from the edgel chains in the linker
  vcl_vector< vsol_spatial_object_2d_sptr > image_curves;

  dbdet_edgel_chain_list_iter f_it = CFG.frags.begin();
  for (; f_it != CFG.frags.end(); f_it++)
  {
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

    //-------------------------------------------------------------------
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
        ks[j] = 1/dbgl_arc_algo::compute_arc_radius_from_three_points(pts[k1], pts[k1+1], pts[k1+2]);
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

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(image_curves, "contours");
  output_data_[0].push_back(output_vsol);
  return true;
}

bool
dbdet_sel_extract_contours_process::finish()
{
  return true;
}

