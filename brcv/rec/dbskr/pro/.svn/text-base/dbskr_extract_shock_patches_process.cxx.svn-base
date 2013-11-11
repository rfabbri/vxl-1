// This is dbskr/pro/dbskr_extract_shock_patches_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_extract_shock_patches_process.h>

#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_shock_patch_sptr.h>
#include <dbskr/algo/dbskr_rec_algs.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vsol/vsol_polygon_2d.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>

#include <dbskr/pro/dbskr_shock_patch_storage.h>

dbskr_extract_shock_patches_process::dbskr_extract_shock_patches_process() : bpro1_process()
{
  if ( !parameters()->add( "load shock patch storage from file" , "-st", (bool) true ) ||
       !parameters()->add( "Input shock patch storage <filename...>" , 
                           "-stf" ,
                           //bpro1_filepath("W:\\eth-80\\horse\\horse1_fragments\\temp\\horse1-090-180-map-mirror-con.esf__patch_storage.bin","*.bin") 
                           bpro1_filepath("W:\\ICCV07-expts\\eth80\\patches\\cow\\cow2-090-180-patches_color__6_9_3_3_0.5\\cow2-090-180_patch_storage.bin", "*.bin")
                           ) ||
       !parameters()->add( "if load from storage, load esfs as well?" , "-loadesfs", (bool) true ) ||
       !parameters()->add( "if load esfs, postfix: " , "-postfix", (vcl_string) "patch_strg.bin" ) ||
       
       !parameters()->add( "min depth:" , "-min_depth" , (int) 5 ) ||
       !parameters()->add( "max depth:" , "-max_depth" , (int) 5 ) ||
       !parameters()->add( "depth intervals" , "-depth_int" , (int) 1 ) ||
       !parameters()->add( "area threshold" , "-area" , (double) 5.0 ) ||
       !parameters()->add( "circular ends?" , "-circular" , (bool) true) ||
       !parameters()->add( "traced shock pruning thres" , "-prune" , (double) 0.8)
       )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dbskr_extract_shock_patches_process::clone() const
{
  return new dbskr_extract_shock_patches_process(*this);
}

vcl_vector< vcl_string > dbskr_extract_shock_patches_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_extract_shock_patches_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock_patch");
  return to_return;
}

bool dbskr_extract_shock_patches_process::execute()
{

  dbskr_shock_patch_storage_sptr shock_p;

  bool bin_st;
  parameters()->get_value( "-st", bin_st);

  bool loadesfs;
  parameters()->get_value( "-loadesfs", loadesfs);

  bpro1_filepath input_path;
  parameters()->get_value( "-stf" , input_path);
  vcl_string st_file = input_path.path;

  if (bin_st) {
    shock_p = dbskr_shock_patch_storage_new();
    
    vsl_b_ifstream ifs(st_file.c_str());
    shock_p->b_read(ifs);
    ifs.close();

    //: load the esf files if exist
    if (loadesfs) { 
      vcl_string postfix;
      parameters()->get_value("-postfix", postfix);
      vcl_string esf_prefix = st_file.substr(0, st_file.length()-postfix.size());
      for (unsigned iii = 0; iii < shock_p->size(); iii++) {
        dbskr_shock_patch_sptr sp = shock_p->get_patch(iii);
        vcl_ostringstream oss;
        oss << sp->id();
        vcl_string patch_esf_name = esf_prefix+oss.str()+".esf";
        dbsk2d_xshock_graph_fileio file_io;
        dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
        sp->set_shock_graph(sg);
      }
    }
  } else {

  dbsk2d_shock_storage_sptr shock;
  dbsk2d_shock_graph_sptr sg;
  shock.vertical_cast(input_data_[0][0]);
  sg = shock->get_shock_graph();
  
  if (!sg)
  {
    vcl_cout << "Problems in getting shock graph!\n";
    return false;
  }

  int min_depth, max_depth, depth_int;
  parameters()->get_value( "-min_depth", min_depth);
  parameters()->get_value( "-max_depth", max_depth);
  parameters()->get_value( "-depth_int", depth_int);
  double area_thres;
  parameters()->get_value( "-area", area_thres);
  bool circular_ends;
  parameters()->get_value( "-circular", circular_ends);
  double shock_pruning_threshold;
  parameters()->get_value( "-prune", shock_pruning_threshold);

  // create the output storage class
  shock_p = dbskr_shock_patch_storage_new();
  
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;

    double prev_area = 0;
    for (int d = min_depth; d <= max_depth; d += depth_int) {
      dbskr_shock_patch_sptr sp = extract_patch_from_subgraph(sg, *v_itr, d, area_thres, circular_ends);
      if (sp->get_outer_boundary()) {
        double area = sp->get_outer_boundary()->area();
        if (area != prev_area) {
          shock_p->add_patch(sp);
          prev_area = area;
        } else                     // when depth is not being increased anymore, stop
          break;
      }
    }
  }

  }

  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,shock_p));

  return true;
}
