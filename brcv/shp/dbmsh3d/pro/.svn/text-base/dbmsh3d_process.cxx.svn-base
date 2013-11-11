//: This is lemsvxlsrc/brcv/shp/dbmsh3d/pro/dbmsh3d_process.cxx
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>

void dbmsh3d_pro_base::move_pts_to_mesh ()
{
  assert (pro_data_ == PD_PTS);
  _copy_pts_to_mesh ();
  pts_.clear ();
  pro_data_ = PD_MESH;
}

void dbmsh3d_pro_base::move_mesh_to_pts ()
{
  assert (pro_data_ == PD_MESH);
  _copy_mesh_to_pts ();
  reset_mesh ();
  pro_data_ = PD_PTS;
}

void dbmsh3d_pro_base::move_sg3pi_to_pts ()
{
  assert (pro_data_ == PD_SG3PI);
  _copy_sg3pi_to_pts ();
  reset_sg3pi ();
  pro_data_ = PD_PTS;
}

void dbmsh3d_pro_base::move_sg3pi_to_mesh ()
{
  assert (pro_data_ == PD_SG3PI);
  _copy_sg3pi_to_mesh ();
  reset_sg3pi ();
  pro_data_ = PD_MESH;
}

//###########################################################################

void dbmsh3d_pro_base::_copy_oripts_to_pts ()
{
  assert (pts_.size() == 0);
  for (unsigned int i=0; i<oripts_.size(); i++)
    pts_.push_back (oripts_[i].first);
}

void dbmsh3d_pro_base::_copy_pts_to_mesh ()
{
  assert (mesh_->vertexmap().size() == 0);
  for (unsigned int i=0; i<pts_.size(); i++) {
    vgl_point_3d<double> P = pts_[i];
    dbmsh3d_vertex* V = mesh_->_new_vertex ();
    V->get_pt().set (P.x(), P.y(), P.z());
    mesh_->_add_vertex (V);
  }
}

void dbmsh3d_pro_base::_copy_mesh_to_pts ()
{
  vcl_map<int, dbmsh3d_vertex*>::iterator it = mesh_->vertexmap().begin();
  for (; it != mesh_->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    V->set_vid (pts_.size());
    pts_.push_back (V->pt());
  }
}

void dbmsh3d_pro_base::_copy_mesh_to_ifs_faces ()
{
  vcl_map<int, dbmsh3d_face*>::iterator it = mesh_->facemap().begin();
  for (; it != mesh_->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    vcl_vector<int> vids;
    for (unsigned int j=0; j<F->vertices().size(); j++)
      vids.push_back (F->vertices(j)->vid());
    ifs_faces_.push_back (vids);
  }
}

void dbmsh3d_pro_base::_copy_sg3pi_to_pts ()
{
  assert (pts_.size() == 0);
  for (unsigned int i=0; i<sg3pi_->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi_->data()[i].size(); j++) {
      vgl_point_3d<double> P = sg3pi_->data()[i][j]->pt();
      pts_.push_back (P);
    }
  }
}

void dbmsh3d_pro_base::_copy_sg3pi_to_mesh ()
{
  assert (mesh_->vertexmap().size() == 0);
  for (unsigned int i=0; i<sg3pi_->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi_->data()[i].size(); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi_->data()[i][j];
      dbmsh3d_vertex* V = mesh_->_new_vertex ();
      V->get_pt().set (SP->pt().x(), SP->pt().y(), SP->pt().z());
      mesh_->_add_vertex (V);
      SP->set_id (V->id());
    }
  }
}

void dbmsh3d_pro_base::_copy_valid_V_to_pts (const bool valid)
{
  pts_.clear();
  vcl_map<int, dbmsh3d_vertex*>::iterator it = mesh_->vertexmap().begin();
  for (; it != mesh_->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    if (valid) {
      if (V->is_valid())
        pts_.push_back (V->pt());
    }
    else {
      if (V->is_valid() == false)
        pts_.push_back (V->pt());
    }
  }
}

//###############################################################

