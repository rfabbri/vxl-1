//: This is lemsvxlsrc/brcv/shp/dbmsh3dr/pro/dbmsh3dr_process.h
//  Feb 27, 2007   Ming-Ching Chang

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <rsdl/rsdl_kd_tree.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_feature_face_pt.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3dr/pro/dbmsh3dr_process.h>

//############################################################################

void dbmsh3dr_pro_base::output_min_dist (const char* filename)
{
  vul_printf (vcl_cout, "output_min_dist(): size %u.\n", min_dists_.size());

  FILE  *fp;
  if ((fp = fopen(filename, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file %s.\n", filename);
    return; 
  }

  for (unsigned int i=0; i<min_dists_.size(); i++) {
    vcl_fprintf (fp, "%.16f\n", min_dists_[i]);
  }
  fclose (fp); 
}

bool dbmsh3dr_pro_base::load_hmatrix_01 (vcl_string dirfile)
{
  return dbmsh3d_read_xform_file (dirfile.c_str(), hmatrix_01_);
}

//: affine transform meshp2 to match with meshp1.
void dbmsh3dr_pro_base::xform_hmatrix_01_pro1 ()
{
  if (is_hmatrix_01_identity())
    return;

  //transform using the specified affine xform matrix
  if (pro_[1]->pro_data() == dbmsh3d_pro::PD_PTS)
    dbmsh3d_apply_xform (pro_[1]->pts(), hmatrix_01_);
  else if (pro_[1]->pro_data() == dbmsh3d_pro::PD_ORIPTS)
    dbmsh3d_apply_xform (pro_[1]->oripts(), hmatrix_01_);
  else if (pro_[1]->pro_data() == dbmsh3d_pro::PD_MESH)
    dbmsh3d_apply_xform (pro_[1]->mesh(), hmatrix_01_);
  else
    assert (0);
}

bool dbmsh3dr_pro_base::read_list_file (const char* list_file)
{
  return dbmsh3d_read_list_file (list_file, data_files_, align_files_);
}


bool dbmsh3dr_pro_base::save_all_pts_xyz (const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cout, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cout, "  saving %s : %d files of points ...\n", 
               file, pro_.size());
  
  unsigned int total_pt = 0;
  for (unsigned int i=0; i<pro_.size(); i++) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pro_[i]->mesh()->vertexmap().begin();
    for (; it != pro_[i]->mesh()->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      vcl_fprintf (fp, "%.16f %.16f %.16f\n", V->pt().x(), V->pt().y(), V->pt().z());
      total_pt++;
    }
  }

  fclose (fp);
  vul_printf (vcl_cout, "  totally %d points saved.\n", total_pt);
  return true;
}

bool dbmsh3dr_pro_base::save_all_to_ply2 (const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cout, "  can't open PLY2 file %s to write.\n", file);
    return false; 
  }

  //Count # pts and # faces.
  unsigned int total_pts = 0;
  unsigned int total_faces = 0;
  int vidcounter = 0;
  for (unsigned int i=0; i<pro_.size(); i++) {
    dbmsh3d_mesh* M = pro_[i]->mesh();
    total_pts += M->vertexmap().size();
    total_faces += M->facemap().size();
    
    //Use v->vid() to re-index vertices, starting with id 0.
    vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
    for (; it != M->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      V->set_vid (vidcounter++);
    }
  }
  vul_printf (vcl_cout, "  saving %s : \n\t%d points, %d faces ...\n", 
              file, total_pts, total_faces);
  assert (vidcounter == total_pts);

  vcl_fprintf (fp, "%d\n", total_pts);
  vcl_fprintf (fp, "%d\n", total_faces);

  //Go through each mesh M and save all vertices.
  vidcounter=0;
  for (unsigned int i=0; i<pro_.size(); i++) {
    dbmsh3d_mesh* M = pro_[i]->mesh();    
    vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
    for (; it != M->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      vcl_fprintf (fp, "%.16f ", V->pt().x());
      vcl_fprintf (fp, "%.16f ", V->pt().y());
      vcl_fprintf (fp, "%.16f ", V->pt().z());
      vcl_fprintf (fp, "\n");
      assert (V->vid() == vidcounter);
      vidcounter++;
    }
  }

  //Go through each mesh M and save all faces.  
  int fcounter=0;
  for (unsigned int i=0; i<pro_.size(); i++) {
    dbmsh3d_mesh* M = pro_[i]->mesh();    
    vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
    for (; fit != M->facemap().end(); fit++) {
      dbmsh3d_face* F = (*fit).second;
      F->_ifs_track_ordered_vertices ();

      vcl_fprintf (fp, "%d ", F->vertices().size());
      for (unsigned j=0; j<F->vertices().size(); j++) {
        dbmsh3d_vertex* V = F->vertices(j);
        vcl_fprintf (fp, "%d ", V->vid());
      }
      vcl_fprintf (fp, "\n");
      fcounter++;
    }
  }
  assert (fcounter == total_faces);

  vcl_fclose (fp);
  vul_printf (vcl_cout, "  done.\n");
  return true;
}


//########################################################################

// For partial overlapping meshes, apply the dist_th for corresponding points.
// If the corresponding distance > dist_th, do not consider this pair (0 pulling force).
bool dbmsh3dr_pro_base::run_pp_icp_regstr (const int max_iter, const float conv_th, const float dist_th)
{
  vul_printf (vcl_cout, "run_pp_icp_regstr(): max_iter %d, conv_th %f, dist_th %f.\n", 
              max_iter, conv_th, dist_th);
  bool b_conv;

  if (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_PTS) {
    assert (pro_[1]->pro_data() == dbmsh3d_pro_base::PD_PTS);
    if (pro_[0]->pts().size()==0 || pro_[1]->pts().size()==0)
      return false; //failed.

    b_conv = dbmsh3dr_pp_icp_regstr (pro_[0]->pts(), pro_[1]->pts(), max_iter, conv_th, dist_th, hmatrix_01_);
  }
  else if (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_MESH) {
    assert (pro_[1]->pro_data() == dbmsh3d_pro_base::PD_MESH);    
    if (pro_[0]->mesh()->vertexmap().size()==0 || pro_[1]->mesh()->vertexmap().size()==0)
      return false; //failed.

    vcl_vector<vgl_point_3d<double> > pts1;
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pro_[0]->mesh()->vertexmap().begin();
    for (; it != pro_[0]->mesh()->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      pts1.push_back (V->pt());
    }

    vcl_vector<vgl_point_3d<double> > pts2;
    it = pro_[1]->mesh()->vertexmap().begin();
    for (; it != pro_[1]->mesh()->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;
      pts2.push_back (V->pt());
    }

    b_conv = dbmsh3dr_pp_icp_regstr (pts1, pts2, max_iter, conv_th, dist_th, hmatrix_01_);

    //Should transform pro_[1]->mesh() using hmatrix_01_.
    //But for now just copy value from pts2 to pro_[1]->mesh!!
    it = pro_[1]->mesh()->vertexmap().begin();
    for (unsigned int i=0; it != pro_[1]->mesh()->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* V = (*it).second;
      V->get_pt().set (pts2[i].x(), pts2[i].y(), pts2[i].z());
    }    
  }
  else
    assert (0);
  return true; //successful.
}

