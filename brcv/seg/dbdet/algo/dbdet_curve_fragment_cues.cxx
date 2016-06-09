#include "dbdet_curve_fragment_cues.h"
#include <dbgl/algo/dbgl_diffgeom.h>
#include <vil/algo/vil_colour_space.h>
#include <vcl_cmath.h>

void dbdet_curve_fragment_cues::
compute_all_cues(
      const dbdet_edgel_chain &c, 
      y_feature_vector *features_ptr // indexed by the enum
      )
{
  unsigned const npts = c.edgels.size();
  const dbdet_edgel_list e = c.edgels;
  //const vil_image_view<vxl_uint_32> dt = *dt_;
  y_feature_vector &features = *features_ptr;
  features[Y_ONE] = 1;

  cuvature_cues(c, features_ptr);
  hsv_gradient_cues(c, features_ptr);
  features[Y_EDGE_SPARSITY] = lateral_edge_sparsity_cue(c, features_ptr);
  //mean_conf = mean(cfrag(:,4));

  // compute average edge strength (mean_conf)

  double conf=0;
  for (dbdet_edgel_list_const_iter eit=c.edgels.begin(); eit != c.edgels.end(); eit++) {
    conf += (*eit)->strength;
  }
  features[Y_MEAN_CONF] = conf / npts;
}

void
dbdet_curve_fragment_cues::
cuvature_cues(
      const dbdet_edgel_chain &c, 
      y_feature_vector *features_ptr // indexed by the enum
      )
{
  y_feature_vector &features = *features_ptr;
  unsigned const npts = c.edgels.size();
  // curvature
  vnl_vector<double> k;
  vcl_vector<vgl_point_2d<double> > points;

  //to vector of points..
  for (unsigned i = 0; i < npts; ++i)
  {
    points.push_back(c.edgels[i]->pt);
  } 
  dbgl_compute_curvature(points, &k);
  assert(k.size() == npts);

  for (unsigned i=0; i < npts; ++i)
    if (vnl_math::isnan(k[i]))
      k[i] = 0;

  { // wiggliness is the number of times curvature changes sign
    features[Y_WIGG] = 0;
    for (unsigned i=0; i + 1 < npts; ++i)
      features[Y_WIGG] += ( (k[i+1] >= 0) == (k[i] >= 0) );
    features[Y_WIGG] /= npts;
  }

  features[Y_ABS_K] = k.one_norm() / npts;
}

void
dbdet_curve_fragment_cues::
hsv_gradient_cues(
      const dbdet_edgel_chain &c, 
      y_feature_vector *features_ptr // indexed by the enum
    )
{
  // examine local_dist neighborhood around curve along normals
  unsigned const npts = c.edgels.size();
  vcl_vector< vnl_vector_fixed<double, 2> > n;
  n.reserve(npts);
  vcl_vector<vgl_point_2d<double> > points;

  //to vector of points..
  for (unsigned i = 0; i < npts; ++i)
  {
    points.push_back(c.edgels[i]->pt);
  }
  dbgl_compute_normals(points, &n);

  // get neighborhood points to be examined
  y_feature_vector &features = *features_ptr;

  features[Y_HUE_GRAD] = 0;
  features[Y_SAT_GRAD] = 0;
  features[Y_BG_GRAD]  = 0;

  for (unsigned i=0; i < npts; ++i) {
    double left_x  = points[i].x() - local_dist_ * n[i][0];
    double left_y  = points[i].y() - local_dist_ * n[i][1];
    double right_x = points[i].x() + local_dist_ * n[i][0];
    double right_y = points[i].y() + local_dist_ * n[i][1];

    // make sure image clamps to within bounds
    vil_border_accessor<vil_image_view<vil_rgb<vxl_byte> > >
      im = vil_border_create_accessor(img_,vil_border_create_geodesic(img_));

    double hue_left, hue_right,
           sat_left, sat_right,
           bg_left, bg_right;

    vil_rgb<vxl_byte> rgb = im(left_x,left_y);
    vil_colour_space_RGB_to_HSV<double>(rgb.r, rgb.g, rgb.b, 
        &hue_left, &sat_left, &bg_left);
   rgb = im(right_x,right_y);
   vil_colour_space_RGB_to_HSV<double>(rgb.r, rgb.g, rgb.b, 
        &hue_right, &sat_right, &bg_right);

    // TODO test if imae indexing is (x,y) or (y,x)
    features[Y_SAT_GRAD] += vcl_abs(sat_left - sat_right);
    features[Y_BG_GRAD]  += vcl_abs(bg_left - bg_right) / 255.;
    // TODO need to make angle difference to make sense
    features[Y_HUE_GRAD] += vcl_abs(hue_left - hue_right)/360.;
  }
  features[Y_HUE_GRAD] /= npts;
  features[Y_SAT_GRAD] /= npts;
  features[Y_BG_GRAD]  /= npts;
}

double dbdet_curve_fragment_cues::
lateral_edge_sparsity_cue(
    const dbdet_edgel_chain &c,
    y_feature_vector *features_ptr // indexed by the enum
    )
{
  y_feature_vector &features = *features_ptr;
  unsigned const npts = c.edgels.size();
  unsigned total_edges = 0;
  /*if (use_dt()) {
    for (unsigned i=0; i < npts; ++i) {
      // locate bucket
      unsigned p_i = static_cast<unsigned>(e[i]->pt.x()+0.5);
      unsigned p_j = static_cast<unsigned>(e[i]->pt.y()+0.5);
      
      if (dt(p_i, p_j) > nbr_width_)
        continue;

      // visit all nearby edgels and count the number within a distance
      // mark already visited edgels

      // TODO:optimize access to be row-first
      for (int d_i = -nbr_width_; di < nbr_width_; ++d_i) {
        for (int d_j = -nbr_width_; dj < nbr_width_; ++d_j) {
          if (not_visited(p_i + d_i, p_j + d_j)) {
            unsigned nh_x = static_cast<unsigned>(p_i + d_i);
            unsigned nh_y = static_cast<unsigned>(p_j + d_j);
            total_edges += em.cell(nh_x,nh_y).size();
            mark_visited(nh_x,nh_y);
          }
        }
      }
    }
  } else { // no dt
  }
  */
  // assert(!use_dt());

  const dbdet_edgel_list &e = c.edgels;
  for (unsigned i=0; i < npts; ++i) {
    unsigned p_i = static_cast<unsigned>(e[i]->pt.x()+0.5);
    unsigned p_j = static_cast<unsigned>(e[i]->pt.y()+0.5);
    mark_visited(p_i, p_j);
  }

  for (unsigned i=0; i < npts; ++i) {
    // locate bucket
    unsigned p_i = static_cast<unsigned>(e[i]->pt.x()+0.5);
    unsigned p_j = static_cast<unsigned>(e[i]->pt.y()+0.5);
    
    // visit all nearby edgels and count the number within a distance
    // mark already visited edgels

    // TODO:optimize access to be row-first
    for (int d_i = -nbr_width_; d_i < nbr_width_; ++d_i) {
      for (int d_j = -nbr_width_; d_j < nbr_width_; ++d_j) {
        if (not_visited(p_i + d_i, p_j + d_j)) {
          unsigned nh_x = static_cast<unsigned>(p_i + d_i);
          unsigned nh_y = static_cast<unsigned>(p_j + d_j);
          total_edges += em_.edge_cells(nh_x,nh_y).size();
          mark_visited(nh_x,nh_y);
        }
      }
    }
  }
  visited_id_++;
  
  return total_edges/(features[Y_LEN] = euclidean_length(c));
}
