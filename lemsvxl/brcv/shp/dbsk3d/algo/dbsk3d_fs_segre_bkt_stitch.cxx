//: dbsk3d_fs_segre_bkt_stitch.cxx
//: MingChing Chang
//  May 31, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_vector_fixed.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_kd_tree.h>

#include <dbgl/dbgl_dist.h>
#include <dbnl/dbnl_min_max_3.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbsk3d/algo/dbsk3d_fs_segre_bkt_stitch.h>


// ###########################################################################
//     Top-level stitching functions
// ###########################################################################

//: Recover the original vertex id into a rich mesh.
bool bkt_recover_vid_richmesh (const char* prefix)
{
  vcl_string bktinfo_file = prefix;
  bktinfo_file += "_bktinfo.txt";
  vul_printf (vcl_cout, "bkt_recover_vid_richmesh(): %s\n", bktinfo_file.c_str());

  vcl_FILE* fp;
  if ((fp = vcl_fopen (bktinfo_file.c_str(), "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open txt file %s\n", bktinfo_file.c_str());
    return false;
  }  

  //Read in the original point cloud file prefix.p3d.
  vcl_string orig_p3d = prefix;
  orig_p3d += ".p3d";
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpts;
  if (!dbmsh3d_load_p3d (idpts, orig_p3d.c_str())) {
    vul_printf (vcl_cout, "Can't open original P3D file %s\n", orig_p3d.c_str());
    return false;
  }
  
  //Build a kdtree for the original points idpts.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_idpts (idpts);

  //Rich mesh properties.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  //Read in each mesh file in the prefix_bktinfo_.txt list file.
  int ret, count = 0;
  do {
    char bktfile[128];
    double minx, miny, minz, maxx, maxy, maxz;
    ret = vcl_fscanf (fp, "%s (%lf, %lf, %lf) - (%lf, %lf, %lf)\n", 
                      bktfile, &minx, &miny, &minz, &maxx, &maxy, &maxz);
    if (ret != EOF) {      
      //Read in the richmesh for each prefix_00_00_00 (from bktfile).
      dbmsh3d_richmesh* RM = new dbmsh3d_richmesh;
      vcl_string bkt_surf_ply = bktfile;
      bkt_surf_ply += "-surface.ply";
      if (dbmsh3d_load_ply (RM, bkt_surf_ply.c_str()) == false) {
        vul_printf (vcl_cout, "Can't open PLY file %s\n", bkt_surf_ply.c_str());
        return false;
      }

      //Recover the original vertex id into the richmesh RM.
      recover_vid_richmesh (RM, kdtree);      
      //Print out the properties of each vertex
      RM->print_summary(vcl_cout); 

      //Save the richmesh into "prefix_00_00_00_rsurf.ply
      vcl_string rsurf_ply = bktfile;
      rsurf_ply += "_rsurf.ply";      
      
      dbmsh3d_save_ply (RM, rsurf_ply.c_str(), bkt_vplist, bkt_fplist, ASCII);
      delete RM;
      count++;
    }
  }
  while (ret != EOF);

  delete kdtree;
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Pre-stitch surfaces of %d buckets.\n", count);  
  return true;
}

void recover_vid_richmesh (dbmsh3d_richmesh* RM, rsdl_kd_tree* kdtree)
{
  //Loop through each vertex of rich mesh RM
  vcl_map<int, dbmsh3d_vertex*>::iterator it = RM->vertexmap().begin();
  for (; it != RM->vertexmap().end(); it++) {
    dbmsh3d_richvertex* V = (dbmsh3d_richvertex*) (*it).second;

    //Find the closest id from V to idpts
    int top_n = 1;
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;

    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (V->pt().x(), V->pt().y(), V->pt().z());
    query_pt.set_cartesian (P3);
    kdtree->n_nearest (query_pt, top_n, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == top_n);
    int orig_id = near_neighbor_indices[0];

    //Put the orig_id into the property of V in the richmesh.
    V->add_scalar_property ("id", orig_id);
  }
}

//: Read in the bucket bounding box info.
bool bkt_pre_stitch_buckets (const char* prefix, const float rbox_ratio)
{  
  vcl_string bktinfo_file = prefix;
  bktinfo_file += "_bktinfo.txt";

  vcl_FILE* fp;
  if ((fp = vcl_fopen (bktinfo_file.c_str(), "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open txt file %s\n", bktinfo_file.c_str());
    return false;
  }

  vul_printf (vcl_cout, "bkt_pre_stitch_buckets(): %s\n", bktinfo_file.c_str());  
  
  //The rich mesh of candidate stitching faces (triangles).
  //initial faces marked as 1.
  dbmsh3d_richmesh* candM = new dbmsh3d_richmesh;
  vcl_vector<vcl_string > candM_vplist;  
  vcl_vector<vcl_string > candM_fplist;
  candM_fplist.push_back("i");  

  //Rich mesh properties of surfaces in buckets.
  //vertex property: original id.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  //Read in (x, y, z) points.
  int ret, count = 0;
  do {
    char bktfile[128];
    double minx, miny, minz, maxx, maxy, maxz;
    ret = vcl_fscanf (fp, "%s (%lf, %lf, %lf) - (%lf, %lf, %lf)\n", 
                      bktfile, &minx, &miny, &minz, &maxx, &maxy, &maxz);
    if (ret != EOF) {
      //Process the surfaces in the current bucket.
      vgl_box_3d<double> box;
      box.set_min_x (minx);
      box.set_min_y (miny);
      box.set_min_z (minz);
      box.set_max_x (maxx);
      box.set_max_y (maxy);
      box.set_max_z (maxz);

      //Compute the reduced box.      
      double red = rbox_ratio * dbnl_max3 (box.width(), box.height(), box.depth());
      vgl_box_3d<double> rbox = dbgl_reduce_box (box, red);
      
      //Read in the richmesh from file bktfile.
      dbmsh3d_richmesh* RM = new dbmsh3d_richmesh;
      vcl_string bkt_surf_ply = bktfile;
      bkt_surf_ply += "_rsurf.ply";
      dbmsh3d_load_ply (RM, bkt_surf_ply.c_str(), bkt_vplist, bkt_fplist);
      //Print out the properties of each vertex
      RM->print_summary(vcl_cout); 

      //Put the 'interior' triangles (within rbox) to the file "bktfile_int.ply".
      vcl_string bkt_int_ply = bktfile;
      bkt_int_ply += "_int.ply";
      
      filter_bucket_surf (RM, box, rbox, bkt_int_ply, candM,
                          bkt_vplist, bkt_fplist);

      delete RM;
      count++;
    }
  }
  while (ret != EOF);

  //Save the candidate triangle mesh to prefix_stitch_cand.ply.
  vcl_string stitch_cand_p3d = prefix;
  stitch_cand_p3d += "_stitch_cand.p3d";
  dbmsh3d_save_p3d (candM, stitch_cand_p3d.c_str());
  vcl_string stitch_cand_ply = prefix;
  stitch_cand_ply += "_stitch_cand.ply";
  dbmsh3d_save_ply (candM, stitch_cand_ply.c_str(), candM_vplist, candM_fplist);
  delete candM;

  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Pre-stitch surfaces of %d buckets.\n", count);  
  return true;
}

bool bkt_stitchsurf_recover_vid (const char* prefix)
{
  //Read in the original point cloud file prefix.p3d.
  vcl_string orig_p3d = prefix;
  orig_p3d += ".p3d";
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpts;
  if (!dbmsh3d_load_p3d (idpts, orig_p3d.c_str())) {
    vul_printf (vcl_cout, "Can't open original P3D file %s\n", orig_p3d.c_str());
    return false;
  }
  
  //Build a kdtree for the original points idpts.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_idpts (idpts);

  //Read in the source surface into a rich mesh.
  dbmsh3d_richmesh* RM = new dbmsh3d_richmesh;
  vcl_string surf_file = prefix;
  surf_file += "_stitch_cand-surface.ply";
  if (dbmsh3d_load_ply (RM, surf_file.c_str()) == false) {
    vul_printf (vcl_cout, "Can't open PLY file %s\n", surf_file.c_str());
    return false;
  }
  
  //Rich mesh properties.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  //Recover the original vertex id into the richmesh RM.
  recover_vid_richmesh (RM, kdtree);      
  //Print out the properties of each vertex
  RM->print_summary(vcl_cout); 

  //Save the richmesh into the output file.
  vcl_string surf_ofile = prefix;
  surf_ofile += "_stitch.ply";
  dbmsh3d_save_ply (RM, surf_ofile.c_str(), bkt_vplist, bkt_fplist, ASCII);
  
  delete kdtree;
  delete RM;
  return true;
}

void filter_bucket_surf (dbmsh3d_richmesh* RM, const vgl_box_3d<double>& box,
                         const vgl_box_3d<double>& rbox,
                         const vcl_string& bkt_int_ply, 
                         dbmsh3d_richmesh* candM,
                         const vcl_vector<vcl_string>& bkt_vplist,
                         const vcl_vector<vcl_string>& bkt_fplist)
{
  //Go through all faces in RM
  vcl_map<int, dbmsh3d_face*>::iterator it = RM->facemap().begin();
  while (it != RM->facemap().end()) {
    dbmsh3d_face* F = (*it).second;
    
    if (F->is_inside_box (rbox)) {
      //If F inside the reduced 'interior box' rbox, keep it.
      it++;
    }
    else {
      if (F->is_outside_box (box) == false) {
        //F is a candidate for stitching, add it to candM.
        add_face_check_richmesh_v (candM, F, false);
      }
      
      //Delete F.
      vcl_map<int, dbmsh3d_face*>::iterator next = it;
      next++;
      RM->_del_face (F);
      RM->facemap().erase (it);
      it = next;
    }
  }

  //Save all remaining mesh into file bkt_int_ply "prefix_00_00_00_int.ply".
  dbmsh3d_save_ply (RM, bkt_int_ply.c_str(), bkt_vplist, bkt_fplist, ASCII);

  //Save all boundary triangles (near the bucket) of RM to the candM with attribute 1.  
  RM->IFS_to_MHE();
  dbmsh3d_bnd_chain_set* bnd_chain_set = new dbmsh3d_bnd_chain_set (RM);
  bnd_chain_set->detect_bnd_chains ();

  //Get all boundary triangles of chain >= 10 triangles.
  vcl_vector<dbmsh3d_bnd_chain*>::iterator bit = bnd_chain_set->chainset().begin();
  for (; bit != bnd_chain_set->chainset().end(); bit++) {
    dbmsh3d_bnd_chain* BC = (*bit);
    if (BC->HE_list().size() < 10)
      continue;

    for (unsigned int i=0; i<BC->HE_list().size(); i++) {
      dbmsh3d_face* F = BC->HE_list(i)->face();
      //add the 'internal boundary' triangles with attribute 1.
      add_face_check_richmesh_v (candM, F, true);
    }
  }
  delete bnd_chain_set;
}

void add_face_check_richmesh_v (dbmsh3d_richmesh* candM, const dbmsh3d_face* F,
                                const bool flag)
{  
  //Add F to candM (with boolean rich info 'flag').
  dbmsh3d_face* newF = candM->_new_face ();  
  candM->_add_face (newF);

  //Go through each richvertex of F
  for (unsigned int i=0; i<F->vertices().size(); i++) {
    dbmsh3d_richvertex* V = (dbmsh3d_richvertex*) F->vertices(i);
    double val;
    V->get_scalar_property ("id", val);
    int orig_id = int (val);

    //Add V to candM without duplication.
    dbmsh3d_vertex* newV;
    if (candM->contains_V (orig_id) == false) {
      //Add V to candM, if V not already in candM. 
      newV = candM->_new_vertex (orig_id);
      newV->get_pt().set (V->pt().x(), V->pt().y(), V->pt().z());
      candM->_add_vertex (newV);
    }
    else //Get the existing vertex as newV.
      newV = candM->vertexmap(orig_id);

    newF->_ifs_add_bnd_V (newV);
    newV->set_meshed (true);
  }
}

bool bkt_merge_final_surface (const char* prefix, dbmsh3d_mesh* M)
{
  assert (M->vertexmap().size() == 0);

  //Read all original points into M.
  vcl_string orig_p3d = prefix;
  orig_p3d += ".p3d";
  if (!dbmsh3d_load_p3d (M, orig_p3d.c_str())) {
    vul_printf (vcl_cout, "Can't open original P3D file %s\n", orig_p3d.c_str());
    return false;
  }

  //1) Go through each 'internal' surfaces in each bucket and add triangles to the final surface.
  //   Add triangles using the original vertex ids.
  vcl_string bktinfo_file = prefix;
  bktinfo_file += "_bktinfo.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (bktinfo_file.c_str(), "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open txt file %s\n", bktinfo_file.c_str());
    return false;
  }
  vul_printf (vcl_cout, "bkt_merge_final_surface(): %s\n", bktinfo_file.c_str());  

  //Rich mesh properties.
  vcl_vector<vcl_string > bkt_vplist;
  bkt_vplist.push_back("id");  
  vcl_vector<vcl_string > bkt_fplist;

  //Read in each mesh file in the prefix_bktinfo_.txt list file.
  int ret, count = 0;
  do {
    char bktfile[128];
    double minx, miny, minz, maxx, maxy, maxz;
    ret = vcl_fscanf (fp, "%s (%lf, %lf, %lf) - (%lf, %lf, %lf)\n", 
                      bktfile, &minx, &miny, &minz, &maxx, &maxy, &maxz);
    if (ret != EOF) {
      //Read in the richmesh for each prefix_00_00_00 (from bktfile).
      dbmsh3d_richmesh* RM = new dbmsh3d_richmesh;
      vcl_string int_surf_ply = bktfile;
      int_surf_ply += "_int.ply";
      if (!dbmsh3d_load_ply (RM, int_surf_ply.c_str(), bkt_vplist, bkt_fplist)) {
        vul_printf (vcl_cout, "Can't open PLY file %s\n", int_surf_ply.c_str());
        return false;
      }
      vul_printf (vcl_cout, " adding %d faces in %s\n", RM->facemap().size(), int_surf_ply.c_str());

      //Put all triangles of RM into M
      create_faces_from_richmesh_vid (RM, M);
      delete RM;
      count++;
    }
  }
  while (ret != EOF);
  vcl_fclose (fp);

  //2) Go through the stitch surface and add triangles to the final surface.
  //   Be careful to maintaining a 2-manifold in stitching.
  dbmsh3d_richmesh* RM = new dbmsh3d_richmesh;
  vcl_string stitch_surf = prefix;
  stitch_surf += "_stitch.ply";
  if (!dbmsh3d_load_ply (RM, stitch_surf.c_str(), bkt_vplist, bkt_fplist)) {
    vul_printf (vcl_cout, "Can't open stitching surface %s\n", stitch_surf.c_str());
    return false;
  }
  
  create_faces_from_richmesh_vid_topo (RM, M);
  delete RM;

  //Save the final output surface
  vcl_string final_surf = prefix;
  final_surf += "_bktsth_final.ply";
  dbmsh3d_save_ply (M, final_surf.c_str());

  ///vcl_string final_surf2 = prefix;
  ///final_surf2 += "_bktsth_final.ply2";
  ///dbmsh3d_save_ply2 (M, final_surf2.c_str());

  return true;
}

//: Put all triangles of RM to M
void create_faces_from_richmesh_vid (dbmsh3d_richmesh* RM, dbmsh3d_mesh* M)
{
  //Go through all faces in RM and create one in M.
  vcl_map<int, dbmsh3d_face*>::iterator it = RM->facemap().begin();
  for (; it != RM->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;

    //Create a new face in M
    dbmsh3d_face* newF = M->_new_face ();
    M->_add_face (newF);
    
    //Go through each richvertex of F
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      dbmsh3d_richvertex* V = (dbmsh3d_richvertex*) F->vertices(i);
      double val;
      V->get_scalar_property ("id", val);
      int orig_id = int (val);

      dbmsh3d_vertex* newV = M->vertexmap (orig_id);
      assert (newV->id() == orig_id);      
      newF->_ifs_add_bnd_V (newV);
      newV->set_meshed (true);
    }
  }
}

