//:
// \file
// \author Ozge C Ozcanli (ozge at lems dot brown dot edu)
// \date 04/07/09

#include "dbrec_part_hierarchy_builder.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vil/vil_image_resource.h>

#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_base.h>
#include <brec/brec_part_gaussian.h>

brec_part_hierarchy_sptr dbrec_part_hierarchy_builder::construct_mi_detector()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("mi_detector");

  //: LAYER 0: two primitive:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=2.0,lambda1=2.0,theta=0,bright=false)
  h->add_vertex(p_0_0);
  
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=5.0,lambda1=1.0,theta=0,bright=false)
  h->add_vertex(p_0_1);
  
  //: create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, false, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 5.0f, 1.0f, 0.0f, false, 1);
  
  pi_0_0->detection_threshold_ = 0.6f;
  pi_0_1->detection_threshold_ = 0.6f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.1f;
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_1->add_incoming_edge(e_1_0_to_second);

  vnl_vector_fixed<float,2> c_0_0(59.0f,41.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_1_0(54.0f,15.0f); // center measured from the image

  // train this edge
  vnl_vector_fixed<float,2> sample1 = c_0_1_0 - c_0_0; // center difference measured from the image
  
  c_0_0 = vnl_vector_fixed<float,2>(274.0f,41.0f); // center measured from the image
  c_0_1_0 = vnl_vector_fixed<float,2>(259.0f,15.0f); // center measured from the image
  vnl_vector_fixed<float,2> sample2 = c_0_1_0 - c_0_0; // center difference measured from the image

  // calculate angle and dists
  float a1, d1;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->set_min_stand_dev_dist(2.0f);
  e_1_0_to_second->set_min_stand_dev_angle(5.0f);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);
  
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d1, a1);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);

  h->add_edge_no_check(e_1_0_to_second);

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_1 = new brec_part_base(1, 1);
  h->add_vertex(p_1_1);
  p_1_1->detection_threshold_ = 0.5f;
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_1_to_central = new brec_hierarchy_edge(p_1_1, p_0_0, true);
  p_1_1->add_outgoing_edge(e_1_1_to_central);
  p_0_0->add_incoming_edge(e_1_1_to_central);
  h->add_edge_no_check(e_1_1_to_central);

  // create an edge to the second part of p_1_1
  brec_hierarchy_edge_sptr e_1_1_to_second = new brec_hierarchy_edge(p_1_1, p_0_1, false);
  p_1_1->add_outgoing_edge(e_1_1_to_second);
  p_0_1->add_incoming_edge(e_1_1_to_second);

  c_0_0 = vnl_vector_fixed<float,2>(59.0f,41.0f); // center measured from the image
  c_0_1_0 = vnl_vector_fixed<float,2>(54.0f,15.0f); // center measured from the image

  // train this edge
  sample1 = c_0_1_0 - c_0_0; // center difference measured from the image
  
  // calculate angle and dists
  e_1_1_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_1_to_second->set_min_stand_dev_dist(2.0f);
  e_1_1_to_second->set_min_stand_dev_angle(5.0f);
  e_1_1_to_second->update_dist_model(d1);
  e_1_1_to_second->update_angle_model(a1);
  
  h->add_edge_no_check(e_1_1_to_second);
  
  return h;

}

