//: dbsk3d_provis_bktseg.cxx
//: MingChing Chang
//  Mar 28, 2007

#include <dbgl/dbgl_dist.h>
#include <dbnl/dbnl_min_max_3.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbsk3d/pro/dbsk3d_process.h>
#include <dbsk3d/algo/dbsk3d_fs_segre_bkt_stitch.h>

#include <dbmsh3d/vis/dbmsh3d_vis_bucket_3d.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>

//: Surface Reconstruction with Adaptive Bucketing and Stitching.
//  1) Divide input points into space-division buckets.
//  2) Meshing all confident surfaces of each bucket.
//       - Meshing surface in each bucket using all points there.          
//       - Keep all triangles not completely outside each reduced bucket 
//         (by size dc), i.e. partially inside ones as finalized triangles in finalM_faces.
//  3) Loop through each bucket and finalize all remaining triangles.
//       - Identify the stitching sausage.
//       - Meshing surface in the sausage region, using all existing 
//         finalized triangles as inital triangles.
//       - Finalize only triangles intersecting the current bucket.
//         
//  Options:
//    - boxr is the extended box ratio.
//
SoSeparator* run_seg_adpt_bucketing (const vcl_vector<vgl_point_3d<double> >& input_pts,
                                     vcl_vector<vcl_vector<int> >& finalM_faces,
                                     const vcl_string dirprefix,
                                     const int npbkt, const bool b_check_dup,
                                     const float bdsphr_rr, const int bdsphr_sr,
                                     const float seg_msr, const int seg_topo_opt)
{
  SoSeparator* root = new SoSeparator;

  //The confident dist. ratio of faces near meshing boundary.
  // dc = d_median * bktseg_cdr
  const float bktseg_cdr = 3;

  // The stitching boundary overlapping ratio: initial faces to support stitching and to keep enough overlaps.
  // db - dc >= d_max.
  // d_max = d_median * seg_msr.
  const float bktseg_bor = seg_msr + bktseg_cdr;

  //1) Divide input points into space-division buckets.
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > all_pts;
  all_pts.resize (input_pts.size());
  for (unsigned int i=0; i<input_pts.size(); i++)
    all_pts[i] = vcl_pair<int, vgl_point_3d<double> > (i, input_pts[i]);
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_idpts (all_pts, npbkt, b_check_dup);

  //Generate bucket list (for generating run files and list files).
  vcl_vector<vcl_string> bucket_list;
  BktStruct->gen_bucket_list (dirprefix, bucket_list);

  //Generate the bucket list file of .P3D files (one for each bucket).
  gen_bktlst_txt (dirprefix, bucket_list);
  //Generate the bucket info file.
  gen_bktinfo_txt (dirprefix, bucket_list, BktStruct);
  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (dirprefix);

  //2) Meshing the surface using all points in each bucket.          
  //   Keep all triangles within each reduced bucket (by size dc)
  //   as finalized triangles in finalM_faces.

  //Save each rich mesh M to a file store the mesh with original vertex ids.
  dbmsh3d_richmesh* RM;
  
  //Rich mesh properties to store the original vertex id.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  double dm = 0, dm_avg = 0;
  int count = 0;
  //Loop through each bucket: the slices, rows, and buckets.  
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        
        //Meshing the surface in B with no initial triangles.
        vcl_vector<vcl_vector<int> > B_initM_faces; //B_initM_faces is empty.
        double d_median;
        RM = run_surface_meshing_rm (B->idpt_list(), B_initM_faces, dirprefix+"-tmp",
                                     bdsphr_rr, bdsphr_sr, seg_msr, seg_topo_opt>1, d_median);

        dm = vcl_max (dm, d_median);
        dm_avg += d_median;
        count++;

        //Save each reconsturcted surface mesh RM in the bucket to a file prefix_00_00_00-surface.ply
        vcl_string B_surf_ply = dirprefix;
        char buf[128];
        B_surf_ply += "_";
        sprintf (buf, "%02d", s);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", r);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", b);
        B_surf_ply += buf;
        B_surf_ply += ".ply";
        dbmsh3d_save_ply (RM, B_surf_ply.c_str(), bkt_vplist, bkt_fplist);

        delete RM;
      }
    }
  }

  //Visualize the buckets.
  root->addChild (draw_bktstr (BktStruct, dbmsh3d_cmd_cw(), SbColor(0,0,1)));

  //   Use the median sampling distance d_median to estimate the following
  //     - dm: the mediam of median samplin distance.
  //     - dmax: the max of triangle size.
  //     - dc: confident distance.
  //     - db: distance of overlapping stitching boundary.
  //     - si: the interior bucket extension for stitching sausage.
  //     - se: the exterior bucket extension for stitching sausage.
  dm_avg /= count;
  vul_printf (vcl_cerr, "\ndm_avg = %f\n", dm_avg);
  double dmax = dm * seg_msr;
  double dc = dm * bktseg_cdr;
  double db = dm * bktseg_bor;
  double si = dc + db;
  double se = dc + dmax;
  vul_printf (vcl_cerr, "\tEstimated median sampling dist. dm : %f.\n", dm);
  vul_printf (vcl_cerr, "\t      average dm of buckets dm_avg : %f.\n", dm_avg);
  vul_printf (vcl_cerr, "\t        maximum triangle size dmax : %f (dm * msr %f).\n", dmax, seg_msr);
  vul_printf (vcl_cerr, "\t                confident dist. dc : %f (dm * cdr %f).\n", dc, bktseg_cdr);
  vul_printf (vcl_cerr, "\t overlapping stitching boundary db : %f (dm * bor %f).\n", db, bktseg_bor);
  vul_printf (vcl_cerr, "\t         stitching sausage int. si : %f (dc + db).\n", si);
  vul_printf (vcl_cerr, "\t         stitching sausage ext. se : %f (dc+dmax).\n", se);

  //Storage for stitching sausage mesh.
  //The stitchM is a full MHE mesh (without the use of IFS to save memory).
  ///vcl_vector<vcl_vector<int> > stitchS_faces;
  dbmsh3d_mesh* stitchM = new dbmsh3d_mesh;

  //Loop through the buckets again to put all confident triangles to finalM_faces.
  // - for non-bordering buckets: triangles within each reduced bucket (by size dc)
  // - for bordering buckets: not reducing the size in the bordering sides.
  vgl_box_3d<double> rbox, sboxin;
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    if (s==0) {
      sboxin.set_min_z (S->min_z());
      rbox.set_min_z (S->min_z());
    }
    else {
      sboxin.set_min_z (S->min_z() + si);
      rbox.set_min_z (S->min_z() + dc);
    }
    if (s==BktStruct->slice_list().size()-1) { 
      sboxin.set_max_z (S->max_z());
      rbox.set_max_z (S->max_z());
    }
    else {
      sboxin.set_max_z (S->max_z() - si);
      rbox.set_max_z (S->max_z() - dc);
    }
    assert (sboxin.min_z() < sboxin.max_z());

    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      //rbox.set_min_y ( r==0 ? R->min_y() : R->min_y() + dc);
      //rbox.set_max_y ( r==S->row_list().size()-1 ? R->max_y() : R->max_y() - dc);
      if (r==0) {
        sboxin.set_min_y (R->min_y());
        rbox.set_min_y (R->min_y());
      }
      else {
        sboxin.set_min_y (R->min_y() + si);
        rbox.set_min_y (R->min_y() + dc);
      }
      if (r==S->row_list().size()-1) {
        sboxin.set_max_y (R->max_y());
        rbox.set_max_y (R->max_y());
      }
      else {
        sboxin.set_max_y (R->max_y() - si);
        rbox.set_max_y (R->max_y() - dc);
      }
      assert (sboxin.min_y() < sboxin.max_y());

      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        //rbox.set_min_x ( b==0 ? B->min_x() : B->min_x() + dc);
        //rbox.set_max_x ( b==R->bucket_list().size()-1 ? B->max_x() : B->max_x() - dc);
        if (b==0) {
          sboxin.set_min_x (B->min_x());
          rbox.set_min_x (B->min_x());
        }
        else {
          sboxin.set_min_x (B->min_x() + si);
          rbox.set_min_x (B->min_x() + dc);
        }
        if (b==R->bucket_list().size()-1) {
          sboxin.set_max_x (B->max_x());
          rbox.set_max_x (B->max_x());
        }
        else {
          sboxin.set_max_x (B->max_x() - si);
          rbox.set_max_x (B->max_x() - dc);
        }
        ///assert (sboxin.min_x() < sboxin.max_x());

        //Read in the reconstructed surfaces in each bucket B.
        vcl_string B_surf_ply = dirprefix;
        char buf[128];
        B_surf_ply += "_";
        sprintf (buf, "%02d", s);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", r);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", b);
        B_surf_ply += buf;
        B_surf_ply += ".ply";
        RM = new dbmsh3d_richmesh;
        dbmsh3d_load_ply (RM, B_surf_ply.c_str(), bkt_vplist, bkt_fplist);

        //Recover the original vertex id.        
        vcl_map<int, dbmsh3d_vertex*>::iterator vit = RM->vertexmap().begin();
        for (; vit != RM->vertexmap().end(); vit++) {
          dbmsh3d_richvertex* RV = (dbmsh3d_richvertex*) (*vit).second;          
          double id;
          RV->get_scalar_property("id", id);
          RV->set_id (int(id));
        }

        //Visualize the reduced bucket.
        root->addChild (draw_box (rbox, dbmsh3d_cmd_cw(), SbColor(0,0.5,0)));

        //For all faces inside the reduced-box:
        vcl_map<int, dbmsh3d_face*>::iterator fit = RM->facemap().begin();
        for (; fit != RM->facemap().end(); fit++) {
          dbmsh3d_face* F = (*fit).second;
          if (F->_ifs_inside_box (rbox)) {
            vcl_vector<int> vids;
            for (unsigned int i=0; i<F->vertices().size(); i++)
              vids.push_back (F->vertices(i)->id());
            assert (vids.size() > 2);
            
            //If the face is completely outside sboxin, add to stitchM. 
            //Otherwise, add to finalM.
            if (F->_ifs_outside_box (sboxin)) //add a new face of vids add it to stitchM.
              add_F_to_M (vids, stitchM);
            else
              finalM_faces.push_back (vids);
          }
        }
        delete RM;
      }
    }
  }

  // Visualize the intermediate mesh of interior trinagles in buckets.
  vcl_string int_surf = dirprefix;
  int_surf += "-int.ply2";
  dbmsh3d_save_ply2 (input_pts, finalM_faces, int_surf.c_str());
  root->addChild (draw_ifs (input_pts, finalM_faces, COLOR_SILVER, true));

  //  3) Loop through each bucket and finalize all remaining triangles.
  //
  // Identify the stitching sausage in and out box.
  vgl_box_3d<double> sboxout; 
  vgl_box_3d<double> box, boxin;  //for trimming the stitchM
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    if (s==0) {
      sboxin.set_min_z (S->min_z());
      sboxout.set_min_z (S->min_z());
      boxin.set_min_z (S->min_z());      
    }
    else {
      sboxin.set_min_z (S->min_z() + si);
      sboxout.set_min_z (S->min_z() - se);
      boxin.set_min_z (S->min_z() + db);
    }
    if (s==BktStruct->slice_list().size()-1) { 
      sboxin.set_max_z (S->max_z());
      sboxout.set_max_z (S->max_z());
      boxin.set_max_z (S->max_z());
    }
    else {
      sboxin.set_max_z (S->max_z() - si);
      sboxout.set_max_z (S->max_z() + se);
      boxin.set_max_z (S->max_z() - db);
    }

    for (unsigned int r=0; r<S->row_list().size(); r++) {
      dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      if (r==0) {
        sboxin.set_min_y (R->min_y());
        sboxout.set_min_y (R->min_y());
        boxin.set_min_y (R->min_y());
      }
      else {
        sboxin.set_min_y (R->min_y() + si);
        sboxout.set_min_y (R->min_y() - se);
        boxin.set_min_y (R->min_y() + db);
      }
      if (r==S->row_list().size()-1) {
        sboxin.set_max_y (R->max_y());
        sboxout.set_max_y (R->max_y());
        boxin.set_max_y (R->max_y());
      }
      else {
        sboxin.set_max_y (R->max_y() - si);
        sboxout.set_max_y (R->max_y() + se);
        boxin.set_max_y (R->max_y() - db);
      }
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());
        if (b==0) {
          sboxin.set_min_x (B->min_x());
          sboxout.set_min_x (B->min_x());
          boxin.set_min_x (B->min_x());
        }
        else {
          sboxin.set_min_x (B->min_x() + si);
          sboxout.set_min_x (B->min_x() - se);
          boxin.set_min_x (B->min_x() + db);
        }
        if (b==R->bucket_list().size()-1) {
          sboxin.set_max_x (B->max_x());
          sboxout.set_max_x (B->max_x());
          boxin.set_max_x (B->max_x());
        }
        else {
          sboxin.set_max_x (B->max_x() - si);
          sboxout.set_max_x (B->max_x() + se);
          boxin.set_max_x (B->max_x() - db);
        }
        
        //Visualize the sausage.
        //if (b==1) {
          ///root->addChild (draw_box (sboxout, dbmsh3d_cmd_cw(), SbColor(1,0,0)));
          ///root->addChild (draw_box (sboxin, dbmsh3d_cmd_cw(), SbColor(0.5,0,0)));
        //}

        //Meshing surface in the sausage region, using all existing 
        //finalized triangles as inital triangles.
        vcl_vector<vcl_vector<int> > S_initM_faces;
        vcl_vector<vcl_pair<int, vgl_point_3d<double> > > S_idpts;

        //To speed up searching, only searching the stitchM is enough.
        //(in comparison to searching the whole finalM_faces.)
        get_sausage_init_faces (input_pts, stitchM, sboxin, sboxout, S_initM_faces);
        
        //Only add the points inside the stitching zone into S_idpts.
        BktStruct->get_sausage_idpts (s, r, b, sboxin, sboxout, S_idpts);

        //Debug: assert that all points of the S_initM_faces are in the S_idpts set.
        vcl_set<int> ptids;
        for (unsigned int i=0; i<S_idpts.size(); i++)
          ptids.insert (S_idpts[i].first);
        for (unsigned int i=0; i<S_initM_faces.size(); i++) {
          for (unsigned int j=0; j<S_initM_faces[i].size(); j++) {
            int id = S_initM_faces[i][j];
            if (ptids.find (id) == ptids.end()) {
              // assert!!!
              S_idpts.push_back (vcl_pair<int, vgl_point_3d<double> > (id, input_pts[id]));
            }
          }
        }
        ptids.clear();

        //Meshing the surface in the sausage with the initial triangles.
        //Q: How to avoid growing of triangles on the 'bad' side of the 
        //   where other finalized triangles exist?
        double d_median;
        RM = run_surface_meshing_rm (S_idpts, S_initM_faces, dirprefix+"-tmp",
                                     bdsphr_rr, bdsphr_sr, seg_msr, seg_topo_opt>1, d_median);
        dm = vcl_max (dm, d_median);

        //Save the stitching mesh.
        vcl_string B_surf_ply = dirprefix;
        char buf[128];
        B_surf_ply += "_";
        sprintf (buf, "%02d", s);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", r);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", b);
        B_surf_ply += buf;
        B_surf_ply += "_stitch.ply";
        dbmsh3d_save_ply (RM, B_surf_ply.c_str(), bkt_vplist, bkt_fplist);

        //Put all new qualified triangles into stitchM (and finalize them).
        // - skip faces completely outside bucket box.
        // - skip faces partially inside boxin (shrinking db = si-dc)
        // - not causing any topological problems 
        //   (non-2-manifold edge-junction or vertex-1-ring).        
        vcl_map<int, dbmsh3d_face*>::iterator fit = RM->facemap().begin();
        for (; fit != RM->facemap().end(); fit++) {
          dbmsh3d_face* F = (*fit).second;
          if (F->_ifs_outside_box (box))
            continue;
          if (F->_ifs_outside_box (boxin) == false)
            continue;

          //Check topology and add F to stitchM
          vcl_vector<int> vids;
          for (unsigned int i=0; i<F->vertices().size(); i++)
            vids.push_back (F->vertices(i)->id());
          add_F_to_M_check_topo (vids, stitchM);
        }

        delete RM;
      }
    }
  }

  //Add all stitching sausage triangles into finalized triangles.
  add_M_faces_to_IFSset (stitchM, finalM_faces);
  delete stitchM;

  //Save final mesh after stitching.
  vcl_string final_surf = dirprefix;
  final_surf += "-stitch-final.ply2";
  dbmsh3d_save_ply2 (input_pts, finalM_faces, final_surf.c_str());

  delete BktStruct;
  return root;
}