void create_faces_from_richmesh_vid_topo (dbmsh3d_richmesh* RM, dbmsh3d_mesh* M)
{
  //Recover the halfedge representation.
  M->IFS_to_MHE ();
  M->build_face_IFS ();

  //Go through all faces in RM and create one in M.
  vcl_map<int, dbmsh3d_face*>::iterator it = RM->facemap().begin();
  for (; it != RM->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    
    //Prepare the vector of original vertex id of F
    vcl_vector<int> vids;
    vids.clear();
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      dbmsh3d_richvertex* V = (dbmsh3d_richvertex*) F->vertices(i);
      double val;
      V->get_scalar_property ("id", val);
      int orig_id = int (val);
      vids.push_back (orig_id);
    }

    vids.push_back (vids[0]); //add the head element to make circular.

    //Skip if introducing of such F violates the 2-manifold assumption.
    if (check_F_M_topo (vids, M) == false)
      continue;

    //Create a new face of F in M

    //1) First create all boundary edges of newF
    vcl_vector<dbmsh3d_edge*> new_edges;
    for (unsigned int i=1; i<vids.size(); i++) {
      dbmsh3d_vertex* newV1 = M->vertexmap (vids[i-1]);
      dbmsh3d_vertex* newV2 = M->vertexmap (vids[i]);
      dbmsh3d_edge* newE = E_sharing_2V (newV1, newV2);
      if (newE==NULL)
        newE = M->add_new_edge (newV1, newV2);
      new_edges.push_back (newE);
    }
    assert (new_edges.size() == F->vertices().size());

    //Create the newF from the list of new_edges.
    dbmsh3d_face* newF = M->_new_face ();
    M->_add_face (newF);

    for(unsigned int i=0; i<new_edges.size(); i++) {
      dbmsh3d_edge* newE = new_edges[i];
      newF->connect_bnd_E_end (newE);
    }
    
    //Go through each original vertex of F
    for (unsigned int i=0; i<vids.size()-1; i++) {
      dbmsh3d_vertex* newV = M->vertexmap (vids[i]);
      assert (newV->id() == vids[i]);      
      newF->_ifs_add_bnd_V (newV);
      newV->set_meshed (true);
    }
  }
}

