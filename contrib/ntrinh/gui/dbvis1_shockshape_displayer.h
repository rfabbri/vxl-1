// This is breye1/bvis1/displayer/dbvis1_shockshape_displayer.h
#ifndef dbvis1_shockshape_displayer_h_
#define dbvis1_shockshape_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of vidpro1_shockshape_storage_sptr.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 10/30/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbvis1_shockshape_displayer : public bvis1_displayer {

public:
  dbvis1_shockshape_displayer(){}

  //: Return type string "shockshape"
  virtual vcl_string type() const { return "shockshape"; }

  //: Create a tableau if the storage object is of type "shockshape"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbvis1_shockshape_displayer_h_
