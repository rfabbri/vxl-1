// This is brcv/rec/dbkpr/dbkpr_view_span_tree.h
#ifndef dbkpr_view_span_tree_h_
#define dbkpr_view_span_tree_h_
//:
// \file
// \brief Compute a spanning tree over views by matching keypoints
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/1/05
//
// \verbatim
//  Modifications
//  Dec 15 2006 Ricardo Fabbri - smart pointer support
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <dbdet/dbdet_keypoint_sptr.h>
#include <dbdet/dbdet_keypoint_corr3d_sptr.h>
#include <dbnl/algo/dbnl_bbf_tree.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vnl/vnl_matrix.h>
#include <mvl/FMatrix.h>


class dbkpr_view_span_link
{
 public:
  //: Default Constructor
  dbkpr_view_span_link()
   : from(-1), to(-1) {}
  //: Constructor
  dbkpr_view_span_link(int f, int t)
   : from(f), to(t) {}
  //: Constructor
  dbkpr_view_span_link(int f, int t, const vcl_map<int,int>& m)
   : from(f), to(t), matches(m) {}

  //: return the reverse link
  // swap to and from, invert the map of matches, and reverse F
  dbkpr_view_span_link reverse() const;
 
  //: the link is from this frame number
  int from;
  //: the link is to this frame number
  int to;
  //: map of index matches for keypoints
  vcl_map<int,int> matches;
  //: the fundamental matrix estimate
  FMatrix F;
};


//: A spanning tree of views
class dbkpr_view_span_tree : public vbl_ref_count
{
 public:
  //: Constructor
  dbkpr_view_span_tree(const vcl_vector<vcl_vector<dbdet_keypoint_sptr> >& keypoints,
                       unsigned int max_bins = 50, double F_std = 1.0,
                       unsigned int num_ran_sam = 20);
  //: Destructor
  ~dbkpr_view_span_tree(){}

  vnl_matrix<int> match_matrix() const;

  //: Traverse the tree computing a global set of correspondences
  // The first index into the resulting vector is the frame number
  // The second index is the keypoint number
  // corresponding keypoints will have the same second index in all frames
  // missing correspondencs are marked with NULL keypoint smart pointers
  vcl_vector<vcl_vector<dbdet_keypoint_sptr> > global_correspondence() const;

  //: Traverse the tree computing a global set of correspondence points
  vcl_vector<dbdet_keypoint_corr3d_sptr> global_corr3d_points() const;
  
  //: For each node (view) a vector of links to neighbor nodes
  const vcl_vector<vcl_vector<dbkpr_view_span_link> > &links() const {return links_; }
  
  //: All the keypoints for each view
  // keypoints()[view_index][point_index]
  const vcl_vector<vcl_vector<dbdet_keypoint_sptr> > & keypoints() const { return keypoints_;}
  

 private:
  //: helper function for recursive correspondence building
  void global_corr_helper(int parent, int current,
                          vcl_vector<vcl_vector<int> >& corr) const;
  
  //: Search for additional consistent matches 
  //  that may have been missed in tentative matching
  void expand_matches(dbkpr_view_span_link& link);
  
  //: compute the fundamental matrix and use it to constrain the matches
  void apply_F_constraints(dbkpr_view_span_link& link);
  
  //: compute tentative matches between view \p v1 and view \p v2
  //  using only keypoint descriptors
  //  return a map of indices in v1 to indices in v2
  vcl_map<int,int> tentative_matches( unsigned int v1, unsigned int v2 ) const;

  //: replace all instances of labels_[v2] in labels_ with labels_[v1]
  // \return false if labels_[v1] == labels_[v2]
  bool merge_labels(int v1, int v2);

  //: maximum number of bins used in BBF searches
  unsigned int max_bins_;

  //: The maximum distance allowed between a point and an epipolar line
  double std_;

  //: Number of random samples to take in RANSAC
  unsigned int num_samples_;
                         
  //: A BBF KD-tree for the points in each view
  vcl_vector<dbnl_bbf_tree<double,128> > bbf_trees_;

  //: All the keypoints for each view
  // keypoints_[view_index][point_index]
  vcl_vector<vcl_vector<dbdet_keypoint_sptr> > keypoints_;

  //: Labels for each frame to determine connected components
  vcl_vector<int> labels_;

  //: For each node a vector of links to neighbor nodes
  vcl_vector<vcl_vector<dbkpr_view_span_link> > links_;
};

#endif // dbkpr_view_span_tree_h_
