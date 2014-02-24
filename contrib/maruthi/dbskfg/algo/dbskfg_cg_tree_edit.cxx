// This is rec/dbskr/dbskfg_cg_tree_edit.cxx


//:
// \file
// 



#include "dbskfg_cg_tree_edit.h"

#include <dbskr/dbskr_directed_tree.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>
#include <dbskr/dbskr_sc_pair.h>
#include <dbskr/dbskr_sm_cor.h>

#include <vbl/vbl_array_1d.h>

#include <vcl_cstdio.h>


//------------------------------------------------------------------------------
//: Constructor
dbskfg_cg_tree_edit::
dbskfg_cg_tree_edit(const dbskr_tree_sptr& tree1, 
    const dbskr_tree_sptr& tree2, 
    bool construct_circular_ends, bool localize_match)
{
  //: handle the VERY special case of FIRST tree's being a single leaf
  //  in this case, if the second tree is not a single leaf, then we fix its root
  //  with a node having three children, and since the algorithm does not allow
  //  splicing of a branch from the root, the first leaf tree cannot match to the
  //  second tree properly. So in this case, switch the trees so that leaf tree could match
  //  properly.
  if (tree1->size() == 2 && tree2->size() != 2) 
  {
    switched_ = true;
    //vcl_cout << "SPECIAL CASE: Switching trees since the first tree is a leaf\n";
    this->set_tree2(tree1);
    this->set_tree1(tree2);
  } 
  else 
  {
    switched_ = false;
    this->set_tree1(tree1);
    this->set_tree2(tree2);
  }
  return_path_ = false;
  final_a1_ = -1;

  LARGE = 1e7;
  MIN_VALUE = 0.0f;
  LARGE_COST_RATIO = 0.3f;
  LARGE_BOUNDARY_COST = 150.0f;
  VERY_LARGE_BOUNDARY_COST = 300.0f;

  construct_circular_ends_ = construct_circular_ends;  // default is true, assuming simple closed curves are being matched
  curve_matching_R_ = 6.0f; // default value
  localize_match_ = localize_match;
  use_approx_ = true; //default value
}

//------------------------------------------------------------------------------
//: Constructor
dbskfg_cg_tree_edit::
dbskfg_cg_tree_edit(const dbskr_directed_tree_sptr& tree1, 
                const dbskr_directed_tree_sptr& tree2, 
                bool construct_circular_ends, bool localize_match)
{
    //: handle the VERY special case of FIRST tree's being a single leaf
    //  in this case, if the second tree is not a single leaf, then we fix its root
    //  with a node having three children, and since the algorithm does not allow
    //  splicing of a branch from the root, the first leaf tree cannot match to the
    //  second tree properly. So in this case, switch the trees so that leaf tree could match
    //  properly.
    if (tree1->size() == 2 && tree2->size() != 2) 
    {
        switched_ = true;
        //vcl_cout << "SPECIAL CASE: Switching trees since the first tree is a leaf\n";
        tree2_ = tree1.ptr();//this->set_tree2(tree1);
        tree1_ = tree2.ptr();//this->set_tree1(tree2);
    } 
    else 
    {
        switched_ = false;
        tree1_ = tree1.ptr();//this->set_tree1(tree1);
        tree2_  = tree2.ptr();//this->set_tree2(tree2);
    }
    return_path_ = false;
    final_a1_ = -1;

    LARGE = 1e7;
    MIN_VALUE = 0.0f;
    LARGE_COST_RATIO = 0.3f;
    LARGE_BOUNDARY_COST = 150.0f;
    VERY_LARGE_BOUNDARY_COST = 300.0f;

    construct_circular_ends_ = construct_circular_ends;  // default is true, assuming simple closed curves are being matched
    curve_matching_R_ = 6.0f; // default value
    localize_match_ = localize_match;
    use_approx_ = true; //default value




}


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//: Get tree1
dbskr_tree_sptr dbskfg_cg_tree_edit::
tree1() const
{
  return static_cast<dbskr_tree* >(this->tree1_.ptr());
}


//------------------------------------------------------------------------------
//: Set tree 1
void dbskfg_cg_tree_edit::
set_tree1(const dbskr_tree_sptr& tree1) 
{
  this->tree1_ = tree1.ptr();
}

  
//------------------------------------------------------------------------------
//: Set tree 2
void dbskfg_cg_tree_edit::
set_tree2(const dbskr_tree_sptr& tree2)
{
  this->tree2_ = tree2.ptr();
}


