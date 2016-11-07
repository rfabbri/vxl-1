// This is brcv/rec/dbru/pro/dbru_compute_rcor_process.cxx

//:
// \file

#include "dbru_compute_rcor_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_observation_sptr.h>

#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>

#include <dbru/algo/dbru_object_matcher.h>

#define MARGIN  (10)

dbru_compute_rcor_process::dbru_compute_rcor_process()
{  
  if (
    !parameters()->add( "Shock matching: ", "-shock", true) ||
    !parameters()->add( "shock sampling ds: " , "-ds" , 1.0f ) ||
    !parameters()->add( "Load shock graph 1 from esf file?" , "-load1" , (bool) true ) ||
    !parameters()->add( "Input esf file 1 <filename...>" , 
    "-esf1" ,
    bpro1_filepath("D:\\projects\\vehicles\\esfs_256_origs\\pruning_threshold_0.0\\Acura_NSX_92_055-135-068-030.esf","*.esf") 
    ) ||
    !parameters()->add( "Load shock graph 2 from esf file?" , "-load2" , (bool) true ) ||
    !parameters()->add( "Input esf file 2 <filename...>" , 
    "-esf2" ,
    bpro1_filepath("D:\\projects\\vehicles\\esfs_256_origs\\pruning_threshold_0.0\\Honda_Prelude_92_055-135-068-030.esf","*.esf") 
    ) ||
    !parameters()->add( "Match file exists?" , "-fileexists" , (bool) true ) ||
    !parameters()->add( "Input/Output shgm file <filename...>" , 
    "-shgm" ,
    bpro1_filepath("","*.shgm") 
    ) ||
    !parameters()->add( "prune threshold: " , "-prune" , 12.0f ) ||
    !parameters()->add( "Use silhouette correspondence of existing region correspondence?:" , "-silcor" , false)  ||
    !parameters()->add( "Use line intersections for region correspondence? (if not shock matching):" , "-line" , false)  ||
    //!parameters()->add( "Use line intersections version 2 for region correspondence?:" , "-line2" , false)  ||
    //!parameters()->add( "Use line intersections version 3 for region correspondence?:" , "-line3" , false)  ||
    !parameters()->add( "Use line intersections version 4 for region correspondence?:" , "-line4" , false)  ||
    !parameters()->add( "save region1 histograms? (if line intersections):" , "-save" , true )  ||
    !parameters()->add( "Use distance transform region correspondence? (if not shock matching and line intersections):" , "-disttrans" , false )  ||
    !parameters()->add( "Use distance transform region correspondence version 2?:" , "-disttrans2" , false )  ||
    //!parameters()->add( "restricted_cvmatch_ratio: " , "-rcmr" , 0.25f ) ||
    !parameters()->add( "rms for line fitting before matching curves: " , "-rms" , 0.05f ) ||
    //!parameters()->add( "R: " , "-R" , 10.0f ) ||
    //!parameters()->add( "Even grid curve matching): " , "-even" , false) ||
    //!parameters()->add( "If even grid matching, geno interpolation (otherwise linear): " , "-linear" , false) ||
    !parameters()->add( "increment: " , "-increment" , 20.0f) 
    //!parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) ||
    ) {
      vcl_cerr << "ERROR: Adding parameters in dbru_compute_rcor_process::dbru_compute_rcor_process()" << vcl_endl;
    }
}


//: Clone the process
bpro1_process*
dbru_compute_rcor_process::clone() const
{
  return new dbru_compute_rcor_process(*this);
}

