// This is breye/bvis/displayer/bvis_vsol2D_displayer.h
#ifndef bvis_vsol2D_displayer_h_
#define bvis_vsol2D_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of vidpro_vsol2D_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/07/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis/bvis_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <bvis/bvis_mapper.h>

class bvis_vsol2D_displayer : public bvis_displayer {

public:
  bvis_vsol2D_displayer(){}

  //: Return type string "vsol2D"
  virtual vcl_string type() const { return "vsol2D"; }

  //: Create a tableau if the storage object is of type vsol2D
  virtual vgui_tableau_sptr make_tableau( bpro_storage_sptr ) const;

};

#endif // bvis_vsol2D_displayer_h_
