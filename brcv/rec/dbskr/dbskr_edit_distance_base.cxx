// This is rec/dbskr/dbskr_edit_distance_base.cxx


// \file
// 



#include "dbskr_edit_distance_base.h"
#include <dbskr/dbskr_directed_tree.h>
#include <vbl/vbl_array_1d.h>
#include <vcl_iostream.h>


#define NEWEPS     float(1e-5)
#define MIN_OF_THREE(X,Y,Z)  (isL((X),(Y),NEWEPS) ? (isL((X),(Z),NEWEPS) ? (X) : (Z)) : (isL((Y),(Z),NEWEPS) ? (Y) : (Z)))
#define MIN(X,Y)   (isL((X),(Y),NEWEPS) ? (X) : (Y))


//------------------------------------------------------------------------------
//:
void dbskr_edit_distance_base::clear() 
{
  pathtable_.clear();
  M_.clear();
  MP_.clear();
  C_.clear();
  CP_.clear();
  A_.clear();
  AP_.clear();
  shock_curve_map_.clear();
}


//------------------------------------------------------------------------------
//: edit tree1 and tree2 so that their topologies are identical with the least cost possible
bool dbskr_edit_distance_base::edit() 
{
  if (!tree1_ || !tree2_) 
  {
    vcl_cout << "trees not read!\n";
    return false;
  }

  size1_ = tree1_->size();
  size2_ = tree2_->size();

  int root1=0;

  //: Determine roots 
  //: Default is comparing the largest node vs centroid
  if ( root_node_selection_ == dbskr_edit_distance_base::DEFAULT)
  {
      root1  = 0;
      root2_ = tree2_->centroid();
  }
  else if ( root_node_selection_ == 
            dbskr_edit_distance_base::LARGEST_RADIUS_BOTH )
  {
      root1  = 0;
      root2_ = 0;

  }
  else if ( root_node_selection_ == 
            dbskr_edit_distance_base::CENTROID_BOTH )
  {

      root1  = tree1_->centroid();
      root2_ = tree2_->centroid();
  }

  vcl_vector<int>& out_darts_root2 = tree2_->out_darts(root2_); 
  
  //: set up flags in T2
  tree2_->set_up(out_darts_root2);

  //: set up flags in T1 such that dart 0's head is root
  vcl_vector<int> tmp2;
  tmp2.push_back(root1);
  tree1_->set_up(tmp2);

  //: order the sub problems in T1
  vcl_vector<int> a1_list = tree1_->order_subproblems();

  //: find special darts in T2
  vcl_vector<int> a2_list = tree2_->find_special_darts(out_darts_root2);

  //: initialize complete subproblem array to all zeros
  vcl_vector<float> tmp(size1_, 0);
  C_ = tmp;

  if (return_path_) 
  {
    for (int i = 0; i<size1_; i++) 
    {
      vcl_vector<pathtable_key> tmp;
      CP_.push_back(tmp);
    }
  }

  //: initialize M (vcl_vector<vcl_vector<float> > M_);
  for (int i = 0; i<size1_; i++) 
  {
    vcl_vector<float> tmp(size2_);
    M_.push_back(tmp);
  }

  if (return_path_) 
  {
    for (int i = 0; i<size1_; i++) 
    {
      vcl_vector<pathtable_key> tmp3;
      vcl_vector<vcl_vector<pathtable_key> > tmp2(size2_, tmp3);
      MP_.push_back(tmp2);
    }
  }

  //  if both are NOT leaf darts or both are leaf darts assign zero cost, otherwise assign infinity
  for (int i = 0; i<size1_; i++)
  {
    for (int j = 0; j<size2_; j++) 
    {
      if (tree1_->leaf(i) == tree2_->leaf(j)) 
      {
        M_[i][j] = MIN_VALUE;
      }
      else
      {
        M_[i][j] = LARGE;
      }
    }
  }

  //: take space for A (vcl_vector<vcl_vector<float> > A_);
  for (int i = 0; i<size1_; i++) 
  {
    vcl_vector<float> tmp(size2_);
    A_.push_back(tmp);
  }

  if (return_path_) 
  {
    for (int i = 0; i<size1_; i++) 
    {
      vcl_vector<pathtable_key> tmp3;
      vcl_vector<vcl_vector<pathtable_key> > tmp2(size2_, tmp3);
      AP_.push_back(tmp2);
    }
  }

  solve_incomplete_subproblems(a1_list, a2_list);
  solve_complete_subproblems();

#if 0
  vcl_string name = "mypathtable.txt";
  vcl_ofstream mypof(name.c_str(), vcl_ios::out);
  mypof << pathtable_.size() << vcl_endl;
  vcl_map<pathtable_key, float>::iterator iter;
  for(iter = pathtable_.begin(); iter != pathtable_.end(); iter++)
    {
        pathtable_key key = (*iter).first;
        float val = iter->second;
        //vcl_cout << "(" << key.first.first << ", " << key.first.second << "), ";
        mypof << key.first.first << " " << key.first.second << " ";
        //vcl_cout << "(" << key.second.first << ", " << key.second.second << ") is the key and its value is " << val << vcl_endl;    
        mypof << key.second.first << " " << key.second.second << " " << val << vcl_endl;    
    }
  //vcl_cout << "size of map: " << pathtable_.size() << vcl_endl;
#endif

  return true;
}


