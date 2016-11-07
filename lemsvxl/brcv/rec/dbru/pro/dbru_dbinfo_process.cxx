// This is brcv/rec/dbru/pro/dbru_dbinfo_process.cxx

//:
// \file

#include "dbru_dbinfo_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbru/dbru_rcor.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree_edit.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_feature_base_sptr.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>

#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbru/pro/dbru_rcor_storage.h>

#define MARGIN  (10)

dbru_dbinfo_process::dbru_dbinfo_process()
{  
  if (
    !parameters()->add( "Shock matching: ", "-shock", false) ||
    !parameters()->add( "shock sampling ds: " , "-ds" , 1.0f ) ||
    !parameters()->add( "Load shock graph 1 from esf file?" , "-load1" , (bool) true ) ||
    !parameters()->add( "Input esf file 1 <filename...>" , 
    "-esf1" ,
    bpro1_filepath("","*.esf") 
    ) ||
    !parameters()->add( "Load shock graph 2 from esf file?" , "-load2" , (bool) true ) ||
    !parameters()->add( "Input esf file 2 <filename...>" , 
    "-esf2" ,
    bpro1_filepath("","*.esf") 
    ) ||
    !parameters()->add( "Match file exists?" , "-fileexists" , (bool) true ) ||
    !parameters()->add( "Input/Output shgm file <filename...>" , 
    "-shgm" ,
    bpro1_filepath("","*.shgm") 
    ) ||
    !parameters()->add( "If using shock matching for region correspondence, elastic splice cost? :" , "-elastic" , true )  ||
    !parameters()->add( "Use line intersections for region correspondence? (if not shock matching):" , "-line" , true )  ||
    !parameters()->add( "Use distance transform region correspondence? (if not shock matching):" , "-disttrans" , false )  ||
    !parameters()->add( "restricted_cvmatch_ratio: " , "-rcmr" , 0.25f ) ||
    !parameters()->add( "rms for line fitting before matching curves: " , "-rms" , 0.05f ) ||
    !parameters()->add( "R: " , "-R" , 10.0f ) ||
    //!parameters()->add( "Even grid curve matching): " , "-even" , false) ||
    //!parameters()->add( "If even grid matching, geno interpolation (otherwise linear): " , "-linear" , false) ||
    !parameters()->add( "increment: " , "-increment" , 20) 
    //!parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) ||
    ) {
      vcl_cerr << "ERROR: Adding parameters in dbru_dbinfo_process::dbru_dbinfo_process()" << vcl_endl;
    }

    total_info_ = 0;
    int_mutual_info_ = 0;
    grad_mutual_info_ = 0;
    color_mutual_info_ = 0;
}


//: Clone the process
bpro1_process*
dbru_dbinfo_process::clone() const
{
  return new dbru_dbinfo_process(*this);
}

void dbru_dbinfo_process::get_parameters()
{
  //pick algorithm
  parameters()->get_value( "-shock" , shock_matching );
  parameters()->get_value( "-line" , line_intersections );
  parameters()->get_value( "-disttrans" , distance_transform );
  
  //curve matching parameters
  parameters()->get_value( "-rms" , rms );
  parameters()->get_value( "-rcmr" , restricted_cvmatch_ratio );
  parameters()->get_value( "-R" , R );
  //parameters()->get_value( "-even" , even_matching );
  //parameters()->get_value( "-linear" , geno_interpolation );
  parameters()->get_value("-increment", increment);

  //shock matching parameters
  parameters()->get_value( "-load1" , load1);
  parameters()->get_value( "-load2" , load2);
  bpro1_filepath input_path;
  parameters()->get_value( "-esf1" , input_path);
  esf_file1 = input_path.path;
  parameters()->get_value( "-esf2" , input_path);
  esf_file2 = input_path.path;  
  parameters()->get_value( "-shgm" , input_path);
  shgm_file = input_path.path;
  parameters()->get_value("-ds", scurve_sample_ds);  

  parameters()->get_value( "-elastic" , elastic_splice_cost );
  
}

bool dbru_dbinfo_process::execute()
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
    tree1->acquire(sg1, elastic_splice_cost, true, false);  // construct_circular_ends is true since matching closed curves in this application
                                                            // dpmatch_combined is false using interval cost computations as given in original edit distance algo of Sebastian et al PAMI 06

    tree2 = new dbskr_tree(scurve_sample_ds);
    tree2->acquire(sg2, elastic_splice_cost, true, false);  // construct_circular_ends is true since matching closed curves in this application
                                                            // dpmatch_combined is false using interval cost computations as given in original edit distance algo of Sebastian et al PAMI 06
  }

  //----------------------------------
  // create the observations
  //----------------------------------
  dbinfo_observation_sptr obs1 = new dbinfo_observation(0, image1, poly1, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, image2, poly2, true, true, false);

  //----------------------------------
  // compute re-mapped images
  //----------------------------------

  vil_image_resource_sptr output_image;

  double matching_cost, info;
  if (shock_matching) {
    output_image = dbru_object_matcher::minfo_shock_matching(obs1, obs2, tree1, tree2,
                     matching_cost,info, true);  // verbose
  } 
  else if (distance_transform) {    
    output_image = dbru_object_matcher::minfo_curve_matching_dt(obs1, obs2,
                     matching_cost, info, R, rms, restricted_cvmatch_ratio, true);  // verbose
  } 
  else if (line_intersections) {
    output_image = dbru_object_matcher::minfo_curve_matching_line(obs1, obs2,
                     matching_cost, info, R, rms, restricted_cvmatch_ratio, increment, true);  // verbose
  }

  vcl_cout << "Matching cost: " << matching_cost << " total info: " << info << vcl_endl;

  //----------------------------------
  // create the output storage class
  //----------------------------------
  clear_output();
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(output_image);
  output_data_[0].push_back(output_storage);

  return true;
}

