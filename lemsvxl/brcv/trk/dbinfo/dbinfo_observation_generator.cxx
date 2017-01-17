#include <vcl_cassert.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <bugl/bugl_random_transform_set_2d.h>
#include <dbinfo/dbinfo_observation.h>


dbinfo_observation_sptr 
dbinfo_observation_generator::generate(dbinfo_observation_sptr const& seed, 
                                       vgl_h_matrix_2d<float> const& H,
                                       const float sufficient_points_thresh)
{
  dbinfo_observation_sptr result= (dbinfo_observation*)0;

  //replicate the feature storage
  vcl_vector<dbinfo_feature_base_sptr> features = seed->features(),
    new_features;
  assert(features.size());
  for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit =  features.begin();
      fit != features.end(); ++fit)
    new_features.push_back((*fit)->feature_base_new());

  //Get the geometry 
 dbinfo_region_geometry_sptr  oldg = seed->geometry();
  assert(oldg);
  
  //and transform it, creating a new geometry
  dbinfo_region_geometry_sptr newg = 
    new dbinfo_region_geometry(*oldg, H, sufficient_points_thresh);
  if(!newg)
    return result;
  //The resulting region is outside the image, so return null
  if(!(newg->sufficient_points()))
    return result;

  // the new observation
  result = new dbinfo_observation(newg, new_features);  
  return result;
}

bool dbinfo_observation_generator::
uniform_about_seed(const unsigned nobs,
                   dbinfo_observation_sptr const& seed,
                   vcl_vector<dbinfo_observation_sptr>& obs,
                   const float dx, const float dy,
                   const float dtheta, const float dscale,
                   const float sufficient_points_thresh)
{
  assert(seed);

  unsigned limit = 2*nobs;//two tries

  obs.clear();
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::equiform_uniform(limit, trans_set,
                                                        dx, dy,
                                                        dtheta, dscale);

  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      dbinfo_observation_sptr ob = generate(seed, trans_set[l],
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      ++i;
    }
  return true;
}

//: generate a set of randomly placed observations about a seed observation
// return the corresponding transforms as well
bool dbinfo_observation_generator::uniform_about_seed(const unsigned nobs,
                               dbinfo_observation_sptr const& seed,
                               vcl_vector<dbinfo_observation_sptr>& obs,
                               vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                               const float dx, const float dy,
                               const float dtheta,
                               const float dscale,
                               const float sufficient_points_thresh)
{
  assert(seed);
  obs.clear();
  xforms.clear();
  unsigned limit = 2*nobs;//two tries
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::equiform_uniform(limit, trans_set,
                                                        dx, dy,
                                                        dtheta, dscale);
  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      vgl_h_matrix_2d<float> trans = trans_set[l];
      dbinfo_observation_sptr ob = generate(seed, trans,
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      xforms.push_back(trans);
      ++i;
    }
  return true;
}

bool dbinfo_observation_generator::
uniform_about_seed_affine_no_skew(const unsigned nobs,
                                  dbinfo_observation_sptr const& seed,
                                  vcl_vector<dbinfo_observation_sptr>& obs,
                                  const float dx, const float dy,
                                  const float dtheta, const float dscale,
                                  const float daspect,
                                  const float sufficient_points_thresh)
{
  assert(seed);

  unsigned limit = 2*nobs;//two tries

  obs.clear();
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::zero_skew_affine_uniform(limit, trans_set,
                                                                dx, dy,
                                                                dtheta, dscale, daspect);

  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      dbinfo_observation_sptr ob = generate(seed, trans_set[l],
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      ++i;
    }
  return true;
}
bool dbinfo_observation_generator::uniform_about_seed_affine_no_skew(const unsigned nobs,
                                       dbinfo_observation_sptr const& seed,
                                       vcl_vector<dbinfo_observation_sptr>& obs,
                                       vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                                       const float dx, const float dy,
                                       const float dtheta, const float dscale,
                                       const float daspect,
                                       const float sufficient_points_thresh)
{
  assert(seed);

  unsigned limit = 2*nobs;//two tries

  obs.clear();
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::zero_skew_affine_uniform(limit, trans_set,
                                                                dx, dy,
                                                                dtheta, dscale, daspect);

  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      dbinfo_observation_sptr ob = generate(seed, trans_set[l],
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      xforms.push_back(trans_set[l]);
      ++i;
    }
  return true;
}
bool dbinfo_observation_generator::
uniform_in_interval_affine_no_skew(const unsigned nobs,
                                   dbinfo_observation_sptr const& seed,
                                   vcl_vector<dbinfo_observation_sptr>& obs,
                                   const float x_min, const float x_max,
                                   const float y_min, const float y_max,
                                   const float theta_min,
                                   const float theta_max,
                                   const float scale_min,
                                   const float scale_max,
                                   const float aspect_min,
                                   const float aspect_max,
                                   const float sufficient_points_thresh)
{
  assert(seed);

  unsigned limit = 2*nobs;//two tries

  obs.clear();
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::
    zero_skew_affine_uniform_interval(limit, trans_set,
                                      x_min, x_max,
                                      y_min, y_max,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max);

  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      dbinfo_observation_sptr ob = generate(seed, trans_set[l],
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      ++i;
    }
  return true;
}

bool dbinfo_observation_generator::
uniform_in_interval_affine_no_skew(const unsigned nobs,
                                   dbinfo_observation_sptr const& seed,
                                   vcl_vector<dbinfo_observation_sptr>& obs,
                                   vcl_vector<vgl_h_matrix_2d<float> >& xforms,
                                   const float x_min, const float x_max,
                                   const float y_min, const float y_max,
                                   const float theta_min,
                                   const float theta_max,
                                   const float scale_min,
                                   const float scale_max,
                                   const float aspect_min,
                                   const float aspect_max,
                                   const float sufficient_points_thresh)
{
  assert(seed);

  unsigned limit = 2*nobs;//two tries
  obs.clear();
  xforms.clear();
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::
    zero_skew_affine_uniform_interval(limit, trans_set,
                                      x_min, x_max,
                                      y_min, y_max,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max);

  for(unsigned i = 0, l=0; i<nobs&&l<limit; ++l)
    {
      dbinfo_observation_sptr ob = generate(seed, trans_set[l],
                                            sufficient_points_thresh);
      if(!ob)
        continue;
      obs.push_back(ob);
      xforms.push_back(trans_set[l]);
      ++i;
    }
  return true;
}
  


