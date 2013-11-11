// classify shock patch sets

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


void eth_usage() {
  vcl_cout << "Usage : <program name> <image dir> <category name> <view_sptr> <patch_storage_dir> <match_output_dir> <scurve_sample_ds> <n>\n";
  vcl_cout << "Modify the source code to change parameters to construct patch storage name depending on class\n";
  vcl_cout << "scurve sample ds is typically 4 for coarse matching\n";
  vcl_cout << "n: use the top n best patch matches between the two patch sets\n";
}
int main_eth(int argc, char *argv[]) {
  
  vcl_string image_dir, patch_dir, match_dir, view_str, cat_str;

  if (argc != 8) {
    eth_usage();
    return -1;
  }
  
  image_dir = argv[1]; cat_str = argv[2]; view_str = argv[3]; patch_dir = argv[4]; match_dir = argv[5];
  float scurve_sample_ds = float(atof(argv[6]));
  int n = atoi(argv[7]);
  
  vcl_vector<vcl_string> cats, cat_par_strs;
  cats.push_back("cow");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("horse");  cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  cats.push_back("dog");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  //cats.push_back("car");    cat_par_strs.push_back("_3_9_3_2_0.3_0.8");
  //cats.push_back("apple");  cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  //cats.push_back("pear");   cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  //cats.push_back("cup");    cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
  //cats.push_back("tomato"); cat_par_strs.push_back("_2_6_2_1_0.3_0.8");
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
    
    vcl_cout << ins_name << vcl_endl;

    vcl_string st_file1 = patch_dir+cats[i]+"/"+ins_name+"-patches"+sorting_method_str+cat_par_strs[i]+"/"+ins_name+"_patch_storage.bin";
    // create the input storage class
    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();
    vcl_vector<dbskr_shock_patch_sptr>& pv1 = st1->get_patches();

    //: load the image
    vcl_string image_file = image_dir+cats[i]+oss.str()+"/"+ins_name+".png";
    vil_image_resource_sptr img = vil_load_image_resource(image_file.c_str());

    //: 
    bool use_intensity = true;
    bool use_gradient = true;
    bool use_color = false;


    //: set the parameters in the patches
    vcl_map<int, dbskr_shock_patch_sptr> map1;
    for (unsigned i = 0; i < pv1.size(); i++) {
      map1[pv1[i]->id()] = pv1[i];
      pv1[i]->extract_observation(img, use_intensity, use_gradient, use_color);
    }

    //: see if all the matches are available for this instance
    //vcl_vector<dbskr_shock_patch_match_sptr> match_vec;
    vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> > match_vec;
    vcl_vector<vcl_string> class_names;
    vcl_vector<vcl_string> ins_names;
    for (unsigned c = 0; c < cats.size(); c++) {
      
      vcl_vector<dbskr_shock_patch_match_sptr> match_vec_c;
      for (unsigned cj = 1; cj < 11; cj++) {
        class_names.push_back(cats[c]);
        
        vcl_ostringstream oss2;
        oss2 << cj;
        vcl_string ins_name2 = cats[c]+oss2.str()+"-"+view_str;
        if (ins_name == ins_name2)
          continue;
        ins_names.push_back(ins_name2);
        vcl_cout  << ins_name2 << " ";

        vcl_string st_file2 = patch_dir+cats[c]+"/"+ins_name2+"-patches"+sorting_method_str+cat_par_strs[c]+"/"+ins_name2+"_patch_storage.bin";
        // create the input storage class
        dbskr_shock_patch_storage_sptr st2 = dbskr_shock_patch_storage_new();
        vsl_b_ifstream ifs2(st_file2.c_str());
        st2->b_read(ifs2);
        ifs2.close();
        vcl_vector<dbskr_shock_patch_sptr>& pv2 = st2->get_patches();

        vcl_string output_name = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[6]+".bin";
        vcl_string output_name_fine = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[6]+"-fine.bin";
        //vcl_string output_name_info = ins_dir+ins_name+"-"+ins_name2+"-sc-"+argv[6]+"-info.bin";
        vcl_string output_name2 = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+".bin";
        vcl_string output_name2_fine = match_dir+cats[c]+"/"+ins_name2+"/"+ins_name2+"-"+ins_name+"-sc-"+argv[5]+"-fine.bin";


        //: load the image
        vcl_string image2_file = image_dir+cats[c]+oss2.str()+"/"+ins_name2+".png";
        vil_image_resource_sptr img2 = vil_load_image_resource(image2_file.c_str());

        vcl_map<int, dbskr_shock_patch_sptr> map2;
        for (unsigned k = 0; k < pv2.size(); k++) {
          map2[pv2[k]->id()] = pv2[k];
          pv2[k]->extract_observation(img2, use_intensity, use_gradient, use_color);
        }

        dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();

        if (vul_file::exists(output_name_fine)) {
          vsl_b_ifstream imfs(output_name_fine.c_str());
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

          match->compute_mutual_infos();
          match->clear_edit_costs();
          
          vcl_cout << "fine match_found!\n";
        } else if (vul_file::exists(output_name2_fine)) {
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

          match->compute_mutual_infos();
          match->clear_edit_costs();

          dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
          new_match->resort_wrt_info();

          match = new_match;

          vcl_cout << "fine match_found!\n";

        } else if (vul_file::exists(output_name)) {
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
           
          match->compute_mutual_infos();
          match->clear_edit_costs();

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

          match->compute_mutual_infos();
          match->clear_edit_costs();
          
          vul_timer t;
          t.mark();
          vcl_cout << "\t\t\twriting fine bin file total time: " << ((t.real()/1000.0f)/60.0f) << " mins.\n";
          vsl_b_ofstream bfs(output_name2_fine.c_str());
          match->b_write(bfs);
          bfs.close();
          
          dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
          new_match->resort_wrt_info();
          
          match = new_match;

          vcl_cout << "match_found, reversing!\n";
        } else {
          match = 0;
          vcl_cout << "no match_found!\n";
        }

        
        //match_vec.push_back(match);
        match_vec_c.push_back(match);

        for (unsigned i = 0; i < pv2.size(); i++) {
          pv2[i]->kill_tree();
          pv2[i]->kill_shock_graph();
        }

      }
      match_vec.push_back(match_vec_c);
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

    int best_id;
    if (match_strat1_simple_voting(pv1, match_vec, best_id, true)) { // use mutual info scores
      vcl_cout << ins_name << " patches voted for category: " << best_id << ": " << cats[best_id] << vcl_endl;
    } else 
      vcl_cout << ins_name << " could not be classified yet\n";


  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 8)
    main_eth(argc, argv);
  //else if (argc == 5)
  //  main_individual(argc, argv);
  else {
  //  individual_usage();
    eth_usage();
  }

  return 0;
}


