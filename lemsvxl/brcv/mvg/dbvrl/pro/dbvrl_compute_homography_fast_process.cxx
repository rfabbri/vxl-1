
//:
// \file
#include<vcl_cstdio.h>
#include "dbvrl_compute_homography_fast_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_point_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vimt/vimt_resample_bilin.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/algo/vil_histogram.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <bvis1/bvis1_manager.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_transform_2d.h>
#include <dbvrl/dbvrl_transform_2d_sptr.h>
#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <dbvrl/pro/dbvrl_region_storage_sptr.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_histogram.h>
#include <vidpro1/vidpro1_repository.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>
#include <bbas/bsta/bsta_otsu_threshold.h>

//: Constructor
dbvrl_compute_homography_fast_process::dbvrl_compute_homography_fast_process(void): bpro1_process(),total_xform()
{

  if(!parameters()->add( "Homography Type" , "-transform_type", vcl_string("Affine")) ||
     !parameters()->add( "Downsample image", "-downsample",(bool)false) ||
     !parameters()->add( "Compute Entropy ","-use_entropy",(bool)true) ||
     !parameters()->add( "Entropy Top Percentage To Keep ","-entropythresh_pcent",(float)0.1) ||
     !parameters()->add( "Use Image Regions","-useregions",(bool)true) ||
     !parameters()->add( "  Region Size","-regionsize",(int)32) ||
     !parameters()->add( "Flow Algorithm (0=none, 1=HS, 2=KL" , "-flowtype" , (int)0)||
     !parameters()->add( "Amount of Motion in Pixels" , "-lmotion" , (float)5.0)||
     !parameters()->add( "Border Size (pixels)" , "-border" , (int)10 ) ||
     !parameters()->add( "First Frame" ,          "-first" ,  (bool)first_frame_ )||
     !parameters()->add( "No of Bins" ,          "-numbins" ,  (int)20) ||
     !parameters()->add( "L range" ,          "-lrange" ,  (float)0.0) ||
     !parameters()->add( "H range" ,          "-hrange" ,  (float)20.0) ||
     !parameters()->add( "Radius for Erosion" ,          "-rerode" ,  (float)0.0) ||
     !parameters()->add( "Radius for Dilation" ,          "-rdilate" ,  (float)0.0) ||
     !parameters()->add( "Random Percentage to Keep" ,    "-randkeep", (float)1.0) ||
     !parameters()->add( "Use Fixed Mask" , "-use_mask" ,  (bool)false) ||
     !parameters()->add( "Mask image file < filename ..>" , "-mask_file" ,
                         bpro1_filepath("","*.*")) ||

     !parameters()->add( "Output file < filename ..>" ,          "-fout" ,  bpro1_filepath("","*.*")) 
     )
    {
      vcl_cerr << "ERROR: Adding parameters in dbvrl_compute_homography_fast_process::vidpro1_kl_affine_register_process()" << vcl_endl;
    }
  else
    {

    }

  total_xform.set_identity();
  first_frame_no=0;
  last_frame_no=0;
}

//: Destructor
dbvrl_compute_homography_fast_process::~dbvrl_compute_homography_fast_process()
{
}


//: Return the name of this process
vcl_string
dbvrl_compute_homography_fast_process::name()
{
  return "Compute Fast Homography";
}


