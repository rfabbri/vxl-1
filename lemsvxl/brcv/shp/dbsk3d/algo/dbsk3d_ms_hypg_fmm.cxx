//: This is dbsk3d/algo/dbsk3d_ms_hypg_fmm.cxx
//MingChing Chang Nov. 15, 2007.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_fmm.h>


//: For each MS, build a separate triangular mesh and add to the
//  
void dbsk3d_ms_hypg_fmm::build_sheets_fmm_mesh ()
{
  vcl_map<int, dbmsh3d_sheet*>::iterator SS_it = ms_hypg_->sheetmap().begin();
  for (; SS_it != ms_hypg_->sheetmap().end(); SS_it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*SS_it).second;

    //build IFS mesh for each MS
    dbmsh3d_mesh* SSM = new dbmsh3d_mesh (MS->facemap());
    SSM->build_IFS_mesh ();

    //Triangulate each MS into a new mesh
    dbmsh3d_fmm_mesh* fmm_mesh = generate_fmm_tri_mesh (SSM);

    //Put the new trimesh into the map.
    sheet_fmm_mesh_map_.insert (vcl_pair<int, dbmsh3d_fmm_mesh*>(MS->id(), fmm_mesh));

    delete SSM;
  }
}
  
//: For each MS, perform geodesic distance transform.
//  treat all boundary and internal curve vertices as sources.
//
void dbsk3d_ms_hypg_fmm::run_fmm_on_sheet_meshes ()
{
  vcl_map<int, dbmsh3d_fmm_mesh*>::iterator it = sheet_fmm_mesh_map_.begin();
  for (; it != sheet_fmm_mesh_map_.end(); it++) {
    dbmsh3d_fmm_mesh* fmm_mesh = (*it).second;

    //Recover the Modified Half-edge representation.
    fmm_mesh->IFS_to_MHE();

    //Run FMM on the new mesh
    fmm_mesh->setup_fast_marching ();

    //: set all boundary and internal curve's nodeElm as sources.
    vcl_set<dbmsh3d_vertex*> incident_Ns;
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) ms_hypg_->sheetmap ((*it).first);
    MS->get_incident_FVs (incident_Ns);

    vcl_set<dbmsh3d_vertex*>::iterator it = incident_Ns.begin();
    for (; it != incident_Ns.end(); it++) {
      dbmsh3d_vertex* V = (*it);
      int id = V->id();
      dbmsh3d_fmm_vertex_3d* source_vertex = (dbmsh3d_fmm_vertex_3d*) fmm_mesh->vertexmap (id);
      fmm_mesh->add_source_vertex (source_vertex);
    }

    //: compute FMM from the starting vertices
    fmm_mesh->run_fmm ();
  }
}