void run_seg_adpt_bucketing_2 (vcl_vector<vgl_point_3d<double> >& pts,
                             const int M, const vcl_string prefix,
                             const bool b_check_dup, const float bktbr,
                             const float msr)
{
  //The bucketing structure
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_pts (pts, M, b_check_dup);

  //Generate bucket list (for generating run files and list files).
  vcl_vector<vcl_string> bucket_list;
  BktStruct->gen_bucket_list (prefix, bucket_list);
  
  //Generate the bucket list file of .P3D files (one for each bucket).
  gen_bktlst_txt (prefix, bucket_list);
  //Generate the bucket info file.
  gen_bktinfo_txt (prefix, bucket_list, BktStruct);
  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (prefix);

  //Save points in extended bucket to a P3D file.
  BktStruct->save_extend_bkt_p3d (prefix, bktbr);

  //Generate the run file to mesh surfaces in buckets.
  gen_bkt1_seg_bat (prefix, bucket_list, msr);
  //Generate the bucketing surface list file.
  gen_bkt1_seg_txt (prefix, bucket_list);
  //Generate the run file to view surfaces in buckets.
  gen_bkt1_seg_view_bat (prefix);

  //Generate the run file for creating rich mesh storing original vertex ids.
  gen_bkt2_rmshid_bat (prefix);
  //Generate the run file for pre-stitching.
  gen_bkt3_prestitch_bat (prefix);

  //Generate the run file to compute the stitching surface.
  gen_bkt4_stitch_bat (prefix, msr);

  //Generate the run file for creating rich mesh of stitching surface.
  gen_bkt5_stitch_rmsh3d_bat (prefix);

  //Generate the list file of final surfaces.
  gen_bktfinal_txt (prefix, bucket_list);
  //Generate the run file to view final surfaces.
  gen_bktfin_view_bat (prefix);

  //Generate the run file to compute the final surface.
  gen_bkt6_final_bat (prefix);

  delete BktStruct;
}


