// This is brcv/seg/dbdet/pro/dbdet_generic_color_edge_detector_process.cxx

//:
// \file

#include "dbdet_generic_color_edge_detector_process.h"

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
#include <brip/brip_vil_float_ops.h>

#include <bil/algo/bil_color_conversions.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_nms.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_utils.h>

//: Constructor
dbdet_generic_color_edge_detector_process::dbdet_generic_color_edge_detector_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Sobel (1x3)");    //0
  gradient_operator_choices.push_back("Sobel (3x3)");    //1
  gradient_operator_choices.push_back("Gaussian");       //2
  gradient_operator_choices.push_back("h0-operator");    //3
  gradient_operator_choices.push_back("h1-operator");    //4

  vcl_vector<vcl_string> color_conversion_choices;
  color_conversion_choices.push_back("Use RGB");    //0
  color_conversion_choices.push_back("Use IHS");    //1
  color_conversion_choices.push_back("Use Lab");    //2
  color_conversion_choices.push_back("Use Luv");    //3

  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  if( !parameters()->add( "Load Component Images"    , "-bLoadComps"   , false ) ||
      !parameters()->add( "Color Space conversion"   , "-col_conv" , color_conversion_choices, 2) ||
      !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 2) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 3.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Parabola Fit type"   , "-parabola_fit" , parabola_fit_type, 0))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_generic_color_edge_detector_process::~dbdet_generic_color_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_generic_color_edge_detector_process::clone() const
{
  return new dbdet_generic_color_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_generic_color_edge_detector_process::name()
{
  return "Generic Color Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_generic_color_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_generic_color_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_generic_color_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  bool bLoadComps;
  parameters()->get_value( "-bLoadComps", bLoadComps);

  if (bLoadComps){ //load 3 component images
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
  }
  else //single RGB image
    to_return.push_back( "image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_generic_color_edge_detector_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_generic_color_edge_detector_process::execute()
{
  //1) get the parameters
  bool bLoadComps;
  unsigned grad_op, col_conv_opt, parabola_fit, out_type;
  double sigma, thresh;
  int N=0;
  
  parameters()->get_value( "-bLoadComps", bLoadComps);
  parameters()->get_value( "-col_conv", col_conv_opt);
  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-parabola_fit", parabola_fit );
  parameters()->get_value( "-out_type", out_type );

  if (bLoadComps && input_data_.size() != 1 && input_data_[0].size() != 3 ){
    vcl_cout << "In dbdet_generic_color_edge_detector_process::execute() - 3 images needed! \n";
    return false;
  }
  else if (input_data_.size() != 1 && input_data_[0].size() != 1){
    vcl_cout << "In dbdet_generic_color_edge_detector_process::execute() - only one color image needed!\n";
    return false;
  }
  clear_output();

  vcl_cout << "Generic color edge detection...";
  vcl_cout.flush();

  //2) get image(s) from the storage class
  vil_image_resource_sptr col_image_sptr, comp_image1_sptr, comp_image2_sptr, comp_image3_sptr;
  vil_image_view<vxl_byte> col_image;
  vil_image_view<vxl_byte> comp1_view, comp2_view, comp3_view;

  if (bLoadComps){
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    comp_image1_sptr = frame_image->get_image();
    comp1_view = comp_image1_sptr->get_view(0, comp_image1_sptr->ni(), 0, comp_image1_sptr->nj() );

    frame_image.vertical_cast(input_data_[0][1]);
    comp_image2_sptr = frame_image->get_image();
    comp2_view = comp_image2_sptr->get_view(0, comp_image2_sptr->ni(), 0, comp_image2_sptr->nj() );

    frame_image.vertical_cast(input_data_[0][2]);
    comp_image3_sptr = frame_image->get_image();
    comp3_view = comp_image3_sptr->get_view(0, comp_image3_sptr->ni(), 0, comp_image3_sptr->nj() );

    //make sure these images are one plane images
    if (comp1_view.nplanes() != 1 || comp2_view.nplanes() != 1 || comp3_view.nplanes() != 1){
      vcl_cout << "In dbdet_generic_color_edge_detector_process::execute() - component images must be monochromatic! \n";
      return false;
    }
  }
  else {
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    col_image_sptr = frame_image->get_image();
    col_image = col_image_sptr->get_view(0, col_image_sptr->ni(), 0, col_image_sptr->nj() );

    //make sure these images are one plane images
    if (col_image.nplanes() != 3){
      vcl_cout << "In dbdet_generic_color_edge_detector_process::execute() - image must be trichromatic! \n";
      return false;
    }
  }

  //3) convert to the desired color space (component images)
  vil_image_view<float> comp1, comp2, comp3;  

  if (bLoadComps){
    vil_convert_cast(comp1_view, comp1);
    vil_convert_cast(comp2_view, comp2);
    vil_convert_cast(comp2_view, comp3);
  }
  else {
    switch(col_conv_opt){
      case 0: //RGB color space
        vil_convert_cast(vil_plane(col_image, 0), comp1);
        vil_convert_cast(vil_plane(col_image, 1), comp2);
        vil_convert_cast(vil_plane(col_image, 2), comp3);
        break;
      case 1: //IHS color space
        brip_vil_float_ops::convert_to_IHS(col_image, comp1, comp2, comp3);
        break;
      case 2: //Lab color space
        convert_RGB_to_Lab(col_image, comp1, comp2, comp3);
        break;
      case 3: //Luv color space
        convert_RGB_to_Luv(col_image, comp1, comp2, comp3);
        break;
    }
  }

  //start the timer
  vul_timer t;

  //4) compute image gradients of each of the components
  vil_image_view<float> f1_dx, f1_dy, f2_dx, f2_dy, f3_dx, f3_dy;
  int scale=1;

  switch (grad_op)
  {
    case 0: // Sobel 1x3
    {
      vil_sobel_1x3 <float, float> (comp1, f1_dx, f1_dy);
      vil_sobel_1x3 <float, float> (comp2, f2_dx, f2_dy);
      vil_sobel_1x3 <float, float> (comp3, f3_dx, f3_dy);
      break;
    }
    case 1: // Sobel 3x3
    {
      vil_sobel_3x3 <float, float> (comp1, f1_dx, f1_dy);
      vil_sobel_3x3 <float, float> (comp2, f2_dx, f2_dy);
      vil_sobel_3x3 <float, float> (comp3, f3_dx, f3_dy);
      break;
    }
    case 2: //Gaussian
    {  
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      dbdet_subpix_convolve_2d(comp1, f1_dx, dbdet_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp1, f1_dy, dbdet_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dx, dbdet_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dy, dbdet_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dx, dbdet_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dy, dbdet_Gy_kernel(sigma), float(), N);
      break;
    }
    case 3: //h0-operator
    {
      scale = (int) vcl_pow(2.0, N);

      //compute gradients   
      dbdet_subpix_convolve_2d(comp1, f1_dx, dbdet_h0_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp1, f1_dy, dbdet_h0_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dx, dbdet_h0_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dy, dbdet_h0_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dx, dbdet_h0_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dy, dbdet_h0_Gy_kernel(sigma), float(), N);
      break;
    }
    case 4:  //h1-operator
    {
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      dbdet_subpix_convolve_2d(comp1, f1_dx, dbdet_h1_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp1, f1_dy, dbdet_h1_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dx, dbdet_h1_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp2, f2_dy, dbdet_h1_Gy_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dx, dbdet_h1_Gx_kernel(sigma), float(), N);
      dbdet_subpix_convolve_2d(comp3, f3_dy, dbdet_h1_Gy_kernel(sigma), float(), N);
      break;
    }
  }
  
  //5) compute the squared norm of the vector-gradient
  vil_image_view<double> grad_mag, nu1, nu2; //eigenvalue and eigenvector
  grad_mag.set_size(f1_dx.ni(), f1_dx.nj());
  nu1.set_size(f1_dx.ni(), f1_dx.nj());
  nu2.set_size(f1_dx.ni(), f1_dx.nj());

  //get the pointers to the memory chunks
  float *f1x  =  f1_dx.top_left_ptr();
  float *f1y  =  f1_dy.top_left_ptr();
  float *f2x  =  f2_dx.top_left_ptr();
  float *f2y  =  f2_dy.top_left_ptr();
  float *f3x  =  f3_dx.top_left_ptr();
  float *f3y  =  f3_dy.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();
  double *n1  =  nu1.top_left_ptr();
  double *n2  =  nu2.top_left_ptr();

  //compute the squared norm of gradient
  for(unsigned long i=0; i<grad_mag.size(); i++){
    double A = f1x[i]*f1x[i]+f2x[i]*f2x[i]+f3x[i]*f3x[i];
    double B = f1x[i]*f1y[i]+f2x[i]*f2y[i]+f3x[i]*f3y[i];
    double C = f1y[i]*f1y[i]+f2y[i]*f2y[i]+f3y[i]*f3y[i];
    double d = sqrt((A-C)*(A-C) + 4*B*B);
    double c = (A-C)/d;

    n1[i] = sqrt((1+c)/2);
    n2[i] = vnl_math_sgn(B)*sqrt((1-c)/2);
    g_mag[i] = vcl_sqrt((A+C+d)/2/3); //take the square root of the squared norm
  }


  double conv_time = t.real();  
  t.mark(); //reset timer

  //Now call the nms code to get the subpixel edge tokens
  vcl_vector<vgl_point_2d<double> > loc;
  vcl_vector<double> orientation, mag;

  dbdet_nms NMS(dbdet_nms_params(thresh, (dbdet_nms_params::PFIT_TYPE)parabola_fit), nu1, nu2, grad_mag);
  NMS.apply(true, loc, orientation, mag);

  double nms_time = t.real();
  
  //create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(comp1.ni(), comp1.nj());

  for (unsigned i=0; i<loc.size(); i++){
    vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);
    edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
  }

  vcl_cout << "done!" << vcl_endl;
  
  vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  vcl_cout << "time taken for nms: " << nms_time << " msec" << vcl_endl;
  vcl_cout << "#edgels = " << edge_map->num_edgels();

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_generic_color_edge_detector_process::finish()
{
  return true;
}

