//: dbsk3d_pro_bktseg.cxx
//: MingChing Chang
//  Mar 28, 2007

#include <dbgl/dbgl_dist.h>
#include <dbnl/dbnl_min_max_3.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbsk3d/pro/dbsk3d_process.h>
#include <dbsk3d/algo/dbsk3d_fs_segre_bkt_stitch.h>

// #################################################################

//: Get the initial triangles that is completely inside the stitching sausage.
//  No need to check the soboxin, because such triangles should not exist in stitchM.
void get_sausage_init_faces (const vcl_vector<vgl_point_3d<double> >& input_pts, 
                             dbmsh3d_mesh* stitchM, 
                             const vgl_box_3d<double>& sboxin, 
                             const vgl_box_3d<double>& sboxout, 
                             vcl_vector<vcl_vector<int> >& S_initM_faces)
{
  //Loop through each face in stitchM and put all that inside 
  //the box sboxout to S_initM_faces.  
  vcl_map<int, dbmsh3d_face*>::iterator fit = stitchM->facemap().begin();
  for (; fit != stitchM->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;

    //Skip the current face if any point of it is outside the box.
    //i.e., if it is not completely inside the box sboxout.
    if (F->is_inside_box(sboxout) == false)
      continue;

    vcl_vector<int> vids;
    F->get_bnd_V_ids (vids);
    S_initM_faces.push_back (vids);
  }
}

// #################################################################

//: Get the points near bucket boundary to be the stitching points.
//  i.e. points outside the reduced-size (by a given ratio) bucket.
//
void get_stitch_surface_pts (dbmsh3d_pt_bktstr* BktStruct, 
                             vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& stitchS_idpts, 
                             const float bkt_box_ratio)
{
  vgl_box_3d<double> box;

  //Loop through each bucket: the slices, rows, and buckets.
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());

        //Compute the reduced-box of B.
        double reduce = bkt_box_ratio * dbnl_max3 (box.width(), box.height(), box.depth());
        vgl_box_3d<double> reduced_box = dbgl_reduce_box (box, reduce);

        //Get the points outside reduced box
        vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpts;
        idpts.clear();
        B->get_pts_outside_reduced_box (reduced_box, idpts);

        //Add to stitchS_idpts
        stitchS_idpts.insert (stitchS_idpts.end(), idpts.begin(), idpts.end());
      }
    }
  }
}

void get_pts_local_bucket_meshing (const dbmsh3d_pt_bucket* B, 
                                   const vcl_vector<vcl_vector<int> >& B_initM_faces, 
                                   const vcl_vector<vgl_point_3d<double> >& all_pts, 
                                   vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& B_meshing_idpts)
{
  //Add all points in bucket B
  B_meshing_idpts.insert (B_meshing_idpts.end(), B->idpt_list().begin(), B->idpt_list().end());

  //Add all points of B_initM_faces.
  for (unsigned int i=0; i<B_initM_faces.size(); i++) {
    for (unsigned int j=0; j<B_initM_faces[i].size(); j++) {
      int id = B_initM_faces[i][j];
      B_meshing_idpts.push_back (vcl_pair<int, vgl_point_3d<double> > (id, all_pts[id]));
    }
  }

  check_dup_adpt_bucketing (B_meshing_idpts, 18);
}


int merge_meshes (const vcl_vector<vgl_point_3d<double> >& finalM_pts, 
                  vcl_vector<vcl_vector<int> >& finalM_faces, 
                  dbmsh3d_mesh* M)
{
  //Directly add all mesh M's faces into finalM_faces.
  vcl_vector<vcl_vector<int> > B_faces;
  add_mesh_faces_IFS (M, B_faces);
  finalM_faces.insert (finalM_faces.end(), B_faces.begin(), B_faces.end());
  return B_faces.size();
}

