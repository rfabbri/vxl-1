// match shock patches 

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
//#include <vcl_sstream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_sm_cor.h>
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

#include <bsol/bsol_algs.h>

dbsk2d_shock_graph_sptr read_esf_from_file(vcl_string fname) {
  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  return sg;
}

vsol_box_2d_sptr read_bbox(vcl_string filename) {
  vcl_ifstream fp(filename.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< filename <<vcl_endl;
    return 0;
  } else {
    vcl_cout<<" Reading "<< filename <<vcl_endl;
  }
  char buffer[2000];

  //2)Read in file header.
  fp.getline(buffer,2000); //# PASCAL Annotation Version 1.00
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //Image size (X x Y x C) : 0 x 0 x 0
  fp.getline(buffer,2000); //Database : "The Oxford/Graz data extension"
  fp.getline(buffer,2000); //Objects with ground truth : 1 { "PASHorse" }
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Note that there might be other objects in the image
  fp.getline(buffer,2000); //# for which ground truth data has not been provided.
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Top left pixel co-ordinates : (1, 1)
  fp.getline(buffer,2000); //
  fp.getline(buffer,2000); //# Details for object 1 ("PASHorse")
  fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
  fp.getline(buffer,2000); //Original label for object 1 "PASHorse" : ""
  vcl_cout << " read: " << buffer << vcl_endl;
  
  //Bounding box for object 1 "PASHorse" (Xmin, Ymin) - (Xmax, Ymax) : (5, 7) - (231, 170)
  fp.getline(buffer,2000);
  vcl_string dum;
  vcl_istringstream oss(vcl_string(buffer).c_str());
  int min_x, min_y, max_x, max_y;
  char c;
  oss >> dum;  //b
  vcl_cout << dum << vcl_endl;
  oss >> dum;  //b
  vcl_cout << dum << vcl_endl;
  oss >> dum;  //f
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // ob
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // 1 
  vcl_cout << dum << vcl_endl;
  oss >> dum;  // "
  vcl_cout << dum << vcl_endl;
  oss >> dum; //(
  vcl_cout << dum << vcl_endl;
  oss >> dum; // Y
  vcl_cout << dum << vcl_endl;
  oss >> dum; // -
  vcl_cout << dum << vcl_endl;
  oss >> dum; // (
  vcl_cout << dum << vcl_endl;
  oss >> dum; // Y
  vcl_cout << dum << vcl_endl;
  oss >> dum; // :
  vcl_cout << dum << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> min_x;
  vcl_cout << "min_x: " << min_x << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> min_y;
  vcl_cout << "min_y: " << min_y << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> dum;
  vcl_cout << "dum: " << dum << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> max_x;
  vcl_cout << "max_x: " << max_x << vcl_endl;
  oss >> c;
  vcl_cout << "c: " << c << vcl_endl;
  oss >> max_y;
  vcl_cout << "max_y: " << max_y << vcl_endl;

  vcl_cout << "read minx: " << min_x << " miny: " << min_y << " max_x: " << max_x << " max_y " << max_y << vcl_endl;

  vsol_box_2d_sptr b = new vsol_box_2d();
  b->add_point(min_x, min_y);
  b->add_point(max_x, max_y);
  vcl_cout << "box minx: " << b->get_min_x() << " miny: " << b->get_min_y() << " max_x: " << b->get_max_x() << " max_y " << b->get_max_y() << vcl_endl;
  return b;
}

void individual_usage()
{
  vcl_cout << "\nIndividual Usage: <program name> <positive_list> <gt box list dir> <negative_list> <true_patches> <false_patches> <match_dir> <shock_curve_sample_ds> <similarity threshold> <top N> <1 if doing positives, 0 if negatives>\n";
  vcl_cout << "Last argument is 1 if matching with circular completions at the leaf scurves, otherwise 0 (open boundary matching)\n";
  vcl_cout << "Use the image patches with similarities less than threshold to their best matching model patch to create the detection bounding box\n";
  vcl_cout << "Use the image patches if only there are at least N different model patches that are matching to the patches of this object\n";
  //vcl_cout << "shock curve sample ds is typically 2.0f\n";
}

