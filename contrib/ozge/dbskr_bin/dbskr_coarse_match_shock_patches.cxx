// match shock patches 

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>

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

void individual_usage()
{
  vcl_cout << "Usage: <program name> <shock_patch_storage1> <shock_patch_storage2> <out_file> <shock_curve_sample_ds> <match_with_circular_completions>\n";
  vcl_cout << "Last argument is 1 if matching with circular completions at the leaf scurves, otherwise 0 (open boundary matching)\n";
  //vcl_cout << "shock curve sample ds is typically 2.0f\n";
}
int main_individual(int argc, char *argv[]) {
  vcl_cout << "Matching shock patches!\n";

  //: out file contains the wrong mathces if any
  vcl_string st_file1, st_file2, output_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 6) {
    individual_usage();
    return -1;
  }
  st_file1 = argv[1];
  st_file2 = argv[2];
  output_file = argv[3];
  float sc_samp_ds = (float)atof(argv[4]);
  bool match_with_circular_completions = (atoi(argv[5]) == 1 ? true : false);
  
  // create the input storage class
  dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file1.c_str());
  st1->b_read(ifs);
  ifs.close();

  // create the input storage class
  dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs2(st_file2.c_str());
  st2->b_read(ifs2);
  ifs2.close();
  vcl_cout << "read the shock patch storages...\n";
  vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
  vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();

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

  vul_timer t;
  t.mark();
  for (unsigned i = 0; i < pv1.size(); i++) {  
    vcl_cout << i << " out of " << pv1.size() << ": ";
    find_patch_correspondences_coarse_edit(pv1[i], pv2, map, match_with_circular_completions);
    pv1[i]->kill_tree();
    pv1[i]->kill_shock_graph();
  }
   
  vcl_cout << "writing bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
  vsl_b_ofstream bfs(output_file.c_str());
  match->b_write(bfs);
  bfs.close();
  return 0;
}