//------------------------------------------------------------------------------
//: incomplete subproblems are kept in M: 
//  M_[d1][d2] = rooted tree edit distance of T1(d1) (after tree of d1 in T1) and T2(d2) (after 
void dbskr_edit_distance_base::
solve_incomplete_subproblems(vcl_vector<int>& a1_list, vcl_vector<int>& a2_list) 
{
  if (return_path_) 
  {
    for (unsigned int i = 0; i<a2_list.size(); i++) 
    {
      int a2 = a2_list[i];
      for (unsigned int j = 0; j<a1_list.size(); j++) 
      {
        int a1 = a1_list[j];
        vcl_vector<int> tmp;
        tmp.push_back(tree1_->mate(tree1_->next(a1)));

        //: this sets the up flags in the appropriate subtree only
        tree1_->set_up(tmp);

        //: phase sets M[parent(a1)][parent(a2)]
        phase_path(a1, a2, false);
      }
    }
  } 
  else 
  {  
    for (unsigned int i = 0; i<a2_list.size(); i++) 
    {
      int a2 = a2_list[i];
      for (unsigned int j = 0; j<a1_list.size(); j++) 
      {
        int a1 = a1_list[j];
        vcl_vector<int> tmp;
        tmp.push_back(tree1_->mate(tree1_->next(a1)));
        
        //: this sets the up flags in the appropriate subtree only
        tree1_->set_up(tmp);
        
        //: phase sets M[parent(a1)][parent(a2)]
        phase(a1, a2, false);
      }
    }
  }
}


//------------------------------------------------------------------------------
//: solve the problem such that a1 is the last dart in the ES of tree1
//  and a2 is the last dart in ES of tree2 
//  if complete is true then solving a complete subproblem otherwise solving an incomplete subproblem
void dbskr_edit_distance_base::
phase_path(int a1, int a2, bool complete) 
{  
  // initialize_A();
  //  (little bit overhead for unused parts while solving incomplete problems)
  for (int i = 0; i<size1_; i++)
  {
    for (int j = 0; j<size2_; j++) 
    {
        A_[i][j] = LARGE;
        AP_[i][j].clear();
    }
  }
  
  int stop1 = (complete ? a1 : tree1_->parent_dart(a1));
  int stop2 = (tree2_->head(a2) == root2_ ? a2 : tree2_->parent_dart(a2));

  int d1 = a1;
  int d2;
  do 
  {
    d2 = a2;
    do 
    {
      subphase_path(a1, a2, d1, d2);
      d2 = tree2_->prev(d2);
    } 
    while (d2 != stop2);
    d1 = tree1_->prev(d1);
  } 
  while (d1 != stop1);
  
  return;
}

