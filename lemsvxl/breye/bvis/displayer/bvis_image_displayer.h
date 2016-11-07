// This is breye/bvis/displayer/bvis_image_displayer.h
#ifndef bvis_image_displayer_h_
#define bvis_image_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of vidpro_image_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/30/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis/bvis_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <bgui/bgui_image_tableau_sptr.h>


class bvis_image_displayer : public bvis_displayer {

public:
  bvis_image_displayer(){}

  //: Return type string "image"
  virtual vcl_string type() const { return "image"; }

  //: Create a tableau if the storage object is of type image
  virtual vgui_tableau_sptr make_tableau( bpro_storage_sptr ) const;
  
  //: Update a tableau with a the storage object if both are of type image
  virtual bool update_tableau( vgui_tableau_sptr, const bpro_storage_sptr& ) const;
 
protected:
  //: Perform the update once types are known
  virtual bool update_image_tableau( bgui_image_tableau_sptr, 
                             const vidpro_image_storage_sptr&) const;

};

#endif // bvis_image_displayer_h_
