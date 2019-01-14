// match shock patches 

#include <ctime>
#include <algorithm>
#include <iostream>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>

dbsk2d_shock_graph_sptr read_esf_from_file(std::string fname) {
  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  return sg;
}

void individual_usage()
{
  std::cout << "\nIndividual Usage: <program name> <shock_patch_storage1> <shock_patch_storage2> <out_file> <shock_curve_sample_ds> <match_with_circular_completions (1 or 0)> <match_with_combined_edit (1 or 0)>\n";
  std::cout << "match_with_circular_completions: 1 if matching with circular completions at the leaf scurves, otherwise 0 (open boundary matching)\n";
  std::cout << "match_with_combined_edit: 1 if matching using combined cost during interval cost computation (see: brcv/rec/dbskr/dbskr_tree_edit_combined.h)\n";
  //std::cout << "shock curve sample ds is typically 2.0f\n";
}
int main_individual(int argc, char *argv[]) {
  std::cout << "Matching shock patches!\n";

  //: out file contains the wrong mathces if any
  std::string st_file1, st_file2, output_file;

  std::cout << "argc: " << argc << std::endl;
  
  if (argc != 7) {
    individual_usage();
    return -1;
  }
  st_file1 = argv[1];
  st_file2 = argv[2];
  output_file = argv[3];
  float sc_samp_ds = (float)atof(argv[4]);
  bool match_with_circular_completions = (atoi(argv[5]) == 1 ? true : false);
  bool match_with_combined_edit = (atoi(argv[6]) == 1 ? true : false);
  
  // create the input storage class
  dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file1.c_str());
  st1->b_read(ifs);
  ifs.close();

  std::string str_name_end = "patch_storage.bin";

  //: read the esfs as well
  std::cout << st1->size() << " patches in str1, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st1->size(); iii++) {
    dbskr_shock_patch_sptr sp = st1->get_patch(iii);
    std::string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
    std::ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  // create the input storage class
  dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs2(st_file2.c_str());
  st2->b_read(ifs2);
  ifs2.close();

  //: read the esfs as well
  std::cout << st2->size() << " patches in str2, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < st2->size(); iii++) {
    dbskr_shock_patch_sptr sp = st2->get_patch(iii);
    std::string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
    std::ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
  }

  std::cout << "read the shock patch storages...\n";
  std::vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
  std::vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  patch_cor_map_type& map = match->get_map();

  match->shock_pruning_threshold_ = 0.8f;
  match->scurve_sample_ds_ = sc_samp_ds;
  match->elastic_splice_cost_ = true;

  //: set the parameters in the patches
  for (unsigned i = 0; i < pv1.size(); i++) {
    pv1[i]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
    pv1[i]->scurve_sample_ds_ = match->scurve_sample_ds_;
    pv1[i]->elastic_splice_cost_ = match->elastic_splice_cost_;
  }

  for (unsigned i = 0; i < pv2.size(); i++) {
    pv2[i]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
    pv2[i]->scurve_sample_ds_ = match->scurve_sample_ds_;
    pv2[i]->elastic_splice_cost_ = match->elastic_splice_cost_;
  }

  std::cout << pv1.size() << " patches: ";
  for (unsigned i = 0; i < pv1.size(); i++) {
    std::cout << i << " ";
    find_patch_correspondences(pv1[i], pv2, map, match_with_circular_completions, match_with_combined_edit);
    pv1[i]->kill_tree();
    pv1[i]->kill_shock_graph();
  }
   
  std::cout << "writing bin file\n";
  vsl_b_ofstream bfs(output_file.c_str());
  match->b_write(bfs);
  bfs.close();
  return 0;
}
/*
void eth_maps_usage() {
  std::cout << "Usage : <program name> <category name> <view_sptr> <match_output_dir> <scurve_sample_ds> <match_with_circular_completions>\n";
  std::cout << "Modify the source code to change parameters to construct patch storage name depending on class\n";
  std::cout << "scurve sample ds is typically 4 for coarse matching\n";
  std::cout << "<match_with_circular_completions> = 0 if end scurves are not to be completed with circular arcs during matching, this is almost always the case for this database, 0 otherwise (closed boundary matching)\n";
}
int main_eth_maps(int argc, char *argv[]) {
  std::cout << "matching shock silhouettes from eth set!\n";
  std::string image_dir, patch_dir, match_dir, view_str, cat_str;

  if (argc != 6) {
    eth_maps_usage();
    return -1;
  }
  
  cat_str = argv[1]; view_str = argv[2]; match_dir = argv[3];
  float scurve_sample_ds = float(atof(argv[4]));
  bool match_with_circular_completions = (atoi(argv[5]) == 1 ? true : false);
  
  std::vector<std::string> cats, cat_par_strs;
  cats.push_back("horse");  cat_par_strs.push_back("_6_15_3_2_0.3_0.6");
  cats.push_back("dog");    cat_par_strs.push_back("_6_15_3_2_0.3_0.6");
  cats.push_back("cow");    cat_par_strs.push_back("_6_15_3_2_0.3_0.6");
  cats.push_back("car");    cat_par_strs.push_back("_6_15_3_2_0.3_0.6");
  cats.push_back("apple");  cat_par_strs.push_back("_2_5_1_1_0.3_0.6");
  cats.push_back("pear");   cat_par_strs.push_back("_2_5_1_1_0.3_0.6");
  cats.push_back("cup");    cat_par_strs.push_back("_2_5_1_1_0.3_0.6");
  cats.push_back("tomato"); cat_par_strs.push_back("_2_5_1_1_0.3_0.6");
  std::string sorting_method_str = "_color_";

  float shock_pruning_threshold = 0.8f;
  bool elastic_splice_cost = true;

  unsigned i = 0;
  for ( ; i < cats.size(); i++) {
    if (cat_str == cats[i])
      break;
  }

  // outputs are written as e.g. ./cow/cow1-090-180/cow1-090-180-cow2-090-180-sc-2.0.bin
  std::string out_dir = match_dir+cats[i]+"/";
  vul_file::make_directory_path(out_dir);
  std::cout << cats[i] << "\n";
  for (unsigned j = 1; j < 11; j++) {
    std::ostringstream oss;
    oss << j;

    std::string ins_name = cats[i]+oss.str()+"-"+view_str;

    std::string ins_dir = out_dir+ins_name+"/";
    vul_file::make_directory_path(ins_dir);
    
    std::cout << "\t\t" << ins_name << std::endl;

    std::string esf_file1 = "./"+cats[i]+"/"+ins_name+"-map.esf";
    // read the esf
    dbsk2d_xshock_graph_fileio reader;
    dbsk2d_shock_graph_sptr sg1 = reader.load_xshock_graph(esf_file1);

    dbskr_tree_sptr tree1 = new dbskr_tree();
    tree1->acquire(sg1, elastic_splice_cost, match_with_circular_completions);

    std::vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
    //std::map<int, dbskr_shock_patch_sptr> map1;
    //: set the parameters in the patches
    for (unsigned iii = 0; iii < pv1.size(); iii++) {
      pv1[iii]->shock_pruning_threshold_ = shock_pruning_threshold;
      pv1[iii]->scurve_sample_ds_ = scurve_sample_ds;
      pv1[iii]->elastic_splice_cost_ = elastic_splice_cost;
      //map1[pv1[iii]->id()] = pv1[iii];
    }

    for (unsigned c = 0; c < cats.size(); c++) {
      
      for (unsigned cj = 1; cj < 11; cj++) {
        std::ostringstream oss2;
        oss2 << cj;
        std::string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;
        std::cout << "\t\t\t" << ins_name2 << std::endl;

        std::string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+"-normal-edit.bin";
        std::string output_name2 = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-normal-edit.bin";

        if (vul_file::exists(output_name) || vul_file::exists(output_name2)) 
          continue;

        std::string st_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/"+ins_name2+"_patch_storage.bin";
        // create the input storage class
        dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
        vsl_b_ifstream ifs2(st_file2.c_str());
        st2->b_read(ifs2);
        ifs2.close();

        //: read the esfs as well
        std::cout << st2->size() << " patches in str2, reading shocks..\n";
        //: load esfs for each patch
        for (unsigned iii = 0; iii < st2->size(); iii++) {
          dbskr_shock_patch_sptr sp = st2->get_patch(iii);
          std::string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
          std::ostringstream oss;
          oss << sp->id();
          patch_esf_name = patch_esf_name+oss.str()+".esf";
          dbsk2d_xshock_graph_fileio file_io;
          dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
          sp->set_shock_graph(sg);
        }

        std::vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();
        
        //std::map<int, dbskr_shock_patch_sptr> map2;
        //for (unsigned iii = 0; iii < pv2.size(); iii++) 
        //  map2[pv2[iii]->id()] = pv2[iii];

        //: match
        dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
        patch_cor_map_type& map = match->get_map();

        match->shock_pruning_threshold_ = shock_pruning_threshold;
        match->scurve_sample_ds_ = scurve_sample_ds;
        match->elastic_splice_cost_ = elastic_splice_cost;

        for (unsigned iii = 0; iii < pv2.size(); iii++) {
          pv2[iii]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
          pv2[iii]->scurve_sample_ds_ = match->scurve_sample_ds_;
          pv2[iii]->elastic_splice_cost_ = match->elastic_splice_cost_;
        }

        vul_timer t;
        t.mark();
        for (unsigned iii = 0; iii < pv1.size(); iii++) {  
          std::cout << iii << " out of " << pv1.size() << ": ";
          find_patch_correspondences(pv1[iii], pv2, map, match_with_circular_completions);
        }

        std::cout << "\t\t\twriting bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
        vsl_b_ofstream bfs(output_name.c_str());
        match->b_write(bfs);
        bfs.close();

        match->clear();

        for (unsigned iii = 0; iii < pv2.size(); iii++) {
          pv2[iii]->kill_tree();
          pv2[iii]->kill_shock_graph();
        }


      }
    }

    for (unsigned iii = 0; iii < pv1.size(); iii++) {
      pv1[iii]->kill_tree();
      pv1[iii]->kill_shock_graph();
    }
  }

  return 0;
}
*/
void eth_usage() {
  std::cout << "\nUsage : <program name> <category name> <view_sptr> <patch_storage_dir> <match_output_dir> <scurve_sample_ds> <match_with_circular_completions> <ins id>\n";
  std::cout << "Modify the source code to change parameters to construct patch storage name depending on class\n";
  std::cout << "scurve sample ds is typically 4 for coarse matching\n";
  std::cout << "<match_with_circular_completions> = 0 if end scurves are not to be completed with circular arcs during matching, this is almost always the case for this database, 0 otherwise (closed boundary matching)\n";
  std::cout << "<match_with_combined_edit>: 1 if matching using combined cost during interval cost computation (see: brcv/rec/dbskr/dbskr_tree_edit_combined.h)\n";
  //std::cout << "if last argument is 1, matches first half of class examplers, if 2 second half, if 3 matches all, if greater than 3 then matches the instance with that id only\n";
  std::cout << "matches the instance given by the last argument\n";
}
int main_eth(int argc, char *argv[]) {
  std::cout << "matching shock patches from eth set!\n";
  std::string image_dir, patch_dir, match_dir, view_str, cat_str;

  if (argc != 9) {
    eth_usage();
    return -1;
  }
  
  cat_str = argv[1]; view_str = argv[2]; patch_dir = argv[3]; match_dir = argv[4];
  float scurve_sample_ds = float(atof(argv[5]));
  bool match_with_circular_completions = (atoi(argv[6]) == 1 ? true : false);
  bool match_with_combined_edit = (atoi(argv[7]) == 1 ? true : false);
  int half = atoi(argv[8]);
  
  std::vector<std::string> cats, cat_par_strs;
  cats.push_back("horse");  cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("cow");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("car");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("apple");  cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("pear");   cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("cup");    cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("tomato"); cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  std::string sorting_method_str = "_color_";

  float shock_pruning_threshold = 0.8f;
  bool elastic_splice_cost = true;

  unsigned i = 0;
  for ( ; i < cats.size(); i++) {
    if (cat_str == cats[i])
      break;
  }

  std::string str_name_end = "patch_storage.bin";
  
  // outputs are written as e.g. ./cow/cow1-090-180/cow1-090-180-cow2-090-180-sc-2.0.bin
  std::string out_dir = match_dir+cats[i]+"/";
  vul_file::make_directory_path(out_dir);
  std::cout << cats[i] << "\n";
  int start_j = 1;
  int end_j = 11;
  /*if (half == 1)
    end_j = 6;
  else if (half == 2)
    start_j = 6;
  else if (half > 3) {
    start_j = half;
    end_j = half + 1;
  }
  */
  start_j = half;
  end_j = half+1;
  for (int j = start_j; j < end_j; j++) {
    std::ostringstream oss;
    oss << j;

    std::string ins_name = cats[i]+oss.str()+"-"+view_str;

    std::string ins_dir = out_dir+ins_name+"/";
    vul_file::make_directory_path(ins_dir);
    
    std::cout << "\t\t" << ins_name << std::endl;

    std::string st_file1 = patch_dir+cats[i]+"/"+ins_name+"-patches"+sorting_method_str+cat_par_strs[i]+"/"+ins_name+"_patch_storage.bin";
    // create the input storage class
    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();

    //: read the esfs as well
    std::cout << st1->size() << " patches in str1, reading shocks..\n";
    //: load esfs for each patch
    for (unsigned iii = 0; iii < st1->size(); iii++) {
      dbskr_shock_patch_sptr sp = st1->get_patch(iii);
      std::string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
      std::ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_xshock_graph_fileio file_io;
      dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
      sp->set_shock_graph(sg);
    }

    std::vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
    //std::map<int, dbskr_shock_patch_sptr> map1;
    //: set the parameters in the patches
    for (unsigned iii = 0; iii < pv1.size(); iii++) {
      pv1[iii]->shock_pruning_threshold_ = shock_pruning_threshold;
      pv1[iii]->scurve_sample_ds_ = scurve_sample_ds;
      pv1[iii]->elastic_splice_cost_ = elastic_splice_cost;
      //map1[pv1[iii]->id()] = pv1[iii];
    }

    for (unsigned c = 0; c < cats.size(); c++) {
      
      for (unsigned cj = 1; cj < 11; cj++) {
        std::ostringstream oss2;
        oss2 << cj;
        std::string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;
        std::cout << "\t\t\t" << ins_name2 << std::endl;

        std::string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+"-normal-edit.bin";
        std::string output_name2 = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-normal-edit.bin";

        if (vul_file::exists(output_name) || vul_file::exists(output_name2)) 
          continue;

        std::string st_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/"+ins_name2+"_patch_storage.bin";
        // create the input storage class
        dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
        vsl_b_ifstream ifs2(st_file2.c_str());
        st2->b_read(ifs2);
        ifs2.close();

        //: read the esfs as well
        std::cout << st2->size() << " patches in str2, reading shocks.. ";
        //: load esfs for each patch
        for (unsigned iii = 0; iii < st2->size(); iii++) {
          dbskr_shock_patch_sptr sp = st2->get_patch(iii);
          std::string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
          std::ostringstream oss;
          oss << sp->id();
          patch_esf_name = patch_esf_name+oss.str()+".esf";
          dbsk2d_xshock_graph_fileio file_io;
          dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
          sp->set_shock_graph(sg);
        }

        std::cout << " done!\n";
        std::vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();
        
        //std::map<int, dbskr_shock_patch_sptr> map2;
        //for (unsigned iii = 0; iii < pv2.size(); iii++) 
        //  map2[pv2[iii]->id()] = pv2[iii];

        //: match
        dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
        patch_cor_map_type& map = match->get_map();

        match->shock_pruning_threshold_ = shock_pruning_threshold;
        match->scurve_sample_ds_ = scurve_sample_ds;
        match->elastic_splice_cost_ = elastic_splice_cost;

        for (unsigned iii = 0; iii < pv2.size(); iii++) {
          pv2[iii]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
          pv2[iii]->scurve_sample_ds_ = match->scurve_sample_ds_;
          pv2[iii]->elastic_splice_cost_ = match->elastic_splice_cost_;
        }

        vul_timer t;
        t.mark();
        std::cout << "pv1 size: " << pv1.size() << " ";
        for (unsigned iii = 0; iii < pv1.size(); iii++) {  
          std::cout << iii << " "; std::cout.flush();
          find_patch_correspondences(pv1[iii], pv2, map, match_with_circular_completions, match_with_combined_edit);
        }

        std::cout << "\n\t\t\twriting bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
        vsl_b_ofstream bfs(output_name.c_str());
        match->b_write(bfs);
        bfs.close();

        match->clear();

        for (unsigned iii = 0; iii < pv2.size(); iii++) {
          pv2[iii]->kill_tree();
          pv2[iii]->kill_shock_graph();
        }


      }
    }

    for (unsigned iii = 0; iii < pv1.size(); iii++) {
      pv1[iii]->kill_tree();
      pv1[iii]->kill_shock_graph();
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 9)
    main_eth(argc, argv);
  else if (argc == 7)
    main_individual(argc, argv);
  else {
    individual_usage();
    eth_usage();
  }

  return 0;
}