//------------------------------------------------------------------------------
//:
void dbskr_edit_distance_base::
phase(int a1, int a2, bool complete) 
{
  //initialize_A();
  //  (little bit overhead for unused parts while solving incomplete problems)
  for (int i = 0; i<size1_; i++)
  {
    for (int j = 0; j<size2_; j++) 
    {
        A_[i][j] = LARGE;
    }
  }
  
  int stop1 = (complete ? a1 : tree1_->parent_dart(a1));
  int stop2 = (tree2_->head(a2) == root2_ ? a2 : tree2_->parent_dart(a2));

  int d1 = a1;
  int d2;
  do 
  {
    d2 = a2;
    do 
    {
      subphase(a1, a2, d1, d2);
      d2 = tree2_->prev(d2);
    } while (d2 != stop2);
    d1 = tree1_->prev(d1);
  } 
  while (d1 != stop1);
  
  return;
}



//------------------------------------------------------------------------------
//: calls basic and just sets A array and M and C arrays if necessary
void dbskr_edit_distance_base::
subphase_path(int a1, int a2, int d1, int d2) 
{
  //: also sets path of (d1, d2) in AP_[d1, d2] before returning
  A_[d1][d2] = basic_path(a1, a2, d1, d2);

  //: see if we've finished solving the incomplete subproblem
  if (tree1_->skip_parent_dart_ccw(d1) == a1 &&
      tree2_->surrogate(tree2_->skip_parent_dart_ccw(d2)) == a2 &&
      tree2_->tail(d2) != root2_) 
  {    
    M_[tree1_->prev(d1)][tree2_->prev(d2)] = A_[d1][d2];
    MP_[tree1_->prev(d1)][tree2_->prev(d2)] = AP_[d1][d2];
  }

  //: (this might be unnecessarily setting C_ array while solving incomplete problems, CHECK!)
  if (tree1_->next(a1) == d1 &&
      tree2_->next(a2) == d2) 
  {
    C_[a1] = A_[d1][d2];
    CP_[a1] = AP_[d1][d2];
  }
  return;
}

//------------------------------------------------------------------------------
//:
void dbskr_edit_distance_base::
subphase(int a1, int a2, int d1, int d2) 
{
  //: also sets path of (d1, d2) in AP_[d1, d2] before returning
  A_[d1][d2] = basic(a1, a2, d1, d2);

  //: see if we've finished solving the incomplete subproblem
  if (tree1_->skip_parent_dart_ccw(d1) == a1 &&
      tree2_->surrogate(tree2_->skip_parent_dart_ccw(d2)) == a2 &&
      tree2_->tail(d2) != root2_) 
  {    
    M_[tree1_->prev(d1)][tree2_->prev(d2)] = A_[d1][d2];
  }

  //: (this might be unnecessarily setting C_ array while solving incomplete problems, CHECK!)
  if (tree1_->next(a1) == d1 &&
      tree2_->next(a2) == d2) 
  {
      C_[a1] = A_[d1][d2];
  }  
  return;
}


