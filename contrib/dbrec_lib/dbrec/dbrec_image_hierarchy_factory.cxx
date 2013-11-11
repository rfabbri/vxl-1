//:
// \file
// \author Ozge C Ozcanli (ozge at lems dot brown dot edu)
// \date 05/26/09

#include "dbrec_image_hierarchy_factory.h"
#include "dbrec_type_id_factory.h"
#include "dbrec_gaussian_primitive.h"
#include "dbrec_image_compositor.h"
#include "dbrec_image_pairwise_models.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vil/vil_image_resource.h>

//: for recognition of vehicles on Steeple Street Data
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_steeple0()
{
  vcl_cout << "In dbrec_image_hierarchy_factory::construct_detector_steeple0() -- creating hierarchy to recognize vehicles in Steeple St Data\n";
    
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  float prim_radius = 5.0f;

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 2.5f, 20.0f, true); // (lambda0=2.0,lambda1=2.5,theta=20,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian (ins->new_type(), 3.0f, 1.0f, 20.0f, false); // (lambda0=3.0,lambda1=1.0,theta=20,bright=false)
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);
  
  // LAYER 1: two layer 1 parts
  //: create the children vector of the first part
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_0);  // two children of type 0_0

  //: create the composition model 
  //: first train the angle and dist models
  vnl_vector_fixed<float,2> e1c(1006.0f,449.0f); // central part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2c(947.0f,405.0f); // central part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3c(895.0f,454.0f); // central part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4c(815.0f,611.0f); // central part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5c(918.0f,605.0f); // central part center measured from the image for the 5th exampler

  vnl_vector_fixed<float,2> e1p2c(994.0f,447.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2p2c(936.0f,404.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3p2c(885.0f,452.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4p2c(805.0f,609.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5p2c(909.0f,599.0f); // second part center measured from the image for the 5th exampler
  
  dbrec_pairwise_indep_gaussian_model* pigm = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm = pigm;
  pigm->update_models(dir_v_0_0, e1c, e1p2c);
  pigm->update_models(dir_v_0_0, e2c, e2p2c);
  pigm->update_models(dir_v_0_0, e3c, e3p2c);
  pigm->update_models(dir_v_0_0, e4c, e4p2c);
  //pigm->update_models(dir_v_0_0, e5c, e5p2c);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; models.push_back(pm);
  dbrec_central_compositor* cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  // LAYER 1: second part
  //: create the children vector of the second part
  vcl_vector<dbrec_part_sptr> ch_1_1;
  ch_1_1.push_back(p_0_0); ch_1_1.push_back(p_0_1);  // two children

  //: create the composition model
  //: first train the angle and dist models
  //: part2 of the previous edge is the center for this edge
  vnl_vector_fixed<float,2> e1p3c(995.0f,451.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2p3c(935.0f,407.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3p3c(882.0f,454.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4p3c(805.0f,613.0f); // second part center measured from the image for the 4th exampler
  //vnl_vector_fixed<float,2> e5p3c(899.0f,602.0f); // second part center measured from the image for the 5th exampler

  dbrec_pairwise_indep_gaussian_model* pigm_1_1 = new dbrec_pairwise_indep_gaussian_model(3.0f, 15.0f);
  dbrec_pairwise_model_sptr pm_1_1 = pigm_1_1;
  pigm_1_1->update_models(dir_v_0_0, e1p2c, e1p3c);
  pigm_1_1->update_models(dir_v_0_0, e2p2c, e2p3c);
  pigm_1_1->update_models(dir_v_0_0, e3p2c, e3p3c);
  pigm_1_1->update_models(dir_v_0_0, e4p2c, e4p3c);
  //pigm_1_1->update_models(dir_v_0_0, e5p2c, e5p3c);
  models.clear(); models.push_back(pm_1_1);
  dbrec_compositor_sptr cc_1_1 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_1 = new dbrec_composition(ins->new_type(), ch_1_1, cc_1_1, 1*prim_radius);

  // LAYER 2: one part
  //: create the children vector
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); ch_2_0.push_back(p_1_1);

  // train the composition
  dbrec_pairwise_indep_gaussian_model* pigm_2_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_2_0 = pigm_2_0;
  pigm_2_0->update_models(dir_v_0_0, e1c, e1p2c);  //sample1 = e1p2c - e1c; // center difference measured from the image
  pigm_2_0->update_models(dir_v_0_0, e2c, e2p2c);
  pigm_2_0->update_models(dir_v_0_0, e3c, e3p2c);
  pigm_2_0->update_models(dir_v_0_0, e4c, e4p2c);
  //pigm_2_0->update_models(dir_v_0_0, e5c, e5p2c);

  models.clear(); models.push_back(pm_2_0);
  dbrec_compositor_sptr cc_2_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_2_0, 2*prim_radius);

  //: create second layer2 node for the dark vehicles

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_2 = new dbrec_gaussian(ins->new_type(), 6.0f, 3.0f, 20.0f, false); // (lambda0=6.0,lambda1=3.0,theta=20,bright=false)
  dbrec_part_sptr p_0_2 = pg_0_2;
  vnl_vector_fixed<float,2> dir_v_0_2;
  pg_0_2->get_direction_vector(dir_v_0_2);

  dbrec_gaussian* pg_0_3 = new dbrec_gaussian(ins->new_type(), 2.0f, 2.0f, 0.0f, false); // (lambda0=2.0,lambda1=2.0,theta=0,bright=false)
  dbrec_part_sptr p_0_3 = pg_0_3;
  vnl_vector_fixed<float,2> dir_v_0_3;
  pg_0_3->get_direction_vector(dir_v_0_3);

  dbrec_gaussian* pg_0_4 = new dbrec_gaussian(ins->new_type(), 1.0f, 1.0f, 0.0f, true); // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  dbrec_part_sptr p_0_4 = pg_0_4;
  vnl_vector_fixed<float,2> dir_v_0_4;
  pg_0_4->get_direction_vector(dir_v_0_4);
  
  // LAYER 1: two layer 1 parts
  vcl_vector<dbrec_part_sptr> ch_1_2;
  ch_1_2.push_back(p_0_2); ch_1_2.push_back(p_0_3);

  // train the composition
  dbrec_pairwise_indep_gaussian_model* pigm_1_2 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_2 = pigm_1_2;

  vnl_vector_fixed<float,2> e1_0_2(988.0f,463.0f); // central part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2_0_2(978.0f,474.0f); // central part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3_0_2(957.0f,496.0f); // central part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4_0_2(906.0f,451.0f); // central part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5_0_2(915.0f,557.0f); // central part center measured from the image for the 5th exampler

  vnl_vector_fixed<float,2> e1_0_3(990.0f,466.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2_0_3(978.0f,476.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3_0_3(962.0f,501.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4_0_3(908.0f,454.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5_0_3(927.0f,563.0f); // second part center measured from the image for the 5th exampler

  pigm_1_2->update_models(dir_v_0_2, e1_0_2, e1_0_3);  //sample1 = e1_0_3 - e1_0_2; // center difference measured from the image
  pigm_1_2->update_models(dir_v_0_2, e2_0_2, e2_0_3);  
  //pigm_1_2->update_models(dir_v_0_2, e3_0_2, e3_0_3); 
  pigm_1_2->update_models(dir_v_0_2, e4_0_2, e4_0_3);  
  //pigm_1_2->update_models(dir_v_0_2, e5_0_2, e5_0_3);  
  
  models.clear(); models.push_back(pm_1_2);
  dbrec_compositor_sptr cc_1_2 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_2 = new dbrec_composition(ins->new_type(), ch_1_2, cc_1_2, 1*prim_radius);
  
  // LAYER 1: second part
  vcl_vector<dbrec_part_sptr> ch_1_3;
  ch_1_3.push_back(p_0_3); ch_1_3.push_back(p_0_4);
  
  //: part2 of the previous edge is the center for this edge
  vnl_vector_fixed<float,2> e1_0_4(977.0f,460.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2_0_4(969.0f,470.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3_0_4(950.0f,492.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4_0_4(899.0f,448.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5_0_4(903.0f,556.0f); // second part center measured from the image for the 5th exampler

  // train this edge
  // train the composition
  dbrec_pairwise_indep_gaussian_model* pigm_1_3 = new dbrec_pairwise_indep_gaussian_model(5.0f, 20.0f);
  dbrec_pairwise_model_sptr pm_1_3 = pigm_1_3;
  pigm_1_3->update_models(dir_v_0_3, e1_0_3, e1_0_4);  //sample1 = e1_0_4 - e1_0_3; // center difference measured from the image
  pigm_1_3->update_models(dir_v_0_3, e2_0_3, e2_0_4);  
  //pigm_1_3->update_models(dir_v_0_3, e3_0_3, e3_0_4);  
  pigm_1_3->update_models(dir_v_0_3, e4_0_3, e4_0_4);  
  //pigm_1_3->update_models(dir_v_0_3, e5_0_3, e5_0_4);  

  models.clear(); models.push_back(pm_1_3);
  dbrec_compositor_sptr cc_1_3 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_3 = new dbrec_composition(ins->new_type(), ch_1_3, cc_1_3, 1*prim_radius);
  
  // LAYER 2: one part
  vcl_vector<dbrec_part_sptr> ch_2_1;
  ch_2_1.push_back(p_1_2); ch_2_1.push_back(p_1_3);
  
  dbrec_pairwise_indep_gaussian_model* pigm_2_1 = new dbrec_pairwise_indep_gaussian_model(2.0f, 15.0f);
  dbrec_pairwise_model_sptr pm_2_1 = pigm_2_1;
  pigm_2_1->update_models(dir_v_0_2, e1_0_2, e1_0_3);
  pigm_2_1->update_models(dir_v_0_2, e2_0_2, e2_0_3);
  //pigm_2_1->update_models(dir_v_0_2, e3_0_2, e3_0_3);
  pigm_2_1->update_models(dir_v_0_2, e4_0_2, e4_0_3);
  //pigm_2_1->update_models(dir_v_0_2, e5_0_2, e5_0_3);

  models.clear(); models.push_back(pm_2_1);
  dbrec_compositor_sptr cc_2_1 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_2_1 = new dbrec_composition(ins->new_type(), ch_2_1, cc_2_1, 2*prim_radius);

  //: now p_2_0 and p_2_1 are the OR'd nodes under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_3_0;
  ch_3_0.push_back(p_2_0); ch_3_0.push_back(p_2_1);
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_3_0 = new dbrec_composition(ins->new_type(), ch_3_0, cc_or, 3*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_3_0);
  
  return h;
}

//: Detector for short vehicles
//  Construct a hierarchy manually for ROI 1
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_roi1_0()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: only one layer 1 part
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); ch_1_0.push_back(p_0_1);

  vnl_vector_fixed<float,2> c1(500.0f,500.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(600.0f,600.0f); // center measured from the image
  
  vnl_vector_fixed<float,2> p1c1(501.0f,503.0f); //
  vnl_vector_fixed<float,2> p1c2(602.0f,602.0f); //
  
  vnl_vector_fixed<float,2> p2c1(498.0f,499.0f); //
  vnl_vector_fixed<float,2> p2c2(597.0f,598.0f); //
  
  dbrec_pairwise_indep_gaussian_model* pigm_1_0_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_0 = pigm_1_0_0;
  pigm_1_0_0->update_models(dir_v_0_0, c1, p1c1);
  pigm_1_0_0->update_models(dir_v_0_0, c2, p1c2);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_pairwise_indep_gaussian_model* pigm_1_0_1 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_1 = pigm_1_0_1;
  pigm_1_0_1->update_models(dir_v_0_0, c1, p2c1);
  pigm_1_0_1->update_models(dir_v_0_0, c2, p2c2);
  
  models.push_back(pm_1_0_1);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  //: now p_1_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_2_0);
  
  return h;
}
// a hierarchy which contains only the prims for roi1_0 detector
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_roi1_0_prims()  
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  //: now prims are OR'd under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_or, prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_1_0);
  
  return h;
}
// a hierarchy which breaks roi1_0 so that there are two compositional parts with 2 prims, and they are composed such that their centers overlap
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_roi1_0_1()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: two layer 1 parts with two prims each
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  vnl_vector_fixed<float,2> c1(500.0f,500.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(600.0f,600.0f); // center measured from the image
  
  vnl_vector_fixed<float,2> p1c1(501.0f,503.0f); //
  vnl_vector_fixed<float,2> p1c2(602.0f,602.0f); //
  
  dbrec_pairwise_indep_gaussian_model* pigm_1_0_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_0 = pigm_1_0_0;
  pigm_1_0_0->update_models(dir_v_0_0, c1, p1c1);
  pigm_1_0_0->update_models(dir_v_0_0, c2, p1c2);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  vcl_vector<dbrec_part_sptr> ch_1_1;
  ch_1_1.push_back(p_0_0); ch_1_1.push_back(p_0_1);

  vnl_vector_fixed<float,2> p2c1(498.0f,499.0f); //
  vnl_vector_fixed<float,2> p2c2(597.0f,598.0f); //
  
  dbrec_pairwise_indep_gaussian_model* pigm_1_1_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_1_0 = pigm_1_1_0;
  pigm_1_1_0->update_models(dir_v_0_0, c1, p2c1);
  pigm_1_1_0->update_models(dir_v_0_0, c2, p2c2);
  
  models.clear();
  models.push_back(pm_1_1_0);

  dbrec_compositor_sptr cc_1_1 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_1 = new dbrec_composition(ins->new_type(), ch_1_1, cc_1_1, 1*prim_radius);

  //: now create p_2_0 which has a central compositor
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); ch_2_0.push_back(p_1_1);

  dbrec_pairwise_indep_gaussian_model* pigm_2_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_2_0 = pigm_2_0;
  pigm_2_0->update_models(dir_v_0_0, c1, c1);
  pigm_2_0->update_models(dir_v_0_0, c2, c2);
  
  models.clear();
  models.push_back(pm_2_0);

  dbrec_compositor_sptr cc_2_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_2_0, 1*prim_radius);
  //

  //: now p_2_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_3_0;
  ch_3_0.push_back(p_2_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_3_0 = new dbrec_composition(ins->new_type(), ch_3_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_3_0);
  
  return h;
}
// the level 1 compositional part of roi1_0_1 with discrete pairwise model for training
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_roi1_0_1_part()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: one layer 1 part with two prims, and the model is discrete 
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  dbrec_pairwise_model_sptr pm_1_0_0 = new dbrec_pairwise_discrete_model(8, 1.0f, 6.0f, 5);   
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  //: now p_2_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_or, 1*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_2_0);
 
  return h;
}

dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_roi1_0_downsampled_by_2()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 2.5f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 1.0f, 0.5f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 1.0f, 0.5f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: only one layer 1 part
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); ch_1_0.push_back(p_0_1);

  vnl_vector_fixed<float,2> c1(500.0f,500.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(600.0f,600.0f); // center measured from the image
  
  vnl_vector_fixed<float,2> p1c1(501.0f,503.0f); //
  vnl_vector_fixed<float,2> p1c2(602.0f,602.0f); //
  
  vnl_vector_fixed<float,2> p2c1(498.0f,499.0f); //
  vnl_vector_fixed<float,2> p2c2(597.0f,598.0f); //
  
  dbrec_pairwise_indep_gaussian_model* pigm_1_0_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_0 = pigm_1_0_0;
  pigm_1_0_0->update_models(dir_v_0_0, c1, p1c1);
  pigm_1_0_0->update_models(dir_v_0_0, c2, p1c2);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_pairwise_indep_gaussian_model* pigm_1_0_1 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_1 = pigm_1_0_1;
  pigm_1_0_1->update_models(dir_v_0_0, c1, p2c1);
  pigm_1_0_1->update_models(dir_v_0_0, c2, p2c2);
  
  models.push_back(pm_1_0_1);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  //: now p_1_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_2_0);
  
  return h;
}


//: Detector for longer vehicles
//  Construct a hierarchy manually for ROI 1
dbrec_hierarchy_sptr
dbrec_image_hierarchy_factory::construct_detector_roi1_1()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(ins->new_type(), 1.0f, 0.5f, 45.0f, false); // (lambda0=1.0,lambda1=0.5,theta=45,bright=false)
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: only one layer 1 part
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); ch_1_0.push_back(p_0_1);

  vnl_vector_fixed<float,2> c1(557.0f,662.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(598.0f,583.0f); // center measured from the image
  vnl_vector_fixed<float,2> c3(605.0f,574.0f); // center measured from the image
  vnl_vector_fixed<float,2> c4(421.0f,776.0f); // center measured from the image

  vnl_vector_fixed<float,2> p1c1(563.0f,655.0f); //
  vnl_vector_fixed<float,2> p1c2(600.0f,579.0f); //
  vnl_vector_fixed<float,2> p1c3(609.0f,569.0f); //
  vnl_vector_fixed<float,2> p1c4(423.0f,773.0f); //

  vnl_vector_fixed<float,2> p2c1(553.0f,666.0f); //
  vnl_vector_fixed<float,2> p2c2(593.0f,586.0f); //
  vnl_vector_fixed<float,2> p2c3(600.0f,579.0f); //
  vnl_vector_fixed<float,2> p2c4(418.0f,778.0f); //

  dbrec_pairwise_indep_gaussian_model* pigm_1_0_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_0 = pigm_1_0_0;
  pigm_1_0_0->update_models(dir_v_0_0, c1, p1c1);
  pigm_1_0_0->update_models(dir_v_0_0, c2, p1c2);
  pigm_1_0_0->update_models(dir_v_0_0, c3, p1c3);
  pigm_1_0_0->update_models(dir_v_0_0, c4, p1c4);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_pairwise_indep_gaussian_model* pigm_1_0_1 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_1 = pigm_1_0_1;
  pigm_1_0_1->update_models(dir_v_0_0, c1, p2c1);
  pigm_1_0_1->update_models(dir_v_0_0, c2, p2c2);
  pigm_1_0_1->update_models(dir_v_0_0, c3, p2c3);
  pigm_1_0_1->update_models(dir_v_0_0, c4, p2c4);
  
  models.push_back(pm_1_0_1);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(ins->new_type(), ch_1_0, cc_1_0, 1*prim_radius);

  //: now p_1_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_2_0 = new dbrec_composition(ins->new_type(), ch_2_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_2_0);
  
  return h;
}

//: manual hierarchies with rotationally invariant parts for sampling experiments (e.g. we will use these hierarchies first to sample instances from them, and second to detect those instances)
  
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_1()
{
  float prim_radius = 5.0f;
  dbrec_gaussian* gg1 = new dbrec_gaussian(1, 2.0f, 1.0f, 0.0f, true);
  dbrec_gaussian* gg2 = new dbrec_gaussian(2, 2.0f, 2.0f, 0.0f, true);

  vcl_vector<dbrec_part_sptr> ch; int d_min, d_max, alpha_min, alpha_max, alpha_range, rho_min, rho_max, rho_range;
  
  //: prepare three compositions with (gg1, gg1), (gg2, gg2), (gg1, gg2)
  int nbins_d = 5, nbins_rho = 9;
  //: first one
  ch.clear();
  ch.push_back(gg1); ch.push_back(gg1);   
  alpha_min = 40; alpha_max = 50;  alpha_range = 180;
  d_min = 6;      d_max = 8;
  rho_min = 80;   rho_max = 100;    rho_range = 180;  
  dbrec_pairwise_rot_invariant_model* prim1 = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, alpha_min, alpha_max, alpha_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp1 = new dbrec_pairwise_compositor(prim1);
  dbrec_composition* comp1 = new dbrec_composition(11, ch, paircomp1, 10.0f);
  comp1->set_class_prior(0.1f);

  //: second one
  ch.clear();
  ch.push_back(gg2); ch.push_back(gg2);   
  alpha_min = -1; alpha_max = -1;  alpha_range = 0; // alpha is irrelevant for this pair
  d_min = 8;      d_max = 10;
  rho_min = -1;   rho_max = -1;    rho_range = 0;  // rho is irrelevant for this pair
  dbrec_pairwise_rot_invariant_model* prim2 = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, alpha_min, alpha_max, alpha_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp2 = new dbrec_pairwise_compositor(prim2);
  dbrec_composition* comp2 = new dbrec_composition(12, ch, paircomp2, 10.0f);
  comp2->set_class_prior(0.1f);

  //: third one
  ch.clear();
  ch.push_back(gg1); ch.push_back(gg2);   
  alpha_min = -1; alpha_max = -1;  alpha_range = 0; // alpha is irrelevant for this pair
  d_min = 8;      d_max = 10;
  rho_min = 80;   rho_max = 100;    rho_range = 180;  
  dbrec_pairwise_rot_invariant_model* prim3 = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, alpha_min, alpha_max, alpha_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp3 = new dbrec_pairwise_compositor(prim3);
  dbrec_composition* comp3 = new dbrec_composition(13, ch, paircomp3, 10.0f);
  comp3->set_class_prior(0.1f);

  ch.clear();
  ch.push_back(comp1); ch.push_back(comp2); ch.push_back(comp3);
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_or = new dbrec_composition(20, ch, cc_or, 10.0f);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_or);

  return h;
}

// only primitives of the hierarchy # 1  
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_1_prims()
{
  float prim_radius = 5.0f;
  dbrec_gaussian* gg1 = new dbrec_gaussian(1, 2.0f, 1.0f, 0.0f, true);
  dbrec_gaussian* gg2 = new dbrec_gaussian(2, 2.0f, 2.0f, 0.0f, true);

  vcl_vector<dbrec_part_sptr> ch;
  ch.push_back(gg1); ch.push_back(gg2);
  
  //: simply prepare an OR node to have the primitives as children
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_or = new dbrec_composition(10, ch, cc_or, prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_or);
  
  return h;
}

dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0_level1()
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  // LAYER 1: one layer 1 part with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  int d_min, d_max, gamma_min, gamma_max, gamma_range, rho_min, rho_max, rho_range;
  int nbins_d = 3;
  int nbins_rho = 8;
  gamma_min = 0; gamma_max = 45;  gamma_range = 180; 
  d_min = 2;      d_max = 5;
  rho_min = 45;   rho_max = 135;    rho_range = 180;  
  dbrec_pairwise_rot_invariant_model* prim = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp = new dbrec_pairwise_compositor(prim);
  dbrec_composition* comp = new dbrec_composition(11, ch_1_0, paircomp, 1*prim_radius);
  comp->set_class_prior(0.1f);
  dbrec_part_sptr p_1_0 = comp;

  //: now p_2_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_3_0;
  ch_3_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_3_0 = new dbrec_composition(30, ch_3_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_3_0);
  
  return h;
}
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0()
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  //dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 1.5f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  // LAYER 1: one layer 1 part with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  int d_min, d_max, gamma_min, gamma_max, gamma_range, rho_min, rho_max, rho_range;
  int nbins_d = 3;
  int nbins_rho = 8;
  //int nbins_d = 30;
  //int nbins_rho = 80;
  //gamma_min = 135; gamma_max = 45;  gamma_range = 180; 
  //gamma_min = 157; gamma_max = 23;  gamma_range = 180; 
  gamma_min = 0; gamma_max = 179;  gamma_range = 180; 
  d_min = 2;      d_max = 5;
  //d_min = 0;      d_max = 5;
  //rho_min = 40;   rho_max = 140;    rho_range = 180;  
  //rho_min = 45;   rho_max = 135;    rho_range = 180;  
  rho_min = 0;   rho_max = 179;    rho_range = 180;  
  dbrec_pairwise_rot_invariant_model* prim = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp = new dbrec_pairwise_compositor(prim);
  dbrec_composition* comp = new dbrec_composition(11, ch_1_0, paircomp, 1*prim_radius);
  comp->set_class_prior(0.1f);
  dbrec_part_sptr p_1_0 = comp;

  //: LAYER 2: one layer 2 part with two child 
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); ch_2_0.push_back(p_1_0); 

  nbins_d = 6;
  nbins_rho = 16;
  gamma_min = 90; gamma_max = 270;  gamma_range = 360; 
  //gamma_min = 135; gamma_max = 225;  gamma_range = 360; 
  d_min = 1;      d_max = 3;
  rho_min = 45;   rho_max = 135;    rho_range = 360;  
  dbrec_pairwise_rot_invariant_model* prim2 = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp2 = new dbrec_pairwise_compositor(prim2);
  dbrec_composition* comp2 = new dbrec_composition(21, ch_2_0, paircomp2, 2*prim_radius);
  comp2->set_class_prior(0.1f);
  dbrec_part_sptr p_2_0 = comp2;

  //: now p_3_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_3_0;
  ch_3_0.push_back(p_2_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_3_0 = new dbrec_composition(30, ch_3_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_3_0);
  
  return h;
}

dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0_prims()
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  //dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 1.5f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  // LAYER 1: one layer 1 OR node with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_1_0 = new dbrec_composition(30, ch_1_0, cc_or, 1*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_1_0);
  
  return h;
}

dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_haifa_prims()
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 1.5f, 1.0f, -45.0f, true); // (lambda0=1.5,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 1.5f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.3f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  // LAYER 1: one layer 1 OR node with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_1_0 = new dbrec_composition(30, ch_1_0, cc_or, 1*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_1_0);
  
  return h;
}

dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_haifa_prims2()
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.5f, -45.0f, true); // (lambda0=2.0,lambda1=1.5,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 2.0f, 1.5f, -45.0f, false); // (lambda0=2.0,lambda1=1.5,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.3f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  
  // LAYER 1: one layer 1 OR node with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_1_0 = new dbrec_composition(30, ch_1_0, cc_or, 1*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_1_0);
  
  return h;
}


dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_2() // a new detector for both short and longer vehicles
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.0f, -45.0f, true); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  
  //dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 2.0f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 1.5f, 1.0f, -45.0f, false); // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;

  dbrec_gaussian* pg_0_2 = new dbrec_gaussian(3, 1.0f, 1.0f, 0.0f, true); // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  dbrec_part_sptr p_0_2 = pg_0_2;
  
  // LAYER 1: two layer 1 parts with two prims
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); 

  int d_min, d_max, gamma_min, gamma_max, gamma_range, rho_min, rho_max, rho_range;
  int nbins_d = 3;
  int nbins_rho = 8;
  gamma_min = 135; gamma_max = 45;  gamma_range = 180; 
  d_min = 2;      d_max = 5;
  rho_min = 45;   rho_max = 135;    rho_range = 180;  
  dbrec_pairwise_rot_invariant_model* prim = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp = new dbrec_pairwise_compositor(prim);
  dbrec_composition* comp = new dbrec_composition(11, ch_1_0, paircomp, 1*prim_radius);
  comp->set_class_prior(0.1f);
  dbrec_part_sptr p_1_0 = comp;

  vcl_vector<dbrec_part_sptr> ch_1_1;
  ch_1_1.push_back(p_0_0); ch_1_1.push_back(p_0_2); 

  gamma_min = -1; gamma_max = -1;  gamma_range = 0;  // gamma not defined
  d_min = 2;      d_max = 5;
  rho_min = 135;   rho_max = 45;    rho_range = 180;  
  prim = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  paircomp = new dbrec_pairwise_compositor(prim);
  comp = new dbrec_composition(12, ch_1_1, paircomp, 1*prim_radius);
  comp->set_class_prior(0.1f);
  dbrec_part_sptr p_1_1 = comp;


  //: LAYER 2: one layer 2 part with two child 
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); ch_2_0.push_back(p_1_1); 

  nbins_d = 6;
  nbins_rho = 16;
  gamma_min = 45; gamma_max = 135;  gamma_range = 360; 
  d_min = 2;      d_max = 5;
  rho_min = 0;   rho_max = 90;    rho_range = 360;  
  //rho_min = -1;   rho_max = -1;    rho_range = 0;  
  dbrec_pairwise_rot_invariant_model* prim2 = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, nbins_d, nbins_rho);
  dbrec_pairwise_compositor* paircomp2 = new dbrec_pairwise_compositor(prim2);
  dbrec_composition* comp2 = new dbrec_composition(21, ch_2_0, paircomp2, 2*prim_radius);
  comp2->set_class_prior(0.1f);
  dbrec_part_sptr p_2_0 = comp2;

  //: now p_3_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_3_0;
  ch_3_0.push_back(p_2_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_3_0 = new dbrec_composition(30, ch_3_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_3_0);
  
  return h;
}