//------------------------------------------------------------------------------
//: Get tree2
dbskr_tree_sptr dbskfg_cg_tree_edit::
tree2() const
{
  return static_cast<dbskr_tree* >(this->tree2_.ptr());
}




//------------------------------------------------------------------------------
//: Read path
bool dbskfg_cg_tree_edit::
read_data(vcl_string treefile1, vcl_string treefile2, vcl_string pathtable_file, 
          bool elastic_splice_cost) 
{
  if (!this->tree1()->acquire(treefile1)) // dpmatch_combined = false
  {  
    vcl_cout << "Problems in reading tree1 from file " << treefile1 << "\n";
    return false;
  }

  if (!this->tree2()->acquire(treefile2)) 
  {
    vcl_cout << "Problems in reading tree2 from file " << treefile2 << "\n";
    return false;
  }

  //: read path table
  vcl_ifstream tf(pathtable_file.c_str());
  
  if (!tf) 
  {
    vcl_cout << "Unable to open path table file " << pathtable_file << vcl_endl;
    return false;
  }

  int cnt;
  tf >> cnt;
  //vcl_cout << "we have " << cnt << " lines\n";

  for (int i = 0; i<cnt; i++) 
  {
    pathtable_key key, key2;
    tf >> key.first.first;
    tf >> key.first.second;
    tf >> key.second.first;
    tf >> key.second.second;
    float val;
    tf >> val;
    pathtable_[key] = val;
    key2.first.first = key.first.second;
    key2.first.second = key.first.first;
    key2.second.first = key.second.second;
    key2.second.second = key.second.first;
    pathtable_[key2] = val;
  }
  return true;
}
//PATHTABLE map is returning 0 if the key is not in the map check to see if its there and return infinity if not













//------------------------------------------------------------------------------
//: Compute deformation cost between two paths on the two trees
float dbskfg_cg_tree_edit::
get_cost(int td1, int d1, int td2, int d2) 
{  
    pathtable_key key;
    key.first.first = tree1_->tail(td1);
    key.first.second = tree1_->head(d1);
    key.second.first = tree2_->tail(td2);
    key.second.second = tree2_->head(d2);

    pathtable_key key2;
    key2.first.first = key.first.second;
    key2.first.second = key.first.first;
    key2.second.first = key.second.second;
    key2.second.second = key.second.first;

    float match_cost;
    vcl_map<pathtable_key, float>::iterator iter;
    iter = pathtable_.find(key);   // searching for either key or key2 is enough
    if (iter == pathtable_.end()) 
    {
        dbskr_sc_pair_sptr curve_pair1 = tree1_->get_curve_pair(td1, d1, construct_circular_ends_);
        dbskr_sc_pair_sptr curve_pair2 = tree2_->get_curve_pair(td2, d2, construct_circular_ends_);
        dbskr_scurve_sptr sc1 = curve_pair1->coarse;
        dbskr_scurve_sptr sc2 = curve_pair2->coarse;

        dbskr_dpmatch d(sc1, sc2);
        d.set_R(curve_matching_R_);

        // test if it is worth computing this match
        double approx_cost = d.approx_cost();
    
        // Amir: This is risky: it should be a function of the size of the shapes
        if ((approx_cost > VERY_LARGE_BOUNDARY_COST) && use_approx_)
        {
            match_cost = LARGE;
        }
        else if ((approx_cost > LARGE_BOUNDARY_COST) && use_approx_)
        {
            match_cost = float(4*approx_cost);
        }
        else 
        {
            //: test if it worths computing this match
            float cost1 = tree1_->get_splice_cost_for_merge(td1, d1);
            float cost2 = tree2_->get_splice_cost_for_merge(td2, d2);
            if ((cost1 > LARGE_COST_RATIO*tree1_->total_splice_cost() || cost2 > LARGE_COST_RATIO*tree2_->total_splice_cost()) && use_approx_) 
            {
                match_cost = LARGE;
            }
            else
            {
                d.Match();
                float init_dr = d.init_dr();
                float init_alp = d.init_phi();
                match_cost = float(d.finalCost() + init_dr + init_alp);

                vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());    

                if (localize_match_ && fmap.size()>3) 
                {
                    dbskr_localize_match lmatch(sc1, sc2, 
                                                curve_pair1->dense, curve_pair2->dense, 
                                                curve_pair1->c_d_map, curve_pair2->c_d_map, 
                                                fmap, (float)curve_matching_R_);
                    lmatch.match();
                    match_cost = lmatch.finalCost() + init_dr + init_alp;
                }

                //: shock curve map is using darts as key
                pathtable_key key_scm;
                key_scm.first.first = td1;
                key_scm.first.second = d1;
                key_scm.second.first = td2;
                key_scm.second.second = d2;
                pathtable_key key2_scm;
                key2_scm.first.first = key_scm.first.second;
                key2_scm.first.second = key_scm.first.first;
                key2_scm.second.first = key_scm.second.second;
                key2_scm.second.second = key_scm.second.first;
                shock_curve_map_[key_scm] = fmap;
                shock_curve_map_[key2_scm] = fmap;
            }

        }

        //: CAUTION we may choose not to cache in the first case as this might make the pathtable huge!!
        pathtable_[key] = match_cost;
        pathtable_[key2] = match_cost;
    } 
    else 
    {
        match_cost = iter->second;
    } 
    return match_cost;
}