//Generate the meshing run file for all buckets.
void gen_bkt1_seg_bat (const vcl_string& prefix,
                       const vcl_vector<vcl_string>& bucket_list,
                       const float msr)
{
  vcl_string runfile = prefix;
  runfile += "_bkt1_seg.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }
  char msrs[40];
  vcl_sprintf (msrs, "%2.0f", msr);

  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string run_cmd = "dbsk3dappw -seg 1 -msr ";
    run_cmd += msrs;
    run_cmd += " -gui 0 -file 1 -f ";
    run_cmd += bucket_list[x];
    vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  }
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate list file for points in buckets: %s.\n", runfile.c_str());
}

//Generate the bucketing surface list file.
void gen_bkt1_seg_txt (const vcl_string& prefix,
                       const vcl_vector<vcl_string>& bucket_list)
{  
  vcl_string listfile = prefix + "_bkt1_seg.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
    return;
  }
  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string fileprefix = bucket_list[x];
    fileprefix += "-surface.ply";
    vcl_fprintf (fp, "%s\n", fileprefix.c_str());
  }
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate list file for surfaces in buckets: %s.\n", listfile.c_str());
}

//Generate the run file to view surfaces in buckets.
void gen_bkt1_seg_view_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bkt1_seg_view.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -list ";
  run_cmd += prefix;
  run_cmd += "_bkt1_seg";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view surfaces in buckets: %s.\n", runfile.c_str());
}

