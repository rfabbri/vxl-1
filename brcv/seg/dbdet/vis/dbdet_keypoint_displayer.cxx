// This is brcv/seg/dbdet/vis/dbdet_keypoint_displayer.cxx

//:
// \file

#include "dbdet_keypoint_displayer.h"
#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_style.h>
#include "dbdet_keypoint_soview2D.h"
#include <dbdet/dbdet_keypoint_sptr.h>
#include <vcl_vector.h>

#include <vcl_iostream.h>

//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dbdet_keypoint_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a keypoint storage object
  dbdet_keypoint_storage_sptr keypoint_storage;
  keypoint_storage.vertical_cast(storage);

  const vcl_vector< dbdet_keypoint_sptr >& keypoints = keypoint_storage->keypoints();

  vgui_style_sptr line_style = vgui_style::new_style(1.0f , 1.0f , 0.0f , 1.0f , 1.0f);
  vgui_style_sptr point_style = vgui_style::new_style(0.0f , 1.0f , 0.0f , 4.0f , 1.0f);
  // Create a new curve tableau
  vgui_easy2D_tableau_sptr easy2D_tab = vgui_easy2D_tableau_new();
  for( vcl_vector< dbdet_keypoint_sptr >::const_iterator itr = keypoints.begin();
       itr != keypoints.end();  ++itr ){
    if(*itr){
      dbdet_keypoint_soview2D* obj = new dbdet_keypoint_soview2D(*itr,false);
      easy2D_tab->add( obj );
      obj->set_style( line_style );
      obj->set_point_style( point_style );
    }
  }

  return easy2D_tab;
}
