//:
// \file
// \author Isabel Restrepo
// \date 5-Aug-2010

#include "dbrec3d_models.h"

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bxml/bxml_find.h>
#include <vnl/vnl_vector_fixed.h>

template<>
float dbrec3d_kernel_model::compute_posterior(float kernel_response)
{
  return kernel_response;

}

template<>
float dbrec3d_kernel_model::compute_posterior(bsta_num_obs<bsta_gauss_sf1> kernel_response)
{
  return kernel_response.mean();
  //return app_model_.prob_density(kernel_response.mean());
  
}

//: XML write - returns itself as an xml_data
bxml_data_sptr dbrec3d_kernel_model::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec3d_kernel_model");
  data->append_text("\n");
  return data;
}

//: XML write - returns itself as an xml_data
bxml_data_sptr dbrec3d_pairwise_gaussian_model::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec3d_pairwise_gaussian_model");
  data->set_attribute("dist_mean0", dist_model_.mean()[0]);
  data->set_attribute("dist_mean1", dist_model_.mean()[1]);
  data->set_attribute("dist_mean2", dist_model_.mean()[2]);
  data->set_attribute("dist_diag_covar0", dist_model_.diag_covar()[0]);
  data->set_attribute("dist_diag_covar1", dist_model_.diag_covar()[1]);
  data->set_attribute("dist_diag_covar2", dist_model_.diag_covar()[2]);
  data->append_text("\n");
  
  return data;
}


//: read self from the bxml_data_sptr
dbrec3d_pairwise_gaussian_model* 
dbrec3d_pairwise_gaussian_model::parse_xml_element(bxml_data_sptr data)
{
  dbrec3d_pairwise_gaussian_model* comp=0;
  bxml_element query("dbrec3d_pairwise_gaussian_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;

  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  float mean0, mean1, mean2, covar0, covar1, covar2;
  bsta_gauss_id3 dist_model;
  
  r_elm->get_attribute("dist_mean0", mean0);
  r_elm->get_attribute("dist_mean1", mean1);
  r_elm->get_attribute("dist_mean2", mean2);

  r_elm->get_attribute("dist_diag_covar0", covar0);
  r_elm->get_attribute("dist_diag_covar1", covar1);
  r_elm->get_attribute("dist_diag_covar2", covar2);

  vnl_vector_fixed<double,3> mean(mean0,mean1,mean2);
  vnl_vector_fixed<double,3> covar(covar0,covar1,covar2);
  dist_model.set_mean(mean); 
  dist_model.set_covar(covar);
  
  comp = new dbrec3d_pairwise_gaussian_model(dist_model);
  dist_model_.set_covar(covar);
  dist_model_.set_mean(mean);
  return comp;
}