//Generate the run file for creating richmesh storing original vertex ids.
void gen_bkt2_rmshid_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bkt2_rmshid.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -gui 0 -bktpst 1 -f ";
  run_cmd += prefix;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file richmesh (recovery vertex ids): %s.\n", runfile.c_str());  
}

//Generate the run file for pre-stitching.
void gen_bkt3_prestitch_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bkt3_prestitch.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -bktpst 2 -gui 0 -f ";
  run_cmd += prefix;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file for pre-stitching: %s.\n", runfile.c_str());  
}

//Generate the run file to compute the stitching surface.
void gen_bkt4_stitch_bat (const vcl_string& prefix, const float msr)
{
  vcl_string runfile = prefix + "_bkt4_stitch.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -seg 1 -f ";
  run_cmd += prefix;
  run_cmd += "_stitch_cand -segop 1 -msr ";
  char msrs[40];
  vcl_sprintf (msrs, "%2.0f", msr);
  run_cmd += msrs;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file for pre-stitching: %s.\n", runfile.c_str()); 
}

//Generate the run file for creating rich mesh of stitching surface.
void gen_bkt5_stitch_rmsh3d_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bkt5_rmshid.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -bktpst 3 -gui 0 -f ";
  run_cmd += prefix;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file richmesh (recovery vertex ids): %s.\n", runfile.c_str());  
}

