//: This is lemsvxlsrc/brcv/shp/dbmsh3d/pro/dbmsh3d_process.cxx
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <bgui3d/bgui3d_file_io.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

//###############################################################
//  Visualization Functions in dbmsh3d_vis_base 
//###############################################################

SoSeparator* dbmsh3d_pro_vis::vis_ptset (const int colorcode, const bool b_draw_idv)
{
  SoSeparator* root = new SoSeparator;
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("bnd,pt"));
  root->addChild (soswitch);

  int ccode = (colorcode == COLOR_NOT_SPECIFIED) ? m_vertex_ccode_ : colorcode;

  if (pro_data_ == PD_PTS)
    return draw_ptset (pts_, colorcode, pt_size_);
  else if (pro_data_ == PD_IDPTS)
    assert (0);
  else if (pro_data_ == PD_ORIPTS)
    assert (0); 
  else if (pro_data_ == PD_SG3PI)
    assert (0); 
  else if (pro_data_ == PD_MESH) {
    if (b_draw_idv) //-idv 1: 
      soswitch->addChild (draw_ptset_idv (mesh_, ccode, cube_size_, user_defined_class_));
    else
      soswitch->addChild (draw_ptset (mesh_, ccode, pt_size_));
  }
  else
    assert (0); 

  return root;
}

SoSeparator* dbmsh3d_pro_vis::vis_ptset_idv ()
{
  SoSeparator* root = new SoSeparator;
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("bnd,pt"));
  root->addChild (soswitch);

  soswitch->addChild (draw_ptset_idv (mesh_, m_vertex_ccode_, 
                                      cube_size_, user_defined_class_));
  return root;
}

SoSeparator* dbmsh3d_pro_vis::vis_ptset_color ()
{
  return draw_pt_set_color (mesh_, mesh_vertex_color_set_, pt_size_, user_defined_class_);
}


