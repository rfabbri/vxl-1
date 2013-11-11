// This is breye1/bvis1/displayer/bvis1_dicom_image_displayer.h
#ifndef bvis1_dicom_image_displayer_h_
#define bvis1_dicom_image_displayer_h_

//:
// \file
// \brief This file modifies the behavior of the standard bvis1_dicom_image_displayer
// for dicom images
// 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date July 5, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/displayer/bvis1_image_displayer.h>



// TODO: make update_image_tableau() of image displayer virtual


class bvis1_dicom_image_displayer : public bvis1_image_displayer {

public:
  bvis1_dicom_image_displayer(){}

  ////: Return type string "image"
  //virtual vcl_string type() const { return bvis1_image_displayer::type(); }

  ////: Create a tableau if the storage object is of type image
  //virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;
  //
  ////: Update a tableau with a the storage object if both are of type image
  //virtual bool update_tableau( vgui_tableau_sptr, const bpro1_storage_sptr& ) const;
 
protected:
  //: Perform the update once types are known
  virtual bool update_image_tableau( bgui_image_tableau_sptr, 
                             const vidpro1_image_storage_sptr&) const;
};

#endif // bvis1_dicom_image_displayer_h_
