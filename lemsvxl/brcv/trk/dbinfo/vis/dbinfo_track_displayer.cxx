// This is brcv/trk/dbinfo/vis/dbinfo_track_displayer.cxx

#include <dbinfo/vis/dbinfo_track_displayer.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/dbinfo_mi_track.h>
#include <bvis1/bvis1_manager.h>
#include <bgui/bgui_vsol2D_tableau.h>


//: Create a tableau if the storage object is of type dbinfo_track
vgui_tableau_sptr
dbinfo_track_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    {
      vcl_cout << "In dbinfo_track_displayer::make_tableau -"
               << " types don't match\n";
      return NULL;
    }
  // Cast the storage object into dbinfo storage object
  dbinfo_track_storage_sptr dbinfo_storage;
  dbinfo_storage.vertical_cast(storage);
 
  // Extract the tracks
  vcl_vector<dbinfo_track_sptr> tracks = dbinfo_storage->tracks();

  if(tracks.empty())
    return bgui_vsol2D_tableau_new();   //empty tableau

  bgui_vsol2D_tableau_sptr stab = bgui_vsol2D_tableau_new();
  unsigned current_frame =
       static_cast<unsigned>(bvis1_manager::instance()->current_frame());

  
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); ++trit)
    {
      dbinfo_track_geometry_sptr tg = (*trit)->track_geometry();
      unsigned start_frame = tg->start_frame();
      vsol_polyline_2d_sptr track_geom = tg->incremental_track(start_frame,
                                                             current_frame);
      if(track_geom)
        stab->add_vsol_polyline_2d(track_geom);
    }
  return stab; 
}

