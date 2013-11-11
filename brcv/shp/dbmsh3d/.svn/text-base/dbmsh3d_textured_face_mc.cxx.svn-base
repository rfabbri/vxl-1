//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_textured_mesh_mc.cxx
//:
// \file
// \brief A textured mesh
//
//
// \author
//  Daniel Crispell  March 14, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include "dbmsh3d_textured_face_mc.h"
#include "dbmsh3d_vertex.h"
#include <vgl/vgl_point_3d.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

dbmsh3d_textured_face_mc::dbmsh3d_textured_face_mc(const dbmsh3d_face* face, vcl_string tex_map_uri) 
: dbmsh3d_face_mc(face->id()) 
{ 
  tex_map_uri_ = tex_map_uri; 

  // copy vertices
  vcl_vector<dbmsh3d_vertex*> old_verts = face->vertices();
  for (unsigned v = 0; v < old_verts.size(); v++) {
    dbmsh3d_vertex* vert = (dbmsh3d_vertex*)old_verts[v];
    _add_vertex(vert);
  }
}

bool dbmsh3d_textured_face_mc::get_texture_image (unsigned char*& bitmap, int& width, int& height, int& plane) const
{
  //read texture from file. tex_map_uri_
  vil_image_view<vxl_byte> image = vil_load (tex_map_uri_.c_str());
  if (image == NULL)
    return false;

  width = image.ni();
  height = image.nj();
  plane = image.nplanes();

  ///bitmap = texture.top_left_ptr();
  bitmap = new unsigned char [width * height * plane];

  for (int j=0; j<image.nj(); j++) {
    for (int i=0; i<image.ni(); i++) {
      for (int p=0; p<image.nplanes(); p++) {
        //height, width, plane
        //bitmap[i + j*width + p*width*height] = image(i,j,p);
        ///bitmap[j + i*height + p*width*height] = image(i,j,p);
        //bitmap[j*width*plane + i*plane + p] = image(i,j,p);
        bitmap[i*height*plane + j*plane + p] = image(i,height-1-j,p);
      }
    }
  }

  return true;
}

void dbmsh3d_textured_face_mc::print()
{
  unsigned i = 0;
  while (i < set_he_.size()) {
    dbmsh3d_halfedge* HE = set_he_[i++];
    dbmsh3d_halfedge* he = HE;
    do {
      dbmsh3d_vertex* s =  HE->edge()->sV();
      dbmsh3d_vertex* e =  HE->edge()->eV();
      vgl_point_2d<double> s_tex = tex_coords_[s->id()];
      vgl_point_2d<double> e_tex = tex_coords_[e->id()];
      vcl_cout << "S=" << HE->edge()->sV() << " " << s->get_pt() << "tex= " << s_tex << vcl_endl;
      vcl_cout << "E=" << HE->edge()->eV() << " " << e->get_pt() << "tex= " << e_tex << vcl_endl;
      HE = HE->next();
    } while (HE != he);
  }
}

