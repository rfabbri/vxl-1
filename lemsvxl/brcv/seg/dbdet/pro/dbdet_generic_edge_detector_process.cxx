// This is brcv/seg/dbdet/pro/dbdet_generic_edge_detector_process.cxx

//:
// \file

#include "dbdet_generic_edge_detector_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vcl_complex.h>
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
#include <vil/vil_bilin_interp.h>

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
#include <vil/vil_fill.h>
#include <vil/vil_border.h>

//: Constructor
dbdet_generic_edge_detector_process::dbdet_generic_edge_detector_process()
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
dbdet_generic_edge_detector_process::~dbdet_generic_edge_detector_process()
{
}


//: Clone the process
bpro1_process*
dbdet_generic_edge_detector_process::clone() const
{
  return new dbdet_generic_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_generic_edge_detector_process::name()
{
  return "Generic Edge Detector";
}


//: Return the number of input frame for this process
int
dbdet_generic_edge_detector_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_generic_edge_detector_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_generic_edge_detector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_generic_edge_detector_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_generic_edge_detector_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_generic_edge_detector_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  vcl_cout << "Generic edge detection...";
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


int padding=10;
  vil_image_view<vxl_byte> padded_img;
  padded_img.set_size(
      greyscale_view.ni()+2*padding,greyscale_view.nj()+2*padding);
  vil_fill(padded_img, vxl_byte(0));

  vil_border_accessor<vil_image_view<vxl_byte> >
    accessor = vil_border_create_accessor(
        greyscale_view,
        vil_border_create_geodesic(greyscale_view));

  int j_max = (int)(padded_img.nj())-padding;
  int i_max = (int)(padded_img.ni())-padding;


  for (int j = -padding ; j < j_max;++j)
  {
      for (int i=-padding;i < i_max;++i)
      {                          
          padded_img(i+padding,j+padding)=accessor(i,j);
      }
  }


  //get the parameters
  unsigned grad_op, conv_algo, parabola_fit;
  double sigma, thresh;
  int N=0;
  bool bsubpix_edges;
  parameters()->get_value( "-grad_op", grad_op);
  parameters()->get_value( "-conv_algo", conv_algo);
  parameters()->get_value( "-int_factor" , N );
  parameters()->get_value( "-sigma", sigma);
  parameters()->get_value( "-thresh", thresh);
  parameters()->get_value( "-parabola_fit", parabola_fit );
  parameters()->get_value( "-bsubpix_edges", bsubpix_edges);

  //start the timer
  vul_timer t;

  //compute image gradients before performing nonmax suppression
  vil_image_view<double> grad_x, grad_y, grad_mag;
  int scale=1;

  switch (grad_op)
  {
    case 0: // Sobel 1x3
    {
      vil_sobel_1x3 <vxl_byte, double> (padded_img, grad_x, grad_y);

      //set the sizes to be the same as the image
      grad_mag.set_size(padded_img.ni(), padded_img.nj());

      //get the pointers to the memory chunks
      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 1: // Sobel 3x3
    {
      vil_sobel_3x3 <vxl_byte, double> (padded_img, grad_x, grad_y);

      //set the sizes to be the same as the image
      grad_mag.set_size(padded_img.ni(), padded_img.nj());

      //get the pointers to the memory chunks
      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 2: //Gaussian
    {  
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(padded_img, grad_x, dbdet_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(padded_img, grad_y, dbdet_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(padded_img, grad_x, dbdet_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(padded_img, grad_y, dbdet_Gy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(grad_x.ni(), grad_x.nj());

      //get the pointers to the memory chunks
      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 3: //h0-operator
    {
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(padded_img, grad_x, dbdet_h0_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(padded_img, grad_y, dbdet_h0_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(padded_img, grad_x, dbdet_h0_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(padded_img, grad_y, dbdet_h0_Gy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(grad_x.ni(), grad_x.nj());

      //get the pointers to the memory chunks
      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 4:  //h1-operator
    {
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      if (conv_algo==0){ //2-d convolutions
        dbdet_subpix_convolve_2d(padded_img, grad_x, dbdet_h1_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d(padded_img, grad_y, dbdet_h1_Gy_kernel(sigma), double(), N);
      }
      else {
        dbdet_subpix_convolve_2d_sep(padded_img, grad_x, dbdet_h1_Gx_kernel(sigma), double(), N);
        dbdet_subpix_convolve_2d_sep(padded_img, grad_y, dbdet_h1_Gy_kernel(sigma), double(), N);
      }

      //compute gradient magnitude
      grad_mag.set_size(grad_x.ni(), grad_x.nj());

      //get the pointers to the memory chunks
      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude
      for(unsigned long i=0; i<grad_mag.size(); i++)
        g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

      break;
    }
    case 5:  //F-operator
    {
      scale = (int) vcl_pow(2.0, N);

      //compute gradients
      vil_image_view<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxx, Ixxy, Ixyy, Iyyy;
      dbdet_subpix_convolve_2d(padded_img, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);

      grad_x.set_size(Ix.ni(), Ix.nj());
      grad_y.set_size(Ix.ni(), Ix.nj());
      grad_mag.set_size(Ix.ni(), Ix.nj());

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

      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();
      double *g_mag  =  grad_mag.top_left_ptr();

      //compute the gradient magnitude and assign the third order orientation as the gradient
      for(unsigned long i=0; i<grad_mag.size(); i++){
        g_mag[i] = vcl_sqrt(ix[i]*ix[i] + iy[i]*iy[i]);

        //compute Fx and Fy
        double Fx = 2*ix[i]*ixx[i]*ixx[i] + 2*ix[i]*ixy[i]*ixy[i] + 2*ixx[i]*iy[i]*ixy[i] + 
                    2*ixy[i]*iyy[i]*iy[i] + 2*ix[i]*iy[i]*ixxy[i] + ixyy[i]*iy[i]*iy[i] + ix[i]*ix[i]*ixxx[i];
        double Fy = 2*iy[i]*iyy[i]*iyy[i] + 2*iy[i]*ixy[i]*ixy[i] + 2*ixy[i]*ix[i]*ixx[i] + 
                    2*ix[i]*iyy[i]*ixy[i]  + 2*ix[i]*iy[i]*ixyy[i] + ixxy[i]*ix[i]*ix[i] + iyyy[i]*iy[i]*iy[i];

        gx[i] = Fx;
        gy[i] = Fy;
      }

      break;
    }
    case 6:  //R-operator
    {
      scale = (int) vcl_pow(2.0, N);

      dbdet_subpix_convolve_2d(padded_img, grad_mag,   dbdet_G_kernel(sigma),   double(), N);

      //compute gradients
      vil_image_view<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxx, Ixxy, Ixyy, Iyyy;
           
      dbdet_subpix_convolve_2d(padded_img, Ix,   dbdet_Gx_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iy,   dbdet_Gy_kernel(sigma),   double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixx,  dbdet_Gxx_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixy,  dbdet_Gxy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iyy,  dbdet_Gyy_kernel(sigma),  double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixxx, dbdet_Gxxx_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixxy, dbdet_Gxxy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Ixyy, dbdet_Gxyy_kernel(sigma), double(), N);
      dbdet_subpix_convolve_2d(padded_img, Iyyy, dbdet_Gyyy_kernel(sigma), double(), N);

      grad_x.set_size(Ix.ni(), Ix.nj());
      grad_y.set_size(Ix.ni(), Ix.nj());

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

      double *gx  =  grad_x.top_left_ptr();
      double *gy  =  grad_y.top_left_ptr();

      //compute the gradient magnitude and assign the third order orientation as the gradient
      for(unsigned long i=0; i<grad_mag.size(); i++)
      {
        //compute Rx and Ry
        double Rx = ix[i]*ixy[i]*ixx[i] -iy[i]*ixx[i]*ixx[i] -ix[i]*iy[i]*ixxx[i] -2*iy[i]*ixy[i]*ixy[i] 
                    -iy[i]*iy[i]*ixxy[i] +ix[i]*ix[i]*ixxy[i] +iy[i]*ixx[i]*iyy[i] +ix[i]*ixy[i]*iyy[i] +ix[i]*iy[i]*ixyy[i];
        double Ry = -ix[i]*iyy[i]*ixx[i] -iy[i]*ixy[i]*ixx[i] -ix[i]*iy[i]*ixxy[i] -iy[i]*ixy[i]*iyy[i] 
                    -iy[i]*iy[i]*ixyy[i] +2*ix[i]*ixy[i]*ixy[i] +ix[i]*ix[i]*ixyy[i] +ix[i]*iyy[i]*iyy[i] +ix[i]*iy[i]*iyyy[i];

        gx[i] = Rx;
        gy[i] = Ry;
      }

      break;
    }
  }
  
  double conv_time = t.real();  
  t.mark(); //reset timer

  //Now call the nms code to get the subpixel edge tokens
  vcl_vector<vgl_point_2d<double> > loc;
  vcl_vector<double> orientation, mag, d2f;
  vcl_vector<vgl_point_2d<int> > pix_loc;

  dbdet_nms NMS(dbdet_nms_params(thresh, (dbdet_nms_params::PFIT_TYPE)parabola_fit), grad_x, grad_y, grad_mag);
  NMS.apply(true, loc, orientation, mag, d2f, pix_loc);

  double nms_time = t.real();
  
  //compute a Gaussian smoothed image from which to compute the intensities
  vil_image_view<double> sm_img;
  if (conv_algo==0) //2-d convolutions
    dbdet_subpix_convolve_2d(padded_img, sm_img, dbdet_G_kernel(sigma), double(), 0);
  else 
    dbdet_subpix_convolve_2d_sep(padded_img, sm_img, dbdet_G_kernel(sigma), double(), 0);

  //create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(padded_img.ni(), padded_img.nj());

  for (unsigned i=0; i<loc.size(); i++){
    if (!bsubpix_edges){
    vgl_point_2d<double> pt(pix_loc[i].x()/scale, pix_loc[i].y()/scale);
    edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
    //edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]), pix_loc[i].x(), pix_loc[i].y());
    }
    else {
      vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);

      // for now get the appearance parameter in a coarse fashion from the image
      vgl_point_2d<double> ptL(loc[i].x()/scale - sigma*vcl_cos(orientation[i]-vnl_math::pi_over_2), 
                               loc[i].y()/scale - sigma*vcl_sin(orientation[i]-vnl_math::pi_over_2));
      vgl_point_2d<double> ptR(loc[i].x()/scale + sigma*vcl_cos(orientation[i]-vnl_math::pi_over_2), 
                               loc[i].y()/scale + sigma*vcl_sin(orientation[i]-vnl_math::pi_over_2));
      
      //we could get these values from
      ////(a)from the original image or
      //dbdet_appearance* lapp = new dbdet_intensity(vil_bilin_interp(padded_img, ptL.x(), ptL.y()));
      //dbdet_appearance* rapp = new dbdet_intensity(vil_bilin_interp(padded_img, ptR.x(), ptR.y()));

      //(b) from the smoothed image
      dbdet_appearance* lapp = new dbdet_intensity(vil_bilin_interp(sm_img, ptL.x(), ptL.y()));
      dbdet_appearance* rapp = new dbdet_intensity(vil_bilin_interp(sm_img, ptR.x(), ptR.y()));

      dbdet_edgel* cur_edgel = new dbdet_edgel(pt, orientation[i], mag[i], d2f[i], -1, lapp, rapp);
      edge_map->insert(cur_edgel);
    }
  }

  // convert back to unpad edges
//create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr padded_edge_map = new dbdet_edgemap(greyscale_view.ni(), 
                                                  greyscale_view.nj());

  vcl_vector<dbdet_edgel*> padded_edges=edge_map->edgels;
  for ( unsigned int i=0; i < padded_edges.size() ; ++i)
  {

      vgl_point_2d<double> new_location;
      new_location.set(padded_edges[i]->pt.x()-(double)padding,
                       padded_edges[i]->pt.y()-(double)padding);
      padded_edges[i]->pt.set(new_location.x(),new_location.y());

      if ( (new_location.x() >= 0) && 
           (new_location.x() <= (double)greyscale_view.ni()-1) && 
           (new_location.y() >= 0) &&
           (new_location.y() <= (double)greyscale_view.nj()-1))
      {
          
          padded_edge_map->
              insert(new dbdet_edgel(padded_edges[i]->pt,
                                     padded_edges[i]->tangent,
                                     padded_edges[i]->strength,
                                     padded_edges[i]->deriv));
          
      } 
  }
//  edge_map->unref();
  edge_map=0;


  vcl_cout << "done!" << vcl_endl;
  
  vcl_cout << "time taken for conv: " << conv_time << " msec" << vcl_endl;
  vcl_cout << "time taken for nms: " << nms_time << " msec" << vcl_endl;
  vcl_cout << "#edgels = " << padded_edge_map->num_edgels();

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(padded_edge_map);
  output_data_[0].push_back(output_edgemap);

  return true;
}

bool
dbdet_generic_edge_detector_process::finish()
{
  return true;
}

