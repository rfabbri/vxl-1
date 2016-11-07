// This is contrib/mleotta/vidreg/vidreg_feature_edgel.cxx

//:
// \file


#include "vidreg_feature_edgel.h"
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_transformation.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdlib.h>


//: Default Constructor
vidreg_feature_edgel::vidreg_feature_edgel()
  : rgrl_feature_face_pt(), magnitude_(0.0), next_(NULL), prev_(NULL)
{
}


//: Constructor
vidreg_feature_edgel::vidreg_feature_edgel(const vnl_vector<double>& loc,
                                           const vnl_vector<double>& norm,
                                           double mag)
  : rgrl_feature_face_pt(loc,norm), magnitude_(mag), next_(NULL), prev_(NULL)
{
}


//: Destructor
vidreg_feature_edgel::~vidreg_feature_edgel()
{
  if(next_)
    next_->prev_ = NULL;
  if(prev_)
    prev_->next_ = NULL;
}


//: Set the next edgel in the chain (also sets next->prev = this)
void
vidreg_feature_edgel::set_next(vidreg_feature_edgel* next)
{
  this->next_ = next;
  if(next_)
    next_->prev_ = this;
}


//: Set the previous edgel in the chain (also sets prev->next = this)
void
vidreg_feature_edgel::set_prev(vidreg_feature_edgel* prev)
{
  this->prev_ = prev;
  if(prev_)
    prev_->next_ = this;
}


//: Computes a weight based on orientation and magnitude
double
vidreg_feature_edgel::absolute_signature_weight(rgrl_feature_sptr other) const
{
  assert(other);
  vidreg_feature_edgel* edgel_ptr = rgrl_cast<vidreg_feature_edgel*>(other);
  double dir_wgt = dot_product(this->normal_, edgel_ptr->normal_);
  if(dir_wgt <= 0.0)
    return 0.0;

  dir_wgt = vcl_asin(dir_wgt);
  dir_wgt *= vnl_math::two_over_pi;

  // should also use magnitude somehow
  return dir_wgt;
}


//: Transform the feature
rgrl_feature_sptr
vidreg_feature_edgel::transform( rgrl_transformation const& xform ) const
{
  vidreg_feature_edgel* edgel_ptr = new vidreg_feature_edgel();

  // Capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = edgel_ptr;

  xform.map_location( this->location_, edgel_ptr->location_ );
  xform.map_normal( this->location_, this->normal_, edgel_ptr->normal_ );

  // Ignore scale for now
  edgel_ptr->scale_ = this->scale_;

  edgel_ptr->magnitude_ = this->magnitude_;

  return result_sptr;
}


//: used for sorting by decreasing magnitude
bool vidreg_feature_edgel::dec_mag_order(const rgrl_feature_sptr& f1,
                                         const rgrl_feature_sptr& f2)
{
  return rgrl_cast<vidreg_feature_edgel*>(f1)->magnitude()
       > rgrl_cast<vidreg_feature_edgel*>(f2)->magnitude();
}


//: used for sorting by increasing magnitude
bool vidreg_feature_edgel::inc_mag_order(const rgrl_feature_sptr& f1,
                                         const rgrl_feature_sptr& f2)
{
  return rgrl_cast<vidreg_feature_edgel*>(f1)->magnitude()
       < rgrl_cast<vidreg_feature_edgel*>(f2)->magnitude();
}


//=============================================================================
// vidreg_edge


vidreg_edge::vidreg_edge(vidreg_feature_edgel* head)
  : head_(head), size_(0), avg_magnitude_(0.0)
{
  if(!head_)
    return;

  size_ = 1;
  avg_magnitude_ = head_->magnitude();
  for(vidreg_feature_edgel* e = head_->next();
      e && e != head_; e=e->next()){
    ++size_;
    avg_magnitude_ += e->magnitude();
  }
  avg_magnitude_ /= size_;
}


//: Used for sorting by decreasing magnitude
bool vidreg_edge::dec_mag_order(const vidreg_edge& e1,
                                const vidreg_edge& e2)
{
  return e1.avg_magnitude_ > e2.avg_magnitude_;
}

//: Used for sorting by decreasing size
bool vidreg_edge::dec_size_order(const vidreg_edge& e1,
                                 const vidreg_edge& e2)
{
  return e1.size_ > e2.size_;
}