//------------------------------------------------------------------------------
//: this is the basic function which finds rooted tree edit distance of
//  T1[d1] (left tree of T1 wrt d1) and T2[d2] (left tree of T2 wrt d2)
//  such that a1 is the last dart in the ES of tree1
//  and a2 is the last dart in ES of tree2 (d1 and d2 are any darts in the ES as we go backwards
//  starting from a1 and a2)
//
//  if d1 == a1 and d2 == a2 then zero
//  if d1 is down and d2 == a2, or vice versa, then infinity
//  if d1 = a1 then don't try advancing d1
float dbskr_edit_distance_base::
basic_path(int a1, int a2, int d1, int d2) 
{    
  if (d1 == a1 && d2 == a2) 
    return MIN_VALUE;   // no need to set AP_[d1][d2], it'll stay as empty vector
  
  if (tree1_->up(d1) && d1 != a1) 
  {
    if (tree1_->head(a1) == tree1_->head(d1)) // check if edge to be left-spliced out is a 
    {                                          // sibling of d1's edge, then left splice is disallowed
      AP_[d1][d2] = AP_[tree1_->next(d1)][d2];
      return A_[tree1_->next(d1)][d2];
    } 
    else 
    {
      return maybe_left_splice_out_tree1_vec(d1, d2);  // also sets AP_
    }
  
  } 
  else // either d1 points down or d1 = a1
  {  
    if (tree2_->up(d2)) // don't advance d2 of d2 = a2
    {
      if (d2 == a2) 
        return LARGE;
      
      else // we can assume it is not the case that d2 = a2 and d1 = a1 because this case is treated above
      {      // Thus d1 is down
        if (tree2_->head(a2) == tree2_->head(d2)) 
        {
          AP_[d1][d2] = AP_[d1][tree2_->next(d2)];
          return A_[d1][tree2_->next(d2)];
        } 
        else
        {
          return maybe_left_splice_out_tree2_vec(d1, d2);
        }
      }

    } 
    else // d2 is down, and either d1 is down or d1 == a1
    { 
      if (d1 == a1) 
        return LARGE;

      //: first find the solution
      //  both d1 and d2 are down
      float val1 = witht(a1, a2, d1, d2, d1, d2);
      float val2 = maybe_right_splice_out_tree1(a1, d1, d2);   // calls contract()
      float val3 = maybe_right_splice_out_tree2(a2, d2, d1);   // calls contract()
      
      if (isL(val1,val2,NEWEPS)) 
      {
        if (isL(val1,val3,NEWEPS)) 
        {
          //: now call again to get the vector
          vcl_vector<pathtable_key> vec;
          witht(a1, a2, d1, d2, d1, d2, vec);
          AP_[d1][d2] = vec;
          return val1;
        } 
      } 
      else 
      {
        if (isL(val2,val3,NEWEPS)) 
        {
          AP_[d1][d2] = maybe_right_splice_out_tree1_vec(a1, d1, d2);
          return val2;
        }
      }
      AP_[d1][d2] = maybe_right_splice_out_tree2_vec(a2, d2, d1);
      return val3;
    }
  }
}


//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
basic(int a1, int a2, int d1, int d2) 
{    
  if (d1 == a1 && d2 == a2) 
    return MIN_VALUE;   // no need to set AP_[d1][d2], it'll stay as empty vector
  
  if (tree1_->up(d1) && d1 != a1) 
  {
    if (tree1_->head(a1) == tree1_->head(d1))  // check if edge to be left-spliced out is a 
    {                                          // sibling of d1's edge, then left splice is disallowed
      return A_[tree1_->next(d1)][d2];
    } 
    else
    {
      return maybe_left_splice_out_tree1(d1, d2);  // also sets AP_
    }
  } 
  else // either d1 points down or d1 = a1
  {  
    if (tree2_->up(d2)) // don't advance d2 of d2 = a2
    {
      if (d2 == a2) 
        return LARGE;
      else // we can assume it is not the case that d2 = a2 and d1 = a1 because this case is treated above
      {    // Thus d1 is down
        if (tree2_->head(a2) == tree2_->head(d2)) 
        {
          return A_[d1][tree2_->next(d2)];
        } 
        else
        {
          return maybe_left_splice_out_tree2(d1, d2);
        }
      }
    } 
    else // d2 is down, and either d1 is down or d1 == a1
    { 
      if (d1 == a1) 
        return LARGE;

      //: first find the solution
      //  both d1 and d2 are down
      float val1 = witht(a1, a2, d1, d2, d1, d2);
      float val2 = maybe_right_splice_out_tree1(a1, d1, d2);   // calls contract()
      float val3 = maybe_right_splice_out_tree2(a2, d2, d1);   // calls contract()

      return MIN_OF_THREE(val1, val2, val3);
    }
  }
}



