// match shock patches 

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_utilities.h>
#include <dbskr/dbskr_tree_edit_coarse.h>
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

int main(int argc, char *argv[]) {
  vcl_cout << "Matching shock patches!\n";

  //: out file contains the wrong mathces if any
  vcl_string st_file1, st_file2, output_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 6) {
    vcl_cout << "Usage: <program name> <shock_patch_storage1> <shock_patch_storage2> <patch id1> <patch id2> <out_file>\n";
    //vcl_cout << "polygon area threshold is typically 20\n";
    return -1;
  }
  st_file1 = argv[1];
  st_file2 = argv[2];
  int id1 = atoi(argv[3]);
  int id2 = atoi(argv[4]);
  output_file = argv[5];
  
  vcl_cout << "matching patch1: " << id1 << " to patch2: " << id2 << vcl_endl;

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
  match->scurve_sample_ds_ = 1.0f;
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

  for (unsigned i = 0; i < pv1.size(); i++) {  
    if (pv1[i]->id() != id1)
      continue;
    dbskr_shock_patch_sptr s1 = pv1[i];

    dbsk2d_shock_graph_sptr sg1;
  
  if (!s1->get_traced_boundary())
    s1->trace_outer_boundary();
  sg1 = s1->get_traced_shock();
  if (!sg1) {
    if (s1->extract_simple_shock()) 
      sg1 = s1->get_traced_shock();
    else {
      vcl_cout << "shock graph 1 is not available for patch: " << s1->id() << " skipping\n" << vcl_endl;
      return false;
    }
  }

  dbskr_tree_sptr tree1 = s1->tree();
  if (!tree1) {
    if (s1->prepare_tree()) {
      tree1 = s1->tree();
    } else {
      vcl_cout << "Tree is not available for patch: " << s1->id() << " skipping\n" << vcl_endl;
      return false;
    }
  }
  vcl_cout << "patch: " << s1->id() << " d: " << s1->depth() << "\n";

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> > * patch_v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
  for (unsigned j = 0; j < pv2.size(); j++) {
    if (pv2[j]->id() != id2)
      continue;

    dbsk2d_shock_graph_sptr sg2;
   
    if (!pv2[j]->get_traced_boundary())
      pv2[j]->trace_outer_boundary();
    sg2 = pv2[j]->get_traced_shock();
    if (!sg2) {
      if (pv2[j]->extract_simple_shock()) 
        sg2 = pv2[j]->get_traced_shock();
      else {
        vcl_cout << "shock graph 2 is not available for patch: " << pv2[j]->id() << " skipping\n" << vcl_endl;
        continue;
      }
    }

    dbskr_tree_sptr tree2 = pv2[j]->tree();
    if (!tree2) {
      if (pv2[j]->prepare_tree())
        tree2 = pv2[j]->tree();
      else {
        vcl_cout << "Tree 2 is not available for patch: " << pv2[j]->id() << " skipping\n" << vcl_endl;
        continue;
      }
    }
    vcl_cout << "\t\tpatch: " << pv2[j]->id() << " d: " << pv2[j]->depth() << " ";

    vul_timer t;
    t.mark();

    //instantiate the edit distance algorithm
    dbskr_tree_edit_coarse edit(tree1, tree2);

    edit.save_path(true);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      continue;
    }
    float val = edit.final_cost();
    vcl_cout << " cost: " << val << " time: "<< t.real()/1000.0f << " secs.\n";
    dbskr_sm_cor_sptr sm_cor = edit.get_correspondence_just_map();
    vcl_pair<int, dbskr_sm_cor_sptr> pp;
    pp.first = pv2[j]->id();
    pp.second = sm_cor;
    patch_v->push_back(pp);
    
  }

  //vcl_sort(patch_v->begin(), patch_v->end(), final_cost_less );
  map[s1->id()] = patch_v;
  
  }
   
  vcl_cout << "writing bin file\n";
  vsl_b_ofstream bfs(output_file.c_str());
  match->b_write(bfs);
  bfs.close();
  return 0;
}
