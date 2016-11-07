// This is contrib/mleotta/vidreg/vidreg_initializer.cxx

//:
// \file


#include "vidreg_initializer.h"
#include <vcl_algorithm.h>
#include <vnl/vnl_double_2.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_trans_translation.h>
#include <vidreg/vidreg_feature_pt_desc.h>



//: Constructor
vidreg_initializer::vidreg_initializer(const vcl_vector<rgrl_feature_sptr>& fixed,
                                       const vcl_vector<rgrl_feature_sptr>& moving,
                                       rgrl_view_sptr view)
  : prior_view_(view)
{
  if(fixed.empty() || moving.empty())
    return;

  DebugMacro(  1, "fixed size: "<<fixed.size()<<"  moving size: "<<moving.size()<<'\n' );

  vcl_vector<rgrl_feature_sptr> fixed_sorted(fixed);
  vcl_sort(fixed_sorted.begin(), fixed_sorted.end(),
           vidreg_feature_pt_desc::dec_mag_order);


  typedef vcl_vector<rgrl_feature_sptr>::const_iterator Fitr;
  // build the BBF search tree
  vcl_vector<vnl_vector_fixed<double,128> > descriptors;
  for(Fitr i=fixed_sorted.begin(); i!=fixed_sorted.end(); ++i){
    vidreg_feature_pt_desc* curr_pt = rgrl_cast<vidreg_feature_pt_desc*>(*i);
    if(curr_pt->descriptor().size() == 128)
      descriptors.push_back(curr_pt->descriptor());
    else
      break;
  }

  dbnl_bbf_tree<double,128> bbf_tree(descriptors);

  for(Fitr i=moving.begin(); i!=moving.end(); ++i){
    vidreg_feature_pt_desc* curr_pt = rgrl_cast<vidreg_feature_pt_desc*>(*i);
    if(curr_pt->descriptor().size() != 128)
      continue;

    //find the two closest matches
    vcl_vector<int> matches;
    bbf_tree.n_nearest(curr_pt->descriptor(), matches, 2, 50);
    vidreg_feature_pt_desc* m0 = rgrl_cast<vidreg_feature_pt_desc*>(fixed_sorted[matches[0]]);
    vidreg_feature_pt_desc* m1 = rgrl_cast<vidreg_feature_pt_desc*>(fixed_sorted[matches[1]]);
    double distance2 = vnl_vector_ssd(curr_pt->descriptor(),m0->descriptor());
    double next_distance2 = vnl_vector_ssd(curr_pt->descriptor(),m1->descriptor());
    if( distance2 < next_distance2*0.64){
      double dist = vcl_sqrt(distance2);
      double ratio = vcl_sqrt(next_distance2)/dist;
      matches_.push_back(new match(curr_pt,m0,dist,ratio));
    }
  }
  vcl_deque<match*>::iterator mid = matches_.begin() + matches_.size()/2;
  vcl_partial_sort(matches_.begin(), mid, matches_.end(), match_dist_less);
  while(matches_.end() != mid){
    delete matches_.back();
    matches_.pop_back();
  }

  DebugMacro(  1, "completed matching\n" );

}


//: Destructor
vidreg_initializer::~vidreg_initializer()
{
  for(vcl_deque<match*>::iterator i=matches_.begin(); i != matches_.end(); ++i)
    delete *i;
}


//: Get next initial view.
bool vidreg_initializer::next_initial(rgrl_view_sptr &view,
                                      rgrl_scale_sptr &prior_scale)
{
  if(matches_.empty())
    return false;

  match* m = matches_.front();
  matches_.pop_front();

  DebugMacro(  1, "selected feature: dist = "<<m->distance<<"  ratio = "<<m->dist_ratio << '\n' );


  if(!m || !m->from || !m->to)
    return false;

  rgrl_transformation_sptr xform = new rgrl_trans_translation(m->to->location()
                                                              - m->from->location());

  DebugMacro(  1, "Initial Xform = " << *xform <<'\n' );


  rgrl_mask_box global_region ( prior_view_->from_image_roi()->bounding_box() );

  //rgrl_mask_box initial_region( prior_view_->from_image_roi()->bounding_box() );
  double radius = 30;
  vnl_double_2 x0(m->from->location()), x1(m->from->location());
  x0 -= radius;
  x1 += radius;
  rgrl_mask_box initial_region(x0, x1);

  view = new rgrl_view( prior_view_->from_image_roi(),
                        prior_view_->to_image_roi(),
                        initial_region,
                        global_region,
                        prior_view_->xform_estimator(),
                        xform,
                        prior_view_->resolution() );

  prior_scale = prior_scale_;

  delete m;

  return true;
}


//: scan the queue of initial matches and remove those accounted for in the match_set
void
vidreg_initializer::remove_covered_matches(const rgrl_match_set_sptr& covered_set)
{
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  for(vcl_deque<match*>::iterator i=matches_.begin(); i != matches_.end(); ++i)
  {
    const rgrl_feature_sptr& from_feature = (*i)->from;
    const rgrl_feature_sptr& to_feature = (*i)->to;
    bool covered = false;
    for(from_iter f=covered_set->from_begin(); f!=covered_set->from_end(); ++f)
    {
      if(f.from_feature() != from_feature)
        continue;
      for(to_iter t=f.begin(); t!=f.end(); ++t){
        if(t.to_feature() == to_feature){
          covered = true;
          break;
        }
      }
      if(covered)
        break;
    }
    if(covered){
      delete *i;
      matches_.erase((--i)+1);
    }
  }
  vcl_cout << "Remaining Matches: "<<matches_.size() <<vcl_endl;
}


//: scan the queue of initial matches and remove those accounted for by xform
void
vidreg_initializer::remove_covered_matches(const rgrl_transformation& xform, double thresh)
{
  vcl_deque<match*> new_matches;
  for(vcl_deque<match*>::iterator i=matches_.begin(); i != matches_.end(); ++i)
  {
    rgrl_feature_sptr xformed_from = (*i)->from->transform(xform);
    assert(xformed_from);
    double error = (*i)->to->geometric_error(*xformed_from);
    if(error < thresh){
      delete *i;
    }
    else
      new_matches.push_back(*i);
  }
  matches_ = new_matches;
  vcl_cout << "Remaining Matches: "<<matches_.size() <<vcl_endl;
}


bool
vidreg_initializer::match_dist_less(const match* m0, const match* m1)
{
  return m0->distance < m1->distance;
}