//------------------------------------------------------------------------------
//: compare costs 
bool dbskfg_cg_tree_edit::
compare_cost(vcl_string shgm_filename) 
{
  vcl_ifstream tf(shgm_filename.c_str());
  
  if (!tf) 
  {
    vcl_cout << "Unable to open shgm file " << shgm_filename << vcl_endl;
    return false;
  }

  char buffer[1000];
  tf.getline(buffer, 1000);  // version
  tf.getline(buffer, 1000);  // name 1
  vcl_string line = buffer;
  tf.getline(buffer, 1000);  // name 2
  line = buffer;

  float cost;
  tf >> cost;
  if (vcl_abs(final_cost_ - cost) > 0.001) 
  {
    vcl_cout << "!!!!!!! cost in the file: " << cost << " final_cost: " << final_cost_ << "\n";
    tf.close();
    return false;
  }

  tf.close();
  return true;
}







//------------------------------------------------------------------------------
//: write shgm output file
bool dbskfg_cg_tree_edit::
write_shgm(vcl_string shgm_filename) 
{  
  if (!return_path_) 
  {
    vcl_cout << "Paths are not computed, set flag to true using save_path() before edit()!" << vcl_endl;
    return false;
  }

  vcl_ofstream tf(shgm_filename.c_str());
  
  if (!tf) 
  {
    vcl_cout << "Unable to open shgm file " << shgm_filename << " for write " << vcl_endl;
    return false;
  }

  tf << "match output version 1.0\n";
  // names are unknown to this method but they are unimportant so just output the following
  // to comply with the format of the shgm file
  tf << "name1.shg\n";
  tf << "name2.shg\n";
  tf << final_cost_ << vcl_endl;

  for (int i = CP_[final_a1_].size()-1; i>=0; i--) 
  {
    pathtable_key key = CP_[final_a1_][i];
    //: find node paths
    vcl_vector<int> nodes1 = tree1_->find_node_path(key.first.first, key.first.second);
    vcl_vector<int> nodes2 = tree2_->find_node_path(key.second.first, key.second.second);

    if (switched_) 
    {
      for (unsigned int j = 0; j<nodes2.size(); j++) 
        tf << nodes2[j] << " ";
      tf << vcl_endl;

      for (unsigned int j = 0; j<nodes1.size(); j++) 
      {
        tf << nodes1[j] << " ";
      }
      tf << vcl_endl;
      tf << vcl_endl;
    } 
    else 
    {
      for (unsigned int j = 0; j<nodes1.size(); j++) 
        tf << nodes1[j] << " ";
      tf << vcl_endl;

      for (unsigned int j = 0; j<nodes2.size(); j++) 
        tf << nodes2[j] << " ";
      tf << vcl_endl;
      tf << vcl_endl;    
    }
  }

  tf.close();
  return true;
}







