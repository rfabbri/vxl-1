// This is dbskr/pro/dbskr_shock_match_con_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_match_con_process.h>

#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>

#include <dbskr/dbskr_tree_edit.h>

dbskr_shock_match_con_process::dbskr_shock_match_con_process() : bpro1_process()
{
  if ( !parameters()->add( "Input con file 1 <filename...>" , 
                           "-con1" ,
                           //bpro1_filepath("D:\\projects\\vehicles\\esfs_064_origs_less_sm_0.04\\Chevy_Silverado_Twincab_01_024-297-068-030.esf","*.esf") 
                           bpro1_filepath("D:\\projects\\vehicles\\cons_128_origs\\Chevy_Silverado_Twincab_01_024-297-068-030.esf","*.esf") 
                           ) ||
       !parameters()->add( "Input con file 2 <filename...>" , 
                           "-con2" ,
                           //bpro1_filepath("D:\\projects\\vehicles\\esfs_064_origs_less_sm_0.04\\Geo_Tracker_93_024-297-068-030.esf","*.esf") 
                           bpro1_filepath("D:\\projects\\vehicles\\cons_128_origs\\Geo_Tracker_93_024-297-068-030.esf","*.esf") 
                           ) ||
       !parameters()->add( "Prune Trees?" , "-prunetree" , (bool) false ) ||
       !parameters()->add( "Prune Threshold?" , "-prunethres" , (double) 5.0f ) ||
       !parameters()->add( "Match file exists?" , "-fileexists" , (bool) false ) ||
       !parameters()->add( "Input/Output shgm file <filename...>" , 
                           "-shgm" ,
                           //bpro1_filepath("D:\\projects\\vehicles\\match_elasticsplice_shgms_less_sm_0.04\\064-024-297-068-030\\Chevy_Silverado_Twincab_01_024-297-068-030_Geo_Tracker_93_024-297-068-030.shgm","*.shgm") 
                           bpro1_filepath("D:\\projects\\vehicles\\expts\\128-024-297-068-030\\Chevy_Silverado_Twincab_01_024-297-068-030_Geo_Tracker_93_024-297-068-030.shgm","*.shgm") 
                           ) ||
       !parameters()->add( "scurve sample ds" , "-scurve_ds" , (double) 1.0f ) ||
       !parameters()->add( "elastic splice cost" , "-splice" , (bool) true ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dbskr_shock_match_con_process::clone() const
{
  return new dbskr_shock_match_con_process(*this);
}

vcl_vector< vcl_string > dbskr_shock_match_con_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_shock_match_con_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock_match");
  return to_return;
}

bool dbskr_shock_match_con_process::execute()
{

  bool load1, load2;
  parameters()->get_value( "-load1" , load1);
  parameters()->get_value( "-load2" , load2);
  bpro1_filepath input_path;
  parameters()->get_value( "-esf1" , input_path);
  vcl_string esf_file1 = input_path.path;
  parameters()->get_value( "-esf2" , input_path);
  vcl_string esf_file2 = input_path.path;  
  parameters()->get_value( "-shgm" , input_path);
  vcl_string shgm_file = input_path.path;

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
  
  if (!sg1 || !sg2)
  {
    vcl_cout << "Problems in getting shock graphs!\n";
    return false;
  }

  bool exists;
  parameters()->get_value( "-fileexists" , exists);
  
  bool prune_tree;
  parameters()->get_value( "-prunetree" , prune_tree);
  double prune_thres;
  parameters()->get_value( "-prunethres" , prune_thres);

  //get the subsampling parameter
  double scurve_sample_ds;
  parameters()->get_value( "-scurve_ds" , scurve_sample_ds );
  bool elastic_splice_cost;
  parameters()->get_value( "-splice", elastic_splice_cost);
  
  //: prepare the trees also
  dbskr_tree_sptr tree1 = new dbskr_tree(scurve_sample_ds);
  dbskr_tree_sptr tree2 = new dbskr_tree(scurve_sample_ds);
  if (prune_tree)
    tree1->acquire_and_prune(sg1, prune_thres, elastic_splice_cost);
  else
    tree1->acquire(sg1, elastic_splice_cost);

  if (prune_tree)
    tree2->acquire_and_prune(sg2, prune_thres, elastic_splice_cost);
  else
    tree2->acquire(sg2, elastic_splice_cost);

  // create the output storage class
  dbskr_shock_match_storage_sptr output_match = dbskr_shock_match_storage_new();
  
  if (exists)  // no need for matching
  {  
    dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor(tree1, tree2);
    sm_cor->read_and_construct_from_shgm(shgm_file);
    output_match->set_sm_cor(sm_cor);
  } 
  else  // do the matching
  { 
    vcl_cout << "matching shock graphs...\n";
    clock_t time1, time2;
    time1 = clock();

    //instantiate the edit distance algorithm
    dbskr_tree_edit edit(tree1, tree2);

    edit.save_path(true);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      return false;
    }
    time2 = clock();
    float val = edit.final_cost();
    vcl_cout << " cost: " << val << " time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << "\n";
    edit.write_shgm(shgm_file);
    dbskr_sm_cor_sptr sm_cor = edit.get_correspondence();
    output_match->set_sm_cor(sm_cor);
  }

  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_match));

  return true;
}
