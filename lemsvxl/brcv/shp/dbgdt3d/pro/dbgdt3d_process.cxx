//: This is lemsvxlsrc/brcv/shp/dbgdt3d/pro/dbmsh3d_process.cxx
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbgdt3d/pro/dbgdt3d_process.h>

bool dbgdt3d_pro::load_ply2_gdt (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  reset_gdt_mesh ();
  return dbmsh3d_load_ply2 (gdt_mesh_, dirfile.c_str());
}

