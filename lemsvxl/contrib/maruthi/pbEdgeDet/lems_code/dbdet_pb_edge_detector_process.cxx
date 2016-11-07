// This is brcv/seg/dbdet/pro/dbdet_pb_edge_detector_process.cxx

//:
// \file

#include "dbdet_pb_edge_detector_process.h"

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
#include <dbdet/algo/dbdet_pb_edge_det.h>


//: Constructor
dbdet_pb_edge_detector_process::dbdet_pb_edge_detector_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Sobel (1x3)");    //0
  gradient_operator_choices.push_back("Sobel (3x3)");    //1
  gradient_operator_choices.push_back("Gaussian");       //2
  gradient_operator_choices.push_back("h0-operator");    //3
  gradient_operator_choices.push_back("h1-operator");    //4
  gradient_operator_choices.push_back("F-operator");     //5
  gradient_operator_choices.push_back("R-operator");     //6

  vcl_vector<vcl_string> convolution_choices;
  convolution_choices.push_back("2-D");            //0
  convolution_choices.push_back("1-D");            //1

  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  if( !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 2) ||
      !parameters()->add( "Convolution Algo:"   , "-conv_algo" , convolution_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 5.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Subpixel Edges"  , "-bsubpix_edges" , true ) ||
      !parameters()->add( "Parabola Fit type"   , "-parabola_fit" , parabola_fit_type, 0)) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_pb_edge_detector_process::~dbdet_pb_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_pb_edge_detector_process::clone() const
{
  return new dbdet_pb_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_pb_edge_detector_process::name()
{
  return "Pb Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_pb_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_pb_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_pb_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_pb_edge_detector_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_pb_edge_detector_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbdet_pb_edge_detector_process::execute()" 
                 << " not exactly one input images \n ";
        return false;
    }
    clear_output();

    vcl_cout << "Pb edge detection...";
    vcl_cout.flush();

    // get image from the storage class
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    vil_image_view<vxl_byte> image_view = 
        image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

    //get the parameters
    double sigma, thresh;
    parameters()->get_value( "-sigma", sigma);
    parameters()->get_value( "-thresh", thresh);

    // perfrom third-order edge detection with these parameters
    dbdet_edgemap_sptr edge_map = dbdet_detect_pb_edges(
        image_view, sigma, thresh,3 );

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
dbdet_pb_edge_detector_process::finish()
{
    return true;
}

