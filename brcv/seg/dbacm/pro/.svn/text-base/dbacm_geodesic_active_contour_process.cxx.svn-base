// This is brcv/seg/dbdet/pro/dbacm_geodesic_active_contour_process.cxx

//:
// \file

#include "dbacm_geodesic_active_contour_process.h"

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_threshold.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <bil/algo/bil_edt.h>
//#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <dbacm/dbacm_geodesic_active_contour.h>


// ------------------------------------------------------------------
//: Constructor
dbacm_geodesic_active_contour_process::dbacm_geodesic_active_contour_process()
{
  if( !parameters()->add("Direction(1 OUTWARD : -1 INWARD)", 
    "-direction", (int)1) || 
    !parameters()->add("inflation weight", "-inflation_w", 0.4f) ||
    !parameters()->add("curvature weight", "-curvature_w", 0.1f) ||
    !parameters()->add("Time step", "-timestep", 0.3f) ||  
    !parameters()->add("Number of iterations", "-num_iters", (unsigned)50) || 
    !parameters()->add("Gradient Norm", "-grad_norm", 0.1f)  ||
    !parameters()->add("Use geodesic", "-use_geodesic", true) ||
    !parameters()->add("Original image available?", "-orig_image", true) ||
    !parameters()->add("Initialization image available?", "-init_image", true) ||
    !parameters()->add("Hard constraint image available?", "-hard_contraint_image", false) ||
    !parameters()->add("Return a new image (otherwise overwrite init image)?", "-new_image", false)
  ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ------------------------------------------------------------------
//: Destructor
dbacm_geodesic_active_contour_process::~dbacm_geodesic_active_contour_process()
{
}


// ------------------------------------------------------------------
//: Clone the process
bpro1_process* dbacm_geodesic_active_contour_process::
clone() const
{
  return new dbacm_geodesic_active_contour_process(*this);
}



// ------------------------------------------------------------------
//: Return the name of this process
vcl_string dbacm_geodesic_active_contour_process::
name()
{
  return "Geodesic Active Contour";
}


// ------------------------------------------------------------------
//: Return the number of input frame for this process
int dbacm_geodesic_active_contour_process::
input_frames()
{
  return 1;
}



// ------------------------------------------------------------------
//: Return the number of output frames for this process
int dbacm_geodesic_active_contour_process::
output_frames()
{
  return 1;
}


// ------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbacm_geodesic_active_contour_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  bool orig_image = true; 
  bool init_image = true;
  bool hard_constraint_image = true;

  this->parameters()->get_value("-orig_image", orig_image);
  this->parameters()->get_value("-init_image", init_image);
  this->parameters()->get_value("-hard_contraint_image", hard_constraint_image);

  if (orig_image) to_return.push_back( "image" ); // image to segment
  if (init_image) to_return.push_back( "image" ); // binary image to initialize gac
  if (hard_constraint_image) to_return.push_back( "image" ); // hard constraint image, e.g. Canny

  return to_return;
}


// ------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbacm_geodesic_active_contour_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  bool new_image = false;
  this->parameters()->get_value("-new_image", new_image);

  if (new_image) to_return.push_back("image");

  return to_return;
}


