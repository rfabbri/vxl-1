// This is contrib/Manifold_extraction/vis/Lie_contour_match_displayer.h
#ifndef Lie_contour_match_displayer_h_
#define Lie_contour_match_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of Lie_contour_match_storage_sptr.
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class Lie_contour_match_displayer : public bvis1_displayer {

public:
  Lie_contour_match_displayer(){}

  //: Return type string "Lie_contour_matching"
  virtual vcl_string type() const { return "Lie contour matching"; }

  //: Create a tableau if the storage object is of type image
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // Lie_contour_match_displayer_h_