void dbru_compute_rcor_process::get_parameters()
{
  //pick algorithm
  parameters()->get_value( "-shock" , shock_matching );
  parameters()->get_value( "-prune", prune_threshold );
  parameters()->get_value( "-line" , line_intersections );
  line_intersections2 = false;
  line_intersections3 = false;
  //parameters()->get_value( "-line2" , line_intersections2 );
  //parameters()->get_value( "-line3" , line_intersections3 );
  parameters()->get_value( "-line4" , line_intersections4 );
  parameters()->get_value( "-save" , save_histograms );
  parameters()->get_value( "-silcor" , use_sil_cor );
  parameters()->get_value( "-disttrans" , distance_transform );
  parameters()->get_value( "-disttrans2" , dt2 );
  
  //curve matching parameters
  parameters()->get_value( "-rms" , rms );
  //parameters()->get_value( "-rcmr" , restricted_cvmatch_ratio );
  restricted_cvmatch_ratio = 0.25f;
  //parameters()->get_value( "-R" , R );
  R = 10.0f;
  //parameters()->get_value( "-even" , even_matching );
  //parameters()->get_value( "-linear" , geno_interpolation );

  // if line intersections version 2 is to be used this increment will be the ratio
  parameters()->get_value("-increment", increment);

  //shock matching parameters
  parameters()->get_value( "-load1" , load1);
  parameters()->get_value( "-load2" , load2);
  parameters()->get_value( "-fileexists" , use_shgm);//use_shgm = false;
  
  bpro1_filepath input_path;
  parameters()->get_value( "-esf1" , input_path);
  esf_file1 = input_path.path;
  parameters()->get_value( "-esf2" , input_path);
  esf_file2 = input_path.path;  
  if (use_shgm){
    parameters()->get_value( "-shgm" , input_path);
    shgm_file = input_path.path;
  }
  parameters()->get_value("-ds", scurve_sample_ds);  
  
}