void run_seg_adpt_bucketing_3 (const vcl_vector<vgl_point_3d<double> >& input_pts,
                               const int npbkt, const vcl_string& dirprefix,
                               const bool b_check_dup, const float bktbr,
                               const float bdsphr_rr, const float bdsphr_sr, 
                               const float seg_msr, const int seg_topo_opt)
{
  //The bucketing structure
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > all_pts;
  all_pts.resize (input_pts.size());
  for (unsigned int i=0; i<input_pts.size(); i++)
    all_pts[i] = vcl_pair<int, vgl_point_3d<double> > (i, input_pts[i]);
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_idpts (all_pts, npbkt, b_check_dup);

  //Generate bucket list (for generating run files and list files).
  vcl_vector<vcl_string> bucket_list;
  BktStruct->gen_bucket_list (dirprefix, bucket_list);
  
  //Save points in each bucket to a P3D file.
  BktStruct->save_bucket_p3d (dirprefix);

  //Generate the bucket list file of .P3D files (one for each bucket).
  gen_bktlst_txt (dirprefix, bucket_list);
  //Generate the bucket info file.
  gen_bktinfo_txt (dirprefix, bucket_list, BktStruct);
  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (dirprefix);

  //Generate the stitching surface
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > stitchS_idpts;
  //-bktbr: box ratio
  get_stitch_surface_pts (BktStruct, stitchS_idpts, bktbr);

  //Mesh the stitching surface S.
  vcl_vector<vcl_vector<int> > stitchS_init_faces;
  dbmsh3d_mesh* M;
  double d_median;
  M = run_surface_meshing (stitchS_idpts, stitchS_init_faces, dirprefix+"-tmp",
                           bdsphr_rr, bdsphr_sr, seg_msr, seg_topo_opt, d_median);

  //Save the reconsturcted surface to .ply file.       
  dbmsh3d_save_ply (M, (dirprefix + "-stitch.ply").c_str());
  delete M;
  delete BktStruct;
}

