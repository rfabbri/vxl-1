// This is /algo/dbetl/dbetl_epiprofile.cxx
//:
// \file

#include "dbetl_epiprofile.h"
#include "dbetl_point_2d.h" 
#include <dbecl/dbecl_episeg.h>
#include <vcl_algorithm.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_bilin_interp.h>
#include <vgl/vgl_distance.h>


bool dbetl_cmp_dist(const dbetl_point_2d_sptr& rhs, 
                   const dbetl_point_2d_sptr& lhs)
{
  return rhs->dist() < lhs->dist();
}


//: Computes the intersections of an epipolar line with a vector of episegs
vcl_vector<dbetl_point_2d_sptr> 
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, double angle )
{
  vcl_vector<dbetl_point_2d_sptr> to_return;
  
  for( vcl_vector<dbecl_episeg_sptr>::const_iterator itr = episegs.begin();
       itr != episegs.end();  ++itr )
  {
    // intersect with the bounding wedge first
    if( angle < (*itr)->min_angle() || angle > (*itr)->max_angle() )
      continue;

    double index = (*itr)->index(angle);
    to_return.push_back(new dbetl_point_2d(*itr, index));
  }

  vcl_sort(to_return.begin(), to_return.end(), dbetl_cmp_dist);

  return to_return;
}


//: Computes the intersections of an epipolar line with a vector of episegs
//  and also compiles statistics from the image along the profile
vcl_vector<dbetl_point_2d_sptr> 
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, 
                 const vil_image_resource_sptr& image,
                 double angle )
{
  vcl_vector<dbetl_point_2d_sptr> to_return = dbetl_epiprofile( episegs, angle );

  // need at least 2 points to sample intensity
  if( to_return.size() < 2 )
    return to_return;

  vil_image_view<vxl_byte > img_view = image->get_view();

  vgl_point_2d<double> prev_pt = to_return.front()->pt();
  for( vcl_vector<dbetl_point_2d_sptr>::iterator itr = to_return.begin()+1;
       itr != to_return.end();  ++itr )
  {
    vgl_point_2d<double> curr_pt = (*itr)->pt();
    double dist = vgl_distance(curr_pt,prev_pt);
    int n = (int)vcl_floor(dist);

    dbetl_image_stats stats;
    // if the points are less than a pixel apart, sample the midpoint
    if(n<1){
      vgl_point_2d<double> mid = centre(prev_pt, curr_pt);
      double r = vil_bilin_interp_safe(img_view, mid.x(), mid.y(), 0);
      double g = vil_bilin_interp_safe(img_view, mid.x(), mid.y(), 1);
      double b = vil_bilin_interp_safe(img_view, mid.x(), mid.y(), 2);
      stats.add_sample(vil_rgb<double>(r,g,b));
    }
    // else sample at roughly 1 pixel increments and average
    else{
      // offset the sample set such that it lies evenly between the bounds
      double offset = (dist - double(n))/2.0 + 0.5;
      for(int i=0; i<n; ++i){
        vgl_point_2d<double> pt = midpoint(prev_pt, curr_pt, (i+offset)/dist);
        double r = vil_bilin_interp_safe(img_view, pt.x(), pt.y(), 0)/255.0;
        double g = vil_bilin_interp_safe(img_view, pt.x(), pt.y(), 1)/255.0;
        double b = vil_bilin_interp_safe(img_view, pt.x(), pt.y(), 2)/255.0;
        stats.add_sample(vil_rgb<double>(r,g,b));
      }
    }

    (*itr)->set_stats_near(stats);
    (*(itr-1))->set_stats_far(stats);

    prev_pt = curr_pt;
  }

  return to_return;
}


//: Computes the intersections of several epipolar lines with a vector of episegs.
//  Also compiles statistics from the image along the profile.
//  Also links intersection point across neighboring epipolar lines
vcl_vector<vcl_vector<dbetl_point_2d_sptr> >
dbetl_epiprofile( const vcl_vector<dbecl_episeg_sptr>& episegs, 
                 const vil_image_resource_sptr& image,
                 double start_angle, double step, int num )
{
  // Compute each epipolar profile
  vcl_vector<vcl_vector<dbetl_point_2d_sptr> > results(num);
  for(int i=0; i<num; ++i){
    results[i] = dbetl_epiprofile(episegs, image, start_angle + i*step);
  }

  // Link the resulting points
  typedef vcl_vector<dbetl_point_2d_sptr>::iterator pnt_iterator;
  for(int i=1; i<num; ++i){
    pnt_iterator start = results[i].begin();
    for( pnt_iterator p_itr = results[i-1].begin(); p_itr != results[i-1].end(); ++p_itr){
      pnt_iterator c_itr = start;
      while(c_itr != results[i].end() && (*c_itr)->episeg() != (*p_itr)->episeg()) ++c_itr;
      if(c_itr == results[i].end())
        continue;
      (*c_itr)->set_prev(*p_itr);
      (*p_itr)->set_next(*c_itr);
      start = c_itr+1;
    }
  }

  return results;
}