//------------------------------------------------------------------------------
//: if next(d1) is down, there is a left branch of head(d1), try splicing it
float dbskr_edit_distance_base::
maybe_left_splice_out_tree1_vec(int d1, int d2) 
{
  int d = tree1_->next(d1);
  if (tree1_->up(d)) 
  {
    AP_[d1][d2] = AP_[d][d2];
    return A_[d][d2];
  }

  //: else d is left sibling of d1
  float val1 = A_[d][d2];
  int parent = tree1_->next(tree1_->mate(d));
  float val2 = A_[parent][d2] + tree1_->subtree_delete_cost(d);
  
  if (isL(val1,val2,NEWEPS)) 
  {
    AP_[d1][d2] = AP_[d][d2];
    return val1;
  } 
  else 
  {
    AP_[d1][d2] = AP_[parent][d2];
    return val2;
  }
}

//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::maybe_left_splice_out_tree1(int d1, int d2) 
{
  int d = tree1_->next(d1);
  if (tree1_->up(d)) 
  {
    return A_[d][d2];
  }

  //: else d is left sibling of d1
  float val1 = A_[d][d2];
  int parent = tree1_->next(tree1_->mate(d));
  float val2 = A_[parent][d2] + tree1_->subtree_delete_cost(d);
  
  return MIN(val1, val2);
}

//: if next(d2) is down, there is a left branch of head(d2), try splicing it
float dbskr_edit_distance_base::
maybe_left_splice_out_tree2_vec(int d1, int d2) 
{
  int d = tree2_->next(d2);
  if (tree2_->up(d)) 
  {
    AP_[d1][d2] = AP_[d1][d];
    return A_[d1][d];
  }

  //: else d is left sibling of d2
  float val1 = A_[d1][d];
  int parent = tree2_->next(tree2_->mate(d));
  float val2 = A_[d1][parent] + tree2_->subtree_delete_cost(d);
  
  if (isL(val1,val2,NEWEPS)) 
  {
    AP_[d1][d2] = AP_[d1][d];
    return val1;
  } 
  else 
  {
    AP_[d1][d2] = AP_[d1][parent];
    return val2;
  }
}


//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
maybe_left_splice_out_tree2(int d1, int d2) 
{
  int d = tree2_->next(d2);
  if (tree2_->up(d)) 
  {
    return A_[d1][d];
  }

  //: else d is left sibling of d2
  float val1 = A_[d1][d];
  int parent = tree2_->next(tree2_->mate(d));
  float val2 = A_[d1][parent] + tree2_->subtree_delete_cost(d);
  
  return MIN(val1, val2);
}


//------------------------------------------------------------------------------
//: find the min deformation cost in matching branches starting with down pointing darts d1 and d2
float dbskr_edit_distance_base::
witht(int a1, int a2, int d1, int d2, int td1, int td2) 
{
  float val1 = match(a1, a2, d1, d2, td1, td2);
  float val2 = merge_down_tree1(a1, a2, d1, d2, td1, td2);
  float val3 = merge_down_tree2(a1, a2, d1, d2, td1, td2);
  return MIN_OF_THREE(val1, val2, val3);
}


//------------------------------------------------------------------------------
//: find the min deformation cost in matching branches starting with down pointing darts d1 and d2
float dbskr_edit_distance_base::
witht(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) 
{
  vcl_vector<pathtable_key> vec1, vec2, vec3;
  float val1 = match(a1, a2, d1, d2, td1, td2, vec1);
  float val2 = merge_down_tree1(a1, a2, d1, d2, td1, td2, vec2);
  float val3 = merge_down_tree2(a1, a2, d1, d2, td1, td2, vec3);
  
  if (isL(val1,val2,NEWEPS)) 
  {
    if (isL(val1,val3,NEWEPS)) 
    {
      vec = vec1;
      return val1;
    }
  } 
  else 
  {
    if (isL(val2,val3,NEWEPS)) 
    {
      vec = vec2;
      return val2;
    }
  }
  vec = vec3;
  return val3;  
}


