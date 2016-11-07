// This is Spatemp/vis/dbdet_temporal_map_displayer.h
#ifndef dbdet_temporal_map_displayer_h_
#define dbdet_temporal_map_displayer_h_

//:


#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbdet_temporal_map_displayer : public bvis1_displayer {

public:
  dbdet_temporal_map_displayer(){}

  //: Return type string "sel"
  virtual vcl_string type() const { return "temporalmap"; }

  //: Create a tableau if the storage object is of type "temporalmap"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbdet_temporal_map_displayer_h_
