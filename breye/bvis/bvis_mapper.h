// This is breye/bvis/displayer/bvis_mapper.h
#ifndef bvis_mapper_h_
#define bvis_mapper_h_

//:
// \file
// \brief A class for mapping attributes to a style for display

// \author J.L. Mundy
// \date November 7, 2005
//
// \verbatim
//  Modifications
// \endverbatim
#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_style_sptr.h>
class bvis_mapper : public vbl_ref_count
{

 public:
  bvis_mapper(): mina_(0), maxa_(0), gamma_(1){}
  bvis_mapper(const double min, const double max, 
              const double gamma): mina_(min), maxa_(max), gamma_(gamma){} 

  //:Set parameters
  void set_min(const double min){mina_ = min;}
  void set_max(const double max){maxa_ = max;}
  void set_gamma(const double gamma){gamma_ = gamma;}

  //:The key mapping function
  virtual vgui_style_sptr style(const double attr) const;
 protected:
  //:The bounds on the attribute values
  double mina_ , maxa_ ;
  //:The gamma mapping
  double gamma_;
};
#include <bvis/bvis_mapper_sptr.h>
#endif // bvis_mapper_h_
