// This is brcv/seg/dbdet/pro/dbdet_generic_multicue_edge_detector_process.cxx

//:
// \file

#include "dbdet_generic_multicue_edge_detector_process.h"

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


#include <dbdet/algo/dbdet_compass_edge_detector.h>

//: Constructor
dbdet_generic_multicue_edge_detector_process::dbdet_generic_multicue_edge_detector_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Gaussian");       //0
  gradient_operator_choices.push_back("h0-operator");    //1
  gradient_operator_choices.push_back("h1-operator");    //2

  vcl_vector<vcl_string> cue_choices;
  cue_choices.push_back("Use multiple scales");          //0
  cue_choices.push_back("Use Intensity and Histogram");  //1

  vcl_vector<vcl_string> combinator_choices;
  combinator_choices.push_back("Use feature vector");    //0
  combinator_choices.push_back("Use weighted sum");       //1
  combinator_choices.push_back("Use weighted products");  //2

  vcl_vector<vcl_string> parabola_fit_type;
  parabola_fit_type.push_back("3-point fit");      //0
  parabola_fit_type.push_back("9-point fit");      //1

  if( !parameters()->add( "Cues to use"   , "-cue_choice" , cue_choices, 0) ||
      !parameters()->add( "Cue Combinator"   , "-cue_comb_op" , combinator_choices, 1) ||
      !parameters()->add( "  w1 "    , "-w1"   , 1.0 ) ||
      !parameters()->add( "  w2 "    , "-w2"   , 1.0 ) ||
      !parameters()->add( "  w3 "    , "-w3"   , 1.0 ) ||
      //!parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Threshold"   , "-thresh" , 2.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 0 ) ||
      !parameters()->add( "Parabola Fit type"   , "-parabola_fit" , parabola_fit_type, 0) ||
      !parameters()->add( "Output Combined Response Surface"   , "-boutput_res" , false))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_generic_multicue_edge_detector_process::~dbdet_generic_multicue_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_generic_multicue_edge_detector_process::clone() const
{
  return new dbdet_generic_multicue_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_generic_multicue_edge_detector_process::name()
{
  return "Generic Multicue Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_generic_multicue_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_generic_multicue_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_generic_multicue_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_generic_multicue_edge_detector_process::get_output_type()
{
  bool boutput_res;
  parameters()->get_value( "-boutput_res", boutput_res );

  vcl_vector<vcl_string > to_return;
  if (boutput_res)
    to_return.push_back("image");
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_generic_multicue_edge_detector_process::execute()
{
  //1) get the parameters
  unsigned grad_op, cue_choice_opt, cue_comb_op, parabola_fit;
  double sigma, thresh, w1, w2, w3;
  int N=0;
  bool boutput_res;
  
  parameters()->get_value( "-cue_choice", cue_choice_opt);
  parameters()->get_value( "-cue_comb_op", cue_comb_op);
  parameters()->get_value( "-w1", w1);
  parameters()->get_value( "-w2", w2);
  parameters()->get_value( "-w3", w3);

  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-parabola_fit", parabola_fit );
  parameters()->get_value( "-boutput_res", boutput_res );

  if (input_data_.size() != 1 ){
    vcl_cout << "In dbdet_generic_multicue_edge_detector_process::execute() - only one color image needed!\n";
    return false;
  }
  clear_output();

  vcl_cout << "Generic Multi-Cue edge detection...";
  vcl_cout.flush();

  //2) get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  
  //convert to grayscale
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
  vil_image_view<vxl_byte> greyscale_view;

  if(image_view.nplanes() == 3){
    vil_convert_planes_to_grey(image_view, greyscale_view );
  }
  else if (image_view.nplanes() == 1){
    greyscale_view = image_view;
  }

  //start the timer
  vul_timer t;

  //3) compute image gradients of each of the cues
  vil_image_view<double> f1_dx, f1_dy, f2_dx, f2_dy, f3_dx, f3_dy;
  int scale = (int) vcl_pow(2.0, N);

  switch(cue_choice_opt)
  {
    case 0: //different scales of intensity gradient
    {
      //compute gradients
      dbdet_subpix_convolve_2d(greyscale_view, f1_dx, dbdet_Gx_kernel(1.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f1_dy, dbdet_Gy_kernel(1.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f2_dx, dbdet_Gx_kernel(2.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f2_dy, dbdet_Gy_kernel(2.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f3_dx, dbdet_Gx_kernel(4.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f3_dy, dbdet_Gy_kernel(4.0 * sigma), double(), N);

      break;
    }
    case 1: //intensity gradient and intensity histogram
    {
      //compute intensity gradient
      dbdet_subpix_convolve_2d(greyscale_view, f1_dx, dbdet_Gx_kernel(2.0 * sigma), double(), N);
      dbdet_subpix_convolve_2d(greyscale_view, f1_dy, dbdet_Gy_kernel(2.0 * sigma), double(), N);

      //compute histogram gradient
      vil_image_view<double> hist_grad, hist_ori;
      dbdet_compute_compass_gradient(greyscale_view, 1, 8/2, 0, 2.0*sigma, 2, false, hist_grad, hist_ori);

      //compute the directional derivative from the gradient magnitude and the orientations 
      f2_dx.set_size(f1_dx.ni(), f1_dx.nj());
      f2_dy.set_size(f1_dy.ni(), f1_dy.nj());

      //get pointers to the data
      double* Ori = hist_ori.top_left_ptr();
      double* Mag = hist_grad.top_left_ptr();

      double* Gx = f2_dx.top_left_ptr();
      double* Gy = f2_dy.top_left_ptr();

      for(unsigned long i=0; i<hist_ori.size(); i++){
        Gx[i] = vcl_cos(Ori[i])*Mag[i];
        Gy[i] = vcl_sin(Ori[i])*Mag[i];
      }

      //leave the third component filled with zeros
      f3_dx.set_size(f1_dx.ni(), f1_dx.nj()); f3_dx.fill(0.0);
      f3_dy.set_size(f1_dx.ni(), f1_dx.nj()); f3_dy.fill(0.0);
      
      break;
    }
  }

  //4) compute the combined response surface
  vil_image_view<double> grad_mag, nu1, nu2; //eigenvalue and eigenvector
  grad_mag.set_size(f1_dx.ni(), f1_dx.nj());
  nu1.set_size(f1_dx.ni(), f1_dx.nj());
  nu2.set_size(f1_dx.ni(), f1_dx.nj());

  //get the pointers to the memory chunks
  double *f1x  =  f1_dx.top_left_ptr();
  double *f1y  =  f1_dy.top_left_ptr();
  double *f2x  =  f2_dx.top_left_ptr();
  double *f2y  =  f2_dy.top_left_ptr();
  double *f3x  =  f3_dx.top_left_ptr();
  double *f3y  =  f3_dy.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();
  double *n1  =  nu1.top_left_ptr();
  double *n2  =  nu2.top_left_ptr();

  if (cue_comb_op==0) //compute the squared norm of the vector-gradient
  {
    //compute the squared norm of gradient
    for(unsigned long i=0; i<grad_mag.size(); i++){
      double A = w1*w1*f1x[i]*f1x[i] + w2*w2*f2x[i]*f2x[i] + w3*w3*f3x[i]*f3x[i];
      double B = w1*w1*f1x[i]*f1y[i] + w2*w2*f2x[i]*f2y[i] + w3*w3*f3x[i]*f3y[i];
      double C = w1*w1*f1y[i]*f1y[i] + w2*w2*f2y[i]*f2y[i] + w3*w3*f3y[i]*f3y[i];
//      double d = sqrt((A-C)*(A-C) + 4*B*B);
      //double c = (A-C)/d;

      double l = (A+C+ vcl_sqrt((A-C)*(A-C) + 4*B*B))/2;

      if (vcl_fabs(B)>1e-2){
        n1[i] = B/vcl_sqrt(B*B+(l-A)*(l-A));
        n2[i] = (l-A)/vcl_sqrt(B*B+(l-A)*(l-A));
      }
      else {
        n1[i] = (l-C)/vcl_sqrt(B*B+(l-C)*(l-C));
        n2[i] = B/vcl_sqrt(B*B+(l-C)*(l-C));
      }

      g_mag[i] = vcl_sqrt(l/3); //take the square root of the squared norm
    }
  }
  else if (cue_comb_op==1)
  {  
    for(unsigned long i=0; i<grad_mag.size(); i++){
      double A = w1*vcl_sqrt(f1x[i]*f1x[i] + f1y[i]*f1y[i]);
      double B = w2*vcl_sqrt(f2x[i]*f2x[i] + f2y[i]*f2y[i]);
      double C = w3*vcl_sqrt(f3x[i]*f3x[i] + f3y[i]*f3y[i]);

      n1[i] = f1x[i];
      n2[i] = f1y[i];
      g_mag[i] = (A + B + C)/(w1+w2+w3);
    }
  }
  else if (cue_comb_op==2){
    for(unsigned long i=0; i<grad_mag.size(); i++){
      double A = w1*vcl_sqrt(f1x[i]*f1x[i] + f1y[i]*f1y[i]);
      double B = w2*vcl_sqrt(f2x[i]*f2x[i] + f2y[i]*f2y[i]);
      double C = w3*vcl_sqrt(f3x[i]*f3x[i] + f3y[i]*f3y[i]);

      n1[i] = f1x[i];
      n2[i] = f1y[i];
      g_mag[i] = (A * B * C)/(w1*w2*w3);
    }
  }

  double conv_time = t.real();  
  t.mark(); //reset timer

  //Now call the nms code to get the subpixel edge tokens
  vcl_vector<vgl_point_2d<double> > loc;
  vcl_vector<double> orientation, mag, d2f;
  vcl_vector<vgl_point_2d<int> > pix_loc;

  dbdet_nms NMS(dbdet_nms_params(thresh, (dbdet_nms_params::PFIT_TYPE)parabola_fit), nu1, nu2, grad_mag);
  NMS.apply(true, loc, orientation, mag, d2f, pix_loc);

  double nms_time = t.real();
  
  //create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

  for (unsigned i=0; i<loc.size(); i++){
    vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);
    edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i], d2f[i]));
  }

  vcl_cout << "done!" << vcl_endl;
  
  vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  vcl_cout << "time taken for nms: " << nms_time << " msec" << vcl_endl;
  vcl_cout << "#edgels = " << edge_map->num_edgels();

  // create the output storage class(es)
  if (boutput_res)
  {
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(grad_mag));
    output_data_[0].push_back(output_storage);
  }

  
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_generic_multicue_edge_detector_process::finish()
{
  return true;
}

