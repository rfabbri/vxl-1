//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_fileio_wrl_iv.cxx
//:
// \file
// \brief VRML and OpenInventor IV ASCII file I/O
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

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vcl_cstring.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

bool dbmsh3d_load_wrl_iv (dbmsh3d_mesh* M, const char* file)
{
  return true;
}

bool dbmsh3d_save_wrl_iv (dbmsh3d_mesh* M, const char* file)
{
  return true;
}

//: Read IV as ASCII indexed-face-set.
bool dbmsh3d_load_iv_ifs (dbmsh3d_mesh* M, const char* file)
{
  FILE *fp;
  char    buffer[100] = "";
  bool    this_round_done;
  int     end_of_file;  
  int     iVerticesPerFace = 0;

  if ((fp = vcl_fopen (file, "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open input IV file %s\n", file);
    return false; 
  }

  while (1) {
    //Search for first "vertex" or "point"
    while (vcl_strcmp (buffer, "vertex")!=0 && vcl_strcmp (buffer, "point")!=0) {
      end_of_file = vcl_fscanf (fp, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE;
    }

    //search for "["
    vcl_fscanf (fp, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in (x, y, z) coordinates of vertices
    this_round_done = false;
    while (!this_round_done) {
      double  x = DBL_MAX, y = DBL_MAX, z = DBL_MAX;
      vcl_fscanf (fp, "%lf %lf %lf%s", &x, &y, &z, buffer);
      if (x != DBL_MAX || y != DBL_MAX || z != DBL_MAX) {
        dbmsh3d_vertex* V = M->_new_vertex ();
        V->get_pt().set (x, y, z);
        M->_add_vertex (V);
      }
      else 
        this_round_done = true;
    }

    //Search for first "coordIndex"
    while (vcl_strcmp (buffer, "coordIndex")!=0) {
      end_of_file = vcl_fscanf (fp, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE;
    }
    //search for "["
    vcl_fscanf (fp, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in [?, ?, ?, -1, ?, ?, ?, ?, -1] indices of faces
    //note that a face can have either 3, 4, 5, or more vertices.
    this_round_done = false;
    vcl_vector<int> fids;
    while (!this_round_done) {
      vcl_fscanf (fp, "%s", buffer);
      int vid = atoi (buffer);
      if (vid == -1) {
        //add a new face of fids.
        dbmsh3d_face* F = M->_new_face ();
        for (unsigned int j=0; j<fids.size(); j++) {
          dbmsh3d_vertex* V = M->vertexmap (fids[j]);
          F->_ifs_add_bnd_V (V);
          V->set_meshed (true);
        }
        M->_add_face (F);
        fids.clear();
      }
      else {
        fids.push_back (vid);
      }

      if (vcl_strcmp (buffer, "]")==0)
        this_round_done = true;
    }

  }//end while(1)

READ_IV_DONE:
  vcl_fclose (fp);

  vul_printf (vcl_cout, "\t%u points, %u faces read from %s.\n", 
              M->vertexmap().size(), M->facemap().size(), file);
  return true;
}

bool dbmsh3d_save_iv_ifs (dbmsh3d_mesh* M, const char* file)
{
  assert (0);
  return false;
}
