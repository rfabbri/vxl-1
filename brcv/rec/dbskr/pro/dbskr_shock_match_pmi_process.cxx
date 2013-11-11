// This is dbskr/pro/dbskr_shock_match_pmi_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_match_pmi_process.h>

#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbskr/dbskr_tree_edit_pmi.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_rcor_generator.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/pro/dbru_rcor_storage_sptr.h>

dbskr_shock_match_pmi_process::dbskr_shock_match_pmi_process() : dbskr_shock_match_process()
{
  if ( !parameters()->add( "normal edit distance" , "-normal" , false ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dbskr_shock_match_pmi_process::clone() const
{
  return new dbskr_shock_match_pmi_process(*this);
}

vcl_vector< vcl_string > dbskr_shock_match_pmi_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  to_return.push_back( "shock" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_shock_match_pmi_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock_match");
  to_return.push_back("region_cor");
  to_return.push_back("image");
  return to_return;
}

bool dbskr_shock_match_pmi_process::execute()
{
  bool load1=false, load2=false;
  parameters()->get_value( "-load1" , load1);
  parameters()->get_value( "-load2" , load2);
  bpro1_filepath input_path;
  parameters()->get_value( "-esf1" , input_path);
  vcl_string esf_file1 = input_path.path;
  parameters()->get_value( "-esf2" , input_path);
  vcl_string esf_file2 = input_path.path;  
  parameters()->get_value( "-shgm" , input_path);
  vcl_string shgm_file = input_path.path;
  bool normal_edit=false;
  parameters()->get_value( "-normal", normal_edit);

  bool exists=false;
  parameters()->get_value( "-fileexists" , exists);
  //get the subsampling parameter
  float scurve_sample_ds;
  parameters()->get_value( "-scurve_ds" , scurve_sample_ds );
  bool elastic_splice_cost = true;
  parameters()->get_value( "-elastic" , elastic_splice_cost);

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][2]);
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
  frame_image1.vertical_cast(input_data_[0][4]);
  vil_image_resource_sptr image1 = frame_image1->get_image();
  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][5]);
  vil_image_resource_sptr image2 = frame_image2->get_image();

  if (!image1 || !image2) {
    vcl_cout << "one of the images is missing.\n";
    return false;
  }
  //----------------------------------
  // create the observations
  //----------------------------------
  dbinfo_observation_sptr obs1 = new dbinfo_observation(0, image1, poly1, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, image2, poly2, true, true, false);

  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_storage_sptr shock1, shock2;
  dbsk2d_shock_graph_sptr sg1, sg2;
  if (load1) {
    sg1 = loader.load_xshock_graph(esf_file1);
  } else {
    // get input storage class
    shock1.vertical_cast(input_data_[0][0]);
    sg1 = shock1->get_shock_graph();
  }
  if (load2) {
    sg2 = loader.load_xshock_graph(esf_file2);
  } else {
    shock2.vertical_cast(input_data_[0][1]);
    sg2 = shock2->get_shock_graph();
  }
  if (!sg1 || !sg2) {
    vcl_cout << "Problems in getting shock graphs!\n";
    return false;
  }  
  //: prepare the trees also
  dbskr_tree_sptr tree1 = new dbskr_tree(scurve_sample_ds);
  dbskr_tree_sptr tree2 = new dbskr_tree(scurve_sample_ds);
  tree1->acquire(sg1, elastic_splice_cost, true, false);  // construct_circular_ends is true since curves are closed in this application
                                                          // dpmatch_combined = false since interval cost functions in Sebastian et al. PAMI06 will be used and splice cost function will use original dpmatch
  tree2->acquire(sg2, elastic_splice_cost, true, false);  // construct_circular_ends is true since curves are closed in this application
                                                          // dpmatch_combined = false since interval cost functions in Sebastian et al. PAMI06 will be used and splice cost function will use original dpmatch

  dbskr_sm_cor_sptr sm_cor;
  if (exists) // no need for matching
  { 
    sm_cor = new dbskr_sm_cor(tree1, tree2);
    sm_cor->read_and_construct_from_shgm(shgm_file);
  } 
  else // do the matching
  {  
    vcl_cout << "matching shock graphs...\n";
    clock_t time1, time2;
    time1 = clock();
    if (normal_edit) {
      dbskr_tree_edit edit(tree1, tree2);   
      edit.save_path(true);
      if (!edit.edit()) {
        vcl_cout << "Problems in editing trees\n";
        return false;
      }
      float val = edit.final_cost();
      vcl_cout << " cost: " << val << " ";
      edit.write_shgm(shgm_file);
      sm_cor = edit.get_correspondence();
    } 
    else // use mi
    {       
      dbskr_tree_edit_pmi edit(tree1, tree2, obs1, obs2);   
      edit.save_path(true);
      if (!edit.edit()) {
        vcl_cout << "Problems in editing trees\n";
        return false;
      }
      float val = edit.final_cost();
      vcl_cout << " cost: " << val << " ";
      edit.write_shgm(shgm_file);
      sm_cor = edit.get_correspondence();
    }
    time2 = clock();
    vcl_cout << " time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << "\n";
  }
  
  // create the output storage class
  dbskr_shock_match_storage_sptr output_match = dbskr_shock_match_storage_new();
  //vcl_vector<pathtable_key> path_map;
  output_match->set_sm_cor(sm_cor);
  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_match));

  dbru_rcor_sptr output_rcor = new dbru_rcor(obs1, obs2);
  dbru_rcor_generator::find_correspondence_shock(output_rcor,sm_cor);
  float info = dbinfo_observation_matcher::minfo(obs1, obs2, output_rcor->get_correspondences(), true);
  vcl_cout << " mutual info: " << info << vcl_endl;
  dbru_rcor_storage_sptr output_storage = dbru_rcor_storage_new();
  output_storage->set_rcor(output_rcor);
  output_data_[0].push_back(output_storage);
  
  vil_image_resource_sptr output_image = output_rcor->get_appearance2_on_pixels1();
  vidpro1_image_storage_sptr output_image_storage = vidpro1_image_storage_new();
  output_image_storage->set_image(output_image);
  output_data_[0].push_back(output_image_storage);
  
  return true;
}