void eth_usage() {
  vcl_cout << "Usage : <program name> <category name> <view_sptr> <patch_storage_dir> <match_output_dir> <scurve_sample_ds> <match_with_circular_completions>\n";
  vcl_cout << "Modify the source code to change parameters to construct patch storage name depending on class\n";
  vcl_cout << "scurve sample ds is typically 4 for coarse matching\n";
  vcl_cout << "<match_with_circular_completions> = 0 if end scurves are not to be completed with circular arcs during matching, this is almost always the case for this database, 0 otherwise (closed boundary matching)\n";
}
int main_eth(int argc, char *argv[]) {
  vcl_cout << "matching shock patches from eth set!\n";
  vcl_string image_dir, patch_dir, match_dir, view_str, cat_str;

  if (argc != 7) {
    eth_usage();
    return -1;
  }
  
  cat_str = argv[1]; view_str = argv[2]; patch_dir = argv[3]; match_dir = argv[4];
  float scurve_sample_ds = float(atof(argv[5]));
  bool match_with_circular_completions = (atoi(argv[6]) == 1 ? true : false);
  
  vcl_vector<vcl_string> cats, cat_par_strs;
  cats.push_back("horse");  cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("cow");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("car");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("apple");  cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("pear");   cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("cup");    cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("tomato"); cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  vcl_string sorting_method_str = "_color_";

  float shock_pruning_threshold = 0.8f;
  bool elastic_splice_cost = true;

  unsigned i = 0;
  for ( ; i < cats.size(); i++) {
    if (cat_str == cats[i])
      break;
  }

  
  // outputs are written as e.g. ./cow/cow1-090-180/cow1-090-180-cow2-090-180-sc-2.0.bin
  vcl_string out_dir = match_dir+cats[i]+"/";
  vul_file::make_directory_path(out_dir);
  vcl_cout << cats[i] << "\n";
  for (unsigned j = 1; j < 11; j++) {
    vcl_ostringstream oss;
    oss << j;

    vcl_string ins_name = cats[i]+oss.str()+"-"+view_str;

    vcl_string ins_dir = out_dir+ins_name+"/";
    vul_file::make_directory_path(ins_dir);
    
    vcl_cout << "\t\t" << ins_name << vcl_endl;

    vcl_string st_file1 = patch_dir+cats[i]+"/"+ins_name+"-patches"+sorting_method_str+cat_par_strs[i]+"/"+ins_name+"_patch_storage.bin";
    // create the input storage class
    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();
    vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
    vcl_map<int, dbskr_shock_patch_sptr> map1;
    //: set the parameters in the patches
    for (unsigned i = 0; i < pv1.size(); i++) {
      pv1[i]->shock_pruning_threshold_ = shock_pruning_threshold;
      pv1[i]->scurve_sample_ds_ = scurve_sample_ds;
      pv1[i]->elastic_splice_cost_ = elastic_splice_cost;
      map1[pv1[i]->id()] = pv1[i];
    }

    for (unsigned c = 0; c < cats.size(); c++) {
      
      for (unsigned cj = 1; cj < 11; cj++) {
        vcl_ostringstream oss2;
        oss2 << cj;
        vcl_string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;
        vcl_cout << "\t\t\t" << ins_name2 << vcl_endl;

        vcl_string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+".bin";
        vcl_string output_name_fine = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+"-fine-no-Ainf.bin";
        
        vcl_string output_name2 = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+".bin";
        vcl_string output_name2_fine = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-fine-no-Ainf.bin";

        vcl_string st_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/"+ins_name2+"_patch_storage.bin";
        // create the input storage class
        dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
        vsl_b_ifstream ifs2(st_file2.c_str());
        st2->b_read(ifs2);
        ifs2.close();

        vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();
        vcl_map<int, dbskr_shock_patch_sptr> map2;
        for (unsigned i = 0; i < pv2.size(); i++) 
          map2[pv2[i]->id()] = pv2[i];


        if (vul_file::exists(output_name_fine) || vul_file::exists(output_name2_fine)) 
          continue;

        if (vul_file::exists(output_name)) {

          dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();

          vsl_b_ifstream imfs(output_name.c_str());
          match->b_read(imfs);
          imfs.close();
          
          match->set_id_map1(map1);
          match->set_id_map2(map2);
          
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

          vul_timer t;
          t.mark();
          match->compute_norm_fine_costs_of_cors();  // no need for sorting
                    
          vcl_cout << "match_found!\n";
          vcl_cout << "\t\t\t writing fine fine bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name_fine.c_str());
          match->b_write(bfs);
          bfs.close();

          for (unsigned i = 0; i < pv2.size(); i++) {
            pv2[i]->kill_tree();
          }

          match->clear();

        } else if (vul_file::exists(output_name2)) {

          dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();

          vsl_b_ifstream imfs(output_name2.c_str());
          match->b_read(imfs);
          imfs.close();

          match->set_id_map1(map2);
          match->set_id_map2(map1);

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
          match->compute_norm_fine_costs_of_cors();  // no need for sorting

          for (unsigned i = 0; i < pv2.size(); i++) {
            pv2[i]->kill_tree();
          }

          vul_timer t;
          t.mark();
          vcl_cout << "\t\t\twriting fine bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name2_fine.c_str());
          match->b_write(bfs);
          bfs.close();
          
          match->clear();

          vcl_cout << "match_found, reversing!\n";
        
        } else {  // match

          dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
          patch_cor_map_type& map = match->get_map();

          match->shock_pruning_threshold_ = shock_pruning_threshold;
          match->scurve_sample_ds_ = scurve_sample_ds;
          match->elastic_splice_cost_ = elastic_splice_cost;

          for (unsigned i = 0; i < pv2.size(); i++) {
            pv2[i]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
            pv2[i]->scurve_sample_ds_ = match->scurve_sample_ds_;
            pv2[i]->elastic_splice_cost_ = match->elastic_splice_cost_;
          }

          vul_timer t;
          t.mark();
          for (unsigned i = 0; i < pv1.size(); i++) {  
            vcl_cout << i << " out of " << pv1.size() << ": ";
            find_patch_correspondences_coarse_edit(pv1[i], pv2, map, match_with_circular_completions);
            //pv1[i]->kill_tree();
            //pv1[i]->kill_shock_graph();
          }

          vcl_cout << "\t\t\twriting bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name.c_str());
          match->b_write(bfs);
          bfs.close();

          match->set_id_map1(map1);
          match->set_id_map2(map2);
          match->compute_norm_fine_costs_of_cors();  // no need for sorting

          for (unsigned i = 0; i < pv2.size(); i++) {
            pv2[i]->kill_tree();
          }
           
          vcl_cout << "\t\t\twriting bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfsf(output_name_fine.c_str());
          match->b_write(bfsf);
          bfsf.close();

          match->clear();
        }


      }
    }

  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 7)
    main_eth(argc, argv);
  else if (argc == 6)
    main_individual(argc, argv);
  else {
    individual_usage();
    eth_usage();
  }

  return 0;
}


