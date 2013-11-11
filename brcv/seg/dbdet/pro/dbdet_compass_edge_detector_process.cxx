// This is brcv/seg/dbdet/pro/dbdet_compass_edge_detector_process.cxx

//:
// \file

#include "dbdet_compass_edge_detector_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_convolve_2d.h>
#include <vil/vil_resample_bilin.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <dbdet/algo/dbdet_nms.h>
#include <dbdet/algo/dbdet_compass_edge_detector.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_utils.h>

//: Constructor
dbdet_compass_edge_detector_process::dbdet_compass_edge_detector_process()
{
  vcl_vector<vcl_string> compass_signature_choices;
  compass_signature_choices.push_back("Simple Histogram");    //0
  compass_signature_choices.push_back("Rayleigh Weighted Histogram");  //1
  compass_signature_choices.push_back("Gaussian Weighted Histogram");  //2

  vcl_vector<vcl_string> histogram_distance_choices;
  histogram_distance_choices.push_back("Chi^2 Distance");         //0
  histogram_distance_choices.push_back("Bhattacharya Distance");  //1
  histogram_distance_choices.push_back("Earth Mover's Distance"); //2

  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  if( !parameters()->add( "Compass Signature"   , "-signature_op"  , compass_signature_choices, 0) ||
      !parameters()->add( "Histogram Distance:" , "-hist_dist_op"  , histogram_distance_choices, 2) ||
      !parameters()->add( "Sigma (R=3*sigma)"   , "-sigma"         , 2.0 ) ||
      !parameters()->add( "# of Orientations"   , "-norient"       , 8 ) ||
    //!parameters()->add( "Spacing"             , "-spacing"       , 1 ) ||
      !parameters()->add( "Savitzky-Golay Filter", "-bSG_filter"   , false ) ||
      !parameters()->add( "Strength Threshold"  , "-thresh"        , 0.4 ) ||
      !parameters()->add( "Subpixel Edges"      , "-bsubpix_edges" , true ) ||
      !parameters()->add( "  Parabola Fit type" , "-parabola_fit"  , parabola_fit_type, 0) ||
      !parameters()->add( "Third-order Orientation", "-bthird_order", false ) ||
      !parameters()->add( "Output Strength Image", "-bout_str"     , false ) ||
      !parameters()->add( "Output Orientation Map", "-bout_ori"    , false )) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_compass_edge_detector_process::~dbdet_compass_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_compass_edge_detector_process::clone() const
{
  return new dbdet_compass_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_compass_edge_detector_process::name()
{
  return "Compass Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_compass_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_compass_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_compass_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_compass_edge_detector_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;

  bool bout_str;
  parameters()->get_value( "-bout_str", bout_str );

  if (bout_str) 
    to_return.push_back( "image" );
  to_return.push_back( "edge_map" );

  return to_return;
}


//: Execute the process
bool
dbdet_compass_edge_detector_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_compass_edge_detector_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Compass edge detection...";
  vcl_cout.flush();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
  
  //get the parameters
  unsigned signature_op, hist_dist_op, parabola_fit;
  double sigma, thresh;
  int Norient, spacing=1;
  bool bsubpix_edges, bthird_order, bout_str, bout_ori, bSG_filter;

  parameters()->get_value( "-signature_op", signature_op);
  parameters()->get_value( "-hist_dist_op", hist_dist_op);
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-norient" , Norient);
  //parameters()->get_value( "-spacing", spacing);
  parameters()->get_value( "-bSG_filter" , bSG_filter);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-parabola_fit", parabola_fit);
  parameters()->get_value( "-bsubpix_edges", bsubpix_edges);
  parameters()->get_value( "-bthird_order", bthird_order);
  parameters()->get_value( "-bout_str", bout_str);
  parameters()->get_value( "-bout_ori", bout_ori);

  //Now call the detector with these parameters
  vil_image_view<double> hist_grad;
  dbdet_edgemap_sptr edge_map = dbdet_detect_compass_edges(image_view, spacing, Norient/2, signature_op, sigma, hist_dist_op, bSG_filter, thresh, bthird_order, hist_grad, bout_ori);

  vcl_cout << "done!" << vcl_endl;

  // create the output storage class
  if (bout_str){
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(hist_grad));
    output_data_[0].push_back(output_storage);
  }
  
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_compass_edge_detector_process::finish()
{
  return true;
}