// manual hierarchies for Haifa Dataset

//: a manual detector for haifa dataset, same as roi1_0 detector, the direction for the primitives is set to 0, the orientation is set during parsing
dbrec_hierarchy_sptr dbrec_image_hierarchy_factory::construct_detector_haifa(float orientation_angle)
{
  float prim_radius = 5.0f; 

  // LAYER 0: two primitives:
  dbrec_gaussian* pg_0_0 = new dbrec_gaussian(1, 2.0f, 1.0f, orientation_angle, true); // (lambda0=2.0,lambda1=1.0,theta=orientation_angle,bright=true)
  dbrec_part_sptr p_0_0 = pg_0_0;
  vnl_vector_fixed<float,2> dir_v_0_0;
  //pg_0_0->get_direction_vector(dir_v_0_0);

  dbrec_gaussian* pg_0_1 = new dbrec_gaussian(2, 1.5f, 1.0f, orientation_angle, false); // (lambda0=1.5,lambda1=1.0,theta=orientation_angle,bright=false)
  pg_0_1->set_cutoff_percentage(0.5f);
  dbrec_part_sptr p_0_1 = pg_0_1;
  vnl_vector_fixed<float,2> dir_v_0_1;
  pg_0_1->get_direction_vector(dir_v_0_1);

  // LAYER 1: only one layer 1 part
  vcl_vector<dbrec_part_sptr> ch_1_0;
  ch_1_0.push_back(p_0_0); ch_1_0.push_back(p_0_1); ch_1_0.push_back(p_0_1);

  //: the following samples are with respect to the direction vector of primitives with orientation angle -45 degree
  dbrec_gaussian* dummy = new dbrec_gaussian(10, 2.0f, 1.0f, -45, true);
  dummy->get_direction_vector(dir_v_0_0);
  vnl_vector_fixed<float,2> c1(500.0f,500.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(600.0f,600.0f); // center measured from the image
  
  vnl_vector_fixed<float,2> p1c1(501.0f,503.0f); //
  vnl_vector_fixed<float,2> p1c2(602.0f,602.0f); //
  
  vnl_vector_fixed<float,2> p2c1(498.0f,499.0f); //
  vnl_vector_fixed<float,2> p2c2(597.0f,598.0f); //
  
  dbrec_pairwise_indep_gaussian_model* pigm_1_0_0 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_0 = pigm_1_0_0;
  pigm_1_0_0->update_models(dir_v_0_0, c1, p1c1);
  pigm_1_0_0->update_models(dir_v_0_0, c2, p1c2);
  
  vcl_vector<dbrec_pairwise_model_sptr> models; 
  models.push_back(pm_1_0_0);

  dbrec_pairwise_indep_gaussian_model* pigm_1_0_1 = new dbrec_pairwise_indep_gaussian_model(1.0f, 10.0f);
  dbrec_pairwise_model_sptr pm_1_0_1 = pigm_1_0_1;
  pigm_1_0_1->update_models(dir_v_0_0, c1, p2c1);
  pigm_1_0_1->update_models(dir_v_0_0, c2, p2c2);
  
  models.push_back(pm_1_0_1);

  dbrec_compositor_sptr cc_1_0 = new dbrec_central_compositor(models);
  dbrec_part_sptr p_1_0 = new dbrec_composition(3, ch_1_0, cc_1_0, 1*prim_radius);

  //: now p_1_0 is the OR'd node under the class vehicle
  vcl_vector<dbrec_part_sptr> ch_2_0;
  ch_2_0.push_back(p_1_0); 
  
  dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
  dbrec_part_sptr p_2_0 = new dbrec_composition(4, ch_2_0, cc_or, 2*prim_radius);

  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  h->add_root(p_2_0);
  
  return h;
}