bool dbmsh3dr_pro_base::run_pf_icp_regstr (const int max_iter, const float conv_th, const float dist_th)
{
  vul_printf (vcl_cout, "run_pf_icp_regstr(): max_iter %d, conv_th %f, dist_th %f.\n", 
              max_iter, conv_th, dist_th);

  assert (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_MESH); 
  assert (pro_[1]->pro_data() == dbmsh3d_pro_base::PD_MESH); 
  if (pro_[0]->mesh()->vertexmap().size()==0 || pro_[1]->mesh()->vertexmap().size()==0)
    return false; //failed.

  bool b_conv = dbmsh3dr_pf_icp_regstr (pro_[0]->mesh(), pro_[1]->mesh(), max_iter, conv_th, dist_th, hmatrix_01_);
  return true; //successful.
}

//: use hmatrix_01_ for initial xform guess.
//  result in hmatrix_01_.
bool dbmsh3dr_pro_base::run_rgrl_icp_oripts (const int subsam1, const int subsam2)
{
  unsigned int i;
  vnl_vector_fixed<double,3> P, N;
  vcl_vector<rgrl_feature_sptr> fixedFV, movingFV; 

  vul_printf (vcl_cout, "run_rgrl_icp_oripts(): subsam1 = %d, subsam2 = %d.\n",
              subsam1, subsam2);

  //Generate the first feature vector from the oriented points.
  //Pick one for every subsam1 points.
  for (i=0; i<pro_[0]->oripts().size(); i+=subsam1) {
    P[0] = pro_[0]->oripts(i).first.x();
    P[1] = pro_[0]->oripts(i).first.y();
    P[2] = pro_[0]->oripts(i).first.z();
    N[0] = pro_[0]->oripts(i).second.x();
    N[1] = pro_[0]->oripts(i).second.y();
    N[2] = pro_[0]->oripts(i).second.z();
    fixedFV.push_back (new rgrl_feature_face_pt (P, N));
  }

  //Generate the 2nd feature vector from the oriented points.
  //Pick one for every subsam2 points.
  for (i=0; i<pro_[1]->oripts().size(); i+=subsam2) {
    P[0] = pro_[1]->oripts(i).first.x();
    P[1] = pro_[1]->oripts(i).first.y();
    P[2] = pro_[1]->oripts(i).first.z();
    N[0] = pro_[1]->oripts(i).second.x();
    N[1] = pro_[1]->oripts(i).second.y();
    N[2] = pro_[1]->oripts(i).second.z();
    movingFV.push_back (new rgrl_feature_face_pt (P, N));
  }  

  vnl_matrix<double> init_R (3, 3);
  ///vnl_vector_fixed<double,3> init_t (0, 0, 0);
  double x, y, z;
  hmatrix_01_.get_translation().get_nonhomogeneous (x, y, z);
  vnl_vector_fixed<double,3> init_t (x, y, z);  
  for (unsigned int r=0; r<3; r++)
    for (unsigned int c=0; c<3; c++)
      init_R (r,c) = hmatrix_01_.get_matrix().get(r,c);

  bool success = rgrl_rigid_icp_register (fixedFV, movingFV, init_R, init_t, r_xform_, error_);

  // Output Results
  if (success) {
    vcl_cout<<"Final xform:"<<vcl_endl;

    vcl_cout<<"R =\n"<< r_xform_.R() <<"t = "<< r_xform_.t() <<vcl_endl
            <<"Final alignment error = "<< error_ <<vcl_endl;
    
    //write r_xform_ to hmatrix_01_
    vnl_matrix<double> R = r_xform_.R();
    vnl_vector<double> t = r_xform_.t();
    vnl_matrix_fixed<double,4,4> H;
    unsigned int r, c;
    for (r=0; r<3; r++) 
      for (c=0; c<3; c++) 
        H(r,c) = R(r,c);
    for (r=0; r<3; r++) 
      H(r,3) = t(r);
    for (c=0; c<3; c++) 
      H(3,c) = 0;
    H(3,3) = 1;
    hmatrix_01_.set (H);
  }

  // BeginLatex
  //
  // Register two scans of the stanford bunny 
  // bun000.xyzn1 and bun045.xyzn1
  // R =  8.27805018e-001 -8.58008557e-003  5.60950296e-001
  //      2.41576622e-003  9.99928288e-001  1.17295363e-002
  //     -5.61010710e-001 -8.35464426e-003  8.27766382e-001
  // t = -5.21896741e-002 -3.52993872e-004 -1.09383652e-002
  //
  // EndLatex

  fixedFV.clear();
  movingFV.clear();
  return true;
}

//########################################################################

//: Compute point-point min_dist from pv2 to meshpv.
//  Result stored in the min_dists_[] vector.
bool dbmsh3dr_pro_base::compute_pp_error (const float dist_th)
{
  if (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_PTS) {
    assert (pro_[1]->pro_data() == dbmsh3d_pro_base::PD_PTS);  
    compute_pp_min_dist (pro_[0]->pts(), pro_[1]->pts(), min_dists_, min_ids_);
  }
  else if (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_IDPTS) {
    assert (pro_[1]->pro_data() == dbmsh3d_pro::PD_IDPTS);  
    compute_pp_min_dist (pro_[0]->idpts(), pro_[1]->idpts(), min_dists_, min_ids_);
  }
  else if (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_MESH) {
    assert (pro_[1]->pro_data() == dbmsh3d_pro::PD_MESH); 
    compute_pp_min_dist (pro_[0]->mesh(), pro_[1]->mesh(), min_dists_, min_ids_);
  }
  else
    assert (0);

  //Compute error estimation.
  return get_error_estim (min_dists_, dist_th, dist_min_, dist_mean_, dist_median_, dist_RMS_, dist_max_);
}

void dbmsh3dr_pro_base::normalize_pp_error (const float dist_th)
{
  //threshold and normalize the min_dist
  assert (dist_th > 0);
  normalize_min_dist (dist_th, min_dists_);
}

//: Compute point-face min_dist from pv2 to meshpv.
//  Result stored in the min_dists_[] vector.
bool dbmsh3dr_pro_base::compute_pf_error (const int top_n, const float dist_th)
{
  assert (pro_[0]->pro_data() == dbmsh3d_pro::PD_MESH);  
  assert (pro_[1]->pro_data() == dbmsh3d_pro::PD_MESH); 

  if (pro_[0]->mesh()->is_MHE() == false)
    pro_[0]->mesh()->IFS_to_MHE();

  compute_pf_min_dist (pro_[0]->mesh(), pro_[1]->mesh(), top_n, min_dists_, closest_pts_);

  //Compute error estimation.
  return get_error_estim (min_dists_, dist_th, dist_min_, dist_mean_, dist_median_, dist_RMS_, dist_max_);
}