//Generate the list file of final surfaces prefix_btkfinal.txt.
// All prefix_00_00_00_int.ply files + prefix_stitch_cand-surface.ply
void gen_bktfinal_txt (const vcl_string& prefix, 
                       const vcl_vector<vcl_string>& bucket_list)
{
  vcl_string listfile = prefix + "_bktfinal.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
    return;
  }

  //First put in each internal surfaces in buckets.
  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string fileprefix = bucket_list[x];
    fileprefix += "_int.ply";
    vcl_fprintf (fp, "%s\n", fileprefix.c_str());
  }
  //Finally put in the stitching surface.
  vcl_string stitch_file = prefix;
  stitch_file += "_stitch_cand.ply";
  vcl_fprintf (fp, "%s\n", stitch_file.c_str());

  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate list file of final surfaces: %s.\n", listfile.c_str());
}

//Generate the run file to view final surfaces.
void gen_bktfin_view_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bktfin_view.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -list ";
  run_cmd += prefix;
  run_cmd += "_bktfinal";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view final surfaces: %s.\n", runfile.c_str());
}

//Generate the run file to compute the final surface.
void gen_bkt6_final_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bkt6_final.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -merge 1 -f ";
  run_cmd += prefix;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to produce final surface: %s.\n", runfile.c_str()); 
}

