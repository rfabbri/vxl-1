// This is brcv/seg/dbdet/pro/dbacm_knee_cartilage_seg_process.cxx

//:
// \file

#include "dbacm_knee_cartilage_seg_process.h"

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
//#include <vil/vil_new.h>
//#include <vil/algo/vil_threshold.h>
//
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>


#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
//
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
//
//#include <bil/algo/bil_edt.h>
//#include <dbdet/tracer/dbdet_contour_tracer.h>
//#include <dbacm/dbacm_geodesic_active_contour.h>


// ------------------------------------------------------------------
//: Constructor
dbacm_knee_cartilage_seg_process::
dbacm_knee_cartilage_seg_process()
{
  
  if( 
    !parameters()->add("Segment femur cartilage", 
    "-femur", true) 
    //|| 
  //  !parameters()->add("inflation weight", "-inflation_w", 0.5f) ||
  //  !parameters()->add("curvature weight", "-curvature_w", 0.1f) ||
  //  !parameters()->add("Use geodesic", "-use_geodesic", true) ||
  //  !parameters()->add("Time step", "-timestep", 0.4f) ||  
  //  !parameters()->add("Number of iterations", "-num_iters", (unsigned)10) || 
  //  !parameters()->add("Gradient Norm", "-grad_norm", 0.1f) ||
  //  !parameters()->add("Smoothing sigma", "-sigma", 1.0f) ||
  //  !parameters()->add("Min contour length", "-len_thresh", 20.0f)
      ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ------------------------------------------------------------------
//: Destructor
dbacm_knee_cartilage_seg_process::
~dbacm_knee_cartilage_seg_process()
{
}


// ------------------------------------------------------------------
//: Clone the process
bpro1_process* dbacm_knee_cartilage_seg_process::
clone() const
{
  return new dbacm_knee_cartilage_seg_process(*this);
}



// ------------------------------------------------------------------
//: Return the name of this process
vcl_string dbacm_knee_cartilage_seg_process::
name()
{
  return "Knee cartilage seg";
}


// ------------------------------------------------------------------
//: Return the number of input frame for this process
int dbacm_knee_cartilage_seg_process::
input_frames()
{
  return 1;
}



// ------------------------------------------------------------------
//: Return the number of output frames for this process
int dbacm_knee_cartilage_seg_process::
output_frames()
{
  return 1;
}


// ------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbacm_knee_cartilage_seg_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  return to_return;
}


// ------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbacm_knee_cartilage_seg_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


// ------------------------------------------------------------------
//: Execute the process
bool dbacm_knee_cartilage_seg_process::
execute()
{  
  if ( input_data_.size() != 1 )
  {
    vcl_cout << "In dbacm_knee_cartilage_seg_process::execute()-not exactly one"
             << " input images \n";
    return false;
  }
  this->clear_output();

  // 1. Get data from storage classes

  // a. Inner contour of cartilage
  vidpro1_vsol2D_storage_sptr frame_vsol1;
  frame_vsol1.vertical_cast(input_data_[0][0]);
  vcl_vector<vsol_spatial_object_2d_sptr > all_vsol1 = frame_vsol1->all_data();
 
  // consider the first polygon as the inner contour
  vsol_polygon_2d_sptr polygon = 0;
  for (unsigned int i=0; i<all_vsol1.size(); ++i)
  {
    vsol_spatial_object_2d_sptr p = all_vsol1[i];
    if (p->cast_to_region())
    {
      if (p->cast_to_region()->cast_to_polygon())
      {
        polygon = p->cast_to_region()->cast_to_polygon();
        break;
      }
    }
  }
  // safety check
  if (!polygon)
  {
    vcl_cerr << "Error: Could not find cartilage inner contour " __FILE__ << vcl_endl;
    return false;
  }

  // b. User-selection points
  // There should be at least 4 points: 
  // 2 end points and at least 2 along the contour
  // a. Inner contour of cartilage
  vsol_point_2d_sptr start_pt = 0;
  vsol_point_2d_sptr end_pt = 0;
  vcl_vector<vsol_point_2d_sptr > user_pts;

  vidpro1_vsol2D_storage_sptr frame_vsol2;
  frame_vsol2.vertical_cast(input_data_[0][1]);
  vcl_vector<vsol_spatial_object_2d_sptr > all_vsol2 = frame_vsol2->all_data();
 
  // extract all the points.
  for (unsigned int i=0; i<all_vsol2.size(); ++i)
  {
    vsol_spatial_object_2d_sptr p = all_vsol2[i];
    if (p->cast_to_point())
    {
      user_pts.push_back(p->cast_to_point());
    }
  }

  // safety check
  if (user_pts.size() < 4)
  {
    vcl_cerr << "Error: Not enough contour points " __FILE__ << vcl_endl;
    return false;
  }

  start_pt = user_pts.front();
  end_pt = user_pts.back();

  //// c. image to segment
  //vidpro1_image_storage_sptr frame_image1;
  //frame_image1.vertical_cast(input_data_[0][2]);
  //vil_image_resource_sptr image1_sptr = frame_image1->get_image();
  //vil_image_view<float > img_src;
  //if (image1_sptr->nplanes()==1)
  //{
  //  img_src = *vil_convert_cast(float(), image1_sptr->get_view());
  //}
  //else
  //{
  //  img_src = *vil_convert_cast(float(), 
  //  vil_convert_to_grey_using_rgb_weighting(image1_sptr->get_view()));
  //}

  //// scale img_src to range [0, 1]
  //float min_value, max_value;
  //vil_math_value_range(img_src, min_value, max_value);
  //float offset=0;
  //double scale=1;

  //// ignore constant image
  //if (min_value < max_value)
  //{
  //  offset = -min_value / (max_value-min_value);
  //  scale = 1.0 / (max_value-min_value);
  //}
  //vil_math_scale_and_offset_values(img_src, scale, offset);

  // ==================================================
  // ALGORITHM
  // >> Find correpondence on contour to user selection points
  vcl_vector<int > user_pts_corr;
  for (unsigned int i=0; i<user_pts.size(); ++i)
  {
    vsol_point_2d_sptr pt = user_pts[i];

    // brute-force search for min-distance
    double min_distance = 1e12;
    unsigned int order = 0;
    for (unsigned m=0; m<polygon->size(); ++m)
    {
      double d = pt->distance(polygon->vertex(m));
      if (d<min_distance) 
      {
        min_distance = d;
        order = m;
      }
    }
    user_pts_corr.push_back(order);
  }

  // check whether polygon and user selection are of same direction
  // using order of 3 points [0], [1], and [last]
  int count = (user_pts_corr[0]>user_pts_corr.back() ?  1 : -1) + 
    (user_pts_corr[1]>user_pts_corr[0] ? 1 : -1) +
    (user_pts_corr.back()>user_pts_corr[1]);

  int direction = (count > 0) ? 1 : -1;
 

  // >> Extract points cartilage contour points form the polygon

  // a polyline of cartilage inner contour
  vcl_vector<vsol_point_2d_sptr > inner_contour_pts;
  count = 0;
  int num_user_pts = user_pts_corr.size();
  //unused bool stop = false;
  int start_param = user_pts_corr.front();
  int end_param = user_pts_corr.back();
  for (int i=start_param, point_count =0;;
    // make sure first operand is positive
    i = (i+direction+polygon->size()) % polygon->size(),
    ++point_count)
  {
    inner_contour_pts.push_back(polygon->vertex(i));
    // update parameters of closest point on inner contour
    if (count < num_user_pts && i==user_pts_corr[count])
    {
      user_pts_corr[count] = point_count;
      ++count;
    }
    if (i == end_param) break;
  }

  vcl_vector<vsol_point_2d_sptr > non_inner_contour_pts;
  for (int i = (end_param + direction+polygon->size()) % polygon->size();
    i != start_param; 
    i = (i+direction+polygon->size()) % polygon->size())
  {
    non_inner_contour_pts.push_back(polygon->vertex(i));
  }
  
  vsol_polyline_2d_sptr inner_contour = 
    new vsol_polyline_2d(inner_contour_pts);

  // process parameter
  bool segment_femur = true;
  this->parameters()->get_value("-femur", segment_femur);

  // >> Interpolate outer contour using user-inputs and inner contour
  // by linear interpolation of distance
  
  // interpolate for every interval
  vcl_vector<vsol_point_2d_sptr > outer_contour_pts;

  int dir = (segment_femur) ? 1 : -1;
  for (unsigned int i=0; i<user_pts.size()-1; ++i)
  {
    vcl_vector<vsol_point_2d_sptr > segment_pts;
    for (int j=user_pts_corr[i]; j <= user_pts_corr[i+1]; ++j)
    {
      segment_pts.push_back(inner_contour->vertex(j));
    }
    vsol_polyline_2d_sptr segment = new vsol_polyline_2d(segment_pts);
    
    // compute distance at two end points
    vsol_point_2d_sptr user_start_pt = user_pts[i];
    vsol_point_2d_sptr user_end_pt = user_pts[i+1];

    double start_thickness = user_start_pt->distance(segment->p0());
    double end_thickness = user_end_pt->distance(segment->p1());

    // thickness increment per unit length
    double m = (end_thickness-start_thickness)/segment->length();

    // start interpolation

    double accum_len = 0;
    outer_contour_pts.push_back(user_start_pt);
    for (unsigned int j=1; j<segment->size()-1; ++j)
    {
      accum_len += segment->vertex(j)->distance(segment->vertex(j-1));
      double thickness = start_thickness + accum_len*m;

      // tangent
      vgl_vector_2d<double > t = 
        segment->vertex(j+1)->get_p()-segment->vertex(j-1)->get_p();
      normalize(t);
      
      // normal
      vgl_vector_2d<double > n(-t.y(), t.x());

      vgl_point_2d<double > pt = segment->vertex(j)->get_p()+n*thickness*dir;

      outer_contour_pts.push_back(new vsol_point_2d(pt));
    }
  }
  outer_contour_pts.push_back(user_pts.back());

//  vsol_polyline_2d_sptr outer_contour = 
//    new vsol_polyline_2d(outer_contour_pts);


  // outer polygon uses outer contour and other points of the inner polygon
  for (unsigned int i=0; i<outer_contour_pts.size(); ++i)
  {
    non_inner_contour_pts.push_back(outer_contour_pts[i]);
  }


  vsol_polygon_2d_sptr outer_polygon = 
    new vsol_polygon_2d(non_inner_contour_pts);

  
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  ////
  //output_vsol->add_object(inner_contour->cast_to_spatial_object(), 
  //  "inner_contour");
  //output_vsol->add_object(outer_contour->cast_to_spatial_object(), 
  //  "outer_contour");
  output_vsol->add_object(outer_polygon->cast_to_spatial_object(),
    "outer_polygon");

  
  
  
  //output_vsol->add_object(inner_contour, "trace");
  output_data_[0].push_back(output_vsol);
  return true;
}


// ------------------------------------------------------------------
bool dbacm_knee_cartilage_seg_process::
finish()
{
  return true;
}