//: Fix orientation of Polywork alignment matrix.
//     [ a b c d ]           [  a -b -c  d ]
// H = [ e f g h ]  to  H' = [ -e  f  g -h ]
//     [ i j k l ]           [ -i  j  k -l ]
//     [ m n o p ]           [  m -n -o  p ]
//
void dbmsh3d_pro_base::fix_hmatrix_orient ()
{
  vnl_matrix_fixed<double,4,4> H44 = hmatrix_.get_matrix ();
  H44.put (0,1, -H44(0,1));
  H44.put (0,2, -H44(0,2));
  H44.put (1,0, -H44(1,0));
  H44.put (1,3, -H44(1,3));
  H44.put (2,0, -H44(2,0));
  H44.put (2,3, -H44(2,3));
  H44.put (3,1, -H44(3,1));
  H44.put (3,2, -H44(3,2));

  hmatrix_.set (H44);
}

void dbmsh3d_pro_base::apply_xform_hmatrix ()
{
  //transform using the specified affine xform matrix
  if (pro_data_ == PD_PTS)
    dbmsh3d_apply_xform (pts_, hmatrix_);
  else if (pro_data_ == PD_ORIPTS)
    dbmsh3d_apply_xform (oripts_, hmatrix_);
  else if (pro_data_ == PD_MESH)
    dbmsh3d_apply_xform (mesh_, hmatrix_);
  else if (pro_data_ == PD_SG3PI)
    dbmsh3d_apply_xform (sg3pi_, hmatrix_);
  else
    assert (0);
}

//###############################################################
double dbmsh3d_pro_base::get_avg_samp_dist ()
{  
  vcl_vector<vgl_point_3d<double> > pts;

  if (pro_data_ == PD_PTS) {
    //Estimate avg_samp_dist by avg of k-N-N.
    avg_samp_dist_ = (float) estimate_avg_samp_dist (pts_);
  }
  else if (pro_data_ == PD_IDPTS) {
    for (unsigned int i=0; i<idpts_.size(); i++)
      pts.push_back (idpts_[i].second);
    //Estimate avg_samp_dist by avg of k-N-N.
    avg_samp_dist_ = (float) estimate_avg_samp_dist (pts);
  }
  else if (pro_data_ == PD_ORIPTS) {
    for (unsigned int i=0; i<oripts_.size(); i++)
      pts.push_back (oripts_[i].first);
    //Estimate avg_samp_dist by avg of k-N-N.
    avg_samp_dist_ = (float) estimate_avg_samp_dist (pts);
  }
  else if (pro_data_ == PD_SG3PI) {
    //Estimate avg_samp_dist by grid size.
    sg3pi_->get_sl_sample_dist();
    double d1 = (double) sg3pi_->intra_sl_dist();
    double d2 = (double) sg3pi_->inter_sl_dist();
    avg_samp_dist_ = (float) vcl_max (d1, d2);
  }
  else if (pro_data_ == PD_MESH) {
    if (mesh_->facemap().size() != 0)
      //Estimate avg_samp_dist by avg mesh edge len.
      avg_samp_dist_ = (float) mesh_->get_avg_edge_len_from_F ();
    else
      //Estimate avg_samp_dist by avg of k-N-N.
      avg_samp_dist_ = (float) estimate_avg_samp_dist (mesh_);
  }
  else {
    assert (0);
    avg_samp_dist_ = 0.0f;
  }
  vul_printf (vcl_cout, "\testimate_avg_samp_dist(): %f.\n", avg_samp_dist_);
  return avg_samp_dist_;
}

//###############################################################

//: Load input from various file types.
//  The file prefix is in dir_prefix_
//  Note the order of trying various input file types.
//
bool dbmsh3d_pro_base::load_points_files (vcl_string dirfile)
{  
  if (dirfile == "")
    dirfile = dir_file();

  if (load_p3d(dirfile))
    return true;
  else if (load_xyz(dirfile))
    return true;
  else if (load_ply2_v(dirfile))
    return true;
  else if (load_ply_v(dirfile))
    return true;
  else
    return false;
}

//: Load input face (already meshed) from various file types.
bool dbmsh3d_pro_base::load_faces_files (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();

  if (load_ply_f(dirfile))
    return true;
  else if (load_ply2_f(dirfile))
    return true;
  else
    return false;
}