void dbmsh3dr_pro_base::normalize_pf_error (const float dist_th)
{
  //threshold and normalize the min_dist
  assert (dist_th > 0);
  normalize_min_dist (dist_th, min_dists_);
}

//############################################################################


int dbmsh3dr_pro_base::get_overlapped_pts (const float dist_th)
{
  assert (pro_[0]->pro_data() == dbmsh3d_pro_base::PD_PTS);
  assert (pro_[1]->pro_data() == dbmsh3d_pro_base::PD_PTS);

  if (compute_pp_error (dist_th) == false) {
    //no point overlaps.
    vul_printf (vcl_cout, "get_overlapped_pts(): (M1 %u M2 %u), overlapping 0 points.\n", 
                pro_[0]->pts().size(), pro_[1]->pts().size());
    return 0;
  }
  
  //Move points from pts[] to mesh to set flag of each point.
  pro_[0]->move_pts_to_mesh ();
  pro_[1]->move_pts_to_mesh ();

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[0]->mesh()->vertexmap().begin();
  for (; vit != pro_[0]->mesh()->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_valid (false);
  }
  vit = pro_[1]->mesh()->vertexmap().begin();
  for (; vit != pro_[1]->mesh()->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_valid (false);
  }

  //For each vertex V[i] of M2, the closest distance is in min_dists_[i] to vertex min_ids[i] of M.
  //Mark the closest point pair (both points)'s i_value_ to 1, i.e., set_valid(true).
  unsigned int count = 0;
  vit = pro_[1]->mesh()->vertexmap().begin();
  for (unsigned int i=0; vit != pro_[1]->mesh()->vertexmap().end(); vit++, i++) {
    dbmsh3d_vertex* V = (*vit).second;
    assert (V->id() == i);
    if (min_dists_[i] < dist_th) {
      V->set_valid (true);
      count++;

      int v1id = min_ids_[i];
      dbmsh3d_vertex* V1 = pro_[0]->mesh()->vertexmap(v1id);
      V1->set_valid (true);
    }
  }

  vul_printf (vcl_cout, "get_overlapped_pts(): (M1 %u M2 %u), overlapping %d points.\n", 
              pro_[0]->mesh()->vertexmap().size(), pro_[1]->mesh()->vertexmap().size(), count);
  return count;
}


typedef enum { 
  DATAFILE_BOGUS    = '?',
  DATAFILE_3PI      = '3',
  //DATAFILE_PLY      = 'p',
  DATAFILE_PLY2     = '2',
} DATAFILE_TYPE;

//: Compute the edge weight of the adjacency graph between scans.
bool dbmsh3dr_pro_base::compute_adj_graph_weight (const float dthr,
                                                  vnl_matrix<double>& adj_graph)
{
  vul_printf (vcl_cout, "compute_adj_graph_weight(): dthr = %f, adj_graph matrix size: %dx%d.\n", 
              dthr, data_files_.size(), data_files_.size());

  adj_graph.set_size (data_files_.size(), data_files_.size());

  //Loop thrgouh all data_files_ and compute overlaps
  DATAFILE_TYPE dfi, dfj;
  for (unsigned int i=0; i<data_files_.size(); i++) {
    pro_[0]->pts().clear();
    pro_[0]->reset_mesh();
    //Read in the data_files_[i] .3pi file
    pro_[0]->set_dir_prefix (data_files_[i]);    
    vcl_string suffix = dbul_get_suffix (data_files_[i]);

    if (vcl_strcmp (suffix.c_str(), ".3pi")==0 && pro_[0]->load_3pi (data_files_[i])) {
      dfi = DATAFILE_3PI;      
    }
    else if (vcl_strcmp (suffix.c_str(), ".ply2")==0 && pro_[0]->load_ply2 (data_files_[i])) {
      pro_[0]->set_pro_data (dbmsh3d_pro_base::PD_MESH); 
      dfi = DATAFILE_PLY2;      
    }
    else
      dfi = DATAFILE_BOGUS;

    if (dfi == DATAFILE_BOGUS) {
      //Put weight value to be -1.
      for (unsigned int j=i+1; j< data_files_.size(); j++) { 
        vul_printf (vcl_cout, "\n\n  Can't read data_files_[i %d] %s, adj_graph[%d][%d] = -1.\n\n",
                    i, data_files_[i].c_str(), i, j);
        adj_graph.put (i, j, -1);
      }
      continue;
    } 

    double avg_samp_dist = pro_[0]->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dthr);

    if (dfi == DATAFILE_3PI)
      pro_[0]->move_sg3pi_to_pts();
    else if (dfi == DATAFILE_PLY2)
      pro_[0]->move_mesh_to_pts();

    //Read in the initial alignment file 'f1.txt'
    pro_[0]->load_hmatrix (align_files_[i]);
    pro_[0]->apply_xform_hmatrix (); //Apply hmatrix xform to f1.

    for (unsigned int j=i+1; j< data_files_.size(); j++) {      
      pro_[1]->pts().clear();
      pro_[1]->reset_mesh();
      //Read in the data_files_[j] .3pi file
      pro_[1]->set_dir_prefix (data_files_[j]);
      suffix = dbul_get_suffix (data_files_[j]);

      if (vcl_strcmp (suffix.c_str(), ".3pi")==0 && pro_[1]->load_3pi (data_files_[j])) {    
        dfj = DATAFILE_3PI;
      }
      else if (vcl_strcmp (suffix.c_str(), ".ply2")==0 && pro_[1]->load_ply2 (data_files_[j])) {
        pro_[1]->set_pro_data (dbmsh3d_pro_base::PD_MESH); 
        dfj = DATAFILE_PLY2;
      }
      else
        dfj = DATAFILE_BOGUS;

      if (dfj == DATAFILE_BOGUS) {
        //Put weight value to be -1.
        vul_printf (vcl_cout, "\n\n  Can't read data_files_[j %d] %s, adj_graph[%d][%d] = -1.\n\n",
                    j, data_files_[j].c_str(), i, j);
        adj_graph.put (i, j, -1);
        continue;
      }    
    
      if (dfj == DATAFILE_3PI)
        pro_[1]->move_sg3pi_to_pts();
      else if (dfj == DATAFILE_PLY2)
        pro_[1]->move_mesh_to_pts();

      //Read in the initial alignment file 'f2.txt'
      pro_[1]->load_hmatrix (align_files_[j]);
      pro_[1]->apply_xform_hmatrix (); //Apply hmatrix xform to f2.

      int n_overlap = get_overlapped_pts (error_dist_th);

      //add weight to adj_graph
      adj_graph.put (i, j, n_overlap);
      adj_graph.put (j, i, n_overlap);
      
      vul_printf (vcl_cout, "\n\n  data_files_[i %d] %s, data_files_[j %d] %s,\n",
                  i, data_files_[i].c_str(), j, data_files_[j].c_str());
      vul_printf (vcl_cout, "\n  adj_graph[i %d][j %d] = %d.\n\n",
                  i, j, n_overlap);
    }
  }

  //diagonal elements has weidht 0
  for (unsigned int i=0; i< data_files_.size(); i++)
    adj_graph.put (i, i, 0);

  vul_printf (vcl_cout, "\n\n Final adj_graph matrix:\n\n");

  //Print adjacency matrix.
  adj_graph.print (vcl_cout);

  vul_printf (vcl_cout, "\n\n");

  return true;
}