//------------------------------------------------------------------------------
//: compare path 
bool dbskfg_cg_tree_edit::
compare_path(vcl_string shgm_filename, double precision) 
{
  if (!return_path_) 
  {
    vcl_cout << "Paths are not computed, set flag to true using save_path() before edit()!" << vcl_endl;
    return false;
  }

  vcl_ifstream tf(shgm_filename.c_str());
  
  if (!tf) 
  {
    vcl_cout << "Unable to open shgm file " << shgm_filename << vcl_endl;
    return false;
  }

  char buffer[1000];
  tf.getline(buffer, 1000);  // version
  tf.getline(buffer, 1000);  // name 1
  vcl_string line = buffer;
  tf.getline(buffer, 1000);  // name 2
  line = buffer;

  float cost;
  tf >> cost;
  bool tag = true;
  if (vcl_abs(final_cost_ - cost) > precision) 
  {
    vcl_cout << "!!!!!!! cost in the file: " << cost << " final_cost: " << final_cost_ << "\n";
    tag = false;
  }

  vcl_map<vcl_string, vcl_string> paths;
  vcl_string line2;
  while (!tf.eof()) {
    tf.getline(buffer, 1000);
    line = buffer;
    if (line.length() < 2) // this is new line character or eof
      continue;
    // read the second line
    tf.getline(buffer, 1000);
    line2 = buffer;
    if (line2.length() < 2) {  // something is wrong with this file
      vcl_cout << "T1 and T2 corresponding paths are not on consecutive lines in this file\n!!";
      return false;
    }

    paths[line] = line2;
  }

  vcl_map<vcl_string, vcl_string>::iterator iter;
#if 0
  vcl_cout << "\nPRINTING RESULTS in SHGM FILE\n";
  for (iter = paths.begin(); iter!=paths.end(); iter++) {
    vcl_cout << "T1's path: " << iter->first << " corresponding path in T2: " << iter->second << "\n";
  }
#endif

  //: order in CP_ is NOT the same with the order in file!!!
  for (int i = CP_[final_a1_].size()-1; i>=0; i--) {
    pathtable_key key = CP_[final_a1_][i];
    //vcl_cout << "(" << key.first.first << ", " << key.first.second << "), ";
    //vcl_cout << "(" << key.second.first << ", " << key.second.second << ") " << vcl_endl;
    //: find node paths
    vcl_vector<int> nodes1 = tree1_->find_node_path(key.first.first, key.first.second);
    vcl_vector<int> nodes2 = tree2_->find_node_path(key.second.first, key.second.second);

#if 0
    vcl_cout << "\nPRINTING ALGORITHM RESULTS\n";
    for (unsigned int j = 0; j<nodes1.size(); j++) 
      vcl_cout << nodes1[j] << " ";
    vcl_cout << vcl_endl;
    for (unsigned int j = 0; j<nodes2.size(); j++) 
      vcl_cout << nodes2[j] << " ";
    vcl_cout << vcl_endl;
    vcl_cout << vcl_endl;
#endif

    //: makes this into strings and search in paths
    char buffer[1000];
    line = "";
    for (unsigned int j = 0; j<nodes1.size(); j++) {
      vcl_sprintf(buffer, "%d", nodes1[j]);
      line = line + buffer + " ";
    }

    line2 = "";
    for (unsigned int j = 0; j<nodes2.size(); j++) {
      vcl_sprintf(buffer, "%d", nodes2[j]);
      line2 = line2 + buffer + " ";
    }
    
    iter = paths.find(line);
    if (iter == paths.end()) {
      // search for the path in the reverse order as well
      line = "";
      for (int j = nodes1.size()-1; j>=0; j--) {
        vcl_sprintf(buffer, "%d", nodes1[j]);
        line = line + buffer + " ";
      }

      line2 = "";
      for (int j = nodes2.size()-1; j >= 0; j--) {
        vcl_sprintf(buffer, "%d", nodes2[j]);
        line2 = line2 + buffer + " ";
      }

      iter = paths.find(line);
      if (iter == paths.end()) {
        vcl_cout << "could not find " << line << " nor its reverse in the .shgm file\n";
        tag = false;
      }

    }
    
    if (iter != paths.end() && line2 != iter->second) {
      vcl_cout << "T1's path " << line << " corresponds to " << iter->second << " in the .shgm file,  ";
      vcl_cout << "however, algorithm returned " << line2 << "\n";
      tag = false;
    }

  }

  //vcl_cout << "At this point paths are identical!\n";
  tf.close();
  return tag;
}






