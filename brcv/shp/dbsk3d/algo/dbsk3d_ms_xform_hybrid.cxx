//: dbsk3d_ms_xform_hybrid.cxx
//: MingChing Chang
//  Oct 4, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>

//###############################################################
//  Functions for special case involving two or more xforms 
//###############################################################

dbsk3d_fs_face* A5_contract_L_find_trim_P (dbsk3d_fs_edge* A13L)
{
  dbmsh3d_halfedge* HE = A13L->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    if (FF->is_on_shock_bnd())
      return FF;
    HE = HE->next();
  }
  while (HE != A13L->halfedge());
  assert (0);
  return NULL;
}