//############################################################################

//: Fuse scans using pt-mesh ICP following MST.
//  Save results to alignment file scan_##_mst.txt
//
bool dbmsh3dr_pro_base::fuse_scan_MST (const vcl_vector<vcl_pair<int, int> >& MST, 
                                       const int root_sid, const float dthr,
                                       const int max_ICP_iter, const float icpcv)
{
  vul_printf (vcl_cout, "\n\n fuse_scan_MST(): MST size %d, root_sid %d.\n",
              MST.size(), root_sid);
  vul_printf (vcl_cout, "  dthr %f, max_ICP_iter %d, icpcv %f.\n\n",
              dthr, max_ICP_iter, icpcv);

  vcl_vector<bool> fusion_idx;
  const unsigned sz = data_files_.size();
  fusion_idx.resize (sz);
  assert (data_files_.size() == align_files_.size());
  assert (data_files_.size() == MST.size()+1);
    
  //The root scan is initialized as done.
  fusion_idx[root_sid] = true;

  //Save the root scan's scan_##_fixed.txt to scan_##_mst.txt
  pro_[0]->set_dir_prefix (dbul_get_str_prior (align_files_[root_sid], "_fixed"));
  bool result = pro_[0]->load_hmatrix (align_files_[root_sid]);
  assert (result);
  dbmsh3d_write_xform_file (pro_[0]->dir_prefix() + "_mst.txt", pro_[0]->hmatrix());

  int cur_sid, next_sid;
  //A loop to fuse the next_scan to cur_sid until finish.
  while (find_next_scan_to_fuse (MST, fusion_idx, cur_sid, next_sid)) {
    vul_printf (vcl_cout, "\n\n==================================================\n");
    vul_printf (vcl_cout, "  Fuse together cur_scan %d and next_scan %d:\n",
                cur_sid+1, next_sid+1);
    //Fix the position of cur_scan and move next_scan to fuse with cur_scan.

    //1) Set dir_prefix for pro_[0] and pro_[1].
    pro_[0]->set_dir_prefix (dbul_get_str_prior (align_files_[cur_sid], "_fixed"));
    pro_[1]->set_dir_prefix (dbul_get_str_prior (align_files_[next_sid], "_fixed"));

    //2) Read in original scan to estimate error_dist_th.
    bool result = pro_[0]->load_3pi (pro_[0]->dir_prefix() + "_crop.3pi");  
    assert (result);
    double avg_samp_dist = pro_[0]->get_avg_samp_dist();
    float error_dist_th = float (avg_samp_dist * dthr);    
    pro_[0]->reset_sg3pi();
    pro_[0]->set_pro_data (dbmsh3d_pro_base::PD_MESH);

    //3) Read in smoothed meshed surfaces.
    result = pro_[0]->load_ply (data_files_[cur_sid]);
    assert (result);
    result = pro_[1]->load_ply (data_files_[next_sid]);
    assert (result);

    //4) Read in init. alignment files and xform them.
    //   For cur_scan, use the updated alignment.
    result = pro_[0]->load_hmatrix (pro_[0]->dir_prefix() + "_mst.txt");
    assert (result);
    pro_[0]->apply_xform_hmatrix ();    
    result = pro_[1]->load_hmatrix (align_files_[next_sid]);
    assert (result);
    pro_[1]->apply_xform_hmatrix ();

    //Estimate the Pt-Mesh dist. d_pt-mesh (A*, B*) of init. alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B*) of init. alignment:\n\n");
    result = compute_pf_error (1, error_dist_th);
    assert (result);
    double d_pt_mesh_A_B_mean = dist_mean();

    //Register A and B using Pt-Mesh ICP.
    run_pf_icp_regstr (max_ICP_iter, icpcv, error_dist_th);

    //Estimate the Pt-Mesh dist. d_pt-mesh (A*, B^*) after Pt-Mesh ICP alignment.
    vul_printf (vcl_cout, "\nPt-Mesh dist. d_pt-mesh(A*, B^*) after Pt-Mesh ICP:\n\n");
    result = compute_pf_error (1, error_dist_th);
    assert (result);
    double d_pt_mesh_A_Bh_mean = dist_mean();

    //Result: save alignment file of next_scan to scan_##_mst.txt.
    vul_printf (vcl_cout, "\n  Writing resulting alignment file %s.\n", (pro_[1]->dir_prefix() + "_mst.txt").c_str());
    vgl_h_matrix_3d<double> newH2 = hmatrix_01_ * pro_[1]->hmatrix();
    dbmsh3d_write_xform_file (pro_[1]->dir_prefix() + "_mst.txt", newH2);

    //7) Output summary.
    vul_printf (vcl_cout, "\n Summary of fusing cur_scan %d and next_scan %d:\n",
                cur_sid+1, next_sid+1);
    vul_printf (vcl_cout, "  error_dist_th %f = dist_thr %.2f * avg_samp_dist %f.\n", 
                error_dist_th, dthr, avg_samp_dist);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B*)  mean: %f\n", d_pt_mesh_A_B_mean);
    vul_printf (vcl_cout, "  d_pt-mesh (A*, B^*) mean: %f\n", d_pt_mesh_A_Bh_mean);
    vul_printf (vcl_cout, "  Alignment error is reduced by %.3f%%.\n", 
                (d_pt_mesh_A_Bh_mean - d_pt_mesh_A_B_mean)/d_pt_mesh_A_B_mean * 100);

    //Set next_scan to be finished.
    fusion_idx[next_sid] = true;
  }

  //Output result:.

  return true;
}

//: Look for the next fusion edge in MST 
//  connecting already fused (true) and un-visited (false) scans
//  (stored in the fusion_idx[] array).
//  Return true if any qualified such edge is found and set 
//    cur_sid = already fused scan and next_sid = un-visited scan.
//  Return false if such edge is not found (Fusing is done).
//
bool dbmsh3dr_pro_base::find_next_scan_to_fuse (const vcl_vector<vcl_pair<int, int> >& MST, 
                                                const vcl_vector<bool>& fusion_idx,                             
                                                int& cur_sid, int& next_sid)
{
  //Loop through all edges in MST (in order)
  //and look for the next fusion edge.
  for (unsigned int i=0; i<MST.size(); i++) {
    int sid1 = MST[i].first;
    int sid2 = MST[i].second;
    if (fusion_idx[sid1]==true && fusion_idx[sid2]==false) {
      //next fusion edge found.
      cur_sid = sid1;
      next_sid = sid2;
      return true;
    }
    if (fusion_idx[sid2]==true && fusion_idx[sid1]==false) {
      //next fusion edge found.
      cur_sid = sid2;
      next_sid = sid1;
      return true;
    }
  }

  return false;
}