SoSeparator* dbmsh3d_pro_vis::vis_mesh (const bool b_draw_idv, 
                                        const bool b_shape_hints,
                                        const int colorcode,
                                        const bool b_draw_isolated_pts)
{
  vul_printf (vcl_cout, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());

  SoSeparator* root = new SoSeparator;

  //Add switch
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("boundary,mesh"));
  root->addChild (soswitch); 

  mesh_->mark_unmeshed_pts ();
  
  if (b_draw_idv) { //Draw each individual F,E,V as separate object to allow getInfo.
    soswitch->addChild (draw_M_faces_idv (mesh_, true, m_transp_, colorcode, user_defined_class_));
  }
  else { //Draw the whole mesh as a single object
    soswitch->addChild (draw_M (mesh_, b_shape_hints,  m_transp_, colorcode));
  }

  //Draw the unmeshed vertices as point cloud.
  if (b_draw_isolated_pts)
    root->addChild (draw_ptset (mesh_, m_vertex_ccode_, pt_size_, true));

  unsigned n_unmeshed_pts = mesh_->count_unmeshed_pts ();
  vul_printf (vcl_cout, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

SoSeparator* dbmsh3d_pro_vis::vis_mesh_edges (const int colorcode, const float width,
                                              const bool draw_idv)
{
  if (draw_idv == false)
    return draw_M_edges (mesh_, color_from_code (colorcode), width);
  else
    return draw_M_edges_idv (mesh_, color_from_code (colorcode), width);
}

SoSeparator* dbmsh3d_pro_vis::vis_mesh_pts_edges (const int colorcode, const bool draw_idv,
                                                  const float pt_size)
{
  SoSeparator* root = new SoSeparator;  
  int ccode = (colorcode == COLOR_NOT_SPECIFIED) ? m_vertex_ccode_ : colorcode;
  root->addChild (draw_ptset (mesh_, ccode, pt_size));
  root->addChild (vis_mesh_edges (COLOR_GRAY, draw_idv));
  return root;
}

SoSeparator* dbmsh3d_pro_vis::vis_mesh_bndcurve (const int colorcode, const float width)
{
  return draw_M_bndcurve (mesh_, colorcode, width);
}

SoSeparator* dbmsh3d_pro_vis::vis_mesh_color ()
{
  return draw_M_color (mesh_, true, m_transp_, mesh_face_color_set_, user_defined_class_);
}

//: Visualize the reconstructed surface mesh according to the input option.
SoSeparator* dbmsh3d_pro_vis::vis_mesh_options (int option, const bool draw_idv, const bool showid)
{
  vul_printf (vcl_cout, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());
  SoSeparator* root = new SoSeparator;
  if (mesh_->b_watertight())
    option = 1; //draw gold color for watertight surface

  mesh_->mark_unmeshed_pts ();

  switch (option) {
  case 1: //-v 1: visualize the surface mesh as one single object (fastest).
    if (mesh_->b_watertight())
      root->addChild (draw_M (mesh_, true, m_transp_, COLOR_GOLD));
    else
      root->addChild (draw_M (mesh_, true, m_transp_));
  break;
  case 2: //-v 2: distinguish acute vs obtuse triangles.
    root->addChild (draw_M_bnd_faces_cost_col (mesh_, draw_idv, showid, m_transp_));
  break;
  case 0:
  case 3: //-v 3: distinguish triangle topology.
    root->addChild (draw_M_bnd_faces_topo_col (mesh_, draw_idv, showid, m_transp_,
                                                 user_defined_class_));
    root->addChild (draw_M_topo_vertices (mesh_, 1, cube_size_, user_defined_class_));
  break;
  }

  //Draw the unmeshed vertices as point cloud.
  root->addChild (draw_ptset (mesh_, m_vertex_ccode_, pt_size_, true));

  unsigned n_unmeshed_pts = mesh_->count_unmeshed_pts ();
  vul_printf (vcl_cout, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

//: Visualize the reconstructed surface mesh in animation
SoSeparator* dbmsh3d_pro_vis::vis_mesh_anim (const int nF_batch)
{
  vul_printf (vcl_cout, "vis_mesh_anim(): V: %d, E: %d, F: %d.\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());
  vul_printf (vcl_cout, "  Options:\n");
  vul_printf (vcl_cout, "    -n1: # faces shown in batch: %d\n", nF_batch);

  SoSeparator* root = new SoSeparator;
  root->addChild (draw_M_bnd_faces_anim (mesh_, nF_batch));
  return root;
}

//###############################################################

//: result in mesh_vertex_color_set_[]
void dbmsh3d_pro_vis::compute_mesh_vertex_color_set (const vcl_vector<double>& values)
{
  if (mesh_vertex_color_set_.size() == values.size()) {
    return;
  }
  vul_printf (vcl_cout, "  compute_mesh_vertex_color_set(): %d points.\n", values.size());

  SbColor color;
  unsigned n_over_limit = 0;
  mesh_vertex_color_set_.clear();
  for (unsigned int i=0; i<values.size(); i++) {
    if (values[i] <= 1)
      color = get_color_tone ((float) values[i]);
    else {
      color = SbColor (1.0f,0.0f,1.0f); //pink
      n_over_limit++;
    }
    mesh_vertex_color_set_.push_back (color);
  }
  vul_printf (vcl_cout, "    %d (%.2f%%) non-overlapping points in pink.\n", 
              n_over_limit, double(n_over_limit)/values.size()*100);
}

//: After running compute_mesh_vertex_color_set(), color the faces.
//  result in mesh_face_color_set_[]
void dbmsh3d_pro_vis::compute_face_color_by_value (const vcl_vector<double>& values)
{
  vul_printf (vcl_cout, "  compute_face_color_by_value(): %d points, %d faces.\n", 
              values.size(), mesh_->facemap().size());

  unsigned n_over_limit = 0;
  mesh_face_color_set_.clear();
  SbColor color;

  vcl_map<int, dbmsh3d_face*>::iterator it = mesh_->facemap().begin();
  for (; it != mesh_->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;

    //Compute color using the max of all point values of this face.
    double fvalue = 0;
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      int id = F->vertices(i)->id();
      fvalue += values[id];
    }
    fvalue /= F->vertices().size();

    if (fvalue <= 1)
      color = get_color_tone ((float) fvalue);
    else {
      color = SbColor (1.0f,0.0f,1.0f); //pink
      n_over_limit++;
    }
    mesh_face_color_set_.push_back (color);
  }

  vul_printf (vcl_cout, "    %d (%.2f%%) non-overlapping faces in pink.\n", 
              n_over_limit, double(n_over_limit)/mesh_->facemap().size()*100);
}

//########################################################################

//: load the color info into gene_color_set_
bool dbmsh3d_pro_vis::load_g3d (vcl_string filename)
{
  if (filename == "")
    filename = dir_prefix() + ".g3d";
  else if (dbul_get_suffix (filename) == "")
    filename += ".g3d";
  mesh_vertex_color_set_.clear();
  return dbmsh3d_load_g3d (mesh_, mesh_vertex_color_set_, filename.c_str());
}

void dbmsh3d_pro_vis::save_g3d (vcl_string filename)
{
  if (filename == "")
    filename = dir_prefix() + ".g3d";
  else if (dbul_get_suffix (filename) == "")
    filename += ".g3d";
  dbmsh3d_save_g3d (mesh_, filename.c_str());
}

// ###################################################################

SoSeparator* dbmsh3d_pro_vis::view_xyz (vcl_string filename, const int colorcode)
{
  if (filename == "")
    filename = dir_file() + ".xyz";
  else if (dbul_get_suffix (filename) == "")
    filename += ".xyz";

  if (load_xyz(filename))
    return vis_ptset (colorcode);
  else
    return NULL;
}

SoSeparator* dbmsh3d_pro_vis::view_p3d (vcl_string filename, const int colorcode)
{
  if (filename == "")
    filename = dir_file() + ".p3d";
  else if (dbul_get_suffix (filename) == "")
    filename += ".p3d";

  if (load_p3d(filename))
    return vis_ptset (colorcode);
  else
    return NULL;
}

SoSeparator* dbmsh3d_pro_vis::view_g3d (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".g3d";
  else if (dbul_get_suffix (filename) == "")
    filename += ".g3d";

  if (load_g3d(filename))
    return vis_ptset_color ();
  else
    return NULL;
}

SoSeparator* dbmsh3d_pro_vis::view_ply (vcl_string filename, bool b_shape_hints,
                                        const int colorcode,
                                        const bool b_draw_isolated_pts)
{
  if (filename == "")
    filename = dir_file() + ".ply";
  else if (dbul_get_suffix (filename) == "")
    filename += ".ply";

  if (load_ply(filename))
    return vis_mesh (false, b_shape_hints, colorcode, b_draw_isolated_pts);
  else
    return NULL;
}

SoSeparator* dbmsh3d_pro_vis::view_ply2 (vcl_string filename, bool b_shape_hints,
                                         const int colorcode,
                                         const bool b_draw_isolated_pts)
{
  if (filename == "")
    filename = dir_file() + ".ply2";
  else if (dbul_get_suffix (filename) == "")
    filename += ".ply2";

  if (load_ply2(filename))
    return vis_mesh (false, b_shape_hints, colorcode, b_draw_isolated_pts);
  else
    return NULL;
}


SoSeparator* dbmsh3d_pro_vis::view_iv (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".iv";
  else if (dbul_get_suffix (filename) == "")
    filename += ".iv";
  return (SoSeparator*) bgui3d_import_file (filename);
}

SoSeparator* dbmsh3d_pro_vis::view_wrl (vcl_string filename)
{
  if (filename == "")
    filename = dir_file() + ".wrl";
  else if (dbul_get_suffix (filename) == "")
    filename += ".wrl";
  return (SoSeparator*) bgui3d_import_file (filename);
}


// ###################################################################
//: Read the list file and view each file.
//  Format of list file:
//    One line per dataset: (data_file) [data_alignment_hmatrix]
//    The alignment is optional.
//
//    - option ==0 : nothing.
//             ==1 : save points into pts_[].
//             ==2 : save points into pts_[] and faces into ifs_faces[][].
//    - vis_option ==0 : show each file in random color.
//                 ==1 : show all files in a single color (silver).
//
SoSeparator* dbmsh3d_pro_vis::vis_list_file (vcl_string filename, 
                                             const int option, const int vis_option)
{
  SoSeparator* root = new SoSeparator;
  
  if (dbul_get_suffix (filename) == "")
    filename += ".txt";

  vul_printf (vcl_cout, "vis_list_file(): %s.\n", filename.c_str());

  vcl_ifstream  in;
  vcl_string    linestr;
  in.open (filename.c_str());
  if (in == false) {
    vul_printf (vcl_cout, "Can't open listfile %s\n", filename.c_str());
    return root; 
  }

  //Visualization: either in random colors or a single color.
  VIS_COLOR_CODE ccode = COLOR_RANDOM;
  if (vis_option == 1)
    ccode = COLOR_SILVER;
  init_rand_color (DBMSH3D_MESH_COLOR_SEED);

  //Read in each line of dataset in the list file.
  int file_count = 0;
  int af_count = 0;
  int n_total_pts = 0;
  int n_total_faces = 0;
  while (in) {
    linestr.clear();
    vcl_getline (in, linestr);

    if (linestr.length() == 0 || vcl_strncmp (linestr.c_str(), "#", 1) == 0)
      continue; //skip empty line and comments.

    char file[256] = "", file_af[256] = "";
    vcl_sscanf (linestr.c_str(), "%s %s", file, file_af);
    DBMSH3D_FILE_TYPE type;
    
    //1) Read in the data file
    if (vcl_strcmp (file, "") != 0) {
      reset_data (); //clean up the datastructure before loading.
      pro_data_ = PD_MESH;
      
      vcl_string suffix = dbul_get_suffix (file);
      if (suffix == ".p3d") {
        type = DBMSH3D_FILE_P3D;
        load_p3d (file);
      }
      else if (suffix == ".xyz") {
        type = DBMSH3D_FILE_XYZ;
        load_xyz (file); 
      }
      else if (suffix == ".3pi") {
        type = DBMSH3D_FILE_3PI;
        load_3pi (file); 
        move_sg3pi_to_pts();
      }
      else if (suffix == ".ply") {
        type = DBMSH3D_FILE_PLY;
        load_ply (file);
      }
      else if (suffix == ".ply2") {
        type = DBMSH3D_FILE_PLY2;
        load_ply2 (file);
      }
      else {
        file_count--;
      }
      file_count++;
    }

    //2) Read in the alignment file (if any)
    if (vcl_strcmp (file_af, "") != 0) {      
      load_hmatrix (file_af); //read in the alignment file.
      apply_xform_hmatrix (); //xform the dataset.
      af_count++;
    }

    //3) Display the data file in random color. 
    if (type == DBMSH3D_FILE_P3D || type == DBMSH3D_FILE_XYZ ||
        type == DBMSH3D_FILE_3PI) {
      root->addChild (vis_ptset (ccode));
    }
    else if (type == DBMSH3D_FILE_PLY || type == DBMSH3D_FILE_PLY2) {
      root->addChild (vis_mesh (false, true, ccode, true)); //false
    }

    //4) If option==1, save points into pts_[].
    if (option==1 || option==2)
      _copy_mesh_to_pts ();
    else if (option==2)
      _copy_mesh_to_ifs_faces ();
    
    if (type == DBMSH3D_FILE_3PI)
      n_total_pts += pts_.size();
    else {
      n_total_pts += mesh_->vertexmap().size();
      n_total_faces += mesh_->facemap().size();
    }

    vul_printf (vcl_cout, "\t Currently %u points %u faces loaded.\n", 
                n_total_pts, n_total_faces);
  }

  in.close();
  if (option==1)
    assert (pts_.size() == n_total_pts);

  vul_printf (vcl_cout, "\n====================================================\n");
  vul_printf (vcl_cout, "  vis_list_file(): %d files (%d align. files) shown from %s.\n", 
              file_count, af_count, filename.c_str());
  vul_printf (vcl_cout, "\tTotally %u points, %u faces.\n", 
              n_total_pts, n_total_faces);
  return root;
}