//: Return the number of input frame for this process
int
dbvrl_compute_homography_fast_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbvrl_compute_homography_fast_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbvrl_compute_homography_fast_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbvrl_compute_homography_fast_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbvrl_compute_homography_fast_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In dbvrl_compute_homography_fast_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  vcl_cout<<"\n Computing Fast Homography for frame no "<<last_frame_no << '\n';
  clear_output();
  parameters()->get_value( "-rerode" , eroderadius );
  parameters()->get_value( "-rdilate" , dilateradius );
  bpro1_filepath filename;
  parameters()->get_value("-fout",filename);
  vcl_string fname=filename.path;

  vcl_ofstream ofp(fname.c_str(),vcl_ios::out|vcl_ios::app);
  if(!ofp)
    {
      vcl_cout<<"\n Could not open file "<<fname;
      return false;
    }
  parameters()->get_value( "-first" , first_frame_ );
        
    
  if(first_frame_){
           
    first_frame_no=input_data_[0][0]->frame();
    last_frame_no=input_data_[0][0]->frame();

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image = frame_image->get_image();


    vil_image_view<float> float_curr_view=brip_vil_float_ops::convert_to_float(*image);
    ni_= float_curr_view.ni();
    nj_= float_curr_view.nj();

    total_xform.set_identity();
    ofp<<"Frame No "<<last_frame_no++<<"\n";
    ofp<<total_xform.matrix();
        
    parameters()->set_value( "-first" , false );

    if(intregfirst<=0)
      intregfirst=1;
        
    return true;
  }
  // Get the control parameters

  bool use_mask=false;
  parameters()->get_value( "-use_mask" , use_mask );


  // get image from the storage class
  vidpro1_image_storage_sptr curr_image, last_image;
  curr_image.vertical_cast(input_data_[0][0]);
  last_image.vertical_cast(input_data_[1][0]);

  vil_image_resource_sptr curr_img_sptr = curr_image->get_image();
  vil_image_resource_sptr last_img_sptr = last_image->get_image();

  vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*curr_img_sptr);
  vil_image_view<float> float_last_view= brip_vil_float_ops::convert_to_float(*last_img_sptr);

  ni_= float_curr_view.ni();
  nj_= float_curr_view.nj();

  //false means that the corresponding pixel is used in registration operations
  vil_image_view<float> fixed_mask(ni_, nj_);
  fixed_mask.fill(0.0f);
  
  if(use_mask)
    {
      bpro1_filepath mfile;
      parameters()->get_value("-mask_file", mfile);
      vil_image_view<unsigned char> mask_img = vil_load(mfile.path.c_str());
      for(unsigned r = 0; r<nj_; ++r)
        for(unsigned c = 0; c<ni_; ++c)
          fixed_mask(c, r) = (float)(mask_img(c, r)==0);//for now
    }

  vimt_transform_2d init_xform;
  
    init_xform = register_image_with_optical_flow(float_curr_view,
                                                  float_last_view,
                                                  fixed_mask);
 

  total_xform = total_xform * init_xform.inverse();
  vnl_matrix<double> p=total_xform.matrix();
  ofp<<"Frame No "<<last_frame_no++<<"\n";
  ofp<<p;
  ofp.close();

  vidpro1_image_storage_sptr im=vidpro1_image_storage_new();
  im->set_image(vil_new_image_resource_of_view(fixed_mask));
  output_data_[0].push_back(im);

  return true;  
}
//: Clone the process
bpro1_process*
dbvrl_compute_homography_fast_process::clone() const
{
  return new dbvrl_compute_homography_fast_process(*this);
}

bool
dbvrl_compute_homography_fast_process::finish()
{
  return true;
}


vimt_transform_2d dbvrl_compute_homography_fast_process::
register_image_with_mask(vil_image_view<float> & curr_view,
                         vil_image_view<float> & last_view,
                         vil_image_view<float> & mask)
                         
{
  //: do registration
  vimt_transform_2d init_xform;
  vcl_string transform_type;
  parameters()->get_value( "-transform_type" , transform_type );
  if(transform_type  ==  "Identity")
    {
      vcl_cout << "In dbvrl_compute_homograpy_process:: an identity "
               << "transform doesn't make sense \n";
      assert (false);
    }
  else if(transform_type ==  "Translation")
    {
      vnl_vector<double> T(2);
      T[0]=0.0000001;   T[1]=0.0000001;
      init_xform.set(T, vimt_transform_2d::Translation);
    }
  else if(transform_type ==  "ZoomOnly")
    {
      init_xform.set_zoom_only(1.0, 1.0, 0.0, 0.0);
    }
  else if(transform_type ==  "RigidBody")
    {
      vnl_vector<double> R(3);
      R[0]=0.0000001;   R[1]=0.0;  R[2]=0.0;
      init_xform.set(R, vimt_transform_2d::RigidBody);
        
    }
  else if(transform_type ==  "Similarity")
    {
      vnl_vector<double> S(4);
      S[0]=1.0000001;   S[1]=0.0;  S[2]=0.0; S[3]=0.0;
      init_xform.set(S, vimt_transform_2d::Similarity);
    }
  else if(transform_type ==  "Affine")
    {
      vnl_matrix<double> A(2,3);
      A[0][0] = 1.0;  A[0][1] = 0.0;  A[0][2] = 0.0;
      A[1][0] = 0.0;  A[1][1] = 1.0;  A[1][2] = 0.0;
      init_xform.set_affine(A);
    }
  else if(transform_type ==  "Projective")
    {
      vnl_matrix<double> P(3,3);
      P[0][0] = 1.0;  P[0][1] = 0.0;  P[0][2] = 0.0;
      P[1][0] = 0.0;  P[1][1] = 1.0;  P[1][2] = 0.0;
      P[2][0] = 0.0;  P[2][1] = 0.0;  P[2][2] = 1.0000001;
      init_xform.set_projective(P);
    }
  else if(transform_type ==  "Reflection")
    {
      vgl_point_2d<double> m1(0, 0), m2(1,0);
      init_xform.set_reflection(m1, m2);
    }
  else {
    vcl_cout << "Unrecoverable error in dbvrl_compute_homography_fast_process"
             << " Unkown vimt transform type \n";
    assert(false);
  }
      
  int border = 10;
  parameters()->get_value( "-border" , border );
  dbvrl_world_roi roi(ni_- 2*border, nj_- 2*border,
                      vgl_point_2d<double>(border,border),
                      vgl_vector_2d<double>(0.99,0.0),
                      vgl_vector_2d<double>(0.0,0.99));
  vimt_image_2d_of<float> last_img(last_view, vimt_transform_2d());
  vimt_image_2d_of<float> curr_img(curr_view, init_xform);
  vimt_image_2d_of<float> curr_mask_img(mask, init_xform);
  dbvrl_minimizer minimizer(last_img, curr_img,curr_mask_img, roi);
  vul_timer time;
  minimizer.minimize(init_xform);
  return init_xform;
}