//------------------------------------------------------------------------------
//: compute cost of matching path in T1 from td1 through d1 
//                   against path in T2 from td2 through d2
float dbskr_edit_distance_base::
match(int a1, int a2, int d1, int d2, int td1, int td2) 
{
  //: cost of lower part comes from incomplete subproblem which is already solved
  float lower_val = M_[d1][d2];   // M_ holds the solution for T1(d1) (after tree of d1 in T1) and 
                                  //                           T2(d2) (after tree of d2 in T2)
  //: find cost of upper part
  int mate_td1 = tree1_->mate(td1);
  int newd1 = (a1 == mate_td1 ? a1 : tree1_->next(mate_td1));
  int mate_td2 = tree2_->mate(td2);
  int newd2 = (a2 == mate_td2 ? a2 : tree2_->next(mate_td2));
  float upper_val = A_[newd1][newd2];

  float match_cost = get_cost(td1, d1, td2, d2);
  /*
  pathtable_key key;
  key.first.first = tree1_->tail(td1);
  key.first.second = tree1_->head(d1);
  key.second.first = tree2_->tail(td2);
  key.second.second = tree2_->head(d2);

  float match_cost;
  vcl_map<pathtable_key, float>::iterator iter;
  iter = pathtable_.find(key);
  if (iter == pathtable_.end())
    match_cost = LARGE;
  else 
    match_cost = iter->second;
    */
  return lower_val + upper_val + match_cost;
}


//------------------------------------------------------------------------------
//: compute cost of matching path in T1 from td1 through d1 
//                   against path in T2 from td2 through d2
float dbskr_edit_distance_base::
match(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) 
{  
  //: find cost of upper part
  int mate_td1 = tree1_->mate(td1);
  int newd1 = (a1 == mate_td1 ? a1 : tree1_->next(mate_td1));
  int mate_td2 = tree2_->mate(td2);
  int newd2 = (a2 == mate_td2 ? a2 : tree2_->next(mate_td2));
  float upper_val = A_[newd1][newd2];

  for (unsigned int i = 0; i<AP_[newd1][newd2].size(); i++) 
    vec.push_back(AP_[newd1][newd2][i]);
  
  //: keep darts instead of nodes in the path
  pathtable_key path;
  path.first.first = td1;
  path.first.second = d1;
  path.second.first = td2;
  path.second.second = d2;

  vec.push_back(path);

  float match_cost = get_cost(td1, d1, td2, d2);
  
  //: cost of lower part comes from incomplete subproblem which is already solved
  float lower_val = M_[d1][d2];   // M_ holds the solution for T1(d1) (after tree of d1 in T1) and 
                                  //                           T2(d2) (after tree of d2 in T2)
  for (unsigned int i = 0; i<MP_[d1][d2].size(); i++) 
  {
    vec.push_back(MP_[d1][d2][i]);
  }
  return lower_val + upper_val + match_cost;
}



//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2) 
{
  vcl_vector<int>& ch = tree1_->children(d1);
  float min_cost = LARGE;

  // changing the order of children processing
  for (int i = ch.size()-1; i>=0; i--) 
  {
    int d = ch[i];
    float cost = witht(a1, a2, d, d2, td1, td2);
    
    // just add to this cost the cost of splicing siblings of d
    for (unsigned int j = 0; j<ch.size(); j++) 
    {
      if (ch[j] == d) 
        continue;
      cost += tree1_->subtree_delete_cost(ch[j]);
    }

    if (isL(cost,min_cost,NEWEPS)) 
    {
      min_cost = cost;
    }
  }
  return min_cost;
}

//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) 
{  
  vcl_vector<int>& ch = tree1_->children(d1);
  float min_cost = LARGE;
  vcl_vector<pathtable_key> tmp;
  vcl_vector<vcl_vector<pathtable_key> > big_tmp(ch.size(), tmp);
  int min_id = -1;

  for (int i = ch.size()-1; i>=0; i--) 
  {
    int d = ch[i];
    float cost = witht(a1, a2, d, d2, td1, td2, big_tmp[i]);
    
    // just add to this cost the cost of splicing siblings of d
    for (unsigned int j = 0; j<ch.size(); j++) 
    {
      if (ch[j] == d) continue;
      cost += tree1_->subtree_delete_cost(ch[j]);
    }

    if (isL(cost,min_cost,NEWEPS)) 
    {
      min_cost = cost;
      min_id = i;
    }
  }
  if (min_id >= 0)
  {
    vec = big_tmp[min_id];
  }
  return min_cost;
}