bool dbru_compute_rcor_process::execute()
{
  
  //first get all the parameters
  this->get_parameters();

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[0][3]);

  // The contour needs to be a polygon
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  vsol_polygon_2d_sptr poly1 = vsol_list[0]->cast_to_region()->cast_to_polygon();

  //the second polygon
  vsol_list = input_vsol2->all_data();
  vsol_polygon_2d_sptr poly2 = vsol_list[0]->cast_to_region()->cast_to_polygon();

  if (!poly1 || !poly2) {
    vcl_cout << "one of the polygons is not valid.\n";
    return false;
  }
  
  //----------------------------------
  // get input images
  //----------------------------------
  vidpro1_image_storage_sptr frame_image1;
  frame_image1.vertical_cast(input_data_[0][2]);
  vil_image_resource_sptr image1 = frame_image1->get_image();

  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][5]);
  vil_image_resource_sptr image2 = frame_image2->get_image();

  if (!image1 || !image2) {
    vcl_cout << "one of the images is missing.\n";
    return false;
  }

  //----------------------------------
  // get shock graphs
  //----------------------------------
  if (shock_matching)
  {
    dbsk2d_xshock_graph_fileio loader;
    dbsk2d_shock_storage_sptr shock1, shock2;
    dbsk2d_shock_graph_sptr sg1, sg2;

    if (load1) {
      sg1 = loader.load_xshock_graph(esf_file1);
    } else {
      // get input storage class
      shock1.vertical_cast(input_data_[0][1]);
      sg1 = shock1->get_shock_graph();
    }

    if (load2) {
      sg2 = loader.load_xshock_graph(esf_file2);
    } else {
      shock2.vertical_cast(input_data_[0][4]);
      sg2 = shock2->get_shock_graph();
    }

    if (!sg1 || !sg2)
    {
      vcl_cout << "Problems in getting shock graphs!\n";
      return false;
    }

    tree1 = new dbskr_tree(scurve_sample_ds);
    //tree1->acquire(sg1);
    tree1->acquire_and_prune(sg1, prune_threshold, true, true, false);  // construct_circular_ends is true since curves being matched are closed in this application
                                                                        // dpmatch_combined is false assuming original edit distance alg of Sebastian et al. PAMI06 is being used

    tree2 = new dbskr_tree(scurve_sample_ds);
    //tree2->acquire(sg2);
    tree2->acquire_and_prune(sg2, prune_threshold, true, true, false);  // construct_circular_ends is true since curves being matched are closed in this application
                                                                        // dpmatch_combined is false assuming original edit distance alg of Sebastian et al. PAMI06 is being used
  }

  //----------------------------------
  // create the observations
  //----------------------------------
  dbinfo_observation_sptr obs1 = new dbinfo_observation(0, image1, poly1, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, image2, poly2, true, true, false);

  //----------------------------------
  // compute re-mapped images
  //----------------------------------

  dbru_rcor_sptr output_rcor;
  vul_timer t;
  dbcvr_cv_cor_sptr sil_cor;
  if (use_sil_cor) {
    dbru_rcor_storage_sptr rcor_storage;
    rcor_storage.vertical_cast(input_data_[0][6]);
    sil_cor = rcor_storage->get_rcor()->get_sil_cor();
  }

  if (shock_matching) {
    if (use_shgm) {
      dbskr_sm_cor_sptr sm = new dbskr_sm_cor(tree1, tree2);
      sm->read_and_construct_from_shgm(shgm_file);
      output_rcor = dbru_object_matcher::generate_rcor_shock_matching(obs1, obs2, sm, true);
    } 
    else {
      output_rcor = dbru_object_matcher::generate_rcor_shock_matching(obs1, obs2, tree1, tree2, true);  // verbose
    }
  } 
  else if (distance_transform) {  
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs1, obs2, sil_cor, true); 
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs1, obs2, R, rms, restricted_cvmatch_ratio, true); 
  }
  else if (dt2) {  
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt2(obs1, obs2, sil_cor, increment, true); 
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt2(obs1, obs2, R, rms, restricted_cvmatch_ratio, increment, true); 
  }
  else if (line_intersections) {
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs1, obs2,sil_cor, int(increment), save_histograms, true);  // verbose
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs1, obs2,R, rms, restricted_cvmatch_ratio, int(increment), save_histograms, true);  // verbose
  } 
  else if (line_intersections2) {
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line2(obs1, obs2,sil_cor, increment, save_histograms, true);  // verbose
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line2(obs1, obs2,R, rms, restricted_cvmatch_ratio, increment, save_histograms, true);  // verbose
  }
  else if (line_intersections3) {
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line3(obs1, obs2,sil_cor, int(increment), save_histograms, true);  // verbose
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line3(obs1, obs2,R, rms, restricted_cvmatch_ratio, int(increment), save_histograms, true);  // verbose
  }
  else if (line_intersections4) {
    if (use_sil_cor)
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line4(obs1, obs2,sil_cor, int(increment), save_histograms, true);  // verbose
    else
      output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line4(obs1, obs2,R, rms, restricted_cvmatch_ratio, int(increment), save_histograms, true);  // verbose
  }

  double info = dbinfo_observation_matcher::minfo(obs1, obs2, output_rcor->get_correspondences(), false);
  vcl_cout<< " time: "<< (t.real()/1000.0f) << " seconds info: " << info << vcl_endl;

  //----------------------------------
  // create the output storage class
  //----------------------------------
  clear_output();
  dbru_rcor_storage_sptr output_storage = dbru_rcor_storage_new();
  output_storage->set_rcor(output_rcor);
  output_data_[0].push_back(output_storage);

  vil_image_resource_sptr output_sptr = output_rcor->get_appearance2_on_pixels1();
  vil_image_resource_sptr output_sptr2 = output_rcor->get_used_pixels1();
  vil_image_resource_sptr output_sptr3 = output_rcor->get_used_pixels2();

  //----------------------------------
  // create image output storage class
  //----------------------------------
  vidpro1_image_storage_sptr output_storage1 = vidpro1_image_storage_new();
  output_storage1->set_image(output_sptr);
  output_data_[0].push_back(output_storage1);

  vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
  output_storage2->set_image(output_sptr2);
  output_data_[0].push_back(output_storage2);

  vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
  output_storage3->set_image(output_sptr3);
  output_data_[0].push_back(output_storage3);
  
  return true;
}