//: for steeple street vehicles
brec_part_hierarchy_sptr dbrec_part_hierarchy_builder::construct_detector_steeple0()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("steeple_vehicle0");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=2.0,lambda1=2.5,theta=20,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=3.0,lambda1=1.0,theta=20,bright=false)
  h->add_vertex(p_0_1);

  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 2.5f, 20.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 20.0f, false, 1);
  //pi_0_1->cutoff_percentage_ = 0.5f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_to_c = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_to_c);
  p_0_0->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_to_second = new brec_hierarchy_edge(p_1_0, p_0_0, false);
  p_1_0->add_outgoing_edge(e_to_second);
  p_0_0->add_incoming_edge(e_to_second);

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

  // train this edge
  // subtract central part's center from the second part's center
  vnl_vector_fixed<float,2> sample1 = e1p2c - e1c; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = e2p2c - e2c; // 
  vnl_vector_fixed<float,2> sample3 = e3p2c - e3c; //
  vnl_vector_fixed<float,2> sample4 = e4p2c - e4c; //
  vnl_vector_fixed<float,2> sample5 = e5p2c - e5c; //
  
  e_to_second->set_min_stand_dev_dist(1.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  float a, d;
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 1: second part
  brec_part_base_sptr p_1_1 = new brec_part_base(1, 1);
  h->add_vertex(p_1_1);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_1_1, p_0_0, true);
  p_1_1->add_outgoing_edge(e_to_c);
  p_0_0->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_1
  e_to_second = new brec_hierarchy_edge(p_1_1, p_0_1, false);
  p_1_1->add_outgoing_edge(e_to_second);
  p_0_1->add_incoming_edge(e_to_second);

  //: part2 of the previous edge is the center for this edge
  vnl_vector_fixed<float,2> e1p3c(995.0f,451.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2p3c(935.0f,407.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3p3c(882.0f,454.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4p3c(805.0f,613.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5p3c(899.0f,602.0f); // second part center measured from the image for the 5th exampler

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1p3c - e1p2c; // center difference measured from the image
  sample2 = e2p3c - e2p2c; // 
  sample3 = e3p3c - e3p2c; //
  sample4 = e4p3c - e4p2c; //
  sample5 = e5p3c - e5p2c; //
  
  e_to_second->set_min_stand_dev_dist(3.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(15.0f); // 10 degrees

  // calculate angle and dist
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 2: one part
  brec_part_base_sptr p_2_0 = new brec_part_base(2, 0);
  h->add_vertex(p_2_0);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_2_0, p_1_0, true);
  p_2_0->add_outgoing_edge(e_to_c);
  p_1_0->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_2_0
  e_to_second = new brec_hierarchy_edge(p_2_0, p_1_1, false);
  p_2_0->add_outgoing_edge(e_to_second);
  p_1_1->add_incoming_edge(e_to_second);

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1p2c - e1c; // center difference measured from the image
  sample2 = e2p2c - e2c; // 
  sample3 = e3p2c - e3c; //
  sample4 = e4p2c - e4c; //
  sample5 = e5p2c - e5c; //
  
  e_to_second->set_min_stand_dev_dist(1.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);


  //: create second layer2 node for the dark vehicles



  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);  // (lambda0=6.0,lambda1=3.0,theta=20,bright=false)
  h->add_vertex(p_0_2);
  brec_part_base_sptr p_0_3 = new brec_part_base(0, 3);  // (lambda0=2.0,lambda1=2.0,theta=0,bright=false)
  h->add_vertex(p_0_3);
  brec_part_base_sptr p_0_4 = new brec_part_base(0, 4);  // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  h->add_vertex(p_0_4);

  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 6.0f, 3.0f, 20.0f, false, 2);
  brec_part_gaussian_sptr pi_0_3 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, false, 3);
  brec_part_gaussian_sptr pi_0_4 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, true, 4);
  //pi_0_2->cutoff_percentage_ = 0.3f;
  //pi_0_3->cutoff_percentage_ = 0.5f;
  //pi_0_4->cutoff_percentage_ = 0.5f;
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_3->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_4->cast_to_instance());

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_2 = new brec_part_base(1, 2);
  h->add_vertex(p_1_2);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_1_2, p_0_2, true);
  p_1_2->add_outgoing_edge(e_to_c);
  p_0_2->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_2
  e_to_second = new brec_hierarchy_edge(p_1_2, p_0_3, false);
  p_1_2->add_outgoing_edge(e_to_second);
  p_0_3->add_incoming_edge(e_to_second);

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

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1_0_3 - e1_0_2; // center difference measured from the image
  sample2 = e2_0_3 - e2_0_2; // 
  sample3 = e3_0_3 - e3_0_2; //
  sample4 = e4_0_3 - e4_0_2; //
  sample5 = e5_0_3 - e5_0_2; //
  
  e_to_second->set_min_stand_dev_dist(1.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 1: second part
  brec_part_base_sptr p_1_3 = new brec_part_base(1, 3);
  h->add_vertex(p_1_3);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_1_3, p_0_3, true);
  p_1_3->add_outgoing_edge(e_to_c);
  p_0_3->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_3
  e_to_second = new brec_hierarchy_edge(p_1_3, p_0_4, false);
  p_1_3->add_outgoing_edge(e_to_second);
  p_0_4->add_incoming_edge(e_to_second);

  //: part2 of the previous edge is the center for this edge
  vnl_vector_fixed<float,2> e1_0_4(977.0f,460.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2_0_4(969.0f,470.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3_0_4(950.0f,492.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4_0_4(899.0f,448.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5_0_4(903.0f,556.0f); // second part center measured from the image for the 5th exampler

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1_0_4 - e1_0_3; // center difference measured from the image
  sample2 = e2_0_4 - e2_0_3; // 
  sample3 = e3_0_4 - e3_0_3; //
  sample4 = e4_0_4 - e4_0_3; //
  sample5 = e5_0_4 - e5_0_3; //
  
  e_to_second->set_min_stand_dev_dist(5.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(20.0f); // 10 degrees

  // calculate angle and dist
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 2: one part
  brec_part_base_sptr p_2_1 = new brec_part_base(2, 1);
  h->add_vertex(p_2_1);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_2_1, p_1_2, true);
  p_2_1->add_outgoing_edge(e_to_c);
  p_1_2->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_2_0
  e_to_second = new brec_hierarchy_edge(p_2_1, p_1_3, false);
  p_2_1->add_outgoing_edge(e_to_second);
  p_1_3->add_incoming_edge(e_to_second);

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1_0_3 - e1_0_2; // center difference measured from the image
  sample2 = e2_0_3 - e2_0_2; // 
  sample3 = e3_0_3 - e3_0_2; //
  sample4 = e4_0_3 - e4_0_2; //
  sample5 = e5_0_3 - e5_0_2; //
  
  e_to_second->set_min_stand_dev_dist(2.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(15.0f); // 10 degrees

  // calculate angle and dists
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);



  return h;
}
  
//: for steeple street vehicles, dark vehicles
brec_part_hierarchy_sptr dbrec_part_hierarchy_builder::construct_detector_steeple1()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("steeple_vehicle1");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);  // (lambda0=6.0,lambda1=3.0,theta=20,bright=false)
  h->add_vertex(p_0_2);
  brec_part_base_sptr p_0_3 = new brec_part_base(0, 3);  // (lambda0=2.0,lambda1=2.0,theta=0,bright=false)
  h->add_vertex(p_0_3);
  brec_part_base_sptr p_0_4 = new brec_part_base(0, 4);  // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  h->add_vertex(p_0_4);

  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 6.0f, 3.0f, 20.0f, false, 2);
  brec_part_gaussian_sptr pi_0_3 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, false, 3);
  brec_part_gaussian_sptr pi_0_4 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, true, 4);
  //pi_0_3->cutoff_percentage_ = 0.5f;
  //pi_0_4->cutoff_percentage_ = 0.5f;
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_3->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_4->cast_to_instance());

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_2 = new brec_part_base(1, 2);
  h->add_vertex(p_1_2);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_to_c = new brec_hierarchy_edge(p_1_2, p_0_2, true);
  p_1_2->add_outgoing_edge(e_to_c);
  p_0_2->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_2
  brec_hierarchy_edge_sptr e_to_second = new brec_hierarchy_edge(p_1_2, p_0_3, false);
  p_1_2->add_outgoing_edge(e_to_second);
  p_0_3->add_incoming_edge(e_to_second);

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

  // train this edge
  // subtract central part's center from the second part's center
  vnl_vector_fixed<float,2> sample1 = e1_0_3 - e1_0_2; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = e2_0_3 - e2_0_2; // 
  vnl_vector_fixed<float,2> sample3 = e3_0_3 - e3_0_2; //
  vnl_vector_fixed<float,2> sample4 = e4_0_3 - e4_0_2; //
  vnl_vector_fixed<float,2> sample5 = e5_0_3 - e5_0_2; //
  
  e_to_second->set_min_stand_dev_dist(1.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  float d, a;
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 1: second part
  brec_part_base_sptr p_1_3 = new brec_part_base(1, 3);
  h->add_vertex(p_1_3);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_1_3, p_0_3, true);
  p_1_3->add_outgoing_edge(e_to_c);
  p_0_3->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_3
  e_to_second = new brec_hierarchy_edge(p_1_3, p_0_4, false);
  p_1_3->add_outgoing_edge(e_to_second);
  p_0_4->add_incoming_edge(e_to_second);

  //: part2 of the previous edge is the center for this edge
  vnl_vector_fixed<float,2> e1_0_4(977.0f,460.0f); // second part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> e2_0_4(969.0f,470.0f); // second part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> e3_0_4(950.0f,492.0f); // second part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> e4_0_4(899.0f,448.0f); // second part center measured from the image for the 4th exampler
  vnl_vector_fixed<float,2> e5_0_4(903.0f,556.0f); // second part center measured from the image for the 5th exampler

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1_0_4 - e1_0_3; // center difference measured from the image
  sample2 = e2_0_4 - e2_0_3; // 
  sample3 = e3_0_4 - e3_0_3; //
  sample4 = e4_0_4 - e4_0_3; //
  sample5 = e5_0_4 - e5_0_3; //
  
  e_to_second->set_min_stand_dev_dist(5.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(20.0f); // 10 degrees

  // calculate angle and dist
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_3->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);

  // LAYER 2: one part
  brec_part_base_sptr p_2_1 = new brec_part_base(2, 1);
  h->add_vertex(p_2_1);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_2_1, p_1_2, true);
  p_2_1->add_outgoing_edge(e_to_c);
  p_1_2->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_2_0
  e_to_second = new brec_hierarchy_edge(p_2_1, p_1_3, false);
  p_2_1->add_outgoing_edge(e_to_second);
  p_1_3->add_incoming_edge(e_to_second);

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = e1_0_3 - e1_0_2; // center difference measured from the image
  sample2 = e2_0_3 - e2_0_2; // 
  sample3 = e3_0_3 - e3_0_2; //
  sample4 = e4_0_3 - e4_0_2; //
  sample5 = e5_0_3 - e5_0_2; //
  
  e_to_second->set_min_stand_dev_dist(2.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(15.0f); // 10 degrees

  // calculate angle and dists
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample2, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample3, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample4, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  //e_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample5, d, a);
  //e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);

  h->add_edge_no_check(e_to_second);



  return h;
}

