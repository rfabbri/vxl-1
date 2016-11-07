// This is brl/vidpro1/process/dbdet_image_gradient_process.cxx

//:
// \file

#include "dbdet_image_gradient_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_orientations.h>
#include <vil/algo/vil_convolve_2d.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>

//: Constructor
dbdet_image_gradient_process::dbdet_image_gradient_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Sobel (1x3)");    //0
  gradient_operator_choices.push_back("Sobel (3x3)");    //1
  gradient_operator_choices.push_back("Gaussian");       //2
  gradient_operator_choices.push_back("h0");             //3
  gradient_operator_choices.push_back("h1");             //4

  vcl_vector<vcl_string> convolution_choices;
  convolution_choices.push_back("2-D");            //0
  convolution_choices.push_back("1-D");            //1

  vcl_vector<vcl_string> output_choices;
  output_choices.push_back("|grad I|");            //0
  output_choices.push_back("Ix only");             //1
  output_choices.push_back("Iy only");             //2
  output_choices.push_back("Ixx only");            //3
  output_choices.push_back("Ixy only");            //4
  output_choices.push_back("Iyy only");            //5
  output_choices.push_back("Ixxx only");           //6
  output_choices.push_back("Ixxy only");           //7
  output_choices.push_back("Ixyy only");           //8
  output_choices.push_back("Iyyy only");           //9
  output_choices.push_back("Ix and Iy");           //10
  output_choices.push_back("Ix, Iy and |grad I|"); //11
  output_choices.push_back("H measure");           //12

  if( !parameters()->add( "Gradient Operator"             , "-grad_op" , gradient_operator_choices, 2) ||
      !parameters()->add( "Convolution Algo:"             , "-conv_algo" , convolution_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"              , "-gbsigma" , 1.0 ) ||
      !parameters()->add( "Interpolation factor=2^N, N="  , "-int_factor" , 0 ) ||
      !parameters()->add( "Output these:"                 , "-output_type" , output_choices, 0)) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_image_gradient_process::~dbdet_image_gradient_process()
{
}


//: Clone the process
bpro1_process* 
dbdet_image_gradient_process::clone() const
{
  return new dbdet_image_gradient_process(*this);
}


//: Return the name of the process
vcl_string
dbdet_image_gradient_process::name()
{
  return "Compute Gradients";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbdet_image_gradient_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbdet_image_gradient_process::get_output_type()
{
  unsigned out_type=0;
  parameters()->get_value( "-output_type" , out_type );

  vcl_vector< vcl_string > to_return;
  to_return.push_back("image");
  if (out_type>=10)
    to_return.push_back("image");//another one
  if (out_type>=11)
    to_return.push_back("image");//yet another one
  return to_return;
}


//: Returns the number of input frames to this process
int
dbdet_image_gradient_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbdet_image_gradient_process::output_frames()
{
  unsigned out_type=0;
  parameters()->get_value( "-output_type" , out_type );

  if (out_type==3)
    return 2;
  if (out_type==4)
    return 3;

  return 1; //default
}


//: Run the process on the current frame
bool
dbdet_image_gradient_process::execute()
{
  if ( input_data_.size() != 1 )
  {
      vcl_cout << "In dbdet_image_gradient_process::execute() - not exactly one"
               << " input image \n";
      return false;
  }
  clear_output();

  //get image
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

  ////////////////////////////////////////////
  //get type of gradient operator 
  unsigned grad_op=0, conv_algo=0;
  parameters()->get_value( "-grad_op" , grad_op );
  parameters()->get_value( "-conv_algo", conv_algo);
  double sigma;
  parameters()->get_value( "-gbsigma" , sigma );

  int N=0;
  parameters()->get_value( "-int_factor" , N );

  //declare arrays to hold the gradients
  vil_image_view<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxy, Ixyy, Ixxx, Iyyy, grad_mag;

  switch(grad_op)
  {
    case 0: // Sobel (1x3)
    {
      vil_sobel_1x3 <vxl_byte, double> (greyscale_view, Ix, Iy);

      //compute gradient magnitude
      grad_mag.set_size(Ix.ni(), Iy.nj());

      //get the pointers to the memory chunks
      double *gx  =  Ix.top_left_ptr();
      double *gy  =  Iy.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 1: // Sobel (3x3)
    {  
      vil_sobel_3x3 <vxl_byte, double> (greyscale_view, Ix, Iy);

      //compute gradient magnitude
      grad_mag.set_size(Ix.ni(), Iy.nj());

      //get the pointers to the memory chunks
      double *gx  =  Ix.top_left_ptr();
      double *gy  =  Iy.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 2: // Gaussian
    {
      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(Ix.ni(), Iy.nj());

      //get the pointers to the memory chunks
      double *gx  =  Ix.top_left_ptr();
      double *gy  =  Iy.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 3: // h0
    {
      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, Ix,   dbdet_h0_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iy,   dbdet_h0_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixx,  dbdet_h0_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixy,  dbdet_h0_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyy,  dbdet_h0_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxx, dbdet_h0_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxy, dbdet_h0_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixyy, dbdet_h0_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyyy, dbdet_h0_Gyyy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ix,   dbdet_h0_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iy,   dbdet_h0_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixx,  dbdet_h0_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixy,  dbdet_h0_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyy,  dbdet_h0_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxx, dbdet_h0_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxy, dbdet_h0_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixyy, dbdet_h0_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyyy, dbdet_h0_Gyyy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(Ix.ni(), Iy.nj());

      //get the pointers to the memory chunks
      double *gx  =  Ix.top_left_ptr();
      double *gy  =  Iy.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 4: // h1
    {
      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(greyscale_view, Ix,   dbdet_h1_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iy,   dbdet_h1_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixx,  dbdet_h1_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixy,  dbdet_h1_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyy,  dbdet_h1_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxx, dbdet_h1_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixxy, dbdet_h1_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Ixyy, dbdet_h1_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, Iyyy, dbdet_h1_Gyyy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ix,   dbdet_h1_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iy,   dbdet_h1_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixx,  dbdet_h1_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixy,  dbdet_h1_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyy,  dbdet_h1_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxx, dbdet_h1_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixxy, dbdet_h1_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Ixyy, dbdet_h1_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(greyscale_view, Iyyy, dbdet_h1_Gyyy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(Ix.ni(), Iy.nj());

      //get the pointers to the memory chunks
      double *gx  =  Ix.top_left_ptr();
      double *gy  =  Iy.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
  }

  unsigned out_type=0;
  parameters()->get_value( "-output_type" , out_type );

  //---------------------------------------------------------------------------------------
  //compute the H measure

  vil_image_view<double> H;
  H.set_size(Ix.ni(), Iy.nj());
  H.fill(0.0);

  if (out_type ==12){
    //int n=2;
    double norm_fac = 4.0*sigma;
    //    double mean_fac = ((2*n+1)*(2*n+1)*(2*n+1)*(2*n+1)); //for computing the mean of the measures

    //// compute the H measure at every point inside the margins
    //for (unsigned x = n; x < H.ni()-n; x++){
    //  for (unsigned y = n; y < H.nj()-n; y++)
    //  {
    //    double Ix2 = 0.0;
    //    double Iy2 = 0.0;
    //    double IxIy = 0.0;

    //    //compute the matrix W from the average around the window size
    //    for (int wx = -n; wx <= n; wx++){
    //      for (int wy = -n; wy <= n; wy++)
    //      {
    //        double ix = Ix(x+wx,y+wy); 
    //        double iy = Iy(x+wx,y+wy);

    //        Ix2 += (ix*ix);
    //        Iy2 += (iy*iy);
    //        IxIy += (ix*iy);
    //      }
    //    }

    //    //the H measure = det(W)
    //    double h = vnl_math::max(0.0, (Ix2*Iy2 - IxIy*IxIy)/((Ix2+Iy2)*(Ix2+Iy2))); //remove negative values
    //    H(x,y) = norm_fac*h;

    //    //H(x,y) = norm_fac*vcl_pow(h, 0.25); //normalize
    //  }
    //}

    //second method for computing the H measure
    vil_image_view<double> Ix2, Iy2, IxIy, Ix22, Iy22, IxIy2;

    Ix2.set_size(Ix.ni(), Iy.nj());
    Iy2.set_size(Ix.ni(), Iy.nj());
    IxIy.set_size(Ix.ni(), Iy.nj());
    Ix22.set_size(Ix.ni(), Iy.nj());
    Iy22.set_size(Ix.ni(), Iy.nj());
    IxIy2.set_size(Ix.ni(), Iy.nj());

    double *gx  =  Ix.top_left_ptr();
    double *gy  =  Iy.top_left_ptr();
    double *ix2  =  Ix2.top_left_ptr();
    double *iy2  =  Iy2.top_left_ptr();
    double *ixiy  =  IxIy.top_left_ptr();
    double *ix22  =  Ix22.top_left_ptr();
    double *iy22  =  Iy22.top_left_ptr();
    double *ixiy2  =  IxIy2.top_left_ptr();
    double *h  =  H.top_left_ptr();

    for(unsigned long i=0; i<Ix.size(); i++){
      ix2[i] = gx[i]*gx[i];
      iy2[i] = gy[i]*gy[i];
      ixiy[i] = gx[i]*gy[i];
    }

    //now smooth them with a gaussian
    dbdet_subpix_convolve_2d(Ix2, Ix22,   dbdet_G_kernel(sigma),   double(), 0);
    dbdet_subpix_convolve_2d(Iy2, Iy22,   dbdet_G_kernel(sigma),   double(), 0);
    dbdet_subpix_convolve_2d(IxIy, IxIy2,   dbdet_G_kernel(sigma),   double(), 0);

    //then compute the h measure from it
    //for(unsigned long i=0; i<Ix.size(); i++)
    //  h[i] = norm_fac*vnl_math::max(0.0, (ix22[i]*iy22[i] - ixiy2[i]*ixiy2[i])/((ix22[i]+iy22[i])*(ix22[i]+iy22[i])));

    norm_fac = sqrt(sigma)/25.0;
    for(unsigned long i=0; i<Ix.size(); i++)
      h[i] = norm_fac*vcl_pow(vnl_math::max(0.0, (ix22[i]*iy22[i] - ixiy2[i]*ixiy2[i])), 0.25);// /((ix22[i]+iy22[i])*(ix22[i]+iy22[i])));

  }

  ///////////////////////////////////////////////////////////////////
  

  if (out_type==0 || out_type==11)
  {
    // create the output storage class for |grad I|
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(grad_mag));
    output_data_[0].push_back(output_storage);
  }

  if (out_type==1 || out_type==10 || out_type==11 || out_type==12)
  {
    // create the output storage class for Ix
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ix));
    output_data_[0].push_back(output_storage);
  }

  if (out_type==2 || out_type==10 || out_type==11 || out_type==12)
  {
    // create the output storage class for Iy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Iy));
    output_data_[0].push_back(output_storage);
  }

  if (out_type==3)
  {
    // create the output storage class for Ixx
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ixx));
    output_data_[0].push_back(output_storage);
  }

  if (out_type==4)
  {
    // create the output storage class for Ixy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ixy));
    output_data_[0].push_back(output_storage);
  }
  if (out_type==5)
  {
    // create the output storage class for Iyy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Iyy));
    output_data_[0].push_back(output_storage);
  }
  if (out_type==6)
  {
    // create the output storage class for Ixxx
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ixxx));
    output_data_[0].push_back(output_storage);
  }
  if (out_type==7)
  {
    // create the output storage class for Ixxy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ixxy));
    output_data_[0].push_back(output_storage);
  }
  if (out_type==8)
  {
    // create the output storage class for Ixyy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Ixyy));
    output_data_[0].push_back(output_storage);
  }
  if (out_type==9)
  {
    // create the output storage class for Iyyy
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(Iyyy));
    output_data_[0].push_back(output_storage);
  }
  
  if (out_type==12)
  {
    // create the output storage class for H
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(H));
    output_data_[0].push_back(output_storage);
  }
  
  return true;
}


//: Finish
bool
dbdet_image_gradient_process::finish()
{
  return true;
}