void run_seg_adpt_bucketing_4 (const vcl_vector<vgl_point_3d<double> >& input_pts,
                               vcl_vector<vcl_vector<int> >& finalM_faces,
                               const vcl_string& dirprefix,
                               const int npbkt, const bool b_check_dup, const float bktbr,
                               const float bdsphr_rr, const int bdsphr_sr,
                               const float seg_msr, const int seg_topo_opt,
                               const int n_erode) 
{
  //1) Divide input points into space-division buckets.
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > all_pts;
  all_pts.resize (input_pts.size());
  for (unsigned int i=0; i<input_pts.size(); i++)
    all_pts[i] = vcl_pair<int, vgl_point_3d<double> > (i, input_pts[i]);
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_idpts (all_pts, npbkt, b_check_dup);

  //Get the stitching surface S.
  vcl_vector<vcl_vector<int> > stitchM_faces;
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > stitchS_idpts;
  //-bktbr: box ratio
  get_stitch_surface_pts (BktStruct, stitchS_idpts, bktbr);

  //Mesh the stitching surface S.
  vcl_vector<vcl_vector<int> > stitchS_init_faces; //empty
  dbmsh3d_mesh* M;
  double d_median;
  vcl_string tmpprefix = dirprefix+"-tmp";
  M = run_surface_meshing (stitchS_idpts, stitchS_init_faces, tmpprefix,
                           bdsphr_rr, bdsphr_sr, seg_msr, seg_topo_opt>1, d_median);

  M->IFS_to_MHE();
  erode_mesh_boundary (M, n_erode);

  //Save the faces of S to stitchM_faces.
  add_mesh_faces_IFS (M, stitchM_faces);
  
  //Save the reconsturcted surface to .ply file.       
  dbmsh3d_save_ply2 (input_pts, stitchM_faces, (dirprefix + "-stitch.ply2").c_str());

  delete M;

  //2) Meshing surface in each bucket B with initial triangles from stitching surface S
  
  //Loop through each bucket: the slices, rows, and buckets.
  vgl_box_3d<double> box;
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
        
        //Determine all faces of stitching surface S that intersects B as initial mesh.
        vcl_vector<vcl_vector<int> > B_initM_faces;
        get_faces_intersect_box (stitchM_faces, input_pts, box, B_initM_faces);

        //Get the points for local meshing in this bucket B.
        vcl_vector<vcl_pair<int, vgl_point_3d<double> > > B_meshing_idpts;
        get_pts_local_bucket_meshing (B, B_initM_faces, input_pts, B_meshing_idpts);

        //Meshing the surface in B using the initial mesh.    
        double d_median;    
        M = run_surface_meshing (B_meshing_idpts, B_initM_faces, tmpprefix,
                                 bdsphr_rr, bdsphr_sr, seg_msr, seg_topo_opt>1, d_median);

        //Save the faces of S to B_faces.
        ///vcl_vector<vcl_vector<int> > B_faces;
        ///add_mesh_faces_IFS (pv1->bnd_mesh(), B_faces);

        //Add the surface in bucket B to finalM_faces.
        //(Avoiding duplications and violation of 2-manifold.)
        // -ne: steps of erosion.
        M->IFS_to_MHE();
        erode_mesh_boundary (M, n_erode);
        merge_meshes (input_pts, finalM_faces, M);
        
        //Save the reconsturcted surface to .ply file. 
        vcl_string B_surf_ply = dirprefix;
        char buf[128];
        B_surf_ply += "_";
        sprintf (buf, "%02d", s);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", r);
        B_surf_ply += buf;
        B_surf_ply += "_";
        sprintf (buf, "%02d", b);
        B_surf_ply += buf;
        dbmsh3d_save_ply (M, (B_surf_ply + ".ply").c_str(), true);

        delete M;
        //Update changes in S from the local bucket meshing.

      }
    }
  }

  delete BktStruct;    

  //3) Collect all triangles to the final output surface.
  //No need to add the stitchM_faces, since surfaces from all buckets are sufficient.
  ///finalM_faces.insert (finalM_faces.end(), stitchM_faces.begin(), stitchM_faces.end());

  dbmsh3d_mesh* Ms = build_mesh_from_IFS (input_pts, stitchM_faces);
  Ms->IFS_to_MHE();
  erode_mesh_boundary (Ms, n_erode);
  merge_meshes (input_pts, finalM_faces, Ms);
  delete Ms;

  vcl_string final_surf = dirprefix;
  final_surf += "_bktseg_final.ply2";
  dbmsh3d_save_ply2 (input_pts, finalM_faces, final_surf.c_str());

}

