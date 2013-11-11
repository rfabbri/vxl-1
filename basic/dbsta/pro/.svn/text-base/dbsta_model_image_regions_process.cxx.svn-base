// This is brcv/seg/dbsta/pro/dbsta_model_image_regions_process.cxx

//:
// \file

#include "dbsta_model_image_regions_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbsta/algo/dbsta_image_stats.h>
#include <dbsta/dbsta_histogram.h>
#include <vnl/vnl_vector_fixed.h>



//: Constructor
dbsta_model_image_regions_process::dbsta_model_image_regions_process()
{
  if( !parameters()->add( "number of bins" ,   "-bins" ,   (unsigned int)32 ) ||
      !parameters()->add( "certainty ratio" ,  "-cratio" , 1.5f ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbsta_model_image_regions_process::~dbsta_model_image_regions_process()
{
}


//: Clone the process
bpro1_process* 
dbsta_model_image_regions_process::clone() const
{
  return new dbsta_model_image_regions_process(*this);
}


//: Return the name of this process
vcl_string
dbsta_model_image_regions_process::name()
{
  return "Image Region Stats";
}


//: Return the number of input frame for this process
int
dbsta_model_image_regions_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbsta_model_image_regions_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbsta_model_image_regions_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsta_model_image_regions_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbsta_model_image_regions_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbsta_model_image_regions_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  
   // get the process parameters 
  unsigned int num_bins=0;
  float cratio=0.0f;
  parameters()->get_value( "-bins" , num_bins );
  parameters()->get_value( "-cratio" , cratio );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = vil_convert_cast(float(), image_rsc->get_view());
  float min_val, max_val;
  vil_math_value_range(img, min_val, max_val);
  vcl_cout << min_val << " - "<<max_val<<vcl_endl;
  

  frame_image.vertical_cast(input_data_[0][1]);

  // convert the image to float and smooth it
  image_rsc = frame_image->get_image();
  vil_image_view<float> img2 = vil_convert_cast(float(), image_rsc->get_view());
  vil_math_value_range(img2, min_val, max_val);
  vcl_cout << min_val << " - "<<max_val<<vcl_endl;
  
  vil_image_view<float> joint_img(img.ni(), img.nj(), 2);
  vil_plane(joint_img,0).deep_copy(img);
  vil_plane(joint_img,1).deep_copy(img2);

  
  // get contours from the storage class
  vidpro1_vsol2D_storage_sptr frame_vsol;
  frame_vsol.vertical_cast(input_data_[0][2]);
  vcl_vector<vcl_string> groups = frame_vsol->groups();
  vil_image_view<vxl_byte> idx_image(joint_img.ni(), joint_img.nj(), 1); 
  idx_image.fill(0);
  vxl_byte curr_idx = 0;
  for( vcl_vector<vcl_string>::const_iterator gitr = groups.begin();
       gitr != groups.end();  ++gitr)
  {
    vcl_vector<vsol_spatial_object_2d_sptr> contours = frame_vsol->data_named(*gitr);
    
    vgl_polygon<double> poly_region;
    for( vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator vitr = contours.begin();
         vitr != contours.end();  ++vitr )
    {
      if(vsol_region_2d* r = (*vitr)->cast_to_region())
        if(vsol_polygon_2d* p = r->cast_to_polygon())
        {
          vgl_polygon<double> poly = bsol_algs::vgl_from_poly(p);
          for(unsigned int i=0; i<poly.num_sheets(); ++i)
            poly_region.push_back(poly[i]);
        }
    }
    
    if(poly_region.num_sheets() > 0){ 
      ++curr_idx;      
      vgl_box_2d<double> box(0.0,img.ni()-1.0, 0.0,img.nj()-1.0);
      vgl_polygon_scan_iterator<double> psi(poly_region,false,box);
      for (psi.reset(); psi.next(); ) 
      {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x)
          idx_image(x,y) = curr_idx;
      } 
    }
  }

  
  dbsta_mixture<float> hmix;
  for(unsigned int n=0; n<curr_idx; ++n){
    hmix.insert(dbsta_histogram<float>(joint_img.nplanes(),0.0f,255.999f,num_bins));
  }
  dbsta_image_statistics<float>(joint_img,idx_image,hmix,dbsta_histogram_updater<float>());


  vil_image_view<float> out_img(num_bins,num_bins,3);
  out_img.fill(0.0);
  vcl_cout << (int)curr_idx <<vcl_endl;
  for(unsigned int p=0; p<curr_idx; ++p){
    const dbsta_distribution<float>& hist = hmix.distribution(p);
    for(unsigned int i=0; i<num_bins; ++i){
      for(unsigned int j=0; j<num_bins; ++j){
        vnl_vector_fixed<float,2> pt(i*256.0f/num_bins,j*256.0f/num_bins);
        out_img(i,j,p) = hist.probability(pt);
      }
    }
  }
  vil_math_value_range(out_img, min_val, max_val);
  vcl_cout << "max prob "<<max_val << vcl_endl;
  vil_math_scale_values(out_img,1.0/max_val);

  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( out_img ));
  output_data_[0].push_back(output_storage);
  /*
  output_storage = vidpro1_image_storage_new();
  vil_image_view<vxl_byte> out_img1;
  vil_convert_stretch_range(vil_plane(out_img,0), out_img1);
  output_storage->set_image(vil_new_image_resource_of_view( out_img1 ));
  output_data_[0].push_back(output_storage);
  
  output_storage = vidpro1_image_storage_new();
  vil_image_view<vxl_byte> out_img2;
  vil_convert_stretch_range(vil_plane(out_img,1), out_img2);
  output_storage->set_image(vil_new_image_resource_of_view( out_img2 ));
  output_data_[0].push_back(output_storage);
  */
  
  vil_image_view<vxl_byte> out_img1(joint_img.ni(), joint_img.nj(),3);
  out_img1.fill(0);
  for(unsigned int i=0; i<joint_img.ni(); ++i){
    for(unsigned int j=0; j<joint_img.nj(); ++j){
      vnl_vector_fixed<float,2> pt(joint_img(i,j,0),joint_img(i,j,1));
      float p0 = hmix.distribution(0).probability(pt);
      float p1 = hmix.distribution(1).probability(pt);
      if(p0>(p1*cratio)){
        out_img1(i,j,0) = 255;
      }else if(p1>(p0*cratio)){
        out_img1(i,j,1) = 255;
      }else{
        out_img1(i,j,2) = 255;
      }
    }
  }
 
  output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( out_img1 ));
  output_data_[0].push_back(output_storage);
  
  return true;  
}
    


bool
dbsta_model_image_regions_process::finish()
{
  return true;
}




