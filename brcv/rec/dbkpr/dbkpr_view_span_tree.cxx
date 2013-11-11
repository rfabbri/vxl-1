// This is brcv/rec/dbkpr/dbkpr_view_span_tree.cxx
//:
// \file

#include "dbkpr_view_span_tree.h"
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>
#include <vcl_set.h>
#include <vnl/vnl_double_2.h>

#include <mvl/FMatrixComputeRANSAC.h>


//: return the reverse link
// swap to and from, invert the map of matches, and tranpose F
dbkpr_view_span_link
dbkpr_view_span_link::reverse() const
{
  dbkpr_view_span_link rlnk(this->to, this->from);
  for( vcl_map<int,int>::const_iterator itr = this->matches.begin();
       itr != this->matches.end();  ++itr )
  {
    rlnk.matches[itr->second] = itr->first;
  }
  rlnk.F = this->F.transpose();
  return rlnk;
}


static bool links_match_size_less(const dbkpr_view_span_link& l1,
                                  const dbkpr_view_span_link& l2)
{
  return l1.matches.size() < l2.matches.size();
}

//: Constructor
dbkpr_view_span_tree::
dbkpr_view_span_tree( const vcl_vector<vcl_vector<dbdet_keypoint_sptr> >& keypoints,
                      unsigned int max_bins, double F_std, unsigned int num_ran_sam)
 : max_bins_(max_bins), std_(F_std), num_samples_(num_ran_sam),
   keypoints_(keypoints), links_(keypoints.size())
{
  // construct all of the bbf kd-trees
  for(unsigned int i=0; i<keypoints.size(); ++i){
    vcl_vector<vnl_vector_fixed<double,128> > descriptors(keypoints[i].size());
    for(unsigned int j=0; j<keypoints[i].size(); ++j)
      descriptors[j] = keypoints[i][j]->descriptor();
    bbf_trees_.push_back(descriptors);
    labels_.push_back(i);
  }
  vcl_cout << "build all trees" << vcl_endl;

  vcl_vector<dbkpr_view_span_link> links;
  unsigned int num_views = keypoints_.size();
  for(unsigned int i=0; i<num_views; ++i){
    vcl_cout << "matching " << i << vcl_endl;
    for(unsigned int j=i+1; j<num_views; ++j){
      links.push_back(dbkpr_view_span_link(i,j,tentative_matches(i,j)));
    }
  }
  vcl_sort(links.begin(), links.end(), links_match_size_less);
  while(!links.empty() && links.back().matches.size() > 20){
    dbkpr_view_span_link lnk = links.back();
    if(merge_labels(lnk.from,lnk.to)){
      vcl_cout << "linking " << lnk.from << " to " << lnk.to
               << " ("<< lnk.matches.size()<< " matches)"<< vcl_endl;
      apply_F_constraints(lnk);
      expand_matches(lnk);
      links_[lnk.from].push_back(lnk);
      links_[lnk.to].push_back(lnk.reverse());    
    }
    links.pop_back();
  }  
}


//: Traverse the tree computing a global set of correspondences
// The first index into the resulting vector is the frame number
// The second index is the keypoint number
// corresponding keypoints will have the same second index in all frames
// missing correspondencs are marked with NULL keypoint smart pointers
vcl_vector<vcl_vector<dbdet_keypoint_sptr> >
dbkpr_view_span_tree::global_correspondence() const
{
  // recursively build a matrix of match indices
  vcl_vector<vcl_vector<int> > global_indices(keypoints_.size());
  global_corr_helper(-1,0,global_indices);

  // look up the keypoints for each index
  vcl_vector<dbdet_keypoint_sptr> empty(global_indices[0].size(),NULL);
  vcl_vector<vcl_vector<dbdet_keypoint_sptr> > global_matches(global_indices.size(),empty);
  for(unsigned int i=0; i<global_indices.size(); ++i){
    for(unsigned int j=0; j<global_indices[i].size(); ++j){
      int ind = global_indices[i][j];
      if(ind >= 0)
        global_matches[i][j] = keypoints_[i][ind];
    }
  }

  return global_matches;
}


//: Traverse the tree computing a global set of correspondence points
vcl_vector<dbdet_keypoint_corr3d_sptr>
dbkpr_view_span_tree::global_corr3d_points() const
{
  // recursively build a matrix of match indices
  vcl_vector<vcl_vector<int> > global_indices(keypoints_.size());
  global_corr_helper(-1,0,global_indices);

  // look up the keypoints for each index
  vcl_vector<dbdet_keypoint_corr3d_sptr> global_matches;
  for(unsigned int j=0; j<global_indices[0].size(); ++j){
    dbdet_keypoint_corr3d_sptr kp_corr = new dbdet_keypoint_corr3d(0,0,0);
    for(unsigned int i=0; i<global_indices.size(); ++i){
      int ind = global_indices[i][j];
      if(ind >= 0)
        kp_corr->add_correspondence(keypoints_[i][ind],i);
    }
    if(kp_corr->size() > 1)
      global_matches.push_back(kp_corr);
  }

  return global_matches;
}