/***************************************
//  Various detectors for balloon videos, v2, lane 1, 
//  see: vision/docs/mundy/voxel/ozge/v2_analysis.pptx, 
//  video id: 10080, video dir: vision\projects\kimia\categorization\vehicles\camera_ready_expts\v\v2-data\road429-f50-f229\430-267-380 
***************************************/
brec_part_hierarchy_sptr dbrec_part_hierarchy_builder::construct_detector_10080_car() // for both bright and dark vehicles on lane1
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("10080_car");

  // LAYER 0: three primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=1.5,lambda1=1.5,theta=0,bright=false)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=4.0,lambda1=2.0,theta=0,bright=true)
  h->add_vertex(p_0_1);
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);  // (lambda0=5.0,lambda1=2.0,theta=0,bright=false)
  h->add_vertex(p_0_2);

  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.5f, 1.5f, 0.0f, false, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 4.0f, 2.0f, 0.0f, true, 1);
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 5.0f, 2.0f, 0.0f, false, 2);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());

  // LAYER 1: two layer 1 parts
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_to_c = new brec_hierarchy_edge(p_1_0, p_0_1, true);
  p_1_0->add_outgoing_edge(e_to_c);
  p_0_1->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_to_second = new brec_hierarchy_edge(p_1_0, p_0_0, false);
  p_1_0->add_outgoing_edge(e_to_second);
  p_0_0->add_incoming_edge(e_to_second);

  vnl_vector_fixed<float,2> p1(93.0f,298.0f); // central part center measured from the image for the 1st exampler
  vnl_vector_fixed<float,2> p2(114.0f,296.0f); // central part center measured from the image for the 2nd exampler
  vnl_vector_fixed<float,2> p3(115.0f,305.0f); // central part center measured from the image for the 3rd exampler
  vnl_vector_fixed<float,2> p4(106.0f,305.0f); // central part center measured from the image for the 4th exampler
  
  // train this edge
  // subtract central part's center from the second part's center
  vnl_vector_fixed<float,2> sample1 = p2 - p1; // center difference measured from the image

  e_to_second->set_min_stand_dev_dist(2.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  float a, d;
  e_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  h->add_edge_no_check(e_to_second);

  // LAYER 1: second part
  brec_part_base_sptr p_1_1 = new brec_part_base(1, 1);
  h->add_vertex(p_1_1);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_1_1, p_0_0, true);
  p_1_1->add_outgoing_edge(e_to_c);
  p_0_0->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_1_1
  e_to_second = new brec_hierarchy_edge(p_1_1, p_0_0, false);
  p_1_1->add_outgoing_edge(e_to_second);
  p_0_0->add_incoming_edge(e_to_second);

  // train this edge
  sample1 = p4 - p3; // center difference measured from the image

  e_to_second->set_min_stand_dev_dist(2.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(15.0f); // 10 degrees

  // calculate angle and dist
  e_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  h->add_edge_no_check(e_to_second);

  // LAYER 2: one part
  brec_part_base_sptr p_2_0 = new brec_part_base(2, 0);
  h->add_vertex(p_2_0);
  // the first child becomes the central part, create an edge to the central part
  e_to_c = new brec_hierarchy_edge(p_2_0, p_1_0, true);
  p_2_0->add_outgoing_edge(e_to_c);
  p_1_0->add_incoming_edge(e_to_c);
  h->add_edge_no_check(e_to_c);

  // create an edge to the second part of p_2_0
  e_to_second = new brec_hierarchy_edge(p_2_0, p_1_1, false);
  p_2_0->add_outgoing_edge(e_to_second);
  p_1_1->add_incoming_edge(e_to_second);

  // train this edge
  // subtract central part's center from the second part's center
  sample1 = p3 - p1; // center difference measured from the image
  
  e_to_second->set_min_stand_dev_dist(2.0f); // 1 pixels
  e_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees

  // calculate angle and dists
  e_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d, a);
  e_to_second->update_dist_model(d); e_to_second->update_angle_model(a);
  h->add_edge_no_check(e_to_second);

  return h;
}



  
