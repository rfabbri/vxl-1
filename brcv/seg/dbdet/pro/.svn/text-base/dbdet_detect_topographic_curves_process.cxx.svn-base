// This is brcv/seg/dbdet/pro/dbdet_detect_topographic_curves_process.cxx

//:
// \file

#include "dbdet_detect_topographic_curves_process.h"

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

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_zc_det.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_utils.h>

//: Constructor
dbdet_detect_topographic_curves_process::dbdet_detect_topographic_curves_process()
{
  vcl_vector<vcl_string> gradient_operator_choices;
  gradient_operator_choices.push_back("Gaussian");       //0
  gradient_operator_choices.push_back("h0-operator");    //1
  gradient_operator_choices.push_back("h1-operator");    //2

  vcl_vector<vcl_string> topographic_curve_choices;
  topographic_curve_choices.push_back("fuu=0");          //0
  topographic_curve_choices.push_back("fuv=0");          //1
  topographic_curve_choices.push_back("fvv=0");          //2
  topographic_curve_choices.push_back("det(H)=0");       //3
  topographic_curve_choices.push_back("guu=0");          //4
  topographic_curve_choices.push_back("guv=0");          //5
  topographic_curve_choices.push_back("gvv=0");          //6
  topographic_curve_choices.push_back("det(H_g)=0");     //7
  
  topographic_curve_choices.push_back("tan(theta) = (fxy*(fxx+fyy)/(fxx^2+fyy^2-fxx*fyy+fxy^2)");     //8
  topographic_curve_choices.push_back("tan(theta) = (fx*fxy+fy*fyy)/(fx*fxx+fy*fxy)");                //9

  topographic_curve_choices.push_back("theta(+)");    //10
  topographic_curve_choices.push_back("theta(-)");    //11

  topographic_curve_choices.push_back("lambda(+)");          //12
  topographic_curve_choices.push_back("lambda(-)");          //13
  topographic_curve_choices.push_back("lamdba 1(larger) ");  //14
  topographic_curve_choices.push_back("lambda 2(smaller)");  //15

  topographic_curve_choices.push_back("e:=(fxx-fyy)^2 + 4*fxy^2=0"); //16
  topographic_curve_choices.push_back("Laplacian del(f)=0");         //17

  topographic_curve_choices.push_back("theta_new(+)");    //18
  topographic_curve_choices.push_back("theta_new(-)");    //19


  vcl_vector<vcl_string> curve_classification_choices;
  curve_classification_choices.push_back("Both (+) and (-)");  //0
  curve_classification_choices.push_back("(+) Only");          //1
  curve_classification_choices.push_back("(-) Only");          //2

  if( !parameters()->add( "Topographic Curve"   , "-topo_curve_op" , topographic_curve_choices, 0) ||
      !parameters()->add( "Classification"   , "-subclass" , curve_classification_choices, 0) ||
      !parameters()->add( "Gradient Operator"   , "-grad_op" , gradient_operator_choices, 0) ||
      !parameters()->add( "Sigma (Gaussian)"    , "-sigma"   , 1.0 ) ||
      !parameters()->add( "Gradient Magnitude Threshold"   , "-thresh" , 5.0 ) ||
      !parameters()->add( "Interpolation factor [2^N], N= "  , "-int_factor" , 1 ) ||
      !parameters()->add( "Show vector field"  , "-disp_vec" , false ) ||
      !parameters()->add( "Output implicit function"  , "-out_func" , false )
    ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_detect_topographic_curves_process::~dbdet_detect_topographic_curves_process()
{
}


//: Clone the process
bpro1_process*
dbdet_detect_topographic_curves_process::clone() const
{
  return new dbdet_detect_topographic_curves_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_detect_topographic_curves_process::name()
{
  return "Compute Topographic Curves";
}


//: Return the number of input frame for this process
int
dbdet_detect_topographic_curves_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_detect_topographic_curves_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_detect_topographic_curves_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_detect_topographic_curves_process::get_output_type()
{
  bool out_func;
  parameters()->get_value( "-out_func", out_func );

  vcl_vector<vcl_string > to_return;
  if (out_func) to_return.push_back( "image" );
  to_return.push_back( "edge_map" );

  return to_return;
}


//: Execute the process
bool
dbdet_detect_topographic_curves_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_detect_topographic_curves_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Computing Topographic curve...";
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
  unsigned topo_curve_op, subclass, grad_op;
  double sigma, thresh;
  int N=0;
  bool out_func, disp_vec;
  parameters()->get_value( "-topo_curve_op", topo_curve_op);
  parameters()->get_value( "-subclass", subclass);
  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-out_func", out_func );
  parameters()->get_value( "-disp_vec", disp_vec );
  
  double scale = vcl_pow(2.0, N);

  //image converted to double
  vil_image_view<double> I;

  //if we want topological curves on the gradient image, we have to compute the gradient image first
  if (topo_curve_op>=4 && topo_curve_op<=7)
  {
    vil_image_view<double> gradx, grady;

    switch (grad_op)
    {
      case 0: //Gaussian
      {  
        dbdet_subpix_convolve_2d(greyscale_view, gradx,   dbdet_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grady,   dbdet_Gy_kernel(sigma),   double(), N);
        break;
      }
      case 1: //h0-operator
      {
        dbdet_subpix_convolve_2d(greyscale_view, gradx,   dbdet_h0_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grady,   dbdet_h0_Gy_kernel(sigma),   double(), N);
        break;
      }
      case 2:  //h1-operator
      {
        dbdet_subpix_convolve_2d(greyscale_view, gradx,   dbdet_h1_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(greyscale_view, grady,   dbdet_h1_Gy_kernel(sigma),   double(), N);
        break;
      }
    }

    I.set_size(gradx.ni(), gradx.nj());

    double *gx    =  gradx.top_left_ptr();
    double *gy    =  grady.top_left_ptr();
    double *ii    =  I.top_left_ptr();

    //compute gradient magnitude
    for(unsigned long i=0; i<gradx.size(); i++)
      ii[i] = sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

    //rest of the convolutions do not need to be interpolated
    N=0;
  }
  else 
  {
    //just convert to a double for compatibility
    I.set_size(greyscale_view.ni(), greyscale_view.nj());
    const vxl_byte *gg  =  greyscale_view.top_left_ptr();
    double *ii    =  I.top_left_ptr();

    //compute gradient magnitude
    for(unsigned long i=0; i<greyscale_view.size(); i++)
      ii[i] = double(gg[i]);
  }

  //compute gradients
  vil_image_view<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxx, Ixxy, Ixyy, Iyyy;

  switch (grad_op)
  {
    case 0: //Gaussian
    {  
      dbdet_subpix_convolve_2d(I, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);
      break;
    }
    case 1: //h0-operator
    {
      dbdet_subpix_convolve_2d(I, Ix,   dbdet_h0_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Iy,   dbdet_h0_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Ixx,  dbdet_h0_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixy,  dbdet_h0_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Iyy,  dbdet_h0_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixxx, dbdet_h0_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixxy, dbdet_h0_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixyy, dbdet_h0_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Iyyy, dbdet_h0_Gyyy_kernel(sigma), double(), N);
      break;
    }
    case 2:  //h1-operator
    {
      dbdet_subpix_convolve_2d(I, Ix,   dbdet_h1_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Iy,   dbdet_h1_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(I, Ixx,  dbdet_h1_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixy,  dbdet_h1_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Iyy,  dbdet_h1_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(I, Ixxx, dbdet_h1_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixxy, dbdet_h1_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Ixyy, dbdet_h1_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(I, Iyyy, dbdet_h1_Gyyy_kernel(sigma), double(), N);
      break;
    }
  }

  //compute implicit functions
  vil_image_view<double> F, Fx, Fy, Mask;
  vil_image_view<bool> degenerate_pts;

  F.set_size(Ix.ni(), Ix.nj());
  Fx.set_size(Ix.ni(), Ix.nj());
  Fy.set_size(Ix.ni(), Ix.nj());
  Mask.set_size(Ix.ni(), Ix.nj());
  degenerate_pts.set_size(Ix.ni(), Ix.nj());
  degenerate_pts.fill(false);

  //get the pointers to the memory chunks
  double *ix    =  Ix.top_left_ptr();
  double *iy    =  Iy.top_left_ptr();
  double *ixx   =  Ixx.top_left_ptr();
  double *ixy   =  Ixy.top_left_ptr();
  double *iyy   =  Iyy.top_left_ptr();
  double *ixxx  =  Ixxx.top_left_ptr();
  double *ixxy  =  Ixxy.top_left_ptr();
  double *ixyy  =  Ixyy.top_left_ptr();
  double *iyyy  =  Iyyy.top_left_ptr();

  double *f   =  F.top_left_ptr();
  double *fx  =  Fx.top_left_ptr();
  double *fy  =  Fy.top_left_ptr();
  double *m   =  Mask.top_left_ptr();
  bool *degen =  degenerate_pts.top_left_ptr();

  switch(topo_curve_op)
  {
    case 0: //fuu=0
    case 4: //guu=0
    {
      //compute F=fuu and Fx=fuuu and Fy = Fuuv
      for(unsigned long i=0; i<F.size(); i++)
      {
        double sq_mag = ix[i]*ix[i]+iy[i]*iy[i];

        //use the gradient mag as the mask
        m[i] = vcl_sqrt(sq_mag);

        //compute F
        if (m[i]>1e-3){
          f[i] = (ix[i]*ix[i]*ixx[i]+2*ix[i]*iy[i]*ixy[i]+iy[i]*iy[i]*iyy[i])/sq_mag;
        }
        else {
          f[i] = 0;
          degen[i] = true;
        }

        ////compute Fx and Fy
        //fx[i] = 2*ix[i]*ixx[i]*ixx[i] + 2*ix[i]*ixy[i]*ixy[i] + 2*ixx[i]*iy[i]*ixy[i] + 
        //        2*ixy[i]*iyy[i]*iy[i] + 2*ix[i]*iy[i]*ixxy[i] + ixyy[i]*iy[i]*iy[i] + ix[i]*ix[i]*ixxx[i];
        //fy[i] = 2*iy[i]*iyy[i]*iyy[i] + 2*iy[i]*ixy[i]*ixy[i] + 2*ixy[i]*ix[i]*ixx[i] + 
        //        2*ix[i]*iyy[i]*ixy[i]  + 2*ix[i]*iy[i]*ixyy[i] + ixxy[i]*ix[i]*ix[i] + iyyy[i]*iy[i]*iy[i];  

        //Expt: use Ix and Iy as the direction in which to find the zero crossings
        fx[i] = ix[i];
        fy[i] = iy[i];
      }

      //alternate way to compute the gradient of the function F (this can reduce the number of convolutions as well
      //vil_sobel_3x3 <vxl_byte, double> (F, Fx, Fy);

      break;
    }
    case 1:  //fuv=0
    case 5:  //guv=0
    {
      //compute F=fuv and Fx=fuvu and Fy = Fuvv
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F
        if (m[i]>1e-3){
          f[i] = ((ix[i]*ix[i]-iy[i]*iy[i])*ixy[i]+ix[i]*iy[i]*(iyy[i]-ixx[i]))/m[i];
        }
        else {
          f[i] = 0;
          degen[i] = true;
        }

        //compute Fx and Fy
        fx[i] =  ix[i]*ixy[i]*ixx[i] -iy[i]*ixx[i]*ixx[i] -ix[i]*iy[i]*ixxx[i] -2*iy[i]*ixy[i]*ixy[i] 
                -iy[i]*iy[i]*ixxy[i] +ix[i]*ix[i]*ixxy[i] +iy[i]*ixx[i]*iyy[i] +ix[i]*ixy[i]*iyy[i] +ix[i]*iy[i]*ixyy[i];
        fy[i] = -ix[i]*iyy[i]*ixx[i] -iy[i]*ixy[i]*ixx[i] -ix[i]*iy[i]*ixxy[i] -iy[i]*ixy[i]*iyy[i] 
                -iy[i]*iy[i]*ixyy[i] +2*ix[i]*ixy[i]*ixy[i] +ix[i]*ix[i]*ixyy[i] +ix[i]*iyy[i]*iyy[i] +ix[i]*iy[i]*iyyy[i];
      }
      break;
    }
    case 2:  //fvv=0
    case 6:  //gvv=0
    {
      //compute F=fvv and Fx=fvvu and Fy = Fvvv
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F
        if (m[i]>1e-3){
          f[i] = (iy[i]*iy[i]*ixx[i]-2*ix[i]*iy[i]*ixy[i]+ix[i]*ix[i]*iyy[i])/m[i];
        }
        else {
          f[i] = 0;
          degen[i] = true;
        }

        //compute Fx and Fy
        fx[i] = iy[i]*iy[i]*ixxx[i]-2*ix[i]*ixy[i]*ixy[i]-2*ix[i]*iy[i]*ixxy[i]+2*ix[i]*ixx[i]*iyy[i]+ix[i]*ix[i]*ixxy[i];
        fy[i] = 2*iy[i]*ixx[i]*iyy[i]+iy[i]*iy[i]*ixxy[i]-2*iy[i]*ixy[i]*ixy[i]-2*ix[i]*iy[i]*ixyy[i]+ix[i]*ix[i]*iyyy[i];
      }
      break;
    }
    case 3:  //det(H)=0 
    case 7:  //det(H_g)=0
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F=det(H)
        f[i] = ixx[i]*iyy[i]-ixy[i]*ixy[i];

        //compute Fx and Fy
        fx[i] = ixxx[i]*iyy[i]+ixx[i]*ixyy[i]-2*ixy[i]*ixxy[i];
        fy[i] = ixxy[i]*iyy[i]+ixx[i]*iyyy[i]-2*ixy[i]*ixyy[i];

      }
      break;
    }
    case 8:  //tan(theta)=(fxy*(fxx+fyy)/(fxx^2+fyy^2-fxx*fyy+fxy^2)
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F=det(H)
        f[i] = ixx[i]*iyy[i]-ixy[i]*ixy[i];

        //compute Fx and Fy from tan(theta)
        double theta;
        double den = (ixx[i]*ixx[i]+iyy[i]*iyy[i]-ixx[i]*iyy[i]+ixy[i]*ixy[i]);
        if (vcl_fabs(den)>1e-3){
          theta = vcl_atan(ixy[i]*(ixx[i]+iyy[i])/(den + 1e-10));
        }
        else {
          theta = 0;
          degen[i] = true;
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
    case 9:  //tan(theta)=(fx*fxy+fy*fyy)/(fx*fxx+fy*fxy)
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F=det(H)
        f[i] = ixx[i]*iyy[i]-ixy[i]*ixy[i];

        //compute Fx and Fy from tan(theta)
        double theta;
        double den = (ix[i]*ixx[i]+iy[i]*ixy[i]);
        if (vcl_fabs(den)>1e-3){
          theta = vcl_atan((ix[i]*ixy[i]+iy[i]*iyy[i])/(den + 1e-10));
        }
        else {
          theta = 0;
          degen[i] = true;
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
    case 10:  //tan(theta) +
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F=det(H)
        f[i] = ixx[i]*iyy[i]-ixy[i]*ixy[i];

        //compute Fx and Fy
        double theta;
        if (vcl_fabs(ixx[i])>1e-5){
          if (f[i]<=0)
            theta = vcl_atan((ixy[i] + vcl_sqrt(-f[i]))/ixx[i]);
          else {
            theta = 0;
            degen[i] = true;
          }
        }
        else {
          if (vcl_fabs(ixy[i])>1e-5)
            theta = vcl_atan(iyy[i]/ixy[i]/2);
          else {
            theta = 0;
            degen[i] = true;
          }
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
    case 11:  //tan(theta) -
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F=det(H)
        f[i] = ixx[i]*iyy[i]-ixy[i]*ixy[i];

        //compute Fx and Fy
        double theta;
        if (vcl_fabs(ixx[i])>1e-5){
          if (f[i]<=0)
            theta = vcl_atan((ixy[i] - vcl_sqrt(-f[i]))/ixx[i]);
          else {
            theta = 0;
            degen[i] = true;
          }
        }
        else {
          if (vcl_fabs(ixy[i])>1e-5)
            theta = vnl_math::pi_over_2; //instead of making theta+ and theta- the same at these points
            //theta = vcl_atan(iyy[i]/ixy[i]/2);
          else {
            theta = 0;
            degen[i] = true;
          }
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
    case 12:  //lambda (+)
    {
      //compute F=lambda (+)
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F=lambda+
        double trace = (ixx[i]+iyy[i])/2;

        f[i] = trace + vcl_sqrt((ixx[i]-trace)*(ixx[i]-trace) + ixy[i]*ixy[i]);

        double Sq = ixx[i]*ixx[i]-2*ixx[i]*iyy[i]+iyy[i]*iyy[i]+4*ixy[i]*ixy[i];
        if (Sq>0){
            fx[i] = ixxx[i]*vcl_sqrt(Sq)+ixyy[i]*vcl_sqrt(Sq)+ixx[i]*ixxx[i]-ixx[i]*ixyy[i]-iyy[i]*ixxx[i]+iyy[i]*ixyy[i]+4*ixy[i]*ixxy[i];
            fy[i] = ixxy[i]*vcl_sqrt(Sq)+iyyy[i]*vcl_sqrt(Sq)+ixx[i]*ixxy[i]-ixx[i]*iyyy[i]-iyy[i]*ixxy[i]+iyy[i]*iyyy[i]+4*ixy[i]*ixyy[i];
        }
        else
          degen[i] = true;
      }

      //  //alternate way to compute the gradient of the function F (this can reduce the number of convolutions as well
      //  vil_sobel_3x3 <double, double> (F, Fx, Fy);

      //for computing the eigenvectors (+)
      //if (disp_vec) { //compute vector field of eigen vectors
        //  //compute Fx and Fy (+eigenvector components)
        //  double theta;
        //  if (vcl_fabs(ixy[i])>1e-5){
        //      theta = vcl_atan(((iyy[i]-ixx[i]) + vcl_sqrt((ixx[i]-iyy[i])*(ixx[i]-iyy[i]) + 4*ixy[i]*ixy[i]))/ixy[i]/2);
        //  }
        //  else { //to add ixx[i]==iyy[i] degeneracy
        //    if (vcl_abs(ixx[i])>vcl_abs(iyy[i]))
        //      theta = 0;
        //    else
        //      theta = vnl_math::pi_over_2;
        //  }
        //  fx[i] = -vcl_sin(theta);
        //  fy[i] = vcl_cos(theta);
        //}
      //}

      break;
    }
    case 13:  //lambda(-)
    {
      //compute F=lambda(-)
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F=lambda-
        double trace = (ixx[i]+iyy[i])/2;

        f[i] = trace - vcl_sqrt((ixx[i]-trace)*(ixx[i]-trace) + ixy[i]*ixy[i]);

        double Sq = ixx[i]*ixx[i]-2*ixx[i]*iyy[i]+iyy[i]*iyy[i]+4*ixy[i]*ixy[i];
        if (Sq>0){
            fx[i] = ixxx[i]*vcl_sqrt(Sq)+ixyy[i]*vcl_sqrt(Sq)-ixx[i]*ixxx[i]+ixx[i]*ixyy[i]+iyy[i]*ixxx[i]-iyy[i]*ixyy[i]-4*ixy[i]*ixxy[i];
            fy[i] = ixxy[i]*vcl_sqrt(Sq)+iyyy[i]*vcl_sqrt(Sq)-ixx[i]*ixxy[i]+ixx[i]*iyyy[i]+iyy[i]*ixxy[i]-iyy[i]*iyyy[i]-4*ixy[i]*ixyy[i];
        }
        else
          degen[i] = true;

      }

      //  //alternate way to compute the gradient of the function F (this can reduce the number of convolutions as well
      //  vil_sobel_3x3 <double, double> (F, Fx, Fy);

      //if (disp_vec) { //compute vector field of eigen vectors
      //    //compute Fx and Fy (+eigenvector components)
      //    double theta;
      //    if (vcl_fabs(ixy[i])>1e-5){
      //        theta = vcl_atan(((iyy[i]-ixx[i]) - vcl_sqrt((ixx[i]-iyy[i])*(ixx[i]-iyy[i]) + 4*ixy[i]*ixy[i]))/ixy[i]/2);
      //    }
      //    else { //to add ixx[i]==iyy[i] degeneracy
      //      if (vcl_abs(ixx[i])>vcl_abs(iyy[i]))
      //        theta = vnl_math::pi_over_2;
      //      else
      //        theta = 0;
      //    }

      //    fx[i] = -vcl_sin(theta);
      //    fy[i] = vcl_cos(theta);
      //  }

      break;
    }
    case 14:  //lambda1(larger)
    {
      //compute F=lambda1(larger)
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F=lambda (larger)
        double trace = (ixx[i]+iyy[i])/2;
        f[i] = trace + vnl_math_sgn(trace)*vcl_sqrt((ixx[i]-trace)*(ixx[i]-trace) + ixy[i]*ixy[i]);

        ////for computing the larger eigenvector
        //if (disp_vec) { //compute vector field of eigen vectors
        //  //compute Fx and Fy (larger eigenvector components)
        //  double theta;
        //  if (vcl_fabs(ixy[i])>1e-5){
        //      theta = vcl_atan(((iyy[i]-ixx[i]) + vnl_math_sgn(trace)*vcl_sqrt((ixx[i]-iyy[i])*(ixx[i]-iyy[i]) + 4*ixy[i]*ixy[i]))/ixy[i]/2);
        //  }
        //  else { //to add ixx[i]==iyy[i] degeneracy
        //    if (vcl_abs(ixx[i])>vcl_abs(iyy[i]))
        //      theta = 0;
        //    else
        //      theta = vnl_math::pi_over_2;
        //  }

        //  fx[i] = -vcl_sin(theta);
        //  fy[i] = vcl_cos(theta);
        //}
      }

      //alternate way to compute the gradient of the function F (this can reduce the number of convolutions as well
      vil_sobel_3x3 <double, double> (F, Fx, Fy);

      break;
    }
    case 15:  //lambda2(smaller)
    {
      //compute F=lambda2(smaller)
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F=lambda smaller
        double trace = (ixx[i]+iyy[i])/2;

        f[i] = trace - vnl_math_sgn(trace)*vcl_sqrt((ixx[i]-trace)*(ixx[i]-trace) + ixy[i]*ixy[i]);

        ////for computing the smaller eigenvector
        //if (disp_vec) { //compute vector field of eigen vectors
        //  //compute Fx and Fy (smaller eigenvector components)
        //  double theta;
        //  if (vcl_fabs(ixy[i])>1e-5){
        //      theta = vcl_atan(((iyy[i]-ixx[i]) - vnl_math_sgn(trace)*vcl_sqrt((ixx[i]-iyy[i])*(ixx[i]-iyy[i]) + 4*ixy[i]*ixy[i]))/ixy[i]/2);
        //  }
        //  else { //to add ixx[i]==iyy[i] degeneracy
        //    if (vcl_abs(ixx[i])>vcl_abs(iyy[i]))
        //      theta = vnl_math::pi_over_2;
        //    else
        //      theta = 0;
        //  }

        //  fx[i] = -vcl_sin(theta);
        //  fy[i] = vcl_cos(theta);
        //}
      }

      //alternate way to compute the gradient of the function F (this can reduce the number of convolutions as well
      vil_sobel_3x3 <double, double> (F, Fx, Fy);

      break;
    }
    case 16:  //e:=(fxx-fyy)^2 + 4*fxy^2=0
    {
      //compute F=(fxx-fyy)^2 + 4*fxy^2=0
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F
        f[i] = (ixx[i]-iyy[i])*(ixx[i]-iyy[i])+4*ixy[i]*ixy[i];

        fx[i] = 2*ixx[i]*ixxx[i]-2*ixx[i]*ixyy[i]-2*iyy[i]*ixxx[i]+2*iyy[i]*ixyy[i]+8*ixy[i]*ixxy[i];
        fy[i] = 2*ixx[i]*ixxy[i]-2*ixx[i]*iyyy[i]-2*iyy[i]*ixxy[i]+2*iyy[i]*iyyy[i]+8*ixy[i]*ixyy[i];
      }

      break;
    }
    case 17:  //del(f)=0
    {
      //compute F=fxx+fyy
      for(unsigned long i=0; i<F.size(); i++)
      {
        //no mask
        m[i] = 100;

        //compute F
        f[i] = ixx[i]+iyy[i];

        fx[i] = ixxx[i] + ixyy[i];
        fy[i] = ixxy[i] + iyyy[i];
      }

      break;
    }
    case 18:  //tan(theta)_new +
    {
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F= 
        f[i] = m[i];

        //compute Fx and Fy
        double den = (ixx[i]*iy[i]*iy[i]-iy[i]*ix[i]*ixx[i]-iy[i]*iy[i]*iyy[i]+ixx[i]*ix[i]*ix[i]);
        double s = -4*ixx[i]*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i] + 4*ixy[i]*ix[i]*ix[i]*ix[i]*iy[i]*iyy[i] - 2*ixy[i]*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i] +
                    6*ixy[i]*iy[i]*iy[i]*iy[i]*ix[i]*iyy[i] + 2*ixx[i]*ix[i]*ix[i]*ix[i]*iy[i]*iyy[i] - 6*ixx[i]*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i] +
                    2*ixx[i]*ix[i]*ix[i]*ix[i]*iy[i]*ixy[i] + 4*ixy[i]*ixy[i]*ix[i]*ix[i]*ix[i]*ix[i]  + 
                    4*ixy[i]*ixy[i]*iy[i]*iy[i]*iy[i]*iy[i] + ixx[i]*ixx[i]*ix[i]*ix[i]*ix[i]*ix[i] + 5*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i]*iyy[i] +
                    9*ixy[i]*ixy[i]*ix[i]*ix[i]*iy[i]*iy[i] -4*ixy[i]*ixy[i]*ix[i]*ix[i]*ix[i]*iy[i] -
                    4*ixy[i]*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i] -4*ixy[i]*ixy[i]*iy[i]*iy[i]*iy[i]*ix[i] + 5*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i]*iyy[i] +
                    2*ix[i]*iy[i]*iy[i]*iy[i]*iyy[i]*iyy[i] -4*ixx[i]*ix[i]*ix[i]*ix[i]*ix[i]*iyy[i];

        double theta;
        if (vcl_fabs(den)>1e-5 && s>0)
          theta = vcl_atan((2*ixy[i]*ix[i]*ix[i]+2*ixy[i]*iy[i]*iy[i]+ixx[i]*ix[i]*ix[i]+ix[i]*iy[i]*iyy[i]-iy[i]*iy[i]*iyy[i]-iy[i]*ix[i]*ixy[i] + vcl_sqrt(s))/2/den);
        else {
            theta = 0;
            degen[i] = true;
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
    case 19:  //tan(theta)_new -
    {
      //compute F=fuu*fvv-fuv^2 and Fx= and Fy = 
      for(unsigned long i=0; i<F.size(); i++)
      {
        //compute gradient mag as the mask
        m[i] = vcl_sqrt(ix[i]*ix[i]+iy[i]*iy[i]);

        //compute F= 
        f[i] = m[i];

        //compute Fx and Fy
        double den = (ixx[i]*iy[i]*iy[i]-iy[i]*ix[i]*ixx[i]-iy[i]*iy[i]*iyy[i]+ixx[i]*ix[i]*ix[i]);
        double s = -4*ixx[i]*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i] + 4*ixy[i]*ix[i]*ix[i]*ix[i]*iy[i]*iyy[i] - 2*ixy[i]*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i] +
                    6*ixy[i]*iy[i]*iy[i]*iy[i]*ix[i]*iyy[i] + 2*ixx[i]*ix[i]*ix[i]*ix[i]*iy[i]*iyy[i] - 6*ixx[i]*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i] +
                    2*ixx[i]*ix[i]*ix[i]*ix[i]*iy[i]*ixy[i] + 4*ixy[i]*ixy[i]*ix[i]*ix[i]*ix[i]*ix[i]  + 
                    4*ixy[i]*ixy[i]*iy[i]*iy[i]*iy[i]*iy[i] + ixx[i]*ixx[i]*ix[i]*ix[i]*ix[i]*ix[i] + 5*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i]*iyy[i] +
                    9*ixy[i]*ixy[i]*ix[i]*ix[i]*iy[i]*iy[i] -4*ixy[i]*ixy[i]*ix[i]*ix[i]*ix[i]*iy[i] -
                    4*ixy[i]*iy[i]*iy[i]*iy[i]*iy[i]*iyy[i] -4*ixy[i]*ixy[i]*iy[i]*iy[i]*iy[i]*ix[i] + 5*ix[i]*ix[i]*iy[i]*iy[i]*iyy[i]*iyy[i] +
                    2*ix[i]*iy[i]*iy[i]*iy[i]*iyy[i]*iyy[i] -4*ixx[i]*ix[i]*ix[i]*ix[i]*ix[i]*iyy[i];

        double theta;
        if (vcl_fabs(den)>1e-5 && s>0)
          theta = vcl_atan((2*ixy[i]*ix[i]*ix[i]+2*ixy[i]*iy[i]*iy[i]+ixx[i]*ix[i]*ix[i]+ix[i]*iy[i]*iyy[i]-iy[i]*iy[i]*iyy[i]-iy[i]*ix[i]*ixy[i] - vcl_sqrt(s))/2/den);
        else {
            theta = 0;
            degen[i] = true;
        }

        fx[i] = -vcl_sin(theta);
        fy[i] = vcl_cos(theta);

      }
      break;
    }
  }
  
