//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_fileio_off.cxx
//:
// \file
// \brief Geomview .off mesh file I/O
//
//
// \author
//  MingChing Chang  Jan 11, 2007
//
// \verbatim
//  Modifications
// \endverbatim
//
//-------------------------------------------------------------------------

// See http://www.geomview.org/docs/html/geomview_41.html
//Go to the first, previous, next, last section, table of contents.
//
//OFF Files
//
//The conventional suffix for OFF files is `.off'.
//
//Syntax:
//
//[ST][C][N][4][n]OFF  # Header keyword
//[Ndim]    # Space dimension of vertices, present only if nOFF
//NVertices  NFaces  NEdges   # NEdges not used or checked
//
//x[0]  y[0]  z[0]  # Vertices, possibly with normals,
//      # colors, and/or texture coordinates, in that order,
//      # if the prefixes N, C, ST
//      # are present.
//      # If 4OFF, each vertex has 4 components,
//      # including a final homogeneous component.
//      # If nOFF, each vertex has Ndim components.
//      # If 4nOFF, each vertex has Ndim+1 components.
//...
//x[NVertices-1]  y[NVertices-1]  z[NVertices-1]
//
//          # Faces
//          # Nv = # vertices on this face
//          # v[0] ... v[Nv-1]: vertex indices
//          #    in range 0..NVertices-1
//Nv  v[0] v[1] ... v[Nv-1]  colorspec
//...
//          # colorspec continues past v[Nv-1]
//          # to end-of-line; may be 0 to 4 numbers
//          # nothing: default
//          # integer: colormap index
//          # 3 or 4 integers: RGB[A] values 0..255
//      # 3 or 4 floats: RGB[A] values 0..1
//
//OFF files (name for "object file format") represent collections of planar 
//polygons with possibly shared vertices, a convenient way to describe 
//polyhedra. The polygons may be concave but there's no provision for 
//polygons containing holes.
//
//An OFF file may begin with the keyword OFF; it's recommended but optional, 
//as many existing files lack this keyword.
//
//Three ASCII integers follow: NVertices, NFaces, and NEdges. 
//Thse are the number of vertices, faces, and edges, respectively. 
//Current software does not use nor check NEdges; 
//it needn't be correct but must be present.
//
//The vertex coordinates follow: dimension * Nvertices floating-point values. 
//They're implicitly numbered 0 through NVertices-1. dimension is 
//either 3 (default) or 4 (specified by the key character 4 directly before 
//OFF in the keyword).
//
//Following these are the face descriptions, typically written with one 
//line per face. Each has the form
//
//N  Vert1 Vert2 ... VertN  [color]
//
//Here N is the number of vertices on this face, and Vert1 through VertN are 
//indices into the list of vertices (in the range 0..NVertices-1).
//
//The optional color may take several forms. Line breaks are significant here: 
//the color description begins after VertN and ends with the end of the line 
//(or the next # comment). A color may be:
//
//nothing
//    the default color 
//one integer
//    index into "the" colormap; see below 
//three or four integers
//    RGB and possibly alpha values in the range 0..255 
//three or four floating-point numbers
//    RGB and possibly alpha values in the range 0..1 
//
//For the one-integer case, the colormap is currently read from the file 
//`cmap.fmap' in Geomview's `data' directory. 
//Some better mechanism for supplying a colormap is likely someday.
//
//The meaning of "default color" varies. If no face of the object has a color, 
//all inherit the environment's default material color. 
//If some but not all faces have colors, the default is gray (R,G,B,A=.666).
//
//A [ST][C][N][n]OFF BINARY format is accepted; See section Binary format. 
//It resembles the ASCII format in almost the way you'd expect, 
//with 32-bit integers for all counters and vertex indices and 32-bit floats 
//for vertex positions (and texture coordinates or vertex colors or normals 
//if COFF/NOFF/CNOFF/STCNOFF/etc. format).
//
//Exception: each face's vertex indices are followed by an integer indicating 
//how many color components accompany it. Face color components must be floats, 
//not integer values. Thus a colorless triangular face might be represented as
//
//int int int int int
//3   17   5   9   0
//
//while the same face colored red might be
//
//int int int int int float float float float
// 3  17   5   9   4   1.0   0.0   0.0   1.0