//: helper function for recursive correspondence building
void
dbkpr_view_span_tree::global_corr_helper(int parent, int current,
                                         vcl_vector<vcl_vector<int> >& corr) const
{
  for( vcl_vector<dbkpr_view_span_link>::const_iterator citr = links_[current].begin();
       citr != links_[current].end();  ++citr )
  {
    if(citr->to != parent){
      // make a copy of the current matches
      vcl_map<int,int> matches(citr->matches);
      // for each existing correspondence
      for( unsigned int i=0; i<corr[current].size(); ++i )
      {
        int ind = corr[current][i];
        // if we have a keypoint for this correspondence (from the parent)
        if(ind != -1){
          // find a match from the current to the child
          vcl_map<int,int>::iterator fitr = matches.find(ind);
          if(fitr != matches.end()){
            // add to the correspondece matrix and remove from the local map
            corr[citr->to][i] = fitr->second;
            matches.erase(fitr);
          }
        }
      }

      // make room for new matches
      int c = corr[current].size();
      for( unsigned int i=0; i<corr.size(); ++i)
        corr[i].resize(c+matches.size(),-1);
      // add new correspondences for the remaining matches
      for( vcl_map<int,int>::const_iterator mitr = matches.begin();
           mitr != matches.end();  ++mitr, ++c)
      {
        corr[current][c] = mitr->first;
        corr[citr->to][c] = mitr->second;
      }
    
      // process child recursively
      global_corr_helper(current, citr->to, corr);
    }
  }
}


vnl_matrix<int>
dbkpr_view_span_tree::match_matrix() const
{
  unsigned int num_views = keypoints_.size();
  vnl_matrix<int> M(num_views,num_views);
  for(unsigned int i=0; i<num_views; ++i){
    for(unsigned int j=0; j<num_views; ++j){
      vcl_cout << "matching " << i << ", "<< j << vcl_endl;
      vcl_map<int,int> matches = tentative_matches(i,j);
      M(i,j) = matches .size();
      vcl_cout << "  matches="<<matches.size()<<vcl_endl;
    }
  }
  return M;
}

static inline bool similar(double v1, double v2, double ratio=0.8)
{
  return (v1<v2)?(v1/v2 > ratio):(v2/v1 > ratio);
}

//: Search for additional consistent matches 
//  that may have been missed in tentative matching
void 
dbkpr_view_span_tree::expand_matches(dbkpr_view_span_link& link)
{
  vcl_vector<vnl_vector_fixed<double,2> > inlier_pos;
  vcl_vector<int> inlier_idx;
  const vcl_vector<dbdet_keypoint_sptr>& all_kpts_from = keypoints_[link.from];
  const vcl_vector<dbdet_keypoint_sptr>& all_kpts_to = keypoints_[link.to];
  vcl_set<int> unmatched_from, unmatched_to;
  for(int i=0; i< (int)all_kpts_from.size(); ++i) unmatched_from.insert(i);
  for(int i=0; i< (int)all_kpts_to.size(); ++i) unmatched_to.insert(i);
  for( vcl_map<int,int>::const_iterator itr = link.matches.begin();
       itr != link.matches.end();  ++itr )
  {
    const dbdet_keypoint_sptr& kp = all_kpts_from[itr->first];
    inlier_idx.push_back(itr->first);
    inlier_pos.push_back(vnl_double_2(kp->x(), kp->y()));
    unmatched_from.erase(itr->first);
    unmatched_to.erase(itr->second);
  }
  dbnl_bbf_tree<double,2> pos_bbf_tree(inlier_pos);
  
  // Consider all unmatched point in the first image   
  for( vcl_set<int>::iterator f_itr = unmatched_from.begin();
       f_itr != unmatched_from.end();  ++f_itr )
  {
    const dbdet_keypoint_sptr& from_kp = all_kpts_from[*f_itr];
    // compute the epipolar line in the second image
    vgl_homg_line_2d<double> e_line = 
      link.F.image2_epipolar_line(vgl_homg_point_2d<double>(*from_kp));
      
    // Find the three nearest matching point in the same image
    vcl_vector<int> indices;
    pos_bbf_tree.n_nearest(vnl_double_2(from_kp->x(),from_kp->y()), indices, 3);
    vcl_map<int,int>::const_iterator match_n1 = link.matches.find(inlier_idx[indices[0]]);
    vcl_map<int,int>::const_iterator match_n2 = link.matches.find(inlier_idx[indices[1]]);
    vcl_map<int,int>::const_iterator match_n3 = link.matches.find(inlier_idx[indices[2]]);
    
    
    double s11 = all_kpts_from[match_n1->first]->scale();
    double s12 = all_kpts_from[match_n2->first]->scale();
    double s13 = all_kpts_from[match_n3->first]->scale();
    double d11 = vgl_distance(*all_kpts_from[match_n1->first], *from_kp)/s11;
    double d12 = vgl_distance(*all_kpts_from[match_n2->first], *from_kp)/s12;
    double d13 = vgl_distance(*all_kpts_from[match_n3->first], *from_kp)/s13;
    double s21 = all_kpts_to[match_n1->second]->scale();
    double s22 = all_kpts_to[match_n2->second]->scale();
    double s23 = all_kpts_to[match_n3->second]->scale();
    double ds1 = s11/s21;
    double ds2 = s12/s22;
    double ds3 = s13/s23;
    
    for( vcl_set<int>::iterator t_itr = unmatched_to.begin();
         t_itr != unmatched_to.end();  ++t_itr )
    {
      const dbdet_keypoint_sptr& to_kp = all_kpts_to[*t_itr];
      double dist = vgl_distance(e_line, vgl_homg_point_2d<double>(*to_kp));
      if(dist > std_) continue;
      double ds = from_kp->scale()/to_kp->scale();
      if( !similar(ds,ds1) || !similar(ds,ds2) || !similar(ds,ds3) ) continue;
      double d21 = vgl_distance(*all_kpts_to[match_n1->second], *to_kp)/s21;
      double d22 = vgl_distance(*all_kpts_to[match_n2->second], *to_kp)/s22;
      double d23 = vgl_distance(*all_kpts_to[match_n3->second], *to_kp)/s23;
      if( similar(d11,d21) && similar(d12,d22) && similar(d13,d23) ){
        vcl_cout <<"found a match" << vcl_endl;
        link.matches.insert(vcl_pair<int,int>(*f_itr,*t_itr));
        break;
      }
    }
  }
}