//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2) 
{
  vcl_vector<int>& ch = tree2_->children(d2);
  float min_cost = LARGE;
  for (int i = ch.size()-1; i>=0; i--) 
  {
    int d = ch[i];
    float cost = witht(a1, a2, d1, d, td1, td2);
    
    // just add to this cost the cost of splicing siblings of d
    for (unsigned int j = 0; j<ch.size(); j++) 
    {
      if (ch[j] == d) continue;
      cost += tree2_->subtree_delete_cost(ch[j]);
    }

    if (isL(cost,min_cost,NEWEPS))
    {
      min_cost = cost;
    }
  }
  return min_cost;
}


//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) 
{
  vcl_vector<int>& ch = tree2_->children(d2);
  float min_cost = LARGE;
  vcl_vector<pathtable_key> tmp;
  //vcl_vector<vcl_vector<pathtable_key> > big_tmp(ch.size(), tmp);
  vbl_array_1d<vcl_vector<pathtable_key> > big_tmp(ch.size(), tmp);
  int min_id = -1;
  for (int i = ch.size()-1; i>=0; i--) {
    int d = ch[i];
    //vcl_vector<pathtable_key> tmp;  
    float cost = witht(a1, a2, d1, d, td1, td2, big_tmp[i]);
    
    // just add to this cost the cost of splicing siblings of d
    for (unsigned int j = 0; j<ch.size(); j++) {
      if (ch[j] == d) continue;
      cost += tree2_->subtree_delete_cost(ch[j]);
    }

    //if (cost < min_cost) {
    if (isL(cost,min_cost,NEWEPS)) {
      min_cost = cost;
      //vec = tmp;
      min_id = i;
    }
  }
  if (min_id >= 0)
    vec = big_tmp[min_id];
  return min_cost;
}



//------------------------------------------------------------------------------
//: this operation is only legal if d1 has a left sibling d, and if d's head is not a leaf,
//  and d's tail has degree at least three
//
//  This procedure splices out the right branch assuming the parent edge was contracted, not matched.
//  It checks this by checking whether the head of a1 is the tail of d1.
//  After splicing off the right branch, it contracts the remaining child edge d.
//  For this reason, it must also check that d's head is not a leaf.
//  CAUTION: if we restrict contractions to involve high degree, this gets more complicated.
float dbskr_edit_distance_base::
maybe_right_splice_out_tree1(int a1, int d1, int d2) 
{
  int d = tree1_->next(tree1_->mate(d1));
  if (!tree1_->up(d) &&                       // if d is up, d1 has no left sibling
      tree1_->head(a1) != tree1_->tail(d1) && // illegal after parent edge matched
      !tree1_->leaf(d))                       // d must be contracted, so not a leaf 
  {                     
    //: case: no right splice
    float val1 = A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
    //: case: right splice the branch of d1 (then contract d, its left sibling)
    float val2 = A_[tree1_->next(d)][d2] + tree1_->subtree_delete_cost(d1) + tree1_->contract_cost(d);
    
    return MIN(val1, val2);
  
  } 
  else // right splice is not legal
  { 
    return A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
  }
}

//------------------------------------------------------------------------------
//:
vcl_vector<pathtable_key>& dbskr_edit_distance_base::
maybe_right_splice_out_tree1_vec(int a1, int d1, int d2) 
{
  int d = tree1_->next(tree1_->mate(d1));
  if (!tree1_->up(d) &&                       // if d is up, d1 has no left sibling
      tree1_->head(a1) != tree1_->tail(d1) && // illegal after parent edge matched
      !tree1_->leaf(d))                       // d must be contracted, so not a leaf 
  {                     
    //: case: no right splice
    float val1 = A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
    
    //: case: right splice the branch of d1 (then contract d, its left sibling)
    float val2 = A_[tree1_->next(d)][d2] + tree1_->subtree_delete_cost(d1) + tree1_->contract_cost(d);
    
    if (isL(val1,val2,NEWEPS)) 
    {
      return AP_[tree1_->next(d1)][d2];
    } 
    else 
    {
      return AP_[tree1_->next(d)][d2];
    }
  
  } 
  else // right splice is not legal
  { 
    return AP_[tree1_->next(d1)][d2];
  }
}


