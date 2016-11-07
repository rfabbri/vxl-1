// This is /vis/dbrl_match_set_displayer.h
#ifndef dbrl_match_set_displayer_h_
#define dbrl_match_set_displayer_h_


#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbrl_match_set_displayer : public bvis1_displayer {

public:
  dbrl_match_set_displayer(){}

  //: Return type string "dbbgm_distribution_image"
  virtual vcl_string type() const { return "dbrl_match_set"; }

  //: Create a tableau if the storage object is of type "dbrl_match_set"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbrl_match_set_displayer_h_

