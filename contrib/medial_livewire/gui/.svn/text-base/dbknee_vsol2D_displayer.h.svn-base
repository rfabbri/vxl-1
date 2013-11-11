// This is dbknee_vsol2D_displayer.h
#ifndef dbknee_vsol2D_displayer_h_
#define dbknee_vsol2D_displayer_h_

//:
// \file
// \brief Modifications of bvis_vsol2D_displayer for knee cartilage apps
// \author Nhon Trinh
// \date 11/15/2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_mapper.h>

class dbknee_vsol2D_displayer : public bvis1_displayer {

public:
  dbknee_vsol2D_displayer(){}

  //: Return type string "vsol2D"
  virtual vcl_string type() const { return "vsol2D"; }

  //: Create a tableau if the storage object is of type vsol2D
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbknee_vsol2D_displayer_h_