#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>

bool dbmsh3d_load_off (dbmsh3d_mesh* M, const char* file)
{
  vcl_ifstream  in;
  vcl_string    linestr;
  in.open (file);
  assert (M->vertexmap().size() == 0);
  int i, j, n, v;
  double x, y, z;
  
  //header
  vcl_getline (in, linestr);
  if (vcl_strncmp (linestr.c_str(), "OFF", 3) == 0)
    vcl_getline (in, linestr);
  else if (vcl_strncmp (linestr.c_str(), "COFF", 4) == 0)
    vcl_getline (in, linestr);
  else if (vcl_strncmp (linestr.c_str(), "NOFF", 4) == 0) {
    vcl_getline (in, linestr); //Ndim
    vcl_getline (in, linestr);
  }
  else if (vcl_strncmp (linestr.c_str(), "CNOFF", 4) == 0)
    vcl_getline (in, linestr);
  else if (vcl_strncmp (linestr.c_str(), "STCNOFF", 4) == 0)
    vcl_getline (in, linestr);

  //NVertices  NFaces  NEdges
  int NVertices = 0, NFaces = 0, NEdges = 0;
  vcl_sscanf (linestr.c_str(), "%d %d %d", &NVertices, &NFaces, &NEdges);

  //Read in each vertex
  for (i=0; i<NVertices; i++) {
    vcl_getline (in, linestr);
    //skip empty lines and comments
    if (linestr.length() == 0 || vcl_strncmp (linestr.c_str(), "#", 1) == 0) { 
      i--;
      continue; 
    }

    vcl_sscanf (linestr.c_str(), "%lf %lf %lf", &x, &y, &z);
    dbmsh3d_vertex* V = M->_new_vertex ();
    V->get_pt().set (x, y, z);
    M->_add_vertex (V);
  }

  //Read in each face
  for (i=0; i<NFaces; i++) {
    vcl_getline (in, linestr);
    //skip empty lines and comments
    if (linestr.length() == 0 || vcl_strncmp (linestr.c_str(), "#", 1) == 0) { 
      i--;
      continue; 
    }
    
    vcl_stringstream linestream (linestr);
    linestream >> n;
    if (n==1) { //skip the single vertex case.
    }
    else {
      dbmsh3d_face* F = M->_new_face ();
      for (j=0; j<n; j++) {
        linestream >> v;
        dbmsh3d_vertex* V = M->vertexmap (v);
        F->_ifs_add_bnd_V (V);
        V->set_meshed (true);
      }
      M->_add_face (F);
    }
  }

  in.close();
  if (NVertices != 0)
    vul_printf (vcl_cout, "  %d points and %d faces loaded from %s.\n", 
                NVertices, NFaces, file);
  return true;
}

bool dbmsh3d_save_off (dbmsh3d_mesh* M, const char* file)
{  
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    vul_printf (vcl_cout, "  can't open output .OFF file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cout, "  saving %s : %d points %d faces...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  //Saving Geomview .off header.
  vcl_fprintf (fp, "%d %d %d\n", M->vertexmap().size(), M->facemap().size(), M->edgemap().size());

  //Saving mesh vertices.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", V->pt().x(), V->pt().y(), V->pt().z());
  }

  //Saving mesh faces.
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    vcl_fprintf (fp, "%d ", F->vertices().size());
    for (unsigned int j=0; j<F->vertices().size(); j++) {
      dbmsh3d_vertex* V = F->vertices(j);
      vcl_fprintf (fp, "%d ", V->id());
    }
    vcl_fprintf (fp, "\n");
  }

  fclose (fp);
  vul_printf (vcl_cout, "  done.\n");

  return true;
}