void run_seg_cell_bucketing (dbmsh3d_pt_set* pts, const vcl_string prefix,
                             const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                             const float msr)
{
  //The bucketing structure
  cell_bucketing (pts, prefix, BUCKET_NX, BUCKET_NY, BUCKET_NZ);
  
  //Generate the bucket list file of .P3D files (one for each bucket).
  vcl_vector<vcl_string> bucket_list;
  /*gen_bktlst_txt (prefix, bucket_list);

  //Generate the run file to view bucketing results.
  gen_bktlst_view_bat (prefix);

  //Generate the meshing run file for all buckets.
  ///gen_bkt1_seg_bat (prefix, bucket_list, msr);
  //Generate the bucketing surface list file.
  gen_bkt1_seg_txt (prefix, bucket_list);
  //Generate the run file to view surfaces in buckets.
  gen_bkt1_seg_view_bat (prefix);
  
  //Generate the run file for pre-stitching.
  gen_bkt3_prestitch_bat (prefix);
  
  //Generate the run file to compute the stitching surface.
  ///gen_bkt4_stitch_bat (prefix, msr);

  //Generate the list files of gaps between buckets (each for a stitching).
  gen_bktbnd_cellxyz_txt (prefix, bbox, BUCKET_NX, BUCKET_NY, BUCKET_NZ);

  //Generate the run file to collect the partial mesh to fill the gap between buckets.
  gen_bktbnd_cellxyz_pm_bat (prefix, bbox, BUCKET_NX, BUCKET_NY, BUCKET_NZ);

  //Generate the run file to compute surfaces to fill the gap between buckets.
  gen_bktbnd_cellxyz_seg_bat (prefix, bbox, BUCKET_NX, BUCKET_NY, BUCKET_NZ);

  //Generate the list file for the final stitching.
  gen_bktsth_txt_cell (prefix, bucket_list, bbox, BUCKET_NX, BUCKET_NY, BUCKET_NZ);

  //Generate the final run file to stitch the surfaces together.
  gen_bktsth_bat (prefix);*/
}


void run_seg_cell_bucketing_2 (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                                  const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                                  const float msr)
{
}