vimt_transform_2d dbvrl_compute_homography_fast_process::
register_image_with_optical_flow(vil_image_view<float> & curr_view,
                                 vil_image_view<float> & last_view,
                                 vil_image_view<float> & mask)
{
  ni_=curr_view.ni();
  nj_=curr_view.nj();

  vil_image_view<float> vx(ni_,nj_);
  vil_image_view<float> vy(ni_,nj_);
  vil_image_view<float> vsum(ni_,nj_);
  vil_image_view<float> entropy_img;
  vul_timer timer;

  int flow_type=0;
  parameters()->get_value( "-flowtype" , flow_type );
  bool use_mask;
  parameters()->get_value( "-use_mask" , use_mask );
  bool use_entropy=false;
  parameters()->get_value( "-use_entropy" , use_entropy );
  float entropythresh_pcent=0;
  parameters()->get_value("-entropythresh_pcent",entropythresh_pcent);
  bool downsample=false;
  parameters()->get_value("-downsample",downsample);

  float motionthresh=0;
  float entropythresh_val=0;

  timer.mark();

 

  if (flow_type == 1) {
    brip_vil_float_ops::Horn_SchunckMotion(curr_view,last_view,vx,vy,0.5f,1);
  }
  else if (flow_type == 2) {
    brip_vil_float_ops::Lucas_KanadeMotion(curr_view,last_view,100,1.0f,vx,vy);
  }
  if (flow_type) {
    vcl_cout << "Optical flow took " << timer.real() << "ms, " << timer.user() << "ms user.\n";

    //: computing the mean of the velocities of the pixels
    float vxmean,vymean;
    vil_math_mean<float,float>(vxmean,vx,0);
    vil_math_mean<float,float>(vymean,vy,0);
    //: subtracting the mean
    vil_math_scale_and_offset_values<float,float>(vx,1.0,-vxmean);
    vil_math_scale_and_offset_values<float,float>(vy,1.0,-vymean);
    //: compute the motion magnitude
    vil_math_image_vector_mag(vx, vy, vsum);
    parameters()->get_value("-lrange",lrange_);
    parameters()->get_value("-hrange",hrange_);
    parameters()->get_value("-numbins",no_of_bins);
    parameters()->get_value("-lmotion",motionthresh);

    //: using otsu thresholding method to select the threshold for motion
    //vcl_vector<double> hist;
    //vil_histogram<float>(vsum,hist,lrange_,hrange_,no_of_bins);
    //bsta_otsu_threshold<double> ot(hist,lrange_,hrange_);
    //motionthresh=ot.threshold();
  }

  bool use_regions=false;
  parameters()->get_value("-useregions",use_regions);
  int region_step=0;
  parameters()->get_value("-regionsize",region_step);

  int compute_radius;
  if (use_regions) {
    compute_radius = (region_step + region_step/2)/2;
  }
  else {
    region_step = 1;
    compute_radius = 15;
  }

  if (use_entropy) {
    // get new resource of image view for entropy function
    vil_image_resource_sptr curr_view_resrc = vil_new_image_resource_of_view(curr_view);

    timer.mark();
    entropy_img = brip_vil_float_ops::entropy(compute_radius, compute_radius, region_step,
                                              curr_view_resrc, 1.0f, true, false, false);
    
    vcl_cout << "Entropy Computation took " << timer.real() << "ms, " << timer.user() << "ms user.\n";
    vcl_cout << "Entropy Image Size = "<< entropy_img.ni() <<" x "<< entropy_img.nj() << "\n";



    vil_math_value_range_percentile<float>(entropy_img,
                                             1.0f - entropythresh_pcent,
                                             entropythresh_val);
    vcl_cout << "Entropy Threshold = "<<entropythresh_val << "\n";
  
  }

  // random decimation of mask
  float rand_keep_pcent = 0.0f;
  parameters()->get_value("-randkeep",rand_keep_pcent);
  rand_keep_pcent = 1.0f - rand_keep_pcent;

  //vil_image_view<float> static_cast<unsigned>(rand_img(vcl_ceil(((float)ni_)/region_step),vcl_ceil(((float)nj_)/region_step)));
  //error: expected unqualified-id before 'static_cast'
  vil_image_view<float> rand_img(static_cast<unsigned>(vcl_ceil(((float)ni_)/region_step)),static_cast<unsigned>(vcl_ceil(((float)nj_)/region_step)));
  if (rand_keep_pcent > 0.0) {
    vnl_random rand_gen;
    for(unsigned i=0; i < vcl_ceil(((float)ni_)/region_step); i++) 
      for(unsigned j=0; j < vcl_ceil(((float)nj_)/region_step); j++) 
        rand_img(i,j) = (float)rand_gen.drand32(0.0,1.0);
  }
  else 
    rand_img.fill(1.0);


  vil_image_view< bool > outgoodpix_bin(ni_,nj_);
  vil_image_view< bool > goodpix_bin(ni_,nj_);

  //Threshold based on motion and entropy
  if (use_entropy) {
    if (flow_type) {
      // use both entropy and optical flow
      for(unsigned i=0; i<ni_; i++)
        for(unsigned j=0; j<nj_; j++)
          if( (vsum(i,j) <= motionthresh) && (entropy_img(i/region_step,j/region_step) >= entropythresh_val) 
                                          && (rand_img(i/region_step,j/region_step) >= rand_keep_pcent) )
            goodpix_bin(i,j)=1;
          else
            goodpix_bin(i,j)=0;
    }
    else {
      // use entropy only
      for(unsigned i=0; i<ni_;i++)
        for(unsigned j=0; j<nj_; j++)
          if( (entropy_img(i/region_step,j/region_step) >= entropythresh_val) &&
                                              (rand_img(i/region_step,j/region_step) >= rand_keep_pcent) )
            goodpix_bin(i,j) = 1;
          else
            goodpix_bin(i,j) = 0;
    }
  } 
  else if (flow_type) {
    // use optical flow only
    for(unsigned i=0;i<ni_;i++)
      for(unsigned j=0;j<nj_;j++)
        if( (vsum(i,j) <= motionthresh) && (rand_img(i/region_step,j/region_step) >= rand_keep_pcent) )
            goodpix_bin(i,j) = 1;
          else
            goodpix_bin(i,j) = 0;
  }
  else {
    // dont use optical flow or entropy
     for(unsigned i=0; i<ni_; i++)
      for(unsigned j=0; j<nj_; j++)
       if(rand_img(i/region_step,j/region_step) >= rand_keep_pcent)
         goodpix_bin(i,j)=1;
        else
          goodpix_bin(i,j) = 0;
  } 
           
  //Apply morphology to clean up the mask
  vil_structuring_element se;
  if (dilateradius > 0) {
    se.set_to_disk(dilateradius);
    vil_binary_dilate(goodpix_bin,outgoodpix_bin,se);
  }
  if (eroderadius > 0) {
    se.set_to_disk(eroderadius);
    vil_binary_erode(outgoodpix_bin,goodpix_bin,se);
  }

  
  //: outgoodpix_bin is the binary mask
  int pix_count = 0;
  for(unsigned i=0; i < ni_; i++)
    for(unsigned j=0; j < nj_; j++)
      if(goodpix_bin(i,j)) {
        mask(i,j)=1.0;
        pix_count++;
      }
      else
        mask(i,j)=0.0f;

  vcl_cout << pix_count <<" of "<< ni_*nj_ <<" pixels are above threshold ("<<((float)pix_count)*100.0f / (ni_*nj_) << "%)\n";
                         
  

  if (downsample) {
    vil_image_view<float> work_im(ni_,nj_);
    vil_image_view<float> mask_small(ni_/2,nj_/2);
    vil_image_view<float> curr_view_small(ni_/2,nj_/2);
    vil_image_view<float> last_view_small(ni_/2,nj_/2);

    // downsample the image - we will account for this in the computed homography later  
    vil_gauss_reduce(curr_view,curr_view_small,work_im);
    vil_gauss_reduce(last_view,last_view_small,work_im);
    vil_gauss_reduce(mask,mask_small,work_im);

    vcl_cout << "downsampled - using "<<pix_count<<" pixels for homography\n";
    timer.mark();
    vimt_transform_2d t2d = this->register_image_with_mask(curr_view_small, last_view_small, mask_small);
    vcl_cout << "Homography computation took " << timer.real() << "ms, " << timer.user() << "ms user.\n";

    // bring transform back to full resolution
    t2d.set_origin( vgl_point_2d<double>(t2d.origin().x()*2.0, 
                                     t2d.origin().y()*2.0));

    return t2d;

  }else {
    vcl_cout << "using "<<pix_count<<" pixels for homography\n";
  
    timer.mark();
    vimt_transform_2d t2d = this->register_image_with_mask(curr_view, last_view, mask);
    vcl_cout << "Homography computation took " << timer.real() << "ms, " << timer.user() << "ms user.\n";

    return t2d;
  }
}