//: Load input meshe from various file types.
bool dbmsh3d_pro_base::load_meshes_files (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();

  if (load_ply(dirfile))
    return true;
  else if (load_ply2(dirfile))
    return true;
  else if (load_off(dirfile))
    return true;
  else if (load_obj(dirfile))
    return true;
  else if (load_m(dirfile))
    return true;
  else
    return false;
}

//###############################################################

void dbmsh3d_pro_base::save_xyz (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".xyz";

  if (pro_data_ == PD_MESH)
    dbmsh3d_save_xyz (mesh_, dirfile.c_str());
  else if (pro_data_ == PD_PTS)
    dbmsh3d_save_xyz (pts_, dirfile.c_str());
  else
    assert (0);
}

bool dbmsh3d_pro_base::load_xyz (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".xyz";

  if (pro_data_ == PD_MESH) {
    reset_mesh ();
    return dbmsh3d_load_xyz (mesh_, dirfile.c_str());
  }
  else if (pro_data_ == PD_PTS) {
    pts_.clear();
    return dbmsh3d_load_xyz (pts_, dirfile.c_str());
  }
  else {
    return false;
  }
}

void dbmsh3d_pro_base::save_p3d (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".p3d";

  if (pro_data_ == PD_MESH)
    dbmsh3d_save_p3d (mesh_, dirfile.c_str());
  else if (pro_data_ == PD_PTS)
    dbmsh3d_save_p3d (pts_, dirfile.c_str());
  else
    assert (0);
}

bool dbmsh3d_pro_base::load_p3d (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".p3d";

  if (pro_data_ == PD_MESH) {
    reset_mesh();
    return dbmsh3d_load_p3d (mesh_, dirfile.c_str());
  }
  else if (pro_data_ == PD_PTS) {
    pts_.clear();
    return dbmsh3d_load_p3d (pts_, dirfile.c_str());
  }
  else {
    return false;
  }
}