//: Estimate surface variance along surface normal.
double dbmsh3dr_pro_base::estimate_surf_var_along_normal (const float dthr, const int top_n,
                                                          const int option)
{
  vul_printf (vcl_cout, "\nestimate_surf_var_along_normal():\n");
  const int N_DATA = data_files_.size();

  double avg_sample_dist;
  if (setup_n_surfs (dthr, avg_sample_dist) == false) {
    vul_printf (vcl_cout, "\nsetup_n_surfs() returns error!\n");
    return -1;
  }

  //Estimate the error threshold dist_th from the first data.
  double dist_th = avg_sample_dist * dthr;
  vul_printf (vcl_cout, "  dist_th %f = dthr %f * avg_samp_dist %f.\n", 
              dist_th, dthr, avg_sample_dist);
    
  //Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
  if (option == 1)
    return compute_M_M_avg_dist_1v (dist_th);
  else ///if (option == 2)
    return compute_M_M_avg_dist (top_n, dist_th);  
}

//: Reduce surface thickness by n_iter iterations.
//  option 1: brute-force, option 4: estimate final avg. dist.
//  option 2: use bucketing, option 5: estimate final avg. dist.
//  option 3: only keep one vertex (hack), option 6: estimate final avg. dist.
//
bool dbmsh3dr_pro_base::reduce_surf_thickness (const float dthr, const int n_iter, const int top_n,
                                               const int option)
{
  vul_printf (vcl_cout, "\nreduce_surf_thickness():\n");
  const int N_DATA = data_files_.size();

  double avg_sample_dist;
  if (setup_n_surfs (dthr, avg_sample_dist) == false) {
    vul_printf (vcl_cout, "\nsetup_n_surfs() returns error!\n");
    return false;
  }

  //Estimate the error threshold dist_th from the first data.
  double dist_th = avg_sample_dist * dthr;
  vul_printf (vcl_cout, "  dist_th %f = dthr %f * avg_samp_dist %f.\n", 
              dist_th, dthr, avg_sample_dist);

  //Reduce surf. thickness.
  for (int i=0; i<n_iter; i++) {
    vul_printf (vcl_cout, "  ===================================\n");
    vul_printf (vcl_cout, "  move_surf_avg_closest_pt ITER %d : \n", i);
    if (option == 1 || option == 4)
      move_surf_avg_closest_pt (top_n, dist_th);
    else if (option == 2 || option == 5)
      move_surf_avg_closest_pt_bkt (top_n, dist_th);
    else if (option == 3 || option == 6)
      move_surf_avg_closest_pt_1v (dist_th);
    else
      assert (0);      
  }

  //Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
  if (option == 4 || option == 5 || option == 6) {
    vul_printf (vcl_cout, "\n  ==========> Average dist. of dthr 2");
    if (option == 4)      
      compute_M_M_avg_dist (top_n, avg_sample_dist* 2);
    else if (option == 5)      
      compute_M_M_avg_dist_bkt (top_n, avg_sample_dist* 2);
    else if (option == 6)
      compute_M_M_avg_dist_1v (avg_sample_dist* 2);
  }

  return true;
}

//: Reduce surface thickness using bucketing.
bool dbmsh3dr_pro_base::reduce_surf_thickness_bkt (const float dthr, const int n_iter, const int top_n,
                                                   const int npbkt)
{  
  //Read in all mesh vertices into pts to run bucketing.
  vcl_vector<vgl_point_3d<double> > pts;

  //The bucketing structure
  bool b_check_dup = false;
  dbmsh3d_pt_bktstr* BktStruct = adpt_bucketing_pts (pts, npbkt, b_check_dup);
  delete BktStruct;

  return true;
}

bool dbmsh3dr_pro_base::setup_n_surfs (const float dthr, double& avg_sample_dist)
{
  const int N_DATA = data_files_.size();
  assert (N_DATA > 1);
  vul_printf (vcl_cout, "\n  setup_n_surfs(): N_DATA: %d, dthr: %f\n", 
              N_DATA, dthr);

  //Read in all data files in data_files[] to pro_[] 
  //and apply translation & rotation with align_files[].
  for (int i=0; i<N_DATA; i++) {
    pro_[i]->set_dir_file (dbul_get_dir_file (data_files_[i]));
    if (pro_[i]->load_meshes_files (data_files_[i]) == false)
      return false;
    if (align_files_[i] != "") {
      if (pro_[i]->load_hmatrix (align_files_[i]) == false)
        return false;    
      pro_[i]->apply_xform_hmatrix ();
    }
  }
  
  //Estimate the avg_sample_dist from the first data.
  pro_[0]->mesh()->IFS_to_MHE();
  avg_sample_dist = pro_[0]->get_avg_samp_dist();
  pro_[0]->mesh()->MHE_to_IFS();

  vul_printf (vcl_cout, "\n    done.\n");

  return true;
}

//: Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
double dbmsh3dr_pro_base::compute_M_M_avg_dist (const int top_n, const double& dist_th)
{
  const int N_DATA = data_files_.size();
  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist(): %d meshes.\n", N_DATA);

  //For each pro_[] build a kd-tree to store all mesh vertices.
  vcl_vector<rsdl_kd_tree*> kdtrees (N_DATA);
  for (int i=0; i<N_DATA; i++) {
    kdtrees[i] = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 
  }
  vul_printf (vcl_cout, "    %d kd-trees built for each data mesh.\n", N_DATA);  
  
  for (int i=0; i<N_DATA; i++) { //Need MHE
    pro_[i]->mesh()->IFS_to_MHE();
  }

  //Loop through each pro_[].mesh and
  //loop through each point, compute the average dist to all other meshes (within th.).
  //Return the final average dist.
  double M_M_avg_dist = 0;
  unsigned int sz = 0;
  for (int i=0; i<N_DATA; i++) {
    vul_printf (vcl_cout, "    mesh %d: \n", i);
    //Compute avg_dist from data[i] to all other data[j].
    //Note that the dist is not symmetric (since we only use approx. pt-mesh dist.).
    //But ignoring half of computation is reasonable.
    for (int j=0; j<N_DATA; j++) {
      if (j==i)
        continue;

      vul_printf (vcl_cout, "m%d ", j);
      //Compute avg_dist of of M_i to M_j.
      //Only consider points within dist_th.
      double avg_dist = dbmsh3d_mesh_mesh_avg_dist (pro_[i]->mesh(), kdtrees[i],
                                                    pro_[j]->mesh(), top_n, dist_th);
      M_M_avg_dist += avg_dist;
      sz++;
    }
    vul_printf (vcl_cout, "\n");
  }

  assert (sz > 0);
  M_M_avg_dist /= sz;

  //Clean up memory and return.
  for (int i=0; i<N_DATA; i++)
    delete kdtrees[i];
  vul_printf (vcl_cout, "    %d kd-trees cleared.\n", N_DATA);

  for (int i=0; i<N_DATA; i++) { //Need MHE
    pro_[i]->mesh()->MHE_to_IFS();
  }

  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist(): M_M_avg_dist: %f.\n\n", M_M_avg_dist);
  return M_M_avg_dist;
}