//############################################################################

//Generate the list files of gaps between buckets (each for a stitching).
void gen_bktbnd_cellxyz_txt (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                             const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ)
{
  int x, y, z;
  vcl_string listfile;
  char buf[128];
  int total = 0;

  double BOX_X = bbox.max_x() - bbox.min_x();
  double BOX_Y = bbox.max_y() - bbox.min_y();
  double BOX_Z = bbox.max_z() - bbox.min_z();
  double BUCKET_LX = BOX_X / BUCKET_NX;
  double BUCKET_LY = BOX_Y / BUCKET_NY;
  double BUCKET_LZ = BOX_Z / BUCKET_NZ;

  //Produce the list of buckets to stitch the cuts ordered in X 
  for (x=1; x<BUCKET_NX; x++) {
    listfile = prefix;
    listfile += "_bktbnd_x_";
    sprintf (buf, "%02d", x-1);
    listfile += buf;
    listfile += "_";
    sprintf (buf, "%02d", x);
    listfile += buf;
    listfile += ".txt";

    vcl_FILE* fp;
    if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
      vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
      return;
    }

    //Put all adjacent buckets in (Y,Z) to this listfile.
    for (int xx=x-1; xx<=x; xx++) {
      for (y=0; y<BUCKET_NY; y++) {
        for (z=0; z<BUCKET_NZ; z++) {        
          vcl_string fileprefix = prefix;
          sprintf (buf, "%02d", xx);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", y);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", z);
          fileprefix += "_";
          fileprefix += buf;
          fileprefix += ".p3d";
          vcl_fprintf (fp, "%s\n", fileprefix.c_str());
        }
      }
    }
    vcl_fclose (fp);
    total++;
  }

  //Produce the list of buckets to stitch the cuts ordered in Y
  for (y=1; y<BUCKET_NY; y++) {
    listfile = prefix;
    listfile += "_bktbnd_y_";
    sprintf (buf, "%02d", y-1);
    listfile += buf;
    listfile += "_";
    sprintf (buf, "%02d", y);
    listfile += buf;
    listfile += ".txt";

    vcl_FILE* fp;
    if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
      vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
      return;
    }

    //Put all adjacent buckets in (Y,Z) to this listfile.
    for (x=0; x<BUCKET_NX; x++) {
      for (int yy=y-1; yy<=y; yy++) {      
        for (z=0; z<BUCKET_NZ; z++) {        
          vcl_string fileprefix = prefix;
          sprintf (buf, "%02d", x);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", yy);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", z);
          fileprefix += "_";
          fileprefix += buf;
          fileprefix += ".p3d";
          vcl_fprintf (fp, "%s\n", fileprefix.c_str());
        }
      }
    }
    vcl_fclose (fp);
    total++;     
  }

  //Produce the list of buckets to stitch the cuts ordered in Z
  for (z=1; z<BUCKET_NZ; z++) {
    listfile = prefix;
    listfile += "_bktbnd_z_";
    sprintf (buf, "%02d", z-1);
    listfile += buf;
    listfile += "_";
    sprintf (buf, "%02d", z);
    listfile += buf;
    listfile += ".txt";

    vcl_FILE* fp;
    if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
      vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
      return;
    }

    //Put all adjacent buckets in (Y,Z) to this listfile.
    for (x=0; x<BUCKET_NX; x++) {
      for (y=0; y<BUCKET_NY; y++) {      
        for (int zz=z-1; zz<=z; zz++) {        
          vcl_string fileprefix = prefix;
          sprintf (buf, "%02d", x);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", y);
          fileprefix += "_";
          fileprefix += buf;
          sprintf (buf, "%02d", zz);
          fileprefix += "_";
          fileprefix += buf;
          fileprefix += ".p3d";
          vcl_fprintf (fp, "%s\n", fileprefix.c_str());
        }
      }
    }
    vcl_fclose (fp);
    total++;    

  }

  vul_printf (vcl_cout, "  Generate %d cell bucket bnd list files.\n", total);
}

