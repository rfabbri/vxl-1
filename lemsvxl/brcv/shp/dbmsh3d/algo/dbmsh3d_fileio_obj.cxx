//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_fileio_obj.cxx
//:
// \file
// \brief Wavefront .obj mesh file I/O
//
//
// \author
//  MingChing Chang  Feb 21, 2007
//
// \verbatim
//  Modifications
// \endverbatim
//
//-------------------------------------------------------------------------

// See http://www.fileformat.info/format/wavefrontobj/
//Wavefront OBJ (object) files are used by Wavefront's Advanced Visualizer 
//application to store geometric objects composed of lines, polygons, 
//and free-form curves and surfaces. Wavefront is best known for its 
//high-end computer graphics tools, including modeling, animation, 
//and image compositing tools. These programs run on powerful workstations 
//such as those made by Silicon Graphics, Inc.
// # Simple Wavefront file
// v 0.0 0.0 0.0
// v 0.0 1.0 0.0
// v 1.0 0.0 0.0
// f 1 2 3

#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>

//: currently we only support reading the index-face-set of the file,
//  ignoring all other attributes.
bool dbmsh3d_load_obj (dbmsh3d_mesh* M, const char* file)
{
  std::ifstream  in;
  std::string    linestr;
  in.open (file);
  assert (M->vertexmap().size() == 0);
  double x, y, z;
  std::string key;
  int v;
  int vidcounter = 1; //the vertex id starts from 1, not 0.
  
  //main loop
  while (in) {
    linestr.clear();
    std::getline (in, linestr);
    //std::cout << linestr << std::endl;
    
    if (linestr.length() == 0 || std::strncmp (linestr.c_str(), "#", 1) == 0)
      continue; //skip empty line and comments.
    
    if (std::strncmp (linestr.c_str(), "v", 1) == 0) { //vertex
      std::sscanf (linestr.c_str(), "v %lf %lf %lf", &x, &y, &z);
      dbmsh3d_vertex* V = M->_new_vertex (vidcounter++);
      V->get_pt().set (x, y, z);
      M->_add_vertex (V);
      continue;
    }
    if (std::strncmp (linestr.c_str(), "f", 1) == 0) { //face
      dbmsh3d_face* F = M->_new_face ();
      std::stringstream ss (std::stringstream::in | std::stringstream::out);
      ss << linestr;
      ss >> key;
      while (ss.good()) { //read in each vertex index of this face.
        v = 0; //reset v
        ss >> v;
        if (v==0)
          continue;
        dbmsh3d_vertex* V = M->vertexmap (v);
        assert (V->id() == v);
        F->_ifs_add_bnd_V (V);
        V->set_meshed (true);
      }
      M->_add_face (F);
      continue;
    }
  }

  in.close();
  vul_printf (std::cout, "  %d points and %d faces loaded from %s.\n", 
              M->vertexmap().size(), M->facemap().size(), file);
  return true;
}

bool dbmsh3d_save_obj (dbmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    vul_printf (std::cout, "  can't open output .OBJ file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points %d faces...\n", 
              file, M->vertexmap().size(), M->facemap().size());

  //Saving mesh vertices.
  std::map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    std::fprintf (fp, "v %.16f %.16f %.16f\n", V->pt().x(), V->pt().y(), V->pt().z());
  }

  //Saving mesh faces.
  std::map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    std::fprintf (fp, "f ");
    for (unsigned int j=0; j<F->vertices().size(); j++) {
      dbmsh3d_vertex* V = F->vertices(j);
      std::fprintf (fp, "%d ", V->id());
    }
    std::fprintf (fp, "\n");
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