//: compute the fundamental matrix and use it to constrain the matches
void
dbkpr_view_span_tree::apply_F_constraints(dbkpr_view_span_link& link)
{
  vcl_vector< vgl_homg_point_2d<double> > pts1, pts2;
  vcl_vector< int > ind1, ind2;
  FMatrixComputeRANSAC fransac(true, std_);
  for( vcl_map<int,int>::const_iterator itr = link.matches.begin();
       itr != link.matches.end();  ++itr )
  {
    ind1.push_back(itr->first);
    ind2.push_back(itr->second);
    pts1.push_back(vgl_homg_point_2d<double>(*keypoints_[link.from][itr->first]));
    pts2.push_back(vgl_homg_point_2d<double>(*keypoints_[link.to][itr->second]));
  }

  // take the best of 15
  int most_inliers = 0;
  vcl_vector<bool> best_inliers;
  for(unsigned int c=0; c<num_samples_; ++c){
    FMatrix F = fransac.compute(pts1,pts2);
    vcl_vector<bool> inliers = fransac.get_inliers();
    int in_count=0;
    for (unsigned int i=0; i<inliers.size(); ++i)
      if(inliers[i])
        ++in_count;
    if(in_count > most_inliers){
      link.F = F;
      best_inliers = inliers;
      most_inliers = in_count;
    }
  }
  
  
  link.matches.clear();
  for (unsigned int i=0; i<best_inliers.size(); ++i){
    if(best_inliers[i]){
      link.matches[ind1[i]] = ind2[i];
    }
  }
}


//: compute tentative matches between view \p v1 and view \p v2
//  using only keypoint descriptors
//  return a map of indices in v1 to indices in v2
vcl_map<int,int>
dbkpr_view_span_tree::tentative_matches( unsigned int v1, unsigned int v2 ) const
{
  const vcl_vector<dbdet_keypoint_sptr>& keys1 = keypoints_[v1];
  const vcl_vector<dbdet_keypoint_sptr>& keys2 = keypoints_[v2];
  const dbnl_bbf_tree<double,128>& bbf_tree = bbf_trees_[v2];

  vcl_map<int,int> matches;
  for (unsigned int i=0; i<keys1.size(); ++i){
    //find the two closest matches
    vcl_vector<int> indices;
    bbf_tree.n_nearest(keys1[i]->descriptor(), indices, 2, max_bins_);
    // check if the distance to the first is less than 80% of the distance to the second
    if( vnl_vector_ssd(keys1[i]->descriptor(),keys2[indices[0]]->descriptor()) <
        vnl_vector_ssd(keys1[i]->descriptor(),keys2[indices[1]]->descriptor())*.64){
      matches[i] = indices[0];
    }
  }
  return matches;
}


//: replace all instances of labels_[v2] in labels_ with labels_[v1]
// \return false if labels_[v1] == labels_[v2]
bool
dbkpr_view_span_tree::merge_labels(int v1, int v2)
{
  int curr = labels_[v1];
  int repl = labels_[v2];
  if(curr == repl)
    return false;
    
  for(unsigned int i=0; i<labels_.size(); ++i)
    if(labels_[i] == repl)
      labels_[i] = curr;
         
  return true;
}
