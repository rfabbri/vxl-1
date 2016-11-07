// This is brcv/seg/dbbgm/vis/dbbgm_image_displayer.h
#ifndef dbbgm_image_displayer_h_
#define dbbgm_image_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbbgm_image_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/31/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <dbvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <dbpro1/bpro1_storage_sptr.h>


class dbbgm_image_displayer : public bvis1_displayer {

public:
  dbbgm_image_displayer(){}

  //: Return type string "dbbgm_image"
  virtual vcl_string type() const { return "dbbgm_image"; }

  //: Create a tableau if the storage object is of type "dbbgm_image"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;
  
  //: Update a tableau from a storage object
  virtual bool update_tableau( vgui_tableau_sptr, const bpro1_storage_sptr& ) const;

};

#endif // dbbgm_image_displayer_h_