//------------------------------------------------------------------------------
//: return the corresponding shock curves and their maps
bool dbskfg_cg_tree_edit::
get_correspondence_just_map_helper(vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                                   vcl_vector< pathtable_key >& path_map)
{
  if (final_a1_ < 0) 
  {
    vcl_cout << "Paths are not computed, call edit()!" << vcl_endl;
    return false;
  }

  map_list.clear();

  for (int i = CP_[final_a1_].size()-1; i>=0; i--) 
  {
    //: CP holds darts
    pathtable_key key = CP_[final_a1_][i];

    dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, construct_circular_ends_);

    if (switched_) 
    {
      //: redo the matching
      dbskr_dpmatch d(sc2, sc1);
      d.set_R(curve_matching_R_);
      d.Match();
      vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
      map_list.push_back(fmap);

      pathtable_key k;
      k.first.first = key.second.first;
      k.first.second = key.second.second;
      k.second.first = key.first.first;
      k.second.second = key.first.second;
      path_map.push_back(k);

    } 
    else 
    {
      path_map.push_back(key);
      vcl_map<pathtable_key, vcl_vector < vcl_pair <int,int> > >::iterator iter;
      //: shock_curve_map_ also uses darts as key (NOT NODES as in pathtable_)
      iter = shock_curve_map_.find(key);   // searching for either key or key2 is enough
      if (iter == shock_curve_map_.end()) {  // redo the matching, its not saved!!
        //vcl_cout << "!!!!!!!!!!!!1MISMATCH IN shock_curve_map_\n";
        dbskr_dpmatch d(sc1, sc2);
        d.set_R(curve_matching_R_);
        d.Match();
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
        map_list.push_back(fmap);

      } 
      else 
      {
        map_list.push_back(iter->second);
      }
    }
  }

  return true;
}



//------------------------------------------------------------------------------
//: return the corresponding shock curves and their maps
bool dbskfg_cg_tree_edit::
get_correspondence(vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                   vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                   vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                   vcl_vector< pathtable_key >& path_map) 
{
  if (final_a1_ < 0) 
  {
    vcl_cout << "Paths are not computed, call edit()!" << vcl_endl;
    return false;
  }

  curve_list1.clear();
  curve_list2.clear();
  map_list.clear();

  for (int i = CP_[final_a1_].size()-1; i>=0; i--) 
  {
    //: CP holds darts
    pathtable_key key = CP_[final_a1_][i];

    dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, construct_circular_ends_);

    if (switched_) 
    {
      curve_list1.push_back(sc2);
      curve_list2.push_back(sc1);
      //: redo the matching
      dbskr_dpmatch d(sc2, sc1);
      d.set_R(curve_matching_R_);
      d.Match();
      vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
      map_list.push_back(fmap);

      pathtable_key k;
      k.first.first = key.second.first;
      k.first.second = key.second.second;
      k.second.first = key.first.first;
      k.second.second = key.first.second;
      path_map.push_back(k);

    } 
    else 
    {
      curve_list1.push_back(sc1);
      curve_list2.push_back(sc2);

      path_map.push_back(key);

      vcl_map<pathtable_key, vcl_vector < vcl_pair <int,int> > >::iterator iter;
      //: shock_curve_map_ also uses darts as key (NOT NODES as in pathtable_)
      iter = shock_curve_map_.find(key);   // searching for either key or key2 is enough
      if (iter == shock_curve_map_.end()) {  // redo the matching, its not saved!!
        //vcl_cout << "!!!!!!!!!!!!1MISMATCH IN shock_curve_map_\n";
        dbskr_dpmatch d(sc1, sc2);
        d.set_R(curve_matching_R_);
        d.Match();
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
        map_list.push_back(fmap);

      } 
      else 
      {
        map_list.push_back(iter->second);
      }
    }
  }

  return true;
}





