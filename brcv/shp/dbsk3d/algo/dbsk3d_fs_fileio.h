#ifndef _dbsk3d_fs_fileio_h_
#define _dbsk3d_fs_fileio_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbsk3d/dbsk3d_fs_fileio.h
//:
// \file
// \brief Fine-scale medial scaffold file (.fs) I/O
//
// \author
//  MingChing Chang  Jan 11, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbsk3d/dbsk3d_fs_mesh.h>

bool save_to_fs (dbsk3d_fs_mesh* fs_mesh, const char *pcOutFile, const int verbose = 1);

bool load_from_fs (dbsk3d_fs_mesh* fs_mesh, const char *pcInFile, 
                   const bool read_gene, const int verbose = 1);

bool save_fs_vtk_attrib (dbsk3d_fs_mesh* M, const char* file);

#endif

