// This is brcv/rec/dbru/vis/dbru_rcor_displayer.h
#ifndef dbru_rcor_displayer_h_
#define dbru_rcor_displayer_h_

//:
// \file
// \brief Display dbru_rcor_storage_sptr
// \author 
// \date 11/31.05
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbru_rcor_displayer : public bvis1_displayer 
{
public:
  dbru_rcor_displayer(){}

  //: Return type string
  virtual vcl_string type() const { return "region_cor"; }

  //: Create a tableau if the storage object is of type image
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbru_rcor_displayer_h_