//: For sorting pairs by their second elements cost
inline bool
fine_cost_less( const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& left,
                const vcl_pair<dbskr_shock_patch_sptr, vcl_pair<int, dbskr_sm_cor_sptr> >& right )
{
  return (left.second.second)->final_norm_cost() < (right.second.second)->final_norm_cost();
}

int main_individual(int argc, char *argv[]) {
  vcl_cout << "Matching shock patches of the test image with the patches of the model, assuming the second shock patch storage is the model storage!!!\n";

  //: out file contains the wrong mathces if any
  vcl_string pos_file, gt_dir, neg_file, pos_patch_dir, neg_patch_dir, match_dir, sc_samp_ds_str;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 11) {
    individual_usage();
    return -1;
  }
  pos_file = argv[1];
  gt_dir = argv[2];
  neg_file = argv[3];
  pos_patch_dir = argv[4]; 
  neg_patch_dir = argv[5];
  match_dir = argv[6];
  sc_samp_ds_str = argv[7];
  float sc_samp_ds = (float)atof(argv[7]);
  float sim_threshold = (float)atof(argv[8]);
  int N = atoi(argv[9]);
  bool do_pos = (atoi(argv[10]) == 1 ? true : false);


  vcl_ifstream fpd((pos_file).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open pos file!\n";
    return -1;
  }
  
  vcl_vector<vcl_string> pos_names;
  while (!fpd.eof()) {
    vcl_string temp;
    fpd >> temp;
    if (temp.size() > 1) {
      pos_names.push_back(temp);
    }
  }
  fpd.close();

  
  vcl_ifstream fpn((neg_file).c_str());
  if (!fpn.is_open()) {
    vcl_cout << "Unable to open neg file!\n";
    return -1;
  }

  vcl_vector<vcl_string> neg_names;
  while (!fpn.eof()) {
    vcl_string temp;
    fpn >> temp;
    if (temp.size() > 1) {
      neg_names.push_back(temp);
    }
  }
  fpn.close();

  unsigned int Ps = pos_names.size();
  unsigned int Ns = neg_names.size();
  vcl_cout << "Ps: " << Ps << " Ns: " << Ns << "\n";
  vcl_cout << "printing pos list: \n";
  for (unsigned int i = 0; i<Ps; i++) {
    vcl_cout << pos_names[i] << "sss\n";
  } vcl_cout << "printing neg list: \n";
  for (unsigned int i = 0; i<Ns; i++) {
    vcl_cout << neg_names[i] << "sss\n";
  }

  vcl_string train_ins = "Horse_horse015-map";
  unsigned train_patch_id = 173205;
  vcl_vector<float> thresholds;
  for (float t = 0.2f; t <= sim_threshold; t += 0.05f) {
    thresholds.push_back(t);
  }
  vcl_cout << "using thresholds: ";
  for (unsigned t = 0; t < thresholds.size(); t++) {
    vcl_cout << thresholds[t] << " ";
  }
  vcl_cout << vcl_endl;
  
  if (do_pos) {
  //: load pos matches
  vcl_vector<dbskr_shock_patch_match_sptr> pos_matches;
  vcl_vector<vsol_box_2d_sptr> gt_boxes;
  for (unsigned i = 0; i < Ps; i++) {
    vcl_string match_file = match_dir + pos_names[i] + "-" + train_ins + "-match-" + sc_samp_ds_str + "-no-circ.bin";
    if (!vul_file::exists(match_file)) {
      vcl_cout << "pos match: " << match_file << " does not exist!!!\n Exiting!\n";
      return 0;
    }

    dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
    vsl_b_ifstream ifs(match_file.c_str());
    match->b_read(ifs);
    ifs.close();

    dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
    new_match->resort_wrt_norm_cost();
    new_match->clear_sm_map_lists();

    pos_matches.push_back(new_match);

    vcl_string box_file = gt_dir + pos_names[i] + ".txt";

    if (!vul_file::exists(box_file)) {
      vcl_cout << "box file: " << box_file << " does not exist!!!\n Exiting!\n";
      return 0;
    }

    vsol_box_2d_sptr box_gt = read_bbox(box_file);
    gt_boxes.push_back(box_gt);
  }
  vcl_cout << "read: " << gt_boxes.size() << " gt boxes and " << pos_matches.size() << " matches\n";

  //: detect objects in pos_matches
  vcl_vector<int> false_positive(thresholds.size(), 0); 
  vcl_vector<int> true_positive(thresholds.size(), 0);
  vcl_vector<float> avg_detect_box_overlap(thresholds.size(), 0);
  int patch_cnt = 0;
  for (unsigned i = 0; i < Ps; i++) {

    //: load the patches
    vcl_string st_file1 = pos_patch_dir + pos_names[i] + "-patches_color__6_18_3_2_0.5_0.8/"+pos_names[i]+"_patch_storage.bin";
    vcl_cout << "loading: " << st_file1 << vcl_endl;

    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();

    vcl_string str_name_end = "patch_storage.bin";

    //: read the esfs as well
    vcl_cout << st1->size() << " patches in str1, reading shocks..";
    patch_cnt += st1->size();
    vcl_map<int, dbskr_shock_patch_sptr> map1;
    //: load esfs for each patch
    for (unsigned iii = 0; iii < st1->size(); iii++) {
      dbskr_shock_patch_sptr sp = st1->get_patch(iii);
      map1[sp->id()] = sp;
      vcl_string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_xshock_graph_fileio file_io;
      dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
      if (!sg || !sg->number_of_vertices() || !sg->number_of_edges())
        continue;
      dbsk2d_compute_bounding_box(sg);
      sp->set_shock_graph(sg);
    }
    vcl_cout << " done\n";

    dbskr_shock_patch_match_sptr match = pos_matches[i];
    patch_cor_map_type map = match->get_map();
    match->set_id_map2(map1);
  
    for (unsigned t = 0; t < thresholds.size(); t++) {
      vcl_cout << "t:  " << thresholds[t] << vcl_endl;
      vcl_cout.flush();


      vsol_box_2d_sptr detection_box;

      vcl_map<int, int> model_ids;
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
    vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = map.begin();
    for ( ; iter != map.end(); iter++) {
      int id1 = iter->first;
      train_patch_vec = iter->second;
      //if (id1 == train_patch_id) {
      //  train_patch_vec = iter->second;
      //  break;
      //}
    //}

          
      if (train_patch_vec == 0 || !train_patch_vec->size()) {
        vcl_cout << "no matches for " << pos_matches[i] << " Exiting!!!\n";
        false_positive[t]++;
        continue;
      }

      int upper = N < int(train_patch_vec->size()) ? N : train_patch_vec->size();

      // check if all top N patches are giving consistent bboxes.
      vcl_vector<vsol_box_2d_sptr> top_boxes;
      for (unsigned iii = 0; int(iii) < upper; iii++) {
        vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];

        //vcl_cout << "best p id:  " << p.first << " cost: " << p.second->final_norm_cost() << vcl_endl;
        //vcl_cout.flush();
        
        if (p.second->final_norm_cost() <= thresholds[t]) {
          dbskr_shock_patch_sptr sp = map1[p.first];
          vsol_box_2d_sptr box = sp->shock_graph()->get_bounding_box(); 
          top_boxes.push_back(box);

          vcl_map<int, int>::iterator iter = model_ids.find(p.first);
          if (iter == model_ids.end()) {
            model_ids[p.first] = 1;
          } else
            iter->second = iter->second + 1;

          vcl_cout << "patch: " << sp->id() << " passed with " << p.second->final_norm_cost() << "\n";
          vcl_cout.flush();
        }   
      }

      if (top_boxes.size() == N && model_ids.size() > 1) {
        if (!detection_box)
          detection_box = new vsol_box_2d();
        for (int kk = 0; kk < N; kk++) {
          detection_box->grow_minmax_bounds(top_boxes[kk]);
        }
      }

    }

    vsol_box_2d_sptr inters;
    if (detection_box) {
      vcl_cout << "object detected at: " << detection_box->get_min_x() << " " << detection_box->get_min_y();
      vcl_cout << " " << detection_box->get_max_x() << " " << detection_box->get_max_y() << vcl_endl;
    }
    if (detection_box && bsol_algs::intersection(detection_box, gt_boxes[i], inters)) {
      vsol_box_2d_sptr uni;
      bsol_algs::box_union(detection_box, gt_boxes[i], uni);
      float ratio = (float)(inters->area()/uni->area());
      vcl_cout << " box overlap ratio: " << ratio << vcl_endl;
      if (ratio >= 0.5) {
        true_positive[t]++;
        avg_detect_box_overlap[t] += ratio;
      } else
        false_positive[t]++;
    } else
      false_positive[t]++;

    }

    st1 = 0;
    
    }

  for (unsigned t = 0; t < thresholds.size(); t++) {
    vcl_cout << "t: " << thresholds[t] << " total: " << gt_boxes.size() << " true_pos: " << true_positive[t] << " false_pos: " << false_positive[t] << " avg overlap ratio: " << avg_detect_box_overlap[t]/float(gt_boxes.size()) << vcl_endl;
  }

  for (unsigned t = 0; t < thresholds.size(); t++) 
    vcl_cout << true_positive[t] << "\t";
  vcl_cout << vcl_endl;
  
  for (unsigned t = 0; t < thresholds.size(); t++) 
    vcl_cout << false_positive[t] << "\t";
  vcl_cout << vcl_endl;
  
  for (unsigned t = 0; t < thresholds.size(); t++) 
    vcl_cout << avg_detect_box_overlap[t]/float(true_positive[t]) << vcl_endl;
  vcl_cout << vcl_endl;

  vcl_cout << "total # of patches: " << patch_cnt << vcl_endl;
  pos_matches.clear();
  gt_boxes.clear();

  } else {

  //: load neg matches
  vcl_cout << "loading neg matches\n";
  vcl_vector<dbskr_shock_patch_match_sptr> neg_matches;
  for (unsigned i = 0; i < Ns; i++) {
    vcl_string match_file1 = match_dir + neg_names[i] + "-" + train_ins + "-match-" + sc_samp_ds_str + "-no-circ.bin";

    dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();

    if (!vul_file::exists(match_file1)) { 
      vcl_cout << "neg match: " << match_file1 << " does not exist!!!\n Exiting!\n";
      return 0;  
    }
    vsl_b_ifstream ifs(match_file1.c_str());
    match->b_read(ifs);
    ifs.close();
    
    dbskr_shock_patch_match_sptr new_match = match->construct_match_just_cost();
    new_match->resort_wrt_norm_cost();
    new_match->clear_sm_map_lists();

    neg_matches.push_back(new_match);
  }

  vcl_cout << "read: " << neg_matches.size() << " matches\n";

  int patch_cnt = 0;
  //: detect objects in neg_matches
  vcl_vector<int> false_neg(thresholds.size(), 0); 
  vcl_vector<int> true_neg(thresholds.size(), 0);
  for (unsigned i = 0; i < Ns; i++) {

    //: load the patches
    vcl_string st_file1 = neg_patch_dir + "bg_graz_/" + neg_names[i] + "-patches_color__6_6_1_2_0.3_0.7/"+neg_names[i]+"_patch_storage.bin";
    if (!vul_file::exists(st_file1)) { 
      st_file1 = neg_patch_dir + "image_/" + neg_names[i] + "-patches_color__6_6_1_2_0.3_0.7/"+neg_names[i]+"_patch_storage.bin";
      if (!vul_file::exists(st_file1)) { 
        vcl_cout << "str file: " << st_file1 << " does not exist!!!\n Exiting!\n";
        return 0;
      }
    }
    vcl_cout << "loading: " << st_file1 << vcl_endl;

    dbskr_shock_patch_storage_sptr st1 = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file1.c_str());
    st1->b_read(ifs);
    ifs.close();

    vcl_string str_name_end = "patch_storage.bin";

    //: read the esfs as well
    vcl_cout << st1->size() << " patches in str1, reading shocks..\n";
    patch_cnt += st1->size();
    vcl_map<int, dbskr_shock_patch_sptr> map1;
    //: load esfs for each patch
    for (unsigned iii = 0; iii < st1->size(); iii++) {
      dbskr_shock_patch_sptr sp = st1->get_patch(iii);
      map1[sp->id()] = sp;
      vcl_string patch_esf_name = st_file1.substr(0, st_file1.length()-str_name_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_xshock_graph_fileio file_io;
      dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph(patch_esf_name);
      if (!sg || !sg->number_of_vertices() || !sg->number_of_edges())
        continue;
      dbsk2d_compute_bounding_box(sg);
      sp->set_shock_graph(sg);
    }

    dbskr_shock_patch_match_sptr match = neg_matches[i];
    patch_cor_map_type map = match->get_map();
    match->set_id_map2(map1);
  
    for (unsigned t = 0; t < thresholds.size(); t++) {

    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
    vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = map.begin();
    
    vcl_map<int, int> model_ids;
    for ( ; iter != map.end(); iter++) {
      int id1 = iter->first;
      //if (id1 == train_patch_id) {
        train_patch_vec = iter->second;
      //  break;
      //}
    //}
      
    if (train_patch_vec == 0 || !train_patch_vec->size()) {
      vcl_cout << "no matches for " << neg_matches[i] << " Exiting!!!\n";
      true_neg[t]++;  // if not able to match then its good
      continue;
    }

    //int upper = N < int(train_patch_vec->size()) ? N : train_patch_vec->size();
    if (int(train_patch_vec->size()) >= N) {
    //vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[upper-1];
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[N-1];
    if (p.second->final_norm_cost() <= thresholds[t]) {
      vcl_map<int, int>::iterator iter = model_ids.find(p.first);
      if (iter == model_ids.end()) {
        model_ids[p.first] = 1;
      } else
        iter->second = iter->second + 1;
    }
    }

    //was_true_neg = false;
    
   // for (unsigned iii = 0; int(iii) < upper; iii++) {
   //   vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];

    //  if (p.second->final_norm_cost() > thresholds[t]) {
        //true_neg[t]++;
   //     was_true_neg = true;
   //     break;
   //   }
      //dbskr_shock_patch_sptr sp = map1[p.first];
      //vsol_box_2d_sptr box = sp->shock_graph()->get_bounding_box(); 
   //   vcl_cout << "patch: " << p.first << " passed with " << p.second->final_norm_cost() << "\n";
      //vsol_box_2d_sptr detection_box = new vsol_box_2d();
  //  }
   
  //  if (!was_true_neg) {
  //    break;
   // }

    }
    if (model_ids.size() > 1) // at least two matches
      false_neg[t]++;
    else 
      true_neg[t]++;
    
    }

    st1 = 0;
  }

  for (unsigned t = 0; t < thresholds.size(); t++) {
    vcl_cout << "t: " << thresholds[t] << " total: " << neg_matches.size() << " true_neg: " << true_neg[t] << " false_neg: " << false_neg[t] << vcl_endl;
  }
  
  for (unsigned t = 0; t < thresholds.size(); t++) 
    vcl_cout << true_neg[t] << "\t";
  vcl_cout << vcl_endl;
  
  for (unsigned t = 0; t < thresholds.size(); t++) 
    vcl_cout << false_neg[t] << "\t";
  vcl_cout << vcl_endl;

  vcl_cout << "total # of patches: " << patch_cnt << vcl_endl;
  neg_matches.clear();

  }

  return 0;
}


int main(int argc, char *argv[]) {
  if (argc == 11)
    main_individual(argc, argv);
  else {
    individual_usage();
  }

  return 0;
}