//Generate the run file to collect the partial mesh to fill the gap between buckets.
// For each bucket boundary, e.g., each list file generated in gen_bktbnd_cellxyz_txt() 
// (for example prefix_bktbnd_x_00_01.txt), crop the triangles and points inside an extended
// bounding box and save remaining partial mesh into a file (prefix_bktbnd_x_00_01_pm.ply).
//
// Note: 
//   1. This should include all triangles that are partially inside the bounding box.
//   2. Run file: prefix_bktbnd_x_00_01_pm.bat
//        dbsk3dappw -crop 2 -list prefix_bktbnd_x_00_01.txt -ofile prefix_btkbnd_x_00_01_pm.ply
//                   -minx -miny -minz -maxx -maxy -maxz
// 
void gen_bktbnd_cellxyz_pm_bat (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                                const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ)
{
  /*vcl_string runfile = prefix + "_bktbndseg.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile);
    return;
  }


  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string fileprefix = bucket_list[x];
    fileprefix += "-surface.ply";
    vcl_fprintf (fp, "%s\n", fileprefix.c_str());
  }
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate list file for surfaces in buckets: %s.\n", listfile.c_str());*/
}

//Generate the run file to compute surfaces to fill the gap between buckets.
// Note:
//   1. Run file: prefix_bktbnd_x_00_01_seg.bat
//        dbsk3dappw -seg 1 -f prefix_btkbnd_x_00_01_pm
//
void gen_bktbnd_cellxyz_seg_bat (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                                 const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ)
{

}

//Generate the list file for final stitching.
// Including all meshes in prefix_bktseg.txt and 
// all meshes generated in each gen_bktbnd_cellxyz_seg_bat().
void gen_bktsth_txt_cell (const vcl_string& prefix, 
                          const vcl_vector<vcl_string>& bucket_list,
                          const vgl_box_3d<double>& bbox,
                          const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ)
{
  vcl_string listfile = prefix + "_bktsth.txt";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", listfile.c_str());
    return;
  }

  //Add all meshes in buckets.
  for (int x=0; x<int(bucket_list.size()); x++) {
    vcl_string fileprefix = bucket_list[x];
    fileprefix += "-surface.ply";
    vcl_fprintf (fp, "%s\n", fileprefix.c_str());
  }

  //Add all bucket boundary meshes.
  int x, y, z;
  double BOX_X = bbox.max_x() - bbox.min_x();
  double BOX_Y = bbox.max_y() - bbox.min_y();
  double BOX_Z = bbox.max_z() - bbox.min_z();
  double BUCKET_LX = BOX_X / BUCKET_NX;
  double BUCKET_LY = BOX_Y / BUCKET_NY;
  double BUCKET_LZ = BOX_Z / BUCKET_NZ;

  //Add all bucket boundary meshes ordered in X
  for (x=1; x<BUCKET_NX; x++) {
  }

  //Add all bucket boundary meshes ordered in Y
  for (y=1; y<BUCKET_NY; y++) {
  }

  //Add all bucket boundary meshes ordered in Z
  for (z=1; z<BUCKET_NZ; z++) {
  }

  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate the list file for final stitching: %s.\n", listfile.c_str());
}

//Generate the final run file to stitch the surfaces together.
// Run file: prefix_bktsth.bat
//   dbsk3dappw -sth 1 -list prefix_bktsth.txt 
//   Final output file: prefix-bktsth-surface.ply
//
void gen_bktsth_bat (const vcl_string& prefix)
{
  vcl_string runfile = prefix + "_bktsth.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output txt file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -sth 1 -list ";
  run_cmd += prefix;
  run_cmd += "_bktsth.txt";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to stitch the final surface: %s.\n", runfile.c_str());
}