//------------------------------------------------------------------------------
//:
float dbskr_edit_distance_base::
maybe_right_splice_out_tree2(int a2, int d2, int d1) 
{
  int d = tree2_->next(tree2_->mate(d2));
  if (!tree2_->up(d) &&                       // if d is up, d1 has no left sibling
      tree2_->head(a2) != tree2_->tail(d2) && // illegal after parent edge matched
      !tree2_->leaf(d))                       // d must be contracted, so not a leaf 
  {                     
    //: case: no right splice
    float val1 = A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
    //: case: right splice the branch of d2 (then contract d, its left sibling)
    float val2 = A_[d1][tree2_->next(d)] + tree2_->subtree_delete_cost(d2) + tree2_->contract_cost(d);
    return MIN(val1, val2);
  } 
  else // right splice is not legal
  { 
    return A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
  }
}


//------------------------------------------------------------------------------
//:
vcl_vector<pathtable_key>& dbskr_edit_distance_base::
maybe_right_splice_out_tree2_vec(int a2, int d2, int d1) 
{
  int d = tree2_->next(tree2_->mate(d2));
  if (!tree2_->up(d) &&                       // if d is up, d1 has no left sibling
      tree2_->head(a2) != tree2_->tail(d2) && // illegal after parent edge matched
      !tree2_->leaf(d))                       // d must be contracted, so not a leaf 
  {                     
    //: case: no right splice
    float val1 = A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
    
    //: case: right splice the branch of d2 (then contract d, its left sibling)
    float val2 = A_[d1][tree2_->next(d)] + tree2_->subtree_delete_cost(d2) + tree2_->contract_cost(d);
    
    //return MIN(val1, val2);
    //if (val1 < val2) {
    if (isL(val1,val2,NEWEPS)) 
    {
      return AP_[d1][tree2_->next(d2)];
    } 
    else 
    {
      return AP_[d1][tree2_->next(d)];
    }
  } 
  else // right splice is not legal
  { 
    return AP_[d1][tree2_->next(d2)];
  }
}


//------------------------------------------------------------------------------
//: initially do not save the paths even if user requires them, but then resolve the best case to 
//  get the paths
void dbskr_edit_distance_base::
solve_complete_subproblems() 
{
  vcl_vector<int> tmp = tree2_->out_darts(root2_);
  int a2 = tree2_->mate(tmp[0]);

  //: first solve without keeping the paths
  for (int a1 = 0; a1<size1_; a1++) 
  {
    vcl_vector<int>& tmp = tree1_->out_darts(tree1_->head(a1));
    tree1_->set_up(tmp);
    phase(a1, a2, true);
  }

  final_cost_ = C_[C_.size()-1];
  final_a1_ = C_.size()-1;

  for (int i = C_.size()-2; i>=0; i--) 
  {
    if (isL(C_[i],final_cost_,NEWEPS)) 
    {
      final_cost_ = C_[i];
      final_a1_ = i;
    }
  }

  //: now solve again to find the path if the user wants it
  if (return_path_) 
  {
    vcl_vector<int>& tmp = tree1_->out_darts(tree1_->head(final_a1_));
    tree1_->set_up(tmp);
    phase_path(final_a1_, a2, true);

    if (C_[final_a1_] != final_cost_) 
    {
      vcl_cout << "PROBLEM in dbskr_edit_distance_base::solve_complete_subproblems(): "
        << "final_cost_ (" << final_cost_ << ") is dif then C_[final_a1_] (" 
        << C_[final_a1_] << ")\n";
    }
  }
  
  return;
}