int check_merge_meshes (const vcl_vector<vgl_point_3d<double> >& finalM_pts, 
                        vcl_vector<vcl_vector<int> >& finalM_faces, 
                        dbmsh3d_mesh* M)
{
  if (finalM_faces.size() == 0) {
    //Directly add all mesh M's faces into finalM_faces.
    vcl_vector<vcl_vector<int> > B_faces;
    add_mesh_faces_IFS (M, B_faces);
    finalM_faces.insert (finalM_faces.end(), B_faces.begin(), B_faces.end());
    return B_faces.size();
  }

  //Compute bounding box of current finalM.
  vgl_box_3d<double> finalM_box;
  for (unsigned int i=0; i<finalM_faces.size(); i++) {
    for (unsigned int j=0; j<finalM_faces[i].size(); j++) {
      int id = finalM_faces[i][j];
      finalM_box.add (finalM_pts[id]);
    }
  }
  
  //Compute bounding box of mesh M.
  vgl_box_3d<double> M_box;
  detect_bounding_box (M, M_box); 

  //Create a mesh Mf for finalM_faces intersecting M_box.
  dbmsh3d_mesh* Mf = create_intersect_mesh (finalM_pts, finalM_faces, M_box); 
  Mf->IFS_to_MHE();
  int count = 0;

  //Go through each face of M
  M->build_IFS_mesh ();
  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    if (F->is_outside_box (finalM_box) == false) { 
      //If F intersects with finalM_box,
      //Brute-forcely check if insertion of F violates 2-manifold assumption.
      bool skip_F = false;

      //1) For each edge of F, check if the corresponding edge in Mf exists
      //   and is a 2-manifold edge, if so, skip F.
      for (unsigned int i=0; i<F->vertices().size(); i++) {
        unsigned j = (i+1) % F->vertices().size();
        int idi = F->vertices(i)->id();
        int idj = F->vertices(j)->id();
        dbmsh3d_vertex* Vi = Mf->vertexmap (idi);
        dbmsh3d_vertex* Vj = Mf->vertexmap (idj);
        if (Vi && Vj) {
          dbmsh3d_edge* E = E_sharing_2V (Vi, Vj);
          if (E) {
            if (E->n_incident_Fs() > 1) {
              skip_F = true;
              break;
            }
          }
        }
      }
      if (skip_F)
        continue; //skip F

      //2) For each vertex of F, check if the corresponding vertex in Mf exists
      //   and is a 1-ring, if so, skip F.
      for (unsigned int i=0; i<F->vertices().size(); i++) {
        int id = F->vertices(i)->id();
        dbmsh3d_vertex* V = Mf->vertexmap (id);
        if (V) {
          VTOPO_TYPE type = V->detect_vtopo_type();
          if (type == VTOPO_2_MANIFOLD_1RING || VTOPO_NON_MANIFOLD_1RING) {
            skip_F = true;
            break;
          }
        }
      }
      if (skip_F)
        continue; //skip F
    }

    //add F to finalM
    vcl_vector<int> fids;
    for (unsigned int i=0; i<F->vertices().size(); i++)
      fids.push_back (F->vertices(i)->id());
    assert (fids.size() != 0);
    finalM_faces.push_back (fids);
    count++;
  }

  delete Mf;
  return count;
}

dbmsh3d_mesh* create_intersect_mesh (const vcl_vector<vgl_point_3d<double> >& pts, 
                                     vcl_vector<vcl_vector<int> >& faces,
                                     const vgl_box_3d<double>& M_box)
{
  //Compute the set of intersecting faces
  vcl_vector<vcl_vector<int> > ifaces;

  for (unsigned int i=0; i<faces.size(); i++) {
    for (unsigned int j=0; j<faces[i].size(); j++) {
      int id = faces[i][j];
      if (M_box.contains (pts[id])) {
        //Add the current face finalM_faces[i] to ifaces.
        ifaces.push_back (faces[i]);
        break;        
      }
    }
  }

  //Compute the set of all points (ids) of the ifaces.
  vcl_set<int> ipts;
  for (unsigned int i=0; i<ifaces.size(); i++) {
    for (unsigned int j=0; j<ifaces[i].size(); j++)
      ipts.insert (ifaces[i][j]);
  }

  //Create the intersecting mesh M
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  //Create all vertices of M
  vcl_set<int>::iterator it = ipts.begin();
  for (; it != ipts.end(); it++) {
    int id = (*it);
    dbmsh3d_vertex* V = M->_new_vertex (id);
    V->get_pt().set (pts[id].x(), pts[id].y(), pts[id].z());
    M->_add_vertex (V);
  }

  //Create all faces of M
  for (unsigned int i=0; i<ifaces.size(); i++) {
    dbmsh3d_face* F = M->_new_face ();
    for (unsigned int j=0; j<ifaces[i].size(); j++) {     
      int id = ifaces[i][j];
      dbmsh3d_vertex* V = M->vertexmap (id);
      F->_ifs_add_bnd_V (V);
    }
    M->_add_face (F);
  }

  return M;
}

