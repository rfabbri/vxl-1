// This is brcv/seg/dbdet/pro/dbdet_third_order_edge_detector_process.cxx

//:
// \file

#include "dbdet_third_order_edge_detector_process.h"


#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>

//: Constructor
dbdet_third_order_edge_detector_process::dbdet_third_order_edge_detector_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Gaussian");       //0
  gradient_operator_choices.push_back("h0-operator");    //1
  gradient_operator_choices.push_back("h1-operator");    //2

  vcl_vector<vcl_string> convolution_choices;
  convolution_choices.push_back("2-D");            //0
  convolution_choices.push_back("1-D");            //1

  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  if( !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 0) ||
      !parameters()->add( "Convolution Algo:"   , "-conv_algo" , convolution_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 2.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Apply adaptive threshold "  , "-badap_thresh" , false ) ||
      !parameters()->add( "Parabola Fit type"   , "-parabola_fit" , parabola_fit_type, 0) ||
      !parameters()->add( "Output on Interp. Grid "  , "-binterp_grid" , false ) ||
      !parameters()->add( "Reduce edgel tokens "  , "-breduce" , true ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbdet_third_order_edge_detector_process::~dbdet_third_order_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_third_order_edge_detector_process::clone() const
{
  return new dbdet_third_order_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_third_order_edge_detector_process::name()
{
  return "Third Order Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_third_order_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_third_order_edge_detector_process::output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_third_order_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_third_order_edge_detector_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_third_order_edge_detector_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_third_order_edge_detector_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Third_order edge detection...";
  vcl_cout.flush();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  //get the parameters
  unsigned grad_op, conv_algo, parabola_fit;
  double sigma, thresh;
  int N;
  bool badap_thresh, reduce_tokens, binterp_grid;
  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-conv_algo", conv_algo);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-breduce" , reduce_tokens );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-badap_thresh" , badap_thresh );
  parameters()->get_value( "-binterp_grid" , binterp_grid );
  
  parameters()->get_value( "-parabola_fit", parabola_fit );

  // perfrom third-order edge detection with these parameters
  dbdet_edgemap_sptr edge_map = dbdet_detect_third_order_edges(image_view, sigma, thresh, N, parabola_fit, grad_op, conv_algo, badap_thresh, binterp_grid, reduce_tokens);

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  vcl_cout << "done!" << vcl_endl;
  vcl_cout << "#edgels = " << edge_map->num_edgels() << vcl_endl;

  vcl_cout.flush();

  return true;
}

bool
dbdet_third_order_edge_detector_process::finish()
{
  return true;
}

