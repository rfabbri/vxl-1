// This is /vis/dbrl_id_point_2d_displayer.h
#ifndef dbrl_id_point_2d_displayer_h_
#define dbrl_id_point_2d_displayer_h_


#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbrl_id_point_2d_displayer : public bvis1_displayer {

public:
  dbrl_id_point_2d_displayer(){}

  //: Return type string "dbbgm_distribution_image"
  virtual vcl_string type() const { return "dbrl_id_point_2d"; }

  //: Create a tableau if the storage object is of type "dbrl_id_point_2d"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbrl_id_point_2d_displayer_h_

