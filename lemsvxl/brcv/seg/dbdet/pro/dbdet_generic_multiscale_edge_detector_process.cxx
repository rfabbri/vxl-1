// This is brcv/seg/dbdet/pro/dbdet_generic_multiscale_edge_detector_process.cxx

//:
// \file

#include "dbdet_generic_multiscale_edge_detector_process.h"

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
#include <vil/algo/vil_convolve_2d.h>
#include <vil/algo/vil_gauss_filter.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_zc_det.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_utils.h>

//: Constructor
dbdet_generic_multiscale_edge_detector_process::dbdet_generic_multiscale_edge_detector_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Gaussian");       //0
  gradient_operator_choices.push_back("h0-operator");    //1
  gradient_operator_choices.push_back("h1-operator");    //2

  if( !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 0) ||
      !parameters()->add( "Minimum Sigma", "-min_sigma" , 1.0 ) ||
      !parameters()->add( "Maximum Sigma", "-max_sigma" , 3.0 ) ||
      //!parameters()->add( "Sigma Steps" ,  "-dsigma" , 0.4 ) ||
      !parameters()->add( "Sigma factor" ,  "-sigma_fac" , 1.4 ) ||
      !parameters()->add( "Noise Sigma" ,  "-nsigma" , 1.5 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 3.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Output min. reliable scale map"  , "-output_min_rel_scale" , false )||
      !parameters()->add( "Output reliable Iuu map"  , "-output_rel_Iuu" , false )||
      !parameters()->add( "Output reliable Iu map"  , "-output_rel_Iu" , false )) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_generic_multiscale_edge_detector_process::~dbdet_generic_multiscale_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_generic_multiscale_edge_detector_process::clone() const
{
  return new dbdet_generic_multiscale_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_generic_multiscale_edge_detector_process::name()
{
  return "Generic Multiscale Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_generic_multiscale_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_generic_multiscale_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_generic_multiscale_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_generic_multiscale_edge_detector_process::get_output_type()
{
  bool output_min_rel_scale, output_rel_Iuu, output_rel_Iu;
  parameters()->get_value( "-output_min_rel_scale", output_min_rel_scale);
  parameters()->get_value( "-output_rel_Iuu", output_rel_Iuu);
  parameters()->get_value( "-output_rel_Iu", output_rel_Iu);
  
  vcl_vector<vcl_string > to_return;
  if (output_min_rel_scale)
    to_return.push_back( "image" );
  if (output_rel_Iuu)
    to_return.push_back( "image" );
  if (output_rel_Iu)
    to_return.push_back( "image" );

  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_generic_multiscale_edge_detector_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_generic_multiscale_edge_detector_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Generic multiscale edge detection...";
  vcl_cout.flush();

  // get image from the storage class
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

  //get the parameters
  unsigned grad_op;
  double min_sigma, max_sigma, /* dsigma, */ sigma_factor, noise_sigma, thresh;
  int N=0;
  bool output_min_rel_scale, output_rel_Iuu, output_rel_Iu;

  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-min_sigma", min_sigma);
  parameters()->get_value( "-max_sigma", max_sigma);
  //parameters()->get_value( "-dsigma", dsigma);
  parameters()->get_value( "-sigma_fac", sigma_factor);
  parameters()->get_value( "-nsigma", noise_sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-int_factor", N);
  parameters()->get_value( "-output_min_rel_scale", output_min_rel_scale);
  parameters()->get_value( "-output_rel_Iuu", output_rel_Iuu);
  parameters()->get_value( "-output_rel_Iu", output_rel_Iu);

  //unsigned Nscales = (unsigned) vcl_floor((max_sigma-min_sigma)/dsigma + 1.0);
  unsigned Nscales = (unsigned) vcl_ceil(vcl_log(max_sigma/min_sigma)/vcl_log(sigma_factor));

  //1) compute image gradients at all scales
  vcl_vector<vil_image_view<double> > I, Ix, Iy, Iu, Iuu;
  I.resize(Nscales); //smoothed images
  Ix.resize(Nscales);
  Iy.resize(Nscales);
  Iu.resize(Nscales);
  Iuu.resize(Nscales);
  
  double scale = vcl_pow(2.0, N);

  double sigma = min_sigma; //current scale
  for (unsigned sc=0; sc<Nscales; sc++)
  {
    //also compute higher derivatives but these don't need to be stored for all scales
    vil_image_view<double> Ixx, Ixy, Iyy;

    //sigma += dsigma; //current scale
    //if (sc>0) sigma *= sigma_factor; //current scale

    switch (grad_op)
    {
      case 0: //Gaussian
      { 
        if (sc==0){
          dbdet_subpix_convolve_2d(greyscale_view, I[sc],  dbdet_G_kernel(sigma),   double(), 0);//keep it the same size as the original
          dbdet_subpix_convolve_2d(greyscale_view, Ix[sc], dbdet_Gx_kernel(sigma),  double(), N);
          dbdet_subpix_convolve_2d(greyscale_view, Iy[sc], dbdet_Gy_kernel(sigma),  double(), N);
          dbdet_subpix_convolve_2d(greyscale_view, Ixx,    dbdet_Gxx_kernel(sigma), double(), N);
          dbdet_subpix_convolve_2d(greyscale_view, Ixy,    dbdet_Gxy_kernel(sigma), double(), N);
          dbdet_subpix_convolve_2d(greyscale_view, Iyy,    dbdet_Gyy_kernel(sigma), double(), N);
        }
        else {
          dbdet_subpix_convolve_2d(I[sc-1], I[sc],  dbdet_G_kernel(sigma_factor),   double(), 0);//keep it the same size as the original
          dbdet_subpix_convolve_2d(I[sc-1], Ix[sc], dbdet_Gx_kernel(sigma),  double(), N);
          dbdet_subpix_convolve_2d(I[sc-1], Iy[sc], dbdet_Gy_kernel(sigma),  double(), N);
          dbdet_subpix_convolve_2d(I[sc-1], Ixx,    dbdet_Gxx_kernel(sigma), double(), N);
          dbdet_subpix_convolve_2d(I[sc-1], Ixy,    dbdet_Gxy_kernel(sigma), double(), N);
          dbdet_subpix_convolve_2d(I[sc-1], Iyy,    dbdet_Gyy_kernel(sigma), double(), N);
        }
        break;
      }
      case 1: //h0-operator
      {
        dbdet_subpix_convolve_2d(greyscale_view, Ix[sc], dbdet_h0_Gx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iy[sc], dbdet_h0_Gy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixx,    dbdet_h0_Gxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixy,    dbdet_h0_Gxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyy,    dbdet_h0_Gyy_kernel(sigma), double(), N);
        break;
      }
      case 2:  //h1-operator
      {
        dbdet_subpix_convolve_2d(greyscale_view, Ix[sc], dbdet_h1_Gx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iy[sc], dbdet_h1_Gy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixx,    dbdet_h1_Gxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixy,    dbdet_h1_Gxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyy,    dbdet_h1_Gyy_kernel(sigma), double(), N);
        break;
      }
    }

    //compute gradient magnitude
    Iu[sc].set_size(Ixx.ni(), Ixx.nj());
    Iuu[sc].set_size(Ixx.ni(), Ixx.nj());

    //get the pointers to the memory chunks
    double *ix   =  Ix[sc].top_left_ptr();
    double *iy   =  Iy[sc].top_left_ptr();
    double *ixx  =  Ixx.top_left_ptr();
    double *ixy  =  Ixy.top_left_ptr();
    double *iyy  =  Iyy.top_left_ptr();

    double *iu   =  Iu[sc].top_left_ptr();
    double *iuu  =  Iuu[sc].top_left_ptr();

    for(unsigned long i=0; i<Ixx.size(); i++)
    {
      double sq_grad = ix[i]*ix[i]+iy[i]*iy[i];

      iu[i] = vcl_sqrt(sq_grad);
      if (sq_grad>1e-5) iuu[i] = (ix[i]*ix[i]*ixx[i]+2*ix[i]*iy[i]*ixy[i]+iy[i]*iy[i]*iyy[i])/sq_grad;
      else              iuu[i] = 10.0;
    }
  }

  double conv_time = t.real();  
  t.mark(); //reset timer

  //2) Now determine the minimum reliable scale map
  vil_image_view<unsigned> min_rel_scale;
  min_rel_scale.set_size(Ix[0].ni(), Ix[0].nj());
  min_rel_scale.fill(100); //dummy value

  unsigned *mrs   =  min_rel_scale.top_left_ptr();

  //determine the critical value function
  vcl_vector <double> c2;

  sigma = min_sigma;
  for (unsigned sc=0; sc<Nscales; sc++){
    //sigma = min_sigma + sc*dsigma; //current scale
    c2.push_back(1.3*noise_sigma/(sigma*sigma*sigma));
    sigma*= sigma_factor; //update sigma
  }

  //at each pixel find the first scale whose value is greater than the critical function
  for(unsigned long i=0; i<min_rel_scale.size(); i++)
  {
    for (unsigned sc=0; sc<Nscales; sc++){
      if (vcl_fabs(Iuu[sc].top_left_ptr()[i])>c2[sc]){
        mrs[i] = sc;
        break;
      }
    }

    //if no scale in use is reliable, just use the largest scale available
    if (mrs[i]==100)
      mrs[i] = Nscales-1;
  }

  //3) smooth the minimum reliable scale map
  vil_image_view<double> sm_min_rel_scale;

  //Gaussian filter the min. rel. scale map
  dbdet_subpix_convolve_2d(min_rel_scale, sm_min_rel_scale, dbdet_G_kernel(0.7),  double(), 0);

  //4) compute the reliable Ix, Iy and Iuu map from the minimum reliable scale map
  vil_image_view<double> rel_Ix, rel_Iy, rel_Iu, rel_Iuu;

  rel_Ix.set_size (sm_min_rel_scale.ni(), sm_min_rel_scale.nj());
  rel_Iy.set_size (sm_min_rel_scale.ni(), sm_min_rel_scale.nj());
  rel_Iu.set_size (sm_min_rel_scale.ni(), sm_min_rel_scale.nj());
  rel_Iuu.set_size(sm_min_rel_scale.ni(), sm_min_rel_scale.nj());

  double *rix  =  rel_Ix.top_left_ptr();
  double *riy  =  rel_Iy.top_left_ptr();
  double *riu  =  rel_Iu.top_left_ptr();
  double *riuu =  rel_Iuu.top_left_ptr();
  double *sm_mrs   =  sm_min_rel_scale.top_left_ptr();

  for(unsigned long i=0; i<sm_min_rel_scale.size(); i++)
  {
    unsigned sc = (unsigned) vcl_floor(sm_mrs[i] + 0.5); //round to the nearest scale
    rix[i]  = Ix[sc].top_left_ptr()[i];
    riy[i]  = Iy[sc].top_left_ptr()[i];
    riu[i]  = Iu[sc].top_left_ptr()[i];
    riuu[i] = Iuu[sc].top_left_ptr()[i];
  }

  double min_rel_time = t.real();  
  t.mark(); //reset timer

  //5) compute the zero crossings of the reliable fuu map

  //Now call the zero crossing code to get the subpixel curve tokens
  vcl_vector<vgl_point_2d<double> > loc;
  vcl_vector<double> orientation, mag;

  dbdet_zc_det ZC(dbdet_zc_det_params(thresh, 1, 1), rel_Ix, rel_Iy, rel_Iuu, rel_Iu); //maximas only (negative third derivative)
  ZC.apply(true, loc, orientation, mag);

  double zc_time = t.real();

   vcl_cout << "done!" << vcl_endl;
  
  vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  vcl_cout << "time taken for min. rel scale determination: " << min_rel_time << " msec" << vcl_endl;
  vcl_cout << "time taken for ZC det: " << zc_time << " msec" << vcl_endl;
  vcl_cout << "#edgels = " << loc.size(); 

  //6) create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr edge_map;
  
  if(output_min_rel_scale || output_rel_Iuu || output_rel_Iu)
  {
    edge_map = new dbdet_edgemap(rel_Iuu.ni(), rel_Iuu.nj());
    for (unsigned i=0; i<loc.size(); i++){
      vgl_point_2d<double> pt(loc[i].x(), loc[i].y());
      edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
    }
  }
  else {
    edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

    for (unsigned i=0; i<loc.size(); i++){
      vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);
      edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
    }
  }

  if(output_min_rel_scale)
  {
    //create output storage class for the min. reliable scale
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(sm_min_rel_scale));
    output_data_[0].push_back(output_storage);
  }

  if(output_rel_Iuu)
  {
    //create output storage class for the reliable Iuu map
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(rel_Iuu));
    output_data_[0].push_back(output_storage);
  }

  if(output_rel_Iu)
  {
    //create output storage class for the reliable Iuu map
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(rel_Iu));
    output_data_[0].push_back(output_storage);
  }

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_generic_multiscale_edge_detector_process::finish()
{
  return true;
}

