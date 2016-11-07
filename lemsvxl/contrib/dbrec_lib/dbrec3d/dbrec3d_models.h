// This is dbrec3d_models.h
#ifndef dbrec3d_models_h
#define dbrec3d_models_h

//:
// \file
// \brief A class that contains the model for different parts 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  5-Aug-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bsta/bsta_gauss_id3.h>
#include <bsta/bsta_gauss_sf1.h>

#include "dbrec3d_part_instance.h"

#include <bxml/bxml_document.h>


class dbrec3d_kernel_model{
  
public:
  dbrec3d_kernel_model(): app_model_(bsta_gauss_sf1(0.025f,0.05)){}
  
  template <class T_data>
  float compute_posterior(T_data sample);
  
  float max_posterior() { return 0.5; } //app_model_.prob_density(app_model_.mean()); }
  
  bxml_data_sptr xml_element() const;
  
private:
  bsta_gauss_sf1 app_model_;
  
};

//: Keeps independent gaussian for each of the parameters. Model MUST be in GLOBAL coordinates
class dbrec3d_pairwise_gaussian_model{
  
public:
//  dbrec3d_pairwise_gaussian_model():dist_model_(bsta_gauss_id3()), rotation_model_(bsta_gaussian_indep<double,4>()){}
//  
//  dbrec3d_pairwise_gaussian_model(bsta_gauss_id3 dist_model, bsta_gaussian_indep<double,4>  rotation_model):
//  dist_model_(dist_model), rotation_model_(rotation_model){}

  dbrec3d_pairwise_gaussian_model():dist_model_(bsta_gauss_id3()){}

  dbrec3d_pairwise_gaussian_model(bsta_gauss_id3 dist_model):dist_model_(dist_model){}
  
  double prob_density(dbrec3d_pair_composite_instance const &ci)
  {
    
    vnl_vector_fixed<double,3> distance(ci.location_child1().x() - ci.location_child2().x(),
                                        ci.location_child1().y() - ci.location_child2().y(), 
                                        ci.location_child1().z() - ci.location_child2().z());
    return dist_model_.prob_density(distance);
  }
  
  double max_posterior() { return dist_model_.prob_density(dist_model_.mean()); }
  
  //: XML write - returns itself as an xml_data
  bxml_data_sptr xml_element() const;

  dbrec3d_pairwise_gaussian_model* parse_xml_element(bxml_data_sptr data);

  //bsta_gauss_id3 model() { return dist_model_; }
  
private:
  
  bsta_gauss_id3 dist_model_;
  //bsta_gaussian_indep<double,4> rotation_model_;

  
};




//class dbrec3d_test_model{
//  
//public:
//  dbrec3d_test_model(){}
//  
//  
//  
//protected:
//  
//  //: Rotation axis associated with this part
//  vnl_float_3 rotation_axis_;
//  
//  //: Number of bins - should this be given by latitude/longitude i.e illumination bins in bvxm
//  unsigned n_bins_;
//  
//  //: The models 
//  vcl_vector<bsta_gaussian_sphere<double, 3> > position_dist;
//  
//  //: The model for relative orientation -start by assuming all arientations are equally likely?
//  //vcl_vector<bsta_gaussian_sphere<double, 3> > orientation_dist;
//
//  
//};
#endif