//  double conv_time = t.real();  
  t.mark(); //reset timer

  //setup a data structure to hold the edgemap
  dbdet_edgemap_sptr edge_map;

  //if we wish to look at the entire implicit function as a vector field
  if (disp_vec)
  {
    //put all the points on the edgemap
    if (out_func){
      edge_map = new dbdet_edgemap(F.ni(), F.nj());
      for (unsigned x = 0; x < F.ni(); x++){
        for (unsigned y = 0; y < F.nj(); y++){
          if (degenerate_pts(x,y)) continue; //do not output degenerate pts
          double orientation = vcl_atan2(Fx(x,y), -Fy(x,y));
          edge_map->insert(new dbdet_edgel(vgl_point_2d<double>(x, y), orientation, Mask(x,y)));
        }
      }
    }
    else {
      edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());
      for (unsigned x = 2*scale; x < F.ni()-2*scale; x++){
        for (unsigned y = 2*scale; y < F.nj()-2*scale; y++){
          if (degenerate_pts(x,y)) continue; //do not output degenerate pts
          double orientation = vcl_atan2(Fx(x,y), -Fy(x,y));
          edge_map->insert(new dbdet_edgel(vgl_point_2d<double>(x/scale, y/scale), orientation, Mask(x,y)));
        }
      }
    }
  }
  else {

    //Now call the zero crossing code to get the subpixel curve tokens
    vcl_vector<vgl_point_2d<double> > loc;
    vcl_vector<double> orientation, mag;

    dbdet_zc_det ZC(dbdet_zc_det_params(thresh), Fx, Fy, F, Mask);
    ZC.apply(true, loc, orientation, mag);

//    double zc_time = t.real();

    //convert to the original image scale coordinates (for the regular operators)
    if (topo_curve_op<4 || topo_curve_op>10){
      for (unsigned i=0; i<loc.size(); i++)
        loc[i].set(loc[i].x()/scale, loc[i].y()/scale);
    }

    //for each curve locations, compute all the gradients to compute the new orientation
    vcl_vector<double> Ixs, Iys, Ixxs, Ixys, Iyys, Ixxys, Ixyys, Ixxxs, Iyyys;

    switch (grad_op)
    {
      case 0: //Interpolated Gaussian
      { 
        dbdet_subpix_convolve_2d(I, loc, Ixs,   dbdet_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iys,   dbdet_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxs,  dbdet_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixys,  dbdet_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyys,  dbdet_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxxs, dbdet_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxys, dbdet_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixyys, dbdet_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyyys, dbdet_Gyyy_kernel(sigma), double(), N);
        break;
      }
      case 1: //h0-operator
      {
        dbdet_subpix_convolve_2d(I, loc, Ixs,   dbdet_h0_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iys,   dbdet_h0_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxs,  dbdet_h0_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixys,  dbdet_h0_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyys,  dbdet_h0_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxxs, dbdet_h0_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxys, dbdet_h0_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixyys, dbdet_h0_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyyys, dbdet_h0_Gyyy_kernel(sigma), double(), N);
        break;
      }
      case 2:  //h1-operator
      {
        dbdet_subpix_convolve_2d(I, loc, Ixs,   dbdet_h1_Gx_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iys,   dbdet_h1_Gy_kernel(sigma),   double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxs,  dbdet_h1_Gxx_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixys,  dbdet_h1_Gxy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyys,  dbdet_h1_Gyy_kernel(sigma),  double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxxs, dbdet_h1_Gxxx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixxys, dbdet_h1_Gxxy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Ixyys, dbdet_h1_Gxyy_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(I, loc, Iyyys, dbdet_h1_Gyyy_kernel(sigma), double(), N);
        break;
      }
    }
        
    //Now, compute and update each curveloc with its new orientation
    vcl_vector<double> curve_orientations(loc.size());
    vcl_vector<bool> correct_subclass(loc.size(), true);

    switch(topo_curve_op)
    {
      case 0: //fuu=0
      case 4: //guu=0
      {
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          //compute Fx and Fy
          double Fx = 2*Ixs[i]*Ixxs[i]*Ixxs[i] + 2*Ixs[i]*Ixys[i]*Ixys[i] + 2*Ixxs[i]*Iys[i]*Ixys[i] + 
                      2*Ixys[i]*Iyys[i]*Iys[i] + 2*Ixs[i]*Iys[i]*Ixxys[i] + Ixyys[i]*Iys[i]*Iys[i] + Ixs[i]*Ixs[i]*Ixxxs[i];
          double Fy = 2*Iys[i]*Iyys[i]*Iyys[i] + 2*Iys[i]*Ixys[i]*Ixys[i] + 2*Ixys[i]*Ixs[i]*Ixxs[i] + 
                      2*Ixs[i]*Iyys[i]*Ixys[i]  + 2*Ixs[i]*Iys[i]*Ixyys[i] + Ixxys[i]*Ixs[i]*Ixs[i] + Iyyys[i]*Iys[i]*Iys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

          //determine whether the located zero crossings are at a maxima or minima
          if (subclass>0){ //positive extrema or negative extrema only
            double fuuu = 2*Ixs[i]*Ixs[i]*Ixxs[i]*Ixxs[i]+Ixs[i]*Ixs[i]*Ixs[i]*Ixxxs[i]+4*Iys[i]*Ixys[i]*Ixs[i]*Ixxs[i] +
                          2*Ixs[i]*Ixs[i]*Ixys[i]*Ixys[i]+3*Iys[i]*Ixs[i]*Ixs[i]*Ixxys[i]+4*Iys[i]*Ixs[i]*Ixys[i]*Iyys[i] +
                          3*Ixs[i]*Iys[i]*Iys[i]*Ixyys[i]+2*Iys[i]*Iys[i]*Ixys[i]*Ixys[i]+2*Iys[i]*Iys[i]*Iyys[i]*Iyys[i] +
                          Iys[i]*Iys[i]*Iys[i]*Iyyys[i];
            correct_subclass[i] = (subclass==1) ? fuuu<0 : fuuu>0;
          }
        }
        break;
      }
      case 1:  //fuv=0
      case 5:  //guv=0
      {
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          //compute Fx and Fy
          double Fx = Ixs[i]*Ixys[i]*Ixxs[i] -Iys[i]*Ixxs[i]*Ixxs[i] -Ixs[i]*Iys[i]*Ixxxs[i] -2*Iys[i]*Ixys[i]*Ixys[i] 
                      -Iys[i]*Iys[i]*Ixxys[i] +Ixs[i]*Ixs[i]*Ixxys[i] +Iys[i]*Ixxs[i]*Iyys[i] +Ixs[i]*Ixys[i]*Iyys[i] +Ixs[i]*Iys[i]*Ixyys[i];
          double Fy = -Ixs[i]*Iyys[i]*Ixxs[i] -Iys[i]*Ixys[i]*Ixxs[i] -Ixs[i]*Iys[i]*Ixxys[i] -Iys[i]*Ixys[i]*Iyys[i] 
                      -Iys[i]*Iys[i]*Ixyys[i] +2*Ixs[i]*Ixys[i]*Ixys[i] +Ixs[i]*Ixs[i]*Ixyys[i] +Ixs[i]*Iyys[i]*Iyys[i] +Ixs[i]*Iys[i]*Iyyys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

          //determine whether the located zero crossings are at a maxima or minima
          if (subclass>0){ //positive extrema or negative extrema only
            double fuvv = -2*Iys[i]*Ixys[i]*Ixs[i]*Ixxs[i]+Iys[i]*Iys[i]*Ixxs[i]*Ixxs[i]+2*Iys[i]*Iys[i]*Ixys[i]*Ixys[i]
                          +Iys[i]*Iys[i]*Iys[i]*Ixxys[i]-2*Iys[i]*Ixs[i]*Ixs[i]*Ixxys[i]-Ixxs[i]*Iys[i]*Iys[i]*Iyys[i]
                          -2*Iys[i]*Ixs[i]*Ixys[i]*Iyys[i]-2*Ixs[i]*Iys[i]*Iys[i]*Ixyys[i]-Iyys[i]*Ixs[i]*Ixs[i]*Ixxs[i]
                          +2*Ixs[i]*Ixs[i]*Ixys[i]*Ixys[i]+Ixs[i]*Ixs[i]*Ixs[i]*Ixyys[i]+Ixs[i]*Ixs[i]*Iyys[i]*Iyys[i]
                          +Ixs[i]*Ixs[i]*Iys[i]*Iyyys[i];
            correct_subclass[i] = (subclass==1) ? fuvv<0 : fuvv>0;
          }
        }
        break;
      }
      case 2:  //fvv=0
      case 6:  //gvv=0
      {
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          //compute Fx and Fy
          double Fx = Iys[i]*Iys[i]*Ixxxs[i]-2*Ixs[i]*Ixys[i]*Ixys[i]-2*Ixs[i]*Iys[i]*Ixxys[i]+2*Ixs[i]*Ixxs[i]*Iyys[i]+Ixs[i]*Ixs[i]*Ixxys[i];
          double Fy = 2*Iys[i]*Ixxs[i]*Iyys[i]+Iys[i]*Iys[i]*Ixxys[i]-2*Iys[i]*Ixys[i]*Ixys[i]-2*Ixs[i]*Iys[i]*Ixyys[i]+Ixs[i]*Ixs[i]*Iyyys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

          //determine whether the located zero crossings are at a maxima or minima
          if (subclass>0){ //positive extrema or negative extrema only
            double fvvv = -Iys[i]*Iys[i]*Iys[i]*Ixxxs[i]+3*Iys[i]*Iys[i]*Ixs[i]*Ixxys[i]-3*Iys[i]*Ixs[i]*Ixs[i]*Ixyys[i]+Ixs[i]*Ixs[i]*Ixs[i]*Iyyys[i];
            correct_subclass[i] = (subclass==1) ? fvvv<0 : fvvv>0;
          }
        }
        break;
      }
      case 3:  //det(H)=0 
      case 7:  //det(H_g)=0
      {
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          //compute Fx and Fy
          double Fx = Ixxxs[i]*Iyys[i]+Ixxs[i]*Ixyys[i]-2*Ixys[i]*Ixxys[i];
          double Fy = Ixxys[i]*Iyys[i]+Ixxs[i]*Iyyys[i]-2*Ixys[i]*Ixyys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

          //determine whether the located zero crossings are at a maxima or minima
          if (subclass>0){ //positive extrema or negative extrema only
            double theta = vcl_atan(Ixys[i]*(Ixxs[i]+Iyys[i])/(Ixxs[i]*Ixxs[i]+Iyys[i]*Iyys[i]-Ixxs[i]*Iyys[i]+Ixys[i]*Ixys[i]));
            double feee = sin(theta)*sin(theta)*sin(theta)*Ixxxs[i]-3*cos(theta)*sin(theta)*sin(theta)*Ixxys[i]+3*sin(theta)*Ixyys[i]*cos(theta)*cos(theta)-Iyyys[i]*cos(theta)*cos(theta)*cos(theta);
            double fses = -2*cos(theta)*sin(theta)*sin(theta)*Ixxys[i]+3*sin(theta)*Ixyys[i]*cos(theta)*cos(theta)-sin(theta)*Ixxxs[i]*cos(theta)*cos(theta)-sin(theta)*Ixyys[i]+Iyyys[i]*cos(theta)-Iyyys[i]*cos(theta)*cos(theta)*cos(theta);
         
            correct_subclass[i] = (subclass==1) ? (feee<0)&&(fses<0) : (feee>0)&&(fses>0);
          }
        }
        break;
      }
      case 8:
      case 9:
      case 10:
      case 11:
        break;
      case 12: //lambda (+)
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Sq = Ixxs[i]*Ixxs[i]-2*Ixxs[i]*Iyys[i]+Iyys[i]*Iyys[i]+4*Ixys[i]*Ixys[i];
          if (Sq>0){
              double Fx = Ixxxs[i]*vcl_sqrt(Sq)+Ixyys[i]*vcl_sqrt(Sq)+Ixxs[i]*Ixxxs[i]-Ixxs[i]*Ixyys[i]-Iyys[i]*Ixxxs[i]+Iyys[i]*Ixyys[i]+4*Ixys[i]*Ixxys[i];
              double Fy = Ixxys[i]*vcl_sqrt(Sq)+Iyyys[i]*vcl_sqrt(Sq)+Ixxs[i]*Ixxys[i]-Ixxs[i]*Iyyys[i]-Iyys[i]*Ixxys[i]+Iyys[i]*Iyyys[i]+4*Ixys[i]*Ixyys[i];
              //save new orientation
              curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

              //if del(H) is positive, lambda+ is the larger eigenvalue
              correct_subclass[i] = (subclass==0) || ((subclass==1) && (Ixxs[i]+Iyys[i])>0);
          }
          else { 
            correct_subclass[i] = false;
          } 
        }
        break;
      case 13: //lambda (-)
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Sq = Ixxs[i]*Ixxs[i]-2*Ixxs[i]*Iyys[i]+Iyys[i]*Iyys[i]+4*Ixys[i]*Ixys[i];
          if (Sq>0){
              double Fx = Ixxxs[i]*vcl_sqrt(Sq)+Ixyys[i]*vcl_sqrt(Sq)-Ixxs[i]*Ixxxs[i]+Ixxs[i]*Ixyys[i]+Iyys[i]*Ixxxs[i]-Iyys[i]*Ixyys[i]-4*Ixys[i]*Ixxys[i];
              double Fy = Ixxys[i]*vcl_sqrt(Sq)+Iyyys[i]*vcl_sqrt(Sq)-Ixxs[i]*Ixxys[i]+Ixxs[i]*Iyyys[i]+Iyys[i]*Ixxys[i]-Iyys[i]*Iyyys[i]-4*Ixys[i]*Ixyys[i];
              //save new orientation
              curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));

              //if del(H) is negative, lambda- is the larger eigenvalue
              correct_subclass[i] = (subclass==0) || ((subclass==1) && (Ixxs[i]+Iyys[i])<0);
          }
          else { 
            correct_subclass[i] = false;
          } 
        }
        break;
      case 14: //lambda1 (larger)
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Sq = Ixxs[i]*Ixxs[i]-2*Ixxs[i]*Iyys[i]+Iyys[i]*Iyys[i]+4*Ixys[i]*Ixys[i];
          double s = vnl_math_sgn(Ixxs[i]+Iyys[i]);
          if (Sq>0){
              double Fx = Ixxxs[i]*vcl_sqrt(Sq)+Ixyys[i]*vcl_sqrt(Sq)+ s*(Ixxs[i]*Ixxxs[i]-Ixxs[i]*Ixyys[i]-Iyys[i]*Ixxxs[i]+Iyys[i]*Ixyys[i]+4*Ixys[i]*Ixxys[i]);
              double Fy = Ixxys[i]*vcl_sqrt(Sq)+Iyyys[i]*vcl_sqrt(Sq)+ s*(Ixxs[i]*Ixxys[i]-Ixxs[i]*Iyyys[i]-Iyys[i]*Ixxys[i]+Iyys[i]*Iyyys[i]+4*Ixys[i]*Ixyys[i]);
              //save new orientation
              curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));
          }
          else { 
            correct_subclass[i] = false;
          } 
        }
        break;
      case 15: //lambda2 (smaller)
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Sq = Ixxs[i]*Ixxs[i]-2*Ixxs[i]*Iyys[i]+Iyys[i]*Iyys[i]+4*Ixys[i]*Ixys[i];
          double s = vnl_math_sgn(Ixxs[i]+Iyys[i]);
          if (Sq>0){
              double Fx = Ixxxs[i]*vcl_sqrt(Sq)+Ixyys[i]*vcl_sqrt(Sq)+ s*(-Ixxs[i]*Ixxxs[i]+Ixxs[i]*Ixyys[i]+Iyys[i]*Ixxxs[i]-Iyys[i]*Ixyys[i]-4*Ixys[i]*Ixxys[i]);
              double Fy = Ixxys[i]*vcl_sqrt(Sq)+Iyyys[i]*vcl_sqrt(Sq)+ s*(-Ixxs[i]*Ixxys[i]+Ixxs[i]*Iyyys[i]+Iyys[i]*Ixxys[i]-Iyys[i]*Iyyys[i]-4*Ixys[i]*Ixyys[i]);
              //save new orientation
              curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));
          }
          else { 
            correct_subclass[i] = false;
          } 
        }
        break;
      case 16: //e:=(fxx-fyy)^2 + 4*fxy^2=0
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = vcl_sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Fx = 2*Ixxs[i]*Ixxxs[i]-2*Ixxs[i]*Ixyys[i]-2*Iyys[i]*Ixxxs[i]+2*Iyys[i]*Ixyys[i]+8*Ixys[i]*Ixxys[i];
          double Fy = 2*Ixxs[i]*Ixxys[i]-2*Ixxs[i]*Iyyys[i]-2*Iyys[i]*Ixxys[i]+2*Iyys[i]*Iyyys[i]+8*Ixys[i]*Ixyys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));
          
        }
        break;
      case 17: //del(f)=0
        for (unsigned i=0; i<loc.size();i++)
        {
          //compute gradient magnitude
          mag[i] = sqrt(Ixs[i]*Ixs[i] + Iys[i]*Iys[i]);

          double Fx = Ixxxs[i] + Ixyys[i];
          double Fy = Ixxys[i] + Iyyys[i];

          //save new orientation
          curve_orientations[i] = dbdet_angle0To2Pi(vcl_atan2(Fx, -Fy));
          
        }
        break;
      case 18:
      case 19:
        break;
    }

    //create a new edgemap from the tokens collected from ZC det
    if (out_func){
      edge_map = new dbdet_edgemap(F.ni(), F.nj());

      for (unsigned i=0; i<loc.size(); i++){
        if (correct_subclass[i]){
          if (topo_curve_op<4 || topo_curve_op>7){
            vgl_point_2d<double> pt(loc[i].x()*scale, loc[i].y()*scale);
            edge_map->insert(new dbdet_edgel(pt, curve_orientations[i], mag[i]));
          }
          else { //already at correct scale
            vgl_point_2d<double> pt(loc[i].x(), loc[i].y());
            edge_map->insert(new dbdet_edgel(pt, curve_orientations[i], mag[i]));
          }
        }
      }
    }
    else {
     edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

      for (unsigned i=0; i<loc.size(); i++){
        if (correct_subclass[i]){
          if (topo_curve_op<4 || topo_curve_op>7){
            vgl_point_2d<double> pt(loc[i].x(), loc[i].y());
            edge_map->insert(new dbdet_edgel(pt, curve_orientations[i], mag[i]));
          }
          else {//downscale to match image size
            vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);
            edge_map->insert(new dbdet_edgel(pt, curve_orientations[i], mag[i]));
          }
        }
      }
    }
  }

  vcl_cout << "done!" << vcl_endl;
  
  //vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  //vcl_cout << "time taken for ZC det: " << zc_time << " msec" << vcl_endl;
  vcl_cout << "#edgels = " << edge_map->num_edgels() << vcl_endl;

  // create the output storage classes
  if (out_func){
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view(F));
    output_data_[0].push_back(output_storage);
  }

  //storage class for the edges
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_detect_topographic_curves_process::finish()
{
  return true;
}