void dbmsh3d_pro_base::save_xyzn1 (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".xyzn1";

  assert (pro_data_ == PD_ORIPTS);
  dbmsh3d_save_xyzn1 (oripts_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_xyzn1 (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".xyzn1";

  oripts_.clear();
  bool result = dbmsh3d_load_xyzn1 (oripts_, dirfile.c_str());
  if (result) //Need to set pro_data to PD_ORIPTS
    pro_data_ = PD_ORIPTS;
  return result;
}

void dbmsh3d_pro_base::save_3pi (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".3pi";

  assert (pro_data_ == PD_SG3PI);
  dbmsh3d_save_sg3pi (sg3pi_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_3pi (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".3pi";

  reset_sg3pi ();
  bool result = dbmsh3d_load_sg3pi (sg3pi_, dirfile.c_str());
  if (result) //Need to set pro_data to PD_SG3PI
    pro_data_ = PD_SG3PI;
  return result;
}

void dbmsh3d_pro_base::save_ply (vcl_string dirfile, const bool ascii_mode)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_ply (mesh_, dirfile.c_str(), ascii_mode);
}

bool dbmsh3d_pro_base::load_ply (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply";

  reset_mesh ();
  bool result = dbmsh3d_load_ply (mesh_, dirfile.c_str());
  //check integrity: remove erroneous faces.
  remove_erroneous_Fs_IFS (mesh_);
  return result;
}

//: only load the vertices to the mesh_
bool dbmsh3d_pro_base::load_ply_v (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply";
 
  reset_mesh ();
  return dbmsh3d_load_ply_v (mesh_, dirfile.c_str());
}

//: only load the faces to the mesh_
bool dbmsh3d_pro_base::load_ply_f (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply";
  if (vul_file::exists(dirfile) == false)
    return false;
  return dbmsh3d_load_ply_f (mesh_, dirfile.c_str());
}

void dbmsh3d_pro_base::save_ply2 (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  if (pro_data_ == PD_MESH)
    dbmsh3d_save_ply2 (mesh_, dirfile.c_str());
  else if (pro_data_ == PD_PTS)
    dbmsh3d_save_ply2 (pts_, ifs_faces_, dirfile.c_str());
  else
    assert (0);
}

bool dbmsh3d_pro_base::load_ply2 (vcl_string dirfile, const bool IFS_to_MHE)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  reset_mesh ();
  bool result = dbmsh3d_load_ply2 (mesh_, dirfile.c_str());
  if (IFS_to_MHE) {
    mesh_->IFS_to_MHE ();
    mesh_->build_face_IFS ();
  }
  return result;
}

//: only load the vertices to the mesh_
bool dbmsh3d_pro_base::load_ply2_v (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  reset_mesh ();
  return dbmsh3d_load_ply2_v (mesh_, dirfile.c_str());
}

//: only load the faces to the mesh_
bool dbmsh3d_pro_base::load_ply2_f (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  return dbmsh3d_load_ply2_f (mesh_, dirfile.c_str());
}

void dbmsh3d_pro_base::save_off (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".off";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_off (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_off (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".off";
  
  reset_mesh ();
  return dbmsh3d_load_off (mesh_, dirfile.c_str());
}

void dbmsh3d_pro_base::save_obj (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".obj";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_obj (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_obj (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".obj";

  reset_mesh ();
  return dbmsh3d_load_obj (mesh_, dirfile.c_str());
}

void dbmsh3d_pro_base::save_iv_ifs (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".iv";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_iv_ifs (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_iv_ifs (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".iv";

  reset_mesh ();
  return dbmsh3d_load_iv_ifs (mesh_, dirfile.c_str());
}

void dbmsh3d_pro_base::save_m (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".m";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_m (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_m (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".m";

  reset_mesh ();
  return dbmsh3d_load_m (mesh_, dirfile.c_str());
}


void dbmsh3d_pro_base::save_vtk (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".vtk";

  assert (pro_data_ == PD_MESH);
  dbmsh3d_save_vtk (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_vtk (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".vtk";

  reset_mesh ();
  return dbmsh3d_load_vtk (mesh_, dirfile.c_str());
}

bool dbmsh3d_pro_base::load_hmatrix (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".txt";
  return dbmsh3d_read_xform_file (dirfile.c_str(), hmatrix_);
}

//######################################################################

bool dbmsh3d_pro::load_ply2_fmm (vcl_string dirfile)
{
  if (dirfile == "")
    dirfile = dir_file();
  if (dbul_get_suffix (dirfile) == "")
    dirfile += ".ply2";

  reset_fmm_mesh ();
  return dbmsh3d_load_ply2 (fmm_mesh_, dirfile.c_str());
}

//###########################################################################

bool dbmsh3d_pro::sample_iv_to_pts (int opt, float th, float noise)
{
  vcl_string strFile_IV = dir_file() + ".iv";
  vcl_string strFile_P3D = dir_file() + ".p3d";

  //1) Read in the IV File using home-made pharser (extract vertices and links)
  //   and output to point cloud file.  
  dbmsh3d_load_iv_ifs_intp (strFile_IV.c_str(), strFile_P3D.c_str(), opt, noise, th);

  return dbmsh3d_load_p3d (mesh_, strFile_P3D.c_str());
}

void dbmsh3d_pro::process_pts (const float tx, const float ty, const float tz, 
                               const float rx, const float ry, const float rz,
                               const float scale, const bool shift_1stoct, 
                               const bool checkdup, const int subsam_pts)
{
  //Translate and Rotate the _Boundary
  if (tx!=0 || ty!=0 || tz!=0)
    translate_points (mesh_, tx, ty, tz);
  if (rx!=0 || ry!=0 || rz!=0)
    rotate_points (mesh_, rx, ry, rz);

  //Scale the _Boundary
  if (scale != 1)
    scale_points (mesh_, scale);

  //Check for duplicate
  if (checkdup)
    remove_duplicate_points (mesh_);

  //Subsample the input
  if (subsam_pts != 0) {    
    if (subsam_pts > 100) {
      // if subsam_pts > 100, randomly sample input points into subsam_pts points.
      subsample_points (mesh_, subsam_pts);
    }
    else {
      //else, sample by skipping every subsam_pts points.
      sample_skip_points (mesh_, subsam_pts);
    }
  }

  //Shift to the first Octant
  if (shift_1stoct)
    shift_points_to_first_octant (mesh_);
}