//: Loop through each pro_[].mesh and compute mesh-mesh avg-dist (within th.).
//  Only consider top 1 closest vertices on mesh.
//
double dbmsh3dr_pro_base::compute_M_M_avg_dist_1v (const double& dist_th)
{
  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist_1v():\n");
  const int N_DATA = data_files_.size();

  vcl_vector<vcl_vector<dbmsh3d_vertex*> >* M_V_CV = new vcl_vector<vcl_vector<dbmsh3d_vertex*> >[N_DATA];

  //For each vertex of each mesh, build a CV[N_DATA] to store the closest vertex on other meshes.
  //  M_V_CV[M][V][N_DATA]. Non-used data set to NULL.
  ///M_V_CV.resize (N_DATA);
  setup_M_V_CV (M_V_CV);
  
  double M_M_avg_dist = 0;
  unsigned int count = 0;

  vul_printf (vcl_cout, "\n  Compute the average vector for each V of each M:\n");

  //Loop through all meshes: pro_[i]->mesh()
  for (int i=0; i<N_DATA; i++) {
    vul_printf (vcl_cout, "    mesh %d: \n", i);
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (unsigned int v=0; vit != pro_[i]->mesh()->vertexmap().end(); vit++, v++) {
      dbmsh3d_vertex* V = (*vit).second;
      assert (v < pro_[i]->mesh()->vertexmap().size());

      //Compute the avg_vector to each closest pt on other meshes.
      vgl_vector_3d<double> avg_vec (0, 0, 0);
      int sz = 0;
      for (int j=0; j<N_DATA; j++) {
        if (j==i)
          continue;

        //Compute the closest pt from V to mesh j.
        vgl_point_3d<double> G;
        dbmsh3d_vertex* closestV = M_V_CV[i][v][j];
        assert (closestV != NULL);
        double dist = dbmsh3d_pt_mesh_dist (V->pt(), closestV, G);

        if (dist > dist_th) {
          //Skip if the closest dist > dist_th.
        }
        else {
          //Add the clost pt vector to avg_vector.
          vgl_vector_3d<double> closest_pt_vec = G - V->pt();
          avg_vec += closest_pt_vec;
          sz++;
        }
      }

      if (sz != 0) { //If sz != 0, add to final M_M_avg_dist.
        avg_vec /= sz;
        M_M_avg_dist += avg_vec.length();
        count++;
      }
    }
  }

  ///M_V_CV.clear();
  for (int i=0; i<N_DATA; i++)
    M_V_CV[i].clear();
  ///delete M_V_CV;

  assert (count > 0);
  M_M_avg_dist /= count;

  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist_1v() done: M_M_avg_dist: %f.\n\n", M_M_avg_dist);
  return M_M_avg_dist;
}

//: Reduce surfaces thickness by moving toward avg. closest point on other meshes.
void dbmsh3dr_pro_base::move_surf_avg_closest_pt (const int top_n, const double& dist_th)
{
  vul_printf (vcl_cout, "\n  move_surf_avg_closest_pt():\n");
  const int N_DATA = data_files_.size();
  
  //For each pro_[] build a kd-tree to store all mesh vertices.
  vcl_vector<rsdl_kd_tree*> kdtrees (N_DATA);
  for (int i=0; i<N_DATA; i++) {
    kdtrees[i] = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 
  }
  vul_printf (vcl_cout, "    %d kd-trees built for each data mesh.\n", N_DATA);

  vcl_vector<vcl_vector<vgl_point_3d<float> > > newpts;
  newpts.resize (N_DATA);

  //Loop through each vertex of each pro_[].mesh, 
  //compute the average dist to all other meshes (within th.).
  //Move the current vertex by the avg_vector.

  //Loop through all meshes: pro_[i]->mesh()
  for (int i=0; i<N_DATA; i++) {
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (; vit != pro_[i]->mesh()->vertexmap().end(); vit++) {
      dbmsh3d_vertex* V = (*vit).second;

      //Compute the avg_vector to each closest pt on other meshes.
      vgl_vector_3d<double> avg_vec (0, 0, 0);
      int sz = 0;
      for (int j=0; j<N_DATA; j++) {
        if (j==i)
          continue;

        //Compute the closest pt from V to mesh j.
        vgl_point_3d<double> G;
        double dist = dbmsh3d_pt_mesh_dist (V->pt(), pro_[j]->mesh(), kdtrees[j], top_n, G);

        if (dist > dist_th) {
          //Skip if the closest dist > dist_th.
        }
        else {
          //Add the clost pt vector to avg_vector.
          vgl_vector_3d<double> closest_pt_vec = G - V->pt();
          avg_vec += closest_pt_vec;
          sz++;
        }
      }

      vgl_point_3d<double> new_pt = V->pt();
      if (sz != 0) {
        //If sz != 0, move vertex V by avg_vector.
        avg_vec /= sz;
        new_pt += avg_vec;
      }

      vgl_point_3d<float> P;
      P.set ((float) new_pt.x(), (float) new_pt.y(), (float) new_pt.z());
      newpts[i].push_back (P);
    }
  }

  //Clean up memory and return.
  for (int i=0; i<N_DATA; i++)
    delete kdtrees[i];
  vul_printf (vcl_cout, "    %d kd-trees cleared.\n", N_DATA);

  //Update each vertex's pts.  
  for (int i=0; i<N_DATA; i++) {
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (int j=0; vit != pro_[i]->mesh()->vertexmap().end(); vit++, j++) {
      dbmsh3d_vertex* V = (*vit).second;
      vgl_point_3d<double> P;
      P.set (newpts[i][j].x(), newpts[i][j].y(), newpts[i][j].z());
      V->set_pt (P);
    }
  }

}