//------------------------------------------------------------------------------
//: Return correspondence between two trees
dbskr_sm_cor_sptr dbskfg_cg_tree_edit::
get_correspondence(bool save_pathtable) 
{
  dbskr_sm_cor_sptr cor;

  //
  if (switched_) 
  {
    cor = new dbskr_sm_cor(tree2(), tree1());
  }
  else
  {
    cor = new dbskr_sm_cor(tree1(), tree2());
  }

  vcl_vector<dbskr_scurve_sptr>& curve_list1 = cor->get_curve_list1();
  vcl_vector<dbskr_scurve_sptr>& curve_list2 = cor->get_curve_list2();
  vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list = cor->get_map_list();
  vcl_vector< pathtable_key >& path_map = cor->get_map();


  if (!get_correspondence(curve_list1, curve_list2, map_list, path_map))
    return 0;
  
  if (save_pathtable)
    cor->save_pathtable(pathtable_);

  cor->set_final_cost(final_cost_);
  cor->set_tree_edit_params(tree1()->scurve_sample_ds(),  // assuming both trees have been constructed with the same sample_ds_, interpolate_ds and elastic_splice_cost
                            tree1()->interpolate_ds(),
                            localize_match_, 
                            tree1()->elastic_splice_cost(), 
                            construct_circular_ends_, 
                            false, 
                            false, 
                            curve_matching_R_); 
  return cor;
}




//------------------------------------------------------------------------------
//: Return correspondence between the two shock trees
dbskr_sm_cor_sptr dbskfg_cg_tree_edit::
get_correspondence_just_map()
{
  dbskr_sm_cor_sptr cor = new dbskr_sm_cor();
  
  vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list = cor->get_map_list();
  vcl_vector< pathtable_key >& path_map = cor->get_map();
  if (!get_correspondence_just_map_helper(map_list, path_map))
    return 0;

  cor->set_final_cost(final_cost_);
  cor->set_tree_edit_params(tree1()->scurve_sample_ds(), tree1()->interpolate_ds(), 
    localize_match_, tree1()->elastic_splice_cost(),                        
    construct_circular_ends_, false, false, curve_matching_R_);
  return cor;
}


//------------------------------------------------------------------------------
//: create and write table file in advance (for debugging purposes!!)
bool dbskfg_cg_tree_edit::
populate_table(vcl_string fname) 
{
  if (!tree1_ || !tree2_) 
  {
    vcl_cout << "No trees available!\n";
    return false;
  }

  int n1=tree1_->node_size();
  int n2=tree2_->node_size();

  vcl_vector<int> n11Array;
  vcl_vector<int> n12Array;
  vcl_vector<int> n21Array;
  vcl_vector<int> n22Array;
  vcl_vector<float> costArray;

  float finalCost;

  float totalSpliceCost1= tree1()->total_splice_cost();
  float totalSpliceCost2= tree2()->total_splice_cost();
  int n11,n12,n21,n22;

  for (n11=0;n11<n1;n11++)
  {
    for (n12=0;n12<n1;n12++)
    {

      if(n11 == n12)
        continue;

      vcl_vector<int>& p1 = tree1()->get_dart_path_from_nodes(n11,n12);
      int td1 = p1[0];
      int d1 = p1[p1.size()-1];
      float cost1=tree1_->get_splice_cost_for_merge(td1, d1);
      if (cost1 < LARGE_COST_RATIO*totalSpliceCost1) 
      {

        for (n21=0;n21<n2;n21++) 
        {
          for (n22=n21+1;n22<n2;n22++) 
          {
            vcl_vector<int>& p2=tree2()->get_dart_path_from_nodes(n21,n22);
            int td2 = p2[0];
            int d2 = p2[p2.size()-1];
            float cost2=tree2_->get_splice_cost_for_merge(td2, d2);
            if (cost2 < LARGE_COST_RATIO*totalSpliceCost2) 
            {
              finalCost = get_cost(td1, d1, td2, d2);

              if (finalCost >= 0)
              {
                n11Array.push_back(n11);
                n12Array.push_back(n12);
                n21Array.push_back(n21);
                n22Array.push_back(n22);
                costArray.push_back(finalCost);
              }
            }
          }
        } //end of loop over the second tree
      }
    }
  }//end of loop over the first tree
 
  
  vcl_FILE *tfp=vcl_fopen(fname.c_str(),"w");
    vcl_cout << costArray.size() << " " << n11Array.size() << " "
   <<  n12Array.size() << " " << n21Array.size() << " "
   <<  n22Array.size() << vcl_endl;
  int csize=costArray.size();
  vcl_fprintf(tfp,"%d\n",csize);
  for (unsigned int i=0;i<costArray.size();i++)
    vcl_fprintf(tfp,"%2d %2d %2d %2d %6.2f\n",n11Array[i],n12Array[i],
      n21Array[i],n22Array[i],costArray[i]);
  vcl_fprintf(tfp,"\n");
  vcl_fclose(tfp);

  return true;
}



