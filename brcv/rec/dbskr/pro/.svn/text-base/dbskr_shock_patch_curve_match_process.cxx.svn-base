// This is dbskr/pro/dbskr_shock_patch_curve_match_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_patch_curve_match_process.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_shock_patch_curve_match.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>

#include <dbskr/algo/dbskr_rec_algs.h>

#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_object_matcher.h>
#include <dbru/algo/dbru_object_matcher.h>

dbskr_shock_patch_curve_match_process::dbskr_shock_patch_curve_match_process() : bpro1_process()
{
  if ( !parameters()->add( "n (keeps top n real curve best matches)" , "-n", (int) 3 ) ||
       !parameters()->add( "load shock patch storage1 from file" , "-st1", (bool) true ) ||
       !parameters()->add( "Input shock patch storage1 <filename...>" , 
                           "-st1f" ,
                           //bpro1_filepath("W:\\eth-80\\horse\\horse1_fragments\\temp\\horse1-090-180-map-mirror-con.esf__patch_storage.bin","*.bin") 
                           bpro1_filepath("W:\\eth-80\\horse\\228_frags\\228_frags_prune_depth3\\228.e__patch_storage.bin", "*.bin")
                           ) ||
       !parameters()->add( "load shock patch storage2 from file" , "-st2", (bool) true ) ||
       !parameters()->add( "Input shock patch storage2 <filename...>" , 
                           "-st2f" ,
                           //bpro1_filepath("W:\\eth-80\\horse\\horse3_fragments\\horse3-090-180-map-mirror-con.esf__patch_storage.bin","*.bin") 
                           bpro1_filepath("W:\\eth-80\\horse\\horse1_fragments\\horse1-090-180-map-mirror.esf_0.con_patch_storage.bin","*.bin") 
                           ) ||
       !parameters()->add( "load shock patch matches from file" , "-sm", (bool) true ) ||
       !parameters()->add( "Input shock patch match <filename...>" , 
                           "-smf" ,
                           //bpro1_filepath("W:\\eth-80\\horse\\horse1-horse3-map-mirror-con-match-coarse.bin","*.bin") 
                           bpro1_filepath("W:\\eth-80\\horse\\228-horse1-con-match.bin","*.bin") 
                           ) 
       
                           )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dbskr_shock_patch_curve_match_process::clone() const
{
  return new dbskr_shock_patch_curve_match_process(*this);
}

vcl_vector< vcl_string > dbskr_shock_patch_curve_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  //to_return.push_back( "shock" );
  //to_return.push_back( "shock" );
  to_return.push_back( "shock_patch" );
  to_return.push_back( "shock_patch" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_shock_patch_curve_match_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock_patch_match");
  return to_return;
}

bool dbskr_shock_patch_curve_match_process::execute()
{
  dbskr_shock_patch_storage_sptr shock1, shock2;

  int n ;
  parameters()->get_value( "-n", n);

  bool bin_st1, bin_st2;
  parameters()->get_value( "-st1", bin_st1);
  parameters()->get_value( "-st2", bin_st2);

  if (bin_st1) {
    shock1 = dbskr_shock_patch_storage_new();
    bpro1_filepath input_path;
    parameters()->get_value( "-st1f" , input_path);
    vcl_string st_file1 = input_path.path;

    vsl_b_ifstream ifs(st_file1.c_str());
    shock1->b_read(ifs);
    ifs.close();

  } else 
    shock1.vertical_cast(input_data_[0][0]);

  if (bin_st2) {
    shock2 = dbskr_shock_patch_storage_new();
    bpro1_filepath input_path;
    parameters()->get_value( "-st2f" , input_path);
    vcl_string st_file2 = input_path.path;

    vsl_b_ifstream ifs(st_file2.c_str());
    shock2->b_read(ifs);
    ifs.close();

  } else
    shock2.vertical_cast(input_data_[0][1]);


  // get image from the storage class
  vidpro1_image_storage_sptr image_stg1;
  image_stg1.vertical_cast(input_data_[0][2]);
  vidpro1_image_storage_sptr image_stg2;
  image_stg2.vertical_cast(input_data_[0][3]);

  bool match_available;
  parameters()->get_value( "-sm", match_available);
  
  dbskr_shock_patch_curve_match_sptr match = new dbskr_shock_patch_curve_match();
  match->set_n(n);

  if (match_available) {
    bpro1_filepath input_path;
    parameters()->get_value( "-smf" , input_path);
    vcl_string m_file = input_path.path;
    
    vsl_b_ifstream imfs(m_file.c_str());
    match->b_read(imfs);
    imfs.close();
  } else {

    // only creates the shock match storage
    //dbskr_shock_patch_match_sptr match = find_all_patch_correspondences(shock1->get_patches(), shock2->get_patches(), scurve_sample_ds, elastic_splice);
    vcl_cout << "match was not available, use the tool to match\n";
  }
  
  vcl_vector<dbskr_shock_patch_sptr>& patch_set2 = match->get_patch_set2();

  vcl_map<int, dbskr_shock_patch_sptr> map1;
  for (unsigned i = 0; i < shock1->get_patches().size(); i++) {
    dbskr_shock_patch_sptr sp = shock1->get_patches()[i];
    map1[sp->id()] = sp;
  }
  vcl_map<int, dbskr_shock_patch_sptr> map2;
  for (unsigned i = 0; i < shock2->get_patches().size(); i++) {
    dbskr_shock_patch_sptr sp = shock2->get_patches()[i];
    map2[sp->id()] = sp;
    patch_set2.push_back(sp);
  }

  match->set_id_map1(map1);
  match->set_id_map2(map2);
  
  // create the output storage class
  dbskr_shock_patch_match_storage_sptr output = dbskr_shock_patch_match_storage_new();
  output->set_curve_match(match);
  output->set_image1(image_stg1->get_image());
  output->set_image2(image_stg2->get_image());

  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output));

  return true;
}