//: Reduce surfaces thickness by moving toward avg. closest point on other meshes.
//  Only consider top 1 closest vertices on mesh.
//  This allows removing the kd-tree & keep only the closest pt for each mesh (of each vertex).
//
void dbmsh3dr_pro_base::move_surf_avg_closest_pt_1v (const double& dist_th)
{
  vul_printf (vcl_cout, "\n  move_surf_avg_closest_pt_1v():\n");
  const int N_DATA = data_files_.size();

  ///vcl_vector<vcl_vector<vcl_vector<dbmsh3d_vertex*> > > M_V_CV;
  vcl_vector<vcl_vector<dbmsh3d_vertex*> > * M_V_CV = new vcl_vector<vcl_vector<dbmsh3d_vertex*> >[N_DATA];
  setup_M_V_CV (M_V_CV);
  
  //Loop through each vertex of each pro_[].mesh, 
  //compute the average dist to all other meshes (within th.).
  //Move the current vertex by the avg_vector.
  /*vul_printf (vcl_cout, "\n  allocate space for newpts[%d].\n", N_DATA);
  vcl_vector<vcl_vector<vgl_point_3d<double> > > newpts;
  newpts.resize (N_DATA);
  for (int i=0; i<N_DATA; i++) {
    newpts[i].resize (pro_[i]->mesh()->vertexmap().size());
  }*/
  //Save newpt[i][v] into a file!
  
  const char* out_tmp_file = "out_tmp.xyz";
  FILE* fp;
  if ((fp = vcl_fopen (out_tmp_file, "w")) == NULL) {
    vul_printf (vcl_cout, "  can't open output %s file %s\n", out_tmp_file);
    return;
  }
  unsigned int out_tmp_count = 0;

  vul_printf (vcl_cout, "\n  Move to the average vector for each V of each M:\n");

  //Loop through all meshes: pro_[i]->mesh()
  for (int i=0; i<N_DATA; i++) {
    vul_printf (vcl_cout, "    mesh %d: \n", i);
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (unsigned int v=0; vit != pro_[i]->mesh()->vertexmap().end(); vit++, v++) {
      dbmsh3d_vertex* V = (*vit).second;
      assert (v < pro_[i]->mesh()->vertexmap().size());

      //Compute the avg_vector to each closest pt on other meshes.
      vgl_vector_3d<double> avg_vec (0, 0, 0);
      int sz = 0;
      for (int j=0; j<N_DATA; j++) {
        if (j==i)
          continue;

        //Compute the closest pt from V (of mesh[i]) to mesh[j].
        //Here we have no IFS for mesh[j], can only compute mesh V-V distance!!
        //unless use a slow brute force finding V-F incidence!!
        vgl_point_3d<double> G;
        dbmsh3d_vertex* closestV = M_V_CV[i][v][j];
        assert (closestV != NULL);
        double dist = dbmsh3d_pt_mesh_dist (V->pt(), closestV, G);

        if (dist > dist_th) {
          //Skip if the closest dist > dist_th.
        }
        else {
          //Add the clost pt vector to avg_vector.
          vgl_vector_3d<double> closest_pt_vec = G - V->pt();
          avg_vec += closest_pt_vec;
          sz++;
        }
      }

      vgl_point_3d<double> new_pt = V->pt();
      if (sz != 0) {
        //If sz != 0, move vertex V by avg_vector.
        avg_vec /= sz;
        new_pt += avg_vec;
      }

      ///newpts[i][v] = new_pt;
      vcl_fprintf (fp, "%.16f %.16f %.16f\n", new_pt.x(), new_pt.y(), new_pt.z());
      out_tmp_count++;
    }
  }
  vcl_fclose (fp);

  ///M_V_CV.clear();
  for (int i=0; i<N_DATA; i++)
    M_V_CV[i].clear();
  ///delete M_V_CV;

  if ((fp = vcl_fopen (out_tmp_file, "r")) == NULL) {
    vul_printf (vcl_cout, "  can't open input %s file %s\n", out_tmp_file);
    return; 
  }

  //Update each vertex's pts.  
  unsigned int count = 0;
  for (int i=0; i<N_DATA; i++) {
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (int j=0; vit != pro_[i]->mesh()->vertexmap().end(); vit++, j++) {
      dbmsh3d_vertex* V = (*vit).second;
      ///V->set_pt (newpts[i][j]);
      double x, y, z;
      int ret = vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
      V->set_pt (x, y, z);
      assert (ret != EOF);
      count++;
    }
  }

  //newpts.clear();
  assert (out_tmp_count == count);
}

void dbmsh3dr_pro_base::setup_M_V_CV (vcl_vector<vcl_vector<dbmsh3d_vertex*> > * M_V_CV)
{
  vul_printf (vcl_cout, "\n    setup_M_V_CV():\n");
  const int N_DATA = data_files_.size();

  //Loop through each pro_[i] mesh, build a kd-tree to query the closest V on this mesh.
  for (int i=0; i<N_DATA; i++) {
    rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 

    //Now we have the kdtree for i-th mesh,
    //Loop through all other meshes and query their closest V on mesh[i].
    for (int j=0; j<N_DATA; j++) {
      //Allocate space for M_V_CV[j][N_DATA]
      M_V_CV[j].resize (pro_[j]->mesh()->vertexmap().size());

      //Loop through all vertices of mesh[j]
      vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[j]->mesh()->vertexmap().begin();
      for (unsigned int v=0; vit != pro_[j]->mesh()->vertexmap().end(); vit++, v++) {
        dbmsh3d_vertex* V = (*vit).second;
        assert (v < pro_[j]->mesh()->vertexmap().size());
        M_V_CV[j][v].resize (N_DATA);

        if (i==j) //If i==j, closestV = NULL.
          M_V_CV[j][v][i] = NULL;
        else {
          //Find the closest vertex from V to mesh[i].
          vcl_vector<rsdl_point> near_neighbor_pts;
          vcl_vector<int> near_neighbor_indices;
          rsdl_point query_pt (3, 0);
          vnl_vector_fixed<double,3> P3 (V->pt().x(), V->pt().y(), V->pt().z());
          query_pt.set_cartesian (P3);
          kdtree->n_nearest (query_pt, 1, near_neighbor_pts, near_neighbor_indices);
          assert (near_neighbor_indices.size() == 1);
          int id = near_neighbor_indices[0];
          dbmsh3d_vertex* closestV = pro_[i]->mesh()->vertexmap (id);
          assert (closestV->id() == id);

          //Save the closestV from M_V_CV[j][v] to mesh[i].
          M_V_CV[j][v][i] = closestV;
        }
      }
    }

    vul_printf (vcl_cout, "    kd-tree cleared.\n");
    delete kdtree;
  }
  vul_printf (vcl_cout, "\n    setup_M_V_CV() done.\n");
}

