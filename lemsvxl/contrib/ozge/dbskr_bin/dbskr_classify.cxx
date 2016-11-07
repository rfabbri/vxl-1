// classify shock patch sets

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/dbskr_tree.h>
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


void eth_usage() {
  vcl_cout << "Usage : <program name> <category name> <view_sptr> <patch_storage_dir> <match_output_dir> <scurve_sample_ds> <n> <visualization_n> <id>\n";
  vcl_cout << "Modify the source code to change parameters to construct patch storage name depending on class\n";
  vcl_cout << "scurve sample ds is typically 4 for coarse matching\n";
  vcl_cout << "n: use the top n best patch matches between the two patch sets\n";
}
int main_eth(int argc, char *argv[]) {
  
  vcl_string image_dir, patch_dir, match_dir, view_str, cat_str;

  if (argc != 9) {
    eth_usage();
    return -1;
  }
  
  cat_str = argv[1]; view_str = argv[2]; patch_dir = argv[3]; match_dir = argv[4];
  float scurve_sample_ds = float(atof(argv[5]));
  int n = atoi(argv[6]);
  int visualization_n = atoi(argv[7]);
  int ins_id = atoi(argv[8]);
  
  vcl_vector<vcl_string> cats, cat_par_strs;
#if 0
  cats.push_back("tomato"); cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("horse");  cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("cow");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("car");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("apple");  cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("pear");   cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  cats.push_back("cup");    cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
#endif
  cats.push_back("horse");  cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("cow");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("car");    cat_par_strs.push_back("_6_15_3_2_0.3_0.8");
  cats.push_back("apple");  cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("pear");   cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("cup");    cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  cats.push_back("tomato"); cat_par_strs.push_back("_2_5_1_1_0.3_0.8");
  
  vcl_string sorting_method_str = "_color_";

  float shock_pruning_threshold = 0.8f;
  bool elastic_splice_cost = true;

  unsigned outer_i = 0;
  for ( ; outer_i < cats.size(); outer_i++) {
    if (cat_str == cats[outer_i])
      break;
  }

  vcl_string str_name_end = "patch_storage.bin";

  // outputs are written as e.g. ./cow/cow1-090-180/cow1-090-180-cow2-090-180-sc-2.0.bin
  
  vcl_string out_dir = match_dir+cats[outer_i]+"/";
  vul_file::make_directory_path(out_dir);
  vcl_cout << cats[outer_i] << "\n";
  //for (unsigned j = 1; j < 11; j++) {
  int j = ins_id;
    vcl_ostringstream oss;
    oss << j;

    vcl_string ins_name = cats[outer_i]+oss.str()+"-"+view_str;

    vcl_string out_html_file = match_dir+cats[outer_i]+"/"+cats[outer_i]+"_top_"+argv[7]+"_"+argv[8]+"_matches.html";

    vcl_string ins_dir = out_dir+ins_name+"/";
    vul_file::make_directory_path(ins_dir);
    
    vcl_cout << ins_name << vcl_endl;

    vcl_string st_file1 = patch_dir+cats[outer_i]+"/"+ins_name+"-patches"+sorting_method_str+cat_par_strs[outer_i]+"/"+ins_name+"_patch_storage.bin";
    vcl_string patch_images_dir1 = patch_dir+cats[outer_i]+"/"+ins_name+"-patches"+sorting_method_str+cat_par_strs[outer_i]+"/kept/";
    // create the input storage class
    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();

    //: read the esfs as well
    vcl_cout << st1->size() << " patches in str1, skiiping reading shocks..\n";
    //: load esfs for each patch
    for (unsigned iii = 0; iii < st1->size(); iii++) {
      dbskr_shock_patch_sptr sp = st1->get_patch(iii);
      vcl_string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_xshock_graph_fileio file_io;
      dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
      //if (!dbsk2d_compute_bounding_box(sg))
      //  vcl_cout << "Bounding box not computed!!!!!!!!\n";
      sp->set_shock_graph(sg);
    }

    vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();
    //: set the parameters in the patches
    vcl_map<int, dbskr_shock_patch_sptr> map1;
    for (unsigned i = 0; i < pv1.size(); i++) 
      map1[pv1[i]->id()] = pv1[i];

    //: see if all the matches are available for this instance
    //vcl_vector<dbskr_shock_patch_match_sptr> match_vec;
    vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> > match_vec;
    
    vcl_vector<vcl_string> class_names;
    vcl_vector<vcl_vector<vcl_string> > ins_names;
    for (unsigned c = 0; c < cats.size(); c++) {
      
      vcl_vector<dbskr_shock_patch_match_sptr> match_vec_c;
      vcl_vector<vcl_string> ins_names_c;
      for (unsigned cj = 1; cj < 11; cj++) {
        class_names.push_back(cats[c]);
        
        vcl_ostringstream oss2;
        oss2 << cj;
        vcl_string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;
        ins_names_c.push_back(ins_name2);
        vcl_cout  << ins_name2 << " ";

        vcl_string st_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/"+ins_name2+"_patch_storage.bin";
        // create the input storage class
        dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
        vsl_b_ifstream ifs2(st_file2.c_str());
        st2->b_read(ifs2);
        ifs2.close();

        //: read the esfs as well
        vcl_cout << st2->size() << " patches in str2, skipping shocks..\n";
        //: load esfs for each patch
        for (unsigned iii = 0; iii < st2->size(); iii++) {
          dbskr_shock_patch_sptr sp = st2->get_patch(iii);
          vcl_string patch_esf_name = st_file2.substr(0, st_file2.length()-str_name_end.size());
          vcl_ostringstream oss;
          oss << sp->id();
          patch_esf_name = patch_esf_name+oss.str()+".esf";
          dbsk2d_xshock_graph_fileio file_io;
          dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
          //if (!dbsk2d_compute_bounding_box(sg))
          //  vcl_cout << "Bounding box not computed!!!!!!!!\n";
          sp->set_shock_graph(sg);
        }

        vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();

        vcl_string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+".bin";
        
        //vcl_string output_name_fine = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+"-fine-no-Ainf.bin";
        //: for this run I've created normal edit distance matches
        vcl_string output_name_fine = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[5]+"-normal-edit.bin";
        
        vcl_string output_name2 = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+".bin";
        //vcl_string output_name2_fine = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-fine-no-Ainf.bin";
        vcl_string output_name2_fine = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-normal-edit.bin";

        vcl_map<int, dbskr_shock_patch_sptr> map2;
        for (unsigned kk = 0; kk < pv2.size(); kk++) 
          map2[pv2[kk]->id()] = pv2[kk];

        dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
        if (vul_file::exists(output_name_fine)) {
          vsl_b_ifstream imfs(output_name_fine.c_str());
          match->b_read(imfs);
          imfs.close();
          match->set_id_map1(map1);
          match->set_id_map2(map2);

          match->clear_sm_map_lists();  // take up too much space, we don't need them here!! we just need norm fine cost value

          //match_vec.push_back(match);
          match_vec_c.push_back(match);
          vcl_cout << "fine match_found!\n";
          
        } else if (vul_file::exists(output_name2_fine)) {
          vsl_b_ifstream imfs(output_name2.c_str());
          match->b_read(imfs);
          imfs.close();
          
          match->clear_sm_map_lists();  // take up too much space, we don't need them here!! we just need norm fine cost value

          dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
          new_match->resort_wrt_norm_cost();

          new_match->set_id_map1(map1);
          new_match->set_id_map2(map2);
          
          //match_vec.push_back(new_match);
          match_vec_c.push_back(new_match);

          vcl_cout << "fine match_found!\n";
#if 0
        } else if (vul_file::exists(output_name)) {
          vsl_b_ifstream imfs(output_name.c_str());
          match->b_read(imfs);
          imfs.close();
          
          match->set_id_map1(map1);
          match->set_id_map2(map2);
          
          for (unsigned mm = 0; mm < pv1.size(); mm++) {
            pv1[mm]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
            pv1[mm]->scurve_sample_ds_ = match->scurve_sample_ds_;
            pv1[mm]->elastic_splice_cost_ = match->elastic_splice_cost_;
          }

          for (unsigned mm = 0; mm < pv2.size(); mm++) {
            pv2[mm]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
            pv2[mm]->scurve_sample_ds_ = match->scurve_sample_ds_;
            pv2[mm]->elastic_splice_cost_ = match->elastic_splice_cost_;
          }

          vul_timer t;
          t.mark();
          match->compute_norm_fine_costs_of_cors();  // no need for sorting
          
          //match_vec.push_back(match);
          match_vec_c.push_back(match);
          
          vcl_cout << "match_found!\n";
          vcl_cout << "\t\t\t writing fine fine bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name_fine.c_str());
          match->b_write(bfs);
          bfs.close();
        } else if (vul_file::exists(output_name2)) { 
          vsl_b_ifstream imfs(output_name2.c_str());
          match->b_read(imfs);
          imfs.close();

          match->set_id_map1(map2);
          match->set_id_map2(map1);

          for (unsigned mm = 0; mm < pv1.size(); mm++) {
            pv1[mm]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
            pv1[mm]->scurve_sample_ds_ = match->scurve_sample_ds_;
            pv1[mm]->elastic_splice_cost_ = match->elastic_splice_cost_;
          }

          for (unsigned mm = 0; mm < pv2.size(); mm++) {
            pv2[mm]->shock_pruning_threshold_ = match->shock_pruning_threshold_;
            pv2[mm]->scurve_sample_ds_ = match->scurve_sample_ds_;
            pv2[mm]->elastic_splice_cost_ = match->elastic_splice_cost_;
          }
          match->compute_norm_fine_costs_of_cors();  // no need for sorting

          for (unsigned mm = 0; mm < pv2.size(); mm++) {
            pv2[mm]->kill_tree();
            pv2[mm]->kill_shock_graph();
          }

          vul_timer t;
          t.mark();
          vcl_cout << "\t\t\twriting fine bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name2_fine.c_str());
          match->b_write(bfs);
          bfs.close();
          
          dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
          new_match->resort_wrt_norm_cost();
          
          new_match->set_id_map1(map1);
          new_match->set_id_map2(map2);
          
          //match_vec.push_back(new_match);
          match_vec_c.push_back(new_match);
          vcl_cout << "match_found, reversing!\n";
#endif
        } else {
          //match_vec.push_back(0);
          match_vec_c.push_back(0);
          vcl_cout << "no match_found!\n";
        }

        for (unsigned iii = 0; iii < pv2.size(); iii++) {
         pv2[iii]->kill_tree();
         //pv2[iii]->kill_shock_graph();
        }

      }
      match_vec.push_back(match_vec_c);
      ins_names.push_back(ins_names_c);
    }

    /*if (match_vec.size() > 0) {
      //: given an image (patch set) find its best match among a set of matches
      int best_id = 0;
      if (best_match_norm_top_n(pv1, match_vec, n, best_id)) {
        vcl_cout << ins_name << "\n classified as " << class_names[best_id] << " best matching image: " << ins_names[best_id] << vcl_endl;
      } else {
        vcl_cout << "matches not complete for " << ins_name << vcl_endl;
      }
    } else {
      vcl_cout << "matches not complete for: " << ins_name << vcl_endl;
    }*/

    vcl_vector<vcl_vector<vcl_string> > patch_image_dirs;
    for (unsigned c = 0; c < cats.size(); c++) {
      vcl_vector<vcl_string> patch_image_dirs_c;
      for (unsigned cj = 1; cj < 11; cj++) {
        vcl_ostringstream oss2;
        oss2 << cj;
        vcl_string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;

        vcl_string patch_image_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/kept/";
        patch_image_dirs_c.push_back(patch_image_file2);
      }
      patch_image_dirs.push_back(patch_image_dirs_c);
    }


    vcl_vector<vcl_vector<vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > > best_category_instance_ids;
    int best_id;
    //if (match_strat1_simple_voting(pv1, match_vec, best_id)) {
    if (match_strat1_simple_voting_top_n(pv1, match_vec, best_id, n, visualization_n, best_category_instance_ids)) {
      vcl_cout << ins_name << " patches voted for category: " << best_id << ": " << cats[best_id] << vcl_endl;
    } else 
      vcl_cout << ins_name << " could not be classified yet\n";

    match_vec.clear();
    
    create_html_top_n(pv1, patch_images_dir1, best_category_instance_ids, ins_names, visualization_n, out_html_file, "class "+cats[outer_i]+" ins "+ins_name, patch_image_dirs);
    
    best_category_instance_ids.clear();

    for (unsigned mm = 0; mm < pv1.size(); mm++) {
      pv1[mm]->kill_tree();
      //pv1[mm]->kill_shock_graph();
    }
  //}


  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 9)
    main_eth(argc, argv);
  //else if (argc == 5)
  //  main_individual(argc, argv);
  else {
  //  individual_usage();
    eth_usage();
  }

  return 0;
}


