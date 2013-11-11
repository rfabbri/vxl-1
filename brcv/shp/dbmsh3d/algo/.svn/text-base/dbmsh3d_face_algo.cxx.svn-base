// This is brcv/shp/dbmsh3d/dbmsh3d_face_algo.cxx
//:
// \file
// \brief Find the foot point on a mesh face

#include <vgl/algo/vgl_fit_plane_3d.h>
#include <dbmsh3d/algo/dbmsh3d_face_algo.h>


bool faces_in_same_plane (dbmsh3d_face* F1, dbmsh3d_face* F2)
{
  vgl_fit_plane_3d<double> fit_plane1;
  F1->_ifs_track_ordered_vertices ();
  for (unsigned i=0; i<F1->vertices().size(); i++) {
    dbmsh3d_vertex* v = F1->vertices (i);  
    fit_plane1.add_point (v->pt().x(),v->pt().y(),v->pt().z());
  }
  fit_plane1.fit(1.0); //FIXME - determine a better error margin

  vgl_fit_plane_3d<double> fit_plane2;
  F2->_ifs_track_ordered_vertices ();
  for (unsigned i=0; i<F2->vertices().size(); i++) {
    dbmsh3d_vertex* v = F2->vertices (i);  
    fit_plane2.add_point(v->pt().x(),v->pt().y(),v->pt().z());
  }
  fit_plane2.fit(1.0); //FIXME - determine a better error margin
  
  return fit_plane1.get_plane().normal() == fit_plane2.get_plane().normal();
}


//: return true if a valid footpt is found.
bool dbmsh3d_get_footpt_face (const vgl_point_3d<double>& queryP, const dbmsh3d_face* F, 
                              vgl_point_3d<double>& footpt, double& dist)
{
  if (F->vertices().size() == 3) {
  }
  else { 
    //triangulate the face F.
  }
  return false;
}