//: Reduce surfaces thickness by moving toward avg. closest point using bucketing.
void dbmsh3dr_pro_base::move_surf_avg_closest_pt_bkt (const int top_n, const double& dist_th)
{  
  vul_printf (vcl_cout, "\n  move_surf_avg_closest_pt_bkt():\n");
  const int N_DATA = data_files_.size();
  
  //For each pro_[] build a kd-tree to store all mesh vertices.
  vcl_vector<rsdl_kd_tree*> kdtrees (N_DATA);
  for (int i=0; i<N_DATA; i++) {
    kdtrees[i] = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 
  }
  vul_printf (vcl_cout, "    %d kd-trees built for each data mesh.\n", N_DATA);

  vcl_vector<vcl_vector<vgl_point_3d<float> > > newpts;
  newpts.resize (N_DATA);

  //Loop through each vertex of each pro_[].mesh, 
  //compute the average dist to all other meshes (within th.).
  //Move the current vertex by the avg_vector.

  //Loop through all meshes: pro_[i]->mesh()
  for (int i=0; i<N_DATA; i++) {
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (; vit != pro_[i]->mesh()->vertexmap().end(); vit++) {
      dbmsh3d_vertex* V = (*vit).second;

      //Compute the avg_vector to each closest pt on other meshes.
      vgl_vector_3d<double> avg_vec (0, 0, 0);
      int sz = 0;
      for (int j=0; j<N_DATA; j++) {
        if (j==i)
          continue;

        //Compute the closest pt from V to mesh j.
        vgl_point_3d<double> G;
        double dist = dbmsh3d_pt_mesh_dist (V->pt(), pro_[j]->mesh(), kdtrees[j], top_n, G);

        if (dist > dist_th) {
          //Skip if the closest dist > dist_th.
        }
        else {
          //Add the clost pt vector to avg_vector.
          vgl_vector_3d<double> closest_pt_vec = G - V->pt();
          avg_vec += closest_pt_vec;
          sz++;
        }
      }

      vgl_point_3d<double> new_pt = V->pt();
      if (sz != 0) {
        //If sz != 0, move vertex V by avg_vector.
        avg_vec /= sz;
        new_pt += avg_vec;
      }

      vgl_point_3d<float> P;
      P.set ((float) new_pt.x(), (float) new_pt.y(), (float) new_pt.z());
      newpts[i].push_back (P);
    }
  }

  //Clean up memory and return.
  for (int i=0; i<N_DATA; i++)
    delete kdtrees[i];
  vul_printf (vcl_cout, "    %d kd-trees cleared.\n", N_DATA);

  //Update each vertex's pts.  
  for (int i=0; i<N_DATA; i++) {
    //Loop through all vertices of pro_[i]->mesh()
    vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[i]->mesh()->vertexmap().begin();
    for (int j=0; vit != pro_[i]->mesh()->vertexmap().end(); vit++, j++) {
      dbmsh3d_vertex* V = (*vit).second;
      vgl_point_3d<double> P;
      P.set (newpts[i][j].x(), newpts[i][j].y(), newpts[i][j].z());
      V->set_pt (P);
    }
  }
}

void dbmsh3dr_pro_base::setup_M_V_CV_bkt (vcl_vector<vcl_vector<dbmsh3d_vertex*> > * M_V_CV)
{
  vul_printf (vcl_cout, "\n    setup_M_V_CV_bkt():\n");
  const int N_DATA = data_files_.size();

  //Loop through each pro_[i] mesh, build a kd-tree to query the closest V on this mesh.
  for (int i=0; i<N_DATA; i++) {
    rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 

    //Now we have the kdtree for i-th mesh,
    //Loop through all other meshes and query their closest V on mesh[i].
    for (int j=0; j<N_DATA; j++) {
      //Allocate space for M_V_CV[j][N_DATA]
      M_V_CV[j].resize (pro_[j]->mesh()->vertexmap().size());

      //Loop through all vertices of mesh[j]
      vcl_map<int, dbmsh3d_vertex*>::iterator vit = pro_[j]->mesh()->vertexmap().begin();
      for (unsigned int v=0; vit != pro_[j]->mesh()->vertexmap().end(); vit++, v++) {
        dbmsh3d_vertex* V = (*vit).second;
        assert (v < pro_[j]->mesh()->vertexmap().size());
        M_V_CV[j][v].resize (N_DATA);

        if (i==j) //If i==j, closestV = NULL.
          M_V_CV[j][v][i] = NULL;
        else {
          //Find the closest vertex from V to mesh[i].
          vcl_vector<rsdl_point> near_neighbor_pts;
          vcl_vector<int> near_neighbor_indices;
          rsdl_point query_pt (3, 0);
          vnl_vector_fixed<double,3> P3 (V->pt().x(), V->pt().y(), V->pt().z());
          query_pt.set_cartesian (P3);
          kdtree->n_nearest (query_pt, 1, near_neighbor_pts, near_neighbor_indices);
          assert (near_neighbor_indices.size() == 1);
          int id = near_neighbor_indices[0];
          dbmsh3d_vertex* closestV = pro_[i]->mesh()->vertexmap (id);
          assert (closestV->id() == id);

          //Save the closestV from M_V_CV[j][v] to mesh[i].
          M_V_CV[j][v][i] = closestV;
        }
      }
    }

    vul_printf (vcl_cout, "    kd-tree cleared.\n");
    delete kdtree;
  }
  vul_printf (vcl_cout, "\n    setup_M_V_CV_bkt() done.\n");
}

//: Loop through each pro_[].mesh and compute mesh-mesh avg-dist using bucketing.
double dbmsh3dr_pro_base::compute_M_M_avg_dist_bkt (const int top_n, const double& dist_th)
{
  const int N_DATA = data_files_.size();
  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist_bkt(): %d meshes.\n", N_DATA);

  //For each pro_[] build a kd-tree to store all mesh vertices.
  vcl_vector<rsdl_kd_tree*> kdtrees (N_DATA);
  for (int i=0; i<N_DATA; i++) {
    kdtrees[i] = dbmsh3d_build_kdtree_vertices (pro_[i]->mesh()); 
  }
  vul_printf (vcl_cout, "    %d kd-trees built for each data mesh.\n", N_DATA);  
  
  for (int i=0; i<N_DATA; i++) { //!! Need MHE !!
    pro_[i]->mesh()->IFS_to_MHE();
  }

  //Loop through each pro_[].mesh and
  //loop through each point, compute the average dist to all other meshes (within th.).
  //Return the final average dist.
  double M_M_avg_dist = 0;
  unsigned int sz = 0;
  for (int i=0; i<N_DATA; i++) {
    vul_printf (vcl_cout, "    mesh %d: \n", i);
    //Compute avg_dist from data[i] to all other data[j].
    //Note that the dist is not symmetric (since we only use approx. pt-mesh dist.).
    //But ignoring half of computation is reasonable.
    for (int j=0; j<N_DATA; j++) {
      if (j==i)
        continue;

      vul_printf (vcl_cout, "m%d ", j);
      //Compute avg_dist of of M_i to M_j.
      //Only consider points within dist_th.
      double avg_dist = dbmsh3d_mesh_mesh_avg_dist (pro_[i]->mesh(), kdtrees[i],
                                                    pro_[j]->mesh(), top_n, dist_th);
      M_M_avg_dist += avg_dist;
      sz++;
    }
    vul_printf (vcl_cout, "\n");
  }

  assert (sz > 0);
  M_M_avg_dist /= sz;

  //Clean up memory and return.
  for (int i=0; i<N_DATA; i++)
    delete kdtrees[i];
  vul_printf (vcl_cout, "    %d kd-trees cleared.\n", N_DATA);

  vul_printf (vcl_cout, "\n  compute_M_M_avg_dist_bkt(): M_M_avg_dist: %f.\n\n", M_M_avg_dist);
  return M_M_avg_dist;
}


