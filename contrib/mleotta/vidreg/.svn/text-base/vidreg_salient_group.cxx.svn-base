// This is contrib/mleotta/vidreg/vidreg_salient_group.cxx
#include "vidreg_salient_group.h"
//:
// \file

#include <rgrl/rgrl_view.h>
#include <vidreg/vidreg_feature_edgel.h>
#include <vidreg/vidreg_feature_pt_desc.h>


//: Constructor
vidreg_salient_group::vidreg_salient_group(const rgrl_view_sptr& view,
                                           const vcl_vector<rgrl_match_set_sptr>& matches,
                                           const vcl_vector<rgrl_scale_sptr>& scales,
                                           const rgrl_converge_status_sptr& status)
  : view_(view),
    matches_(matches),
    scales_(scales),
    status_(status),
    prev_group_(NULL)
{
}


//: Constructor
vidreg_salient_group::vidreg_salient_group(const vidreg_feature_group& init_group)
  : view_(NULL),
    matches_(0),
    scales_(0),
    status_(NULL),
    prev_group_(NULL)
{
  assert(init_group.edgels.size() == init_group.edgel_weights.size()
         || init_group.edgel_weights.empty());
  assert(init_group.corners.size() == init_group.corner_weights.size()
         || init_group.corner_weights.empty());

  typedef vcl_vector<rgrl_feature_sptr>::const_iterator Fitr;
  typedef vcl_vector<double>::const_iterator Witr;

  if(init_group.edgel_weights.empty()){
    for(Fitr fi= init_group.edgels.begin(); fi!=init_group.edgels.end(); ++fi)
      weight_map_[*fi] = 1.0;
  }
  else{
    Witr wi= init_group.edgel_weights.begin();
    for(Fitr fi= init_group.edgels.begin(); fi!=init_group.edgels.end(); ++fi, ++wi)
      weight_map_[*fi] = *wi;
  }

  if(init_group.corner_weights.empty()){
    for(Fitr fi= init_group.corners.begin(); fi!=init_group.corners.end(); ++fi)
      weight_map_[*fi] = 1.0;
  }
  else
  {
    Witr wi= init_group.corner_weights.begin();
    for(Fitr fi= init_group.corners.begin(); fi!=init_group.corners.end(); ++fi, ++wi)
      weight_map_[*fi] = *wi;
  }

}


//: Destructor
vidreg_salient_group::~vidreg_salient_group()
{
}


rgrl_view_sptr
vidreg_salient_group::view() const
{
  return view_;
}


const vcl_vector<rgrl_match_set_sptr>&
vidreg_salient_group::matches() const
{
  return matches_;
}


const vcl_vector<rgrl_scale_sptr>&
vidreg_salient_group::scales() const
{
  return scales_;
}

rgrl_converge_status_sptr
vidreg_salient_group::status() const
{
  return status_;
}

const vcl_map<rgrl_feature_sptr, double>&
vidreg_salient_group::weight_map() const
{
  return weight_map_;
}


const vnl_vector<double>&
vidreg_salient_group::growth_center() const
{
  return growth_center_;
}


void
vidreg_salient_group::set_growth_center(const vnl_vector<double>& center)
{
  growth_center_ = center;
}


vidreg_feature_group_sptr
vidreg_salient_group::make_feature_group(double weight_thresh) const
{
  typedef vcl_map<rgrl_feature_sptr, double>::const_iterator Mitr;
  vidreg_feature_group_sptr fg = new vidreg_feature_group();
  for(Mitr i=weight_map_.begin(); i!=weight_map_.end(); ++i){
    if(i->second < weight_thresh)
      continue;
    if(i->first->is_type(vidreg_feature_edgel::type_id())){
      fg->edgels.push_back(i->first);
      fg->edgel_weights.push_back(i->second);
    }
    else if(i->first->is_type(vidreg_feature_pt_desc::type_id())){
      fg->corners.push_back(i->first);
      fg->corner_weights.push_back(i->second);
    }
  }
  return fg;
}


double
vidreg_salient_group::weight(const rgrl_feature_sptr& feature) const
{
  vcl_map<rgrl_feature_sptr, double>::const_iterator itr = weight_map_.find(feature);
  if(itr == weight_map_.end())
    return 0.0;
  return itr->second;
}


void
vidreg_salient_group::set_matches(unsigned index, const rgrl_match_set_sptr& matches)
{
  assert(index < matches_.size());
  matches_[index] = matches;
}


void
vidreg_salient_group::assign_weight(const rgrl_feature_sptr& feature, double weight)
{
  if(weight > 0.0)
    weight_map_[feature] = weight;

  else if(weight == 0.0){
    vcl_map<rgrl_feature_sptr, double>::iterator i = weight_map_.find(feature);
    if(i != weight_map_.end())
      weight_map_.erase(i);
  }
}