// ------------------------------------------------------------------
//: Execute the process
bool dbacm_geodesic_active_contour_process::
execute()
{
  if ( input_data_.size() != 1 )
  {
    vcl_cout << "In dbacm_geodesic_active_contour_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  this->clear_output();

  bool orig_image = true; 
  bool init_image = true;
  bool hard_constraint_image = true;

  this->parameters()->get_value("-orig_image", orig_image);
  this->parameters()->get_value("-init_image", init_image);
  this->parameters()->get_value("-hard_contraint_image", hard_constraint_image);

  if (!init_image)
  {
    vcl_cerr << "Automatic initialization has not been implemented." <<
    "Process stops now. No change was done." << vcl_endl;
    return false;
  }
  

  int image_storage_count = 0;
  // get images from the storage class

  // Image 1: the image to segment
  vil_image_view<float > img_src;
  if (orig_image)
  {
    
    vidpro1_image_storage_sptr frame_image1;
    frame_image1.vertical_cast(input_data_[0][image_storage_count]);
    ++image_storage_count;
    vil_image_resource_sptr image1_sptr = frame_image1->get_image();
  
    if (image1_sptr->nplanes()==1)
    {
      img_src = *vil_convert_cast(float(), image1_sptr->get_view());
    }
    else
    {
      img_src = *vil_convert_cast(float(), 
      vil_convert_to_grey_using_rgb_weighting(image1_sptr->get_view()));
    }
  }

  // Image 2: initial image for geodesic active contour
  vil_image_view<bool > img_seg_binary;
  vidpro1_image_storage_sptr frame_image2 = 0;
  if (init_image)
  {
    
    vil_image_view<float > img_seg;
    frame_image2.vertical_cast(input_data_[0][image_storage_count]);
    ++image_storage_count;

    vil_image_resource_sptr image2_sptr = frame_image2->get_image();
  
    if (image2_sptr->nplanes() == 1)
    {
      img_seg = *vil_convert_cast(float(), image2_sptr->get_view());
    }
    else
    {
      img_seg = *vil_convert_cast(float(), 
      vil_convert_to_grey_using_rgb_weighting(image2_sptr->get_view()));
    }


    // threshold the image to make sure that it is binary
    float min_value=0, max_value=0;
    vil_math_value_range(img_seg, min_value, max_value);
    float threshold = (min_value + max_value) / 2;
    vil_threshold_above<float>(img_seg, img_seg_binary, threshold);
  }



  // Image 3 - Hard constraint image
  vil_image_view<bool > feature_map;
  if (hard_constraint_image)
  {
    vidpro1_image_storage_sptr frame_image3;
    frame_image3.vertical_cast(input_data_[0][image_storage_count]);
    ++image_storage_count;
    vil_image_resource_sptr image3_sptr = frame_image3->get_image();
    vil_image_view<float > image3_view;
    if (image3_sptr->nplanes() == 1)
    {
      image3_view = *vil_convert_cast(float(), image3_sptr->get_view());
    }
    else
    {
      image3_view = *vil_convert_cast(float(), 
      vil_convert_to_grey_using_rgb_weighting(image3_sptr->get_view()));
    }

    // threshold to get feature map (in boolean)
    float min_value=0, max_value=0;
    vil_math_value_range(image3_view, min_value, max_value);
    float threshold = (min_value + max_value) / 2;
    vil_threshold_above<float>(image3_view, feature_map, threshold); 
  }


  // consistency check
  // check sizes of the input images, make sure they are all the same


  // ....


  // size of image
  unsigned int ni = img_seg_binary.ni();
  unsigned int nj = img_seg_binary.nj();

  // if original image is not given, make it a black blank image
  if (orig_image)
  {
    // scale img_src to range [0, 1] 
    float min_value=0, max_value=0;
    vil_math_value_range(img_src, min_value, max_value);
    float offset=0;
    double scale=1;

    // special case: constant image
    if (min_value >= max_value)
    {
      offset = -min_value;
      scale = 1.0;
    }
    // normal case
    else
    {
      offset = -min_value / (max_value-min_value);
      scale = 1.0 / (max_value-min_value);
    }

    vil_math_scale_and_offset_values(img_src, scale, offset);
  }
  else
  {
    img_src.set_size(ni, nj);
    img_src.fill(0.0f);
  }
   
  // get geodesic active contour parameters
  float inflation_w = 0.0f, 
    curvature_w = 0.1f,
    timestep = 0.4f, 
    grad_norm = 0.1f;
  
  bool use_geodesic = true;
  unsigned int num_iters=0;
  int direction=0;

  this->parameters()->get_value("-direction", direction);
  this->parameters()->get_value("-inflation_w", inflation_w);
  this->parameters()->get_value("-curvature_w", curvature_w);
  this->parameters()->get_value("-use_geodesic", use_geodesic);
  this->parameters()->get_value("-timestep", timestep);
  this->parameters()->get_value("-num_iters", num_iters);
  this->parameters()->get_value("-grad_norm", grad_norm);

 
  // Invoke geodesic active contour to segment the image
  dbacm_geodesic_active_contour gac;
  gac.set_direction(direction);
  gac.set_weights(inflation_w, curvature_w, use_geodesic);
  gac.set_timestep(timestep);
  gac.set_num_iterations(num_iters);
  gac.set_image(img_src);


  // Feature map
  if (hard_constraint_image)
  {
    gac.set_feature_map(feature_map);
  }
 
  // gradient norm
  gac.set_gradient_norm(grad_norm);

  //gac.init_levelset_surf(phi_init);
  gac.init_levelset_surf_with_binary_mask(img_seg_binary);

  gac.compute_levelset_func_params();
  
  bool verbose = true;
  gac.evolve(verbose);


  // Get results

  vil_image_view<float > phi_final = gac.levelset_surf();
  
  // threshold to get binary image
  vil_threshold_above<float >(phi_final, img_seg_binary, 0);



  // Check if a new returned image is desired
  bool new_image = false;
  this->parameters()->get_value("-new_image", new_image);

  if (new_image)
  {
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
    image_storage->set_image(vil_new_image_resource_of_view(img_seg_binary));
    output_data_[0].push_back(image_storage);  
  }
  else
  {
    frame_image2->set_image(vil_new_image_resource_of_view(img_seg_binary));  
    // This will make Brown Eye update the display of old storage
    output_data_[0].push_back(frame_image2);  
  }
  return true;
}


// ------------------------------------------------------------------
bool dbacm_geodesic_active_contour_process::
finish()
{
  return true;
}





