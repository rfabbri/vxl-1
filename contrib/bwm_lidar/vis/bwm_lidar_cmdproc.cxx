//: This is bwm_lidar_cmdproc.cxx
//  Creation: Dec 13, 2007   Ming-Ching Chang

#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <vnl/vnl_random.h>

#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbul/dbul_dir_file.h>

#include <bwm_lidar/algo/lidar_labeler.h>
#include <bwm_lidar/algo/world_modeler.h>

#include <bwm_lidar/algo/bwm_lidar_mesh.h>
#include <bwm_lidar/algo/bwm_lidar_mesh.h>
#include <bwm_lidar/pro/bwm_lidar_cmdpara.h>

#include <bgui3d/bgui3d_file_io.h>

#include <bwm_lidar/vis/bwm_lidar_vis.h>
#include <bwm_lidar/vis/bwm_lidar_cmdproc.h>


#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face_mc.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>
#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>
///#include "bwm_lidar_cmdproc.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>

//: for one view
static bool read_calib (const vcl_string &view_f,
                        const vcl_string &camera_f,
                        const vcl_string &center_direction_f,
                        vil_image_view<vxl_byte>& view,
                        vnl_matrix_fixed<double,3,4> &camera,
                        vnl_vector_fixed<double,3> &center,
                        vnl_vector_fixed<double,3> &view_direction);

//: for all views
static bool read_calib_all (const vcl_string& input_path,
                            const vcl_string& texture_prefix,
                            vcl_vector<vil_image_view<vxl_byte> >& views,
                            vcl_vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
                            vcl_vector<vnl_vector_fixed<double,3> > &camera_center,
                            vcl_vector<vnl_vector_fixed<double,3> > &camera_view_direction);

void divide_ground_plane (vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec, 
                          double width, double height, double ground_height);

//#####################################################################
// dbmsh3d command-line process execution.
// false: no task specified.
bool bwm_lidar_check_cmdproc (int argc, char **argv)
{
  //1) Check if no cmdproc specified, result=PRO_RESULT_NO_PROCESS. 
  if (bwm_lidar_cmd_lidar()==0) {
    //Default Viewer: try to open the argv[1] file and visualize it.
    //Assume the first parameter specifies the file to display.
    if (argc>1 && argv[1][0] != '-') {      
      vcl_string dirfile = dbul_get_dir_file (argv[1]);
      vcl_string suffix = dbul_get_suffix (argv[1]);
    }
  }
  return true;
}

SoSeparator* bwm_lidar_cmdproc_execute ()
{
  SoSeparator* _root = new SoSeparator;

  vcl_string prefix;
  if (dbmsh3d_cmd_fileprefix())
    prefix = dbmsh3d_cmd_fileprefix();
  else
    prefix = "";

  //##########################################################################
  //#########################  Lidar Image Meshing  ##########################
  //##########################################################################
  //-run : run lidar image labeling and meshing.
  if (bwm_lidar_cmd_run() != 0) {

    //-d : input directory.
    vcl_string input_path;
    if (bwm_lidar_cmd_dir())
      input_path = bwm_lidar_cmd_dir();
    else
      input_path = ".";

    lidar_labeling_params params;
    vil_image_view<int> labels;
    vil_image_view<unsigned char> labels_colored; 
    vil_image_view<unsigned char> colors;
    vnl_matrix<double> height;
    vnl_matrix<int> occupied;
    world_modeler::generate_model (input_path, params, labels, labels_colored, colors, height, occupied);

    //-ofile 1,2: save output images.
    if (dbmsh3d_cmd_ofile())
      vil_save(colors, (prefix + "-colors.tif").c_str());
      ///vil_save(colors, (input_path + "\\" + prefix + "-colors.tif").c_str());

    vil_image_view<bool> occupied_image(occupied.rows(),occupied.cols());
    occupied_image.fill(false);
    double max_height = 0.0;
    for(unsigned i=0; i<occupied_image.ni(); i++){
      for(unsigned j=0; j<occupied_image.nj(); j++){
        if(occupied(i,j)==1){
          occupied_image(i,j) = true;
        }
        max_height = vnl_math_max(max_height,height(i,j));
      }
    }
    //-ofile 1,2: save output images.
    if (dbmsh3d_cmd_ofile())
      vil_save(occupied_image,(prefix + "-occupied.tif").c_str());
      //vil_save(occupied_image,(input_path + "\\" + prefix + "-occupied.tif").c_str());

    vil_image_view<unsigned char> height_image(height.rows(),height.cols());
    height_image.fill(0);
    for(unsigned i=0; i<height_image.ni(); i++){
      for(unsigned j=0; j<height_image.nj(); j++){
        height_image(i,j) = (int)height(i,j)*255.99/max_height;
      }
    }
    //-ofile 1,2: save output images.
    if (dbmsh3d_cmd_ofile())
      vil_save(height_image,(prefix + "-height.tif").c_str());

    ///get_connected_components::colorize_image(labels,labels_colored);

    //-ofile 1,2: save output images.
    if (dbmsh3d_cmd_ofile())
      vil_save(labels_colored,(prefix + "-labels_colored.tif").c_str());

    //======================= meshing ==============================

    //Estimate ground height.
    double ground_height = estimate_ground_height (labels, height);

    //draw the ground plane.
    SoSeparator* vis_ground = new SoSeparator;
    SbVec3f* ground_plane = new SbVec3f[4];
    ground_plane[0].setValue (0, 0, ground_height);
    ground_plane[1].setValue (labels.ni(), 0, ground_height);
    ground_plane[2].setValue (labels.ni(), labels.nj(), ground_height);
    ground_plane[3].setValue (0, labels.nj(), ground_height);  
    vis_ground->addChild (draw_filled_polygon (ground_plane, 4, SbColor(0.5f, 0.5f, 0.5f), 0));
    delete ground_plane;

    SoSeparator* vis;    
    SoSeparator* vis_building = new SoSeparator;
    vis_building->addChild (vis_ground);

    //vector of meshes for buildings.
    vcl_vector<dbmsh3d_textured_mesh_mc*> M_vec;

    divide_ground_plane (M_vec, labels.ni(), labels.nj(), ground_height);

    if (bwm_lidar_cmd_run()==1) {
      //-run 1: Visualize the meshing of the labelled image data using random color.
      vis = vis_lidar_labeled_data (labels, labels_colored, true,
        colors, height, occupied, ground_height, M_vec);
      vis_building->addChild (vis);
      _root->addChild (vis_building);
    }
    else if (bwm_lidar_cmd_run()==2) {
      //-run 2: Visualize the meshing of the labelled image data 
      //using average color of each building.
      vis = vis_lidar_labeled_data (labels, labels_colored, false,
        colors, height, occupied, ground_height, M_vec);
      vis_building->addChild (vis);
      _root->addChild (vis_building);
    }

    //Save file.
    if (dbmsh3d_cmd_ofile()>1) {
      dbmsh3d_cmd_wrl_ofile2() = (prefix + "-label.wrl").c_str();
      dbmsh3d_save_vis_to_iv_wrl (vis_building);
    }

    //-run 3: Visualize height image with color.
    if (bwm_lidar_cmd_run()==3) {
      SoSeparator* vis_pixel = new SoSeparator;
      vis = vis_lidar_pixel_color (labels, colors, height, occupied, ground_height);
      vis_pixel->addChild (vis_ground);
      vis_pixel->addChild (vis);
      _root->addChild (vis_pixel);

      //Save file.
      if (dbmsh3d_cmd_ofile()>1) {
        dbmsh3d_cmd_wrl_ofile2() = (prefix + "-color.wrl").c_str();
        dbmsh3d_save_vis_to_iv_wrl (vis_pixel);
      }
    }

    //-run 4: Visualize the building polygon with texture mapping.
    if (bwm_lidar_cmd_run()==4) {
      
      //-run 2: Visualize the meshing of the labelled image data 
      //using average color of each building.
      vis = vis_lidar_labeled_data (labels, labels_colored, false,
                                    colors, height, occupied, ground_height, M_vec);
      vis_building->addChild (vis);
      _root->addChild (shift_output (vis_building, -1000, 0, 0));

      //Given all 32 calibrated views, generate texture mapping mesh.

      if (bwm_lidar_cmd_ft() == NULL) {
        vcl_cerr << "specify -ft texture file prefix!!" << vcl_endl;
        return _root;
      }
      vcl_string texture_prefix = bwm_lidar_cmd_ft();

      //Load the view images from files.
      //Load the camera parameters from files.
      vcl_vector<vil_image_view<vxl_byte> > views;
      vcl_vector<vnl_matrix_fixed<double,3,4> > camera_matrices;
      vcl_vector<vnl_vector_fixed<double,3> > camera_center;
      vcl_vector<vnl_vector_fixed<double,3> > camera_view_direction;

      // read calibration for all images (32) 
      bool retval = read_calib_all (input_path, texture_prefix, views, camera_matrices, camera_center, camera_view_direction);

      //The texture mapping VRML file.
      vcl_string output_vrml = prefix + "-textured.wrl";

      ///dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;
      //-f prefix.
      ///bool r = dbmsh3d_load_ply2 (M, (prefix+".ply2").c_str());
      ///assert (r);

      for (unsigned int i=0; i<M_vec.size(); i++) {
        M_vec[i]->IFS_to_MHE();
        vcl_string file = vul_sprintf("bld_%02d.ply2", i);
        dbmsh3d_save_ply2 (M_vec[i], (prefix + file).c_str());
      }

      ///vcl_vector<dbmsh3d_textured_mesh_mc*> meshes;
      ///meshes.push_back (M);
      vcl_vector<vpgl_proj_camera<double> > cameras;
      for (unsigned int i=0; i<camera_matrices.size(); i++) {
        vpgl_proj_camera<double> camera(camera_matrices[i]);
        cameras.push_back(camera);
      }
      //vcl_vector<dbmsh3d_textured_mesh_mc> mesh_out;

      bool r = texturemap_meshes (M_vec, views, cameras, ".");
      assert (r);

      r = save_mesh_vrml(prefix+"-textured.wrl", M_vec, vgl_point_3d<double> (400.0, 300.0, 1000.0));
      assert (r);
    }

    //-run 5: Testing texture mapping.
    if (bwm_lidar_cmd_run()==5) {
      dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;
      //-f prefix.
      bool r = dbmsh3d_load_ply2 (M, (prefix+".ply2").c_str());
      assert (r);

      _root->addChild (draw_M_edges (M, SbColor(0,0,1), 1));
      ///_root->addChild (draw_M (M, dbmsh3d_cmd_shapehints(), 0, dbmsh3d_cmd_colorcode()));

      //texture mapping on face 1443 using file texture_1443.png
      dbmsh3d_textured_face_mc* F = (dbmsh3d_textured_face_mc*) M->facemap(1443);
      F->set_tex_map_uri (prefix + "_texture_1443.png");
      _root->addChild (draw_F_textured (F));

      //texture mapping on the ground plane.
      dbmsh3d_textured_face_mc* Fg = M->_new_face ();

      dbmsh3d_vertex* v[4];
      v[0] = M->_new_vertex ();
      v[0]->set_pt (0, 0, ground_height);
      M->_add_vertex (v[0]);


      v[1] = M->_new_vertex ();
      v[1]->set_pt (labels.ni(), 0, ground_height);
      M->_add_vertex (v[1]);
      

      v[2] = M->_new_vertex ();
      v[2]->set_pt (labels.ni(), labels.nj(), ground_height);
      M->_add_vertex (v[2]);

      v[3] = M->_new_vertex ();
      v[3]->set_pt (0, labels.nj(), ground_height);
      M->_add_vertex (v[3]);

      M->_add_face (Fg);  
      Fg->_add_vertex (v[0], vgl_point_2d<double> (v[0]->pt().x(), v[0]->pt().y()));
      Fg->_add_vertex (v[1], vgl_point_2d<double> (v[1]->pt().x(), v[1]->pt().y()));
      Fg->_add_vertex (v[2], vgl_point_2d<double> (v[2]->pt().x(), v[2]->pt().y()));
      Fg->_add_vertex (v[3], vgl_point_2d<double> (v[3]->pt().x(), v[3]->pt().y()));

      Fg->set_tex_map_uri (prefix + "_ground.png");
      _root->addChild (draw_F_textured (Fg));

    }


    dbmsh3d_app_window_title = "Meshing lidar image: ";
    dbmsh3d_app_window_title += prefix;
  }

  //##########################################################################
  //##########################################################################
  //-lidar 1: mesh labelled lidar text file pixel by pixel.
  if (bwm_lidar_cmd_lidar() == 1) {
    lidar_range_data* LRD = new lidar_range_data;
    //-f : file_prefix     
    bool r = load_lidar_label_file (dbmsh3d_cmd_fileprefix(), LRD);

    //mesh and visualize
    SoSeparator* vis = vis_lidar_data (LRD);
    _root->addChild (vis);

    dbmsh3d_app_window_title = " -- Meshing lidar file: ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_fileprefix();
  }
  //-lidar 2: mesh labelled lidar image in polygons for each building.
  else if (bwm_lidar_cmd_lidar() == 2) {
    //-f prefix
    //Load the lidar image 1
    vcl_string lidar_file_max = dbmsh3d_cmd_fileprefix();
    lidar_file_max += "_max.tif";
    vil_image_resource_sptr lidar_image_max = vil_load_image_resource( lidar_file_max.c_str() );
    assert (lidar_image_max);

    //Load the lidar image 2
    vcl_string lidar_file_min = dbmsh3d_cmd_fileprefix();
    lidar_file_min += "_min.tif";
    vil_image_resource_sptr lidar_image_min = vil_load_image_resource( lidar_file_min.c_str() );
    assert (lidar_image_min);

    //Load the lidar labelling image
    vcl_string lidar_file_labeled = dbmsh3d_cmd_fileprefix();
    lidar_file_labeled += "_labeled.tif";
    vil_image_resource_sptr lidar_image_labeled = vil_load_image_resource( lidar_file_labeled.c_str() );
    assert (lidar_image_labeled);

    //Go through each region of the lidar labelling image and
    // - if it is a building, estimate median height and draw in polygon.
    // - if it is a vegetation, draw pixel-by-pixel

    //mesh and visualize
    SoSeparator* vis = vis_lidar_labeled_image (lidar_image_max, lidar_image_min, lidar_image_labeled);
    _root->addChild (vis);

    dbmsh3d_app_window_title = " -- Meshing lidar image labeled: ";
    dbmsh3d_app_window_title += dbmsh3d_cmd_fileprefix();
  }

  return _root;
}

void divide_ground_plane (vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec, 
                          double width, double height, double ground_height)
{
  dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;
  M_vec.push_back (M);

  const int div_i = 10;
  const int div_j = 10;

  //put in all vertices and all faces.
  for (int i=0; i<width; i+=div_i) {
    for (int j=0; j<height; j+=div_j) {
      
      dbmsh3d_vertex* v[4];
      v[0] = M->_new_vertex ();
      v[0]->set_pt (i, j, ground_height);
      M->_add_vertex (v[0]);

      v[1] = M->_new_vertex ();
      v[1]->set_pt (i+div_i, j, ground_height);
      M->_add_vertex (v[1]);      

      v[2] = M->_new_vertex ();
      v[2]->set_pt (i+div_i, j+div_j, ground_height);
      M->_add_vertex (v[2]);

      v[3] = M->_new_vertex ();
      v[3]->set_pt (i, j+div_j, ground_height);
      M->_add_vertex (v[3]);

      //texture mapping on the ground plane.
      dbmsh3d_textured_face_mc* Fg = M->_new_face ();
      M->_add_face (Fg);  
      Fg->_add_vertex (v[0], vgl_point_2d<double> (v[0]->pt().x(), v[0]->pt().y()));
      Fg->_add_vertex (v[1], vgl_point_2d<double> (v[1]->pt().x(), v[1]->pt().y()));
      Fg->_add_vertex (v[2], vgl_point_2d<double> (v[2]->pt().x(), v[2]->pt().y()));
      Fg->_add_vertex (v[3], vgl_point_2d<double> (v[3]->pt().x(), v[3]->pt().y()));

      //Fg->set_tex_map_uri (prefix + "_ground.png");
      //_root->addChild (draw_F_textured (Fg));
    }
  }
}

//: for one view
bool
read_calib(
    const vcl_string &view_f,
    const vcl_string &camera_f,
    const vcl_string &center_direction_f,
    vil_image_view<vxl_byte>& view,
    vnl_matrix_fixed<double,3,4> &camera,
    vnl_vector_fixed<double,3> &center,
    vnl_vector_fixed<double,3> &view_direction
)
{
  //read camera view image.
  view = vil_load (view_f.c_str());

  //read camera matrix files.
  vcl_ifstream fp( camera_f.c_str() );
  if (!fp) {
    vcl_cerr << "read: error, unable to open file name " << camera_f << vcl_endl;
    return false;
  }

  fp >> camera;

  //read camera center and direction file.
  vcl_ifstream fp2( center_direction_f.c_str() );
  if (!fp2) {
    vcl_cerr << "read: error, unable to open file name " << center_direction_f << vcl_endl;
    return false;
  }

  fp2 >> center;
  fp2 >> view_direction;

  return true;
}


//: for all views
bool
read_calib_all (const vcl_string& input_path,
                const vcl_string& texture_prefix,
    vcl_vector<vil_image_view<vxl_byte> >& views,
    vcl_vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_center,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_view_direction
)
{
  //path to the texture image data.
  ///vcl_string mypath("/vision/video/rfabbri/lafayette-1218/");
  ///vcl_string mypath("V:\\video\\rfabbri\\lafayette-1218\\");
  vcl_string mypath = input_path + "/" + texture_prefix + "/";

  unsigned const n_blocks = 8;
  unsigned const n_directions_per_block = 4;
  camera_matrices.reserve(32);
  camera_center.reserve(32);
  camera_view_direction.reserve(32);
  bool final_retval = true;

  for(unsigned ib=1; ib <= n_blocks; ib++)
    for (unsigned iv=1; iv <= n_directions_per_block; iv++) {
      vcl_string myprefix;

      vcl_stringstream sstr;
      ///sstr << "Lafayette_" << ib << "_v" << iv;
      sstr << texture_prefix.c_str() << "_" << ib << "_v" << iv;
      sstr >> myprefix;

      vcl_string view_f = mypath + myprefix + vcl_string(".jpg");
      vcl_string camera_f = mypath + myprefix + vcl_string("_cam.txt");
      vcl_string center_direction_f = mypath + myprefix + vcl_string("_center-direction.txt");

      vil_image_view<vxl_byte> view;
      vnl_matrix_fixed<double,3,4> camera;
      vnl_vector_fixed<double,3> center;
      vnl_vector_fixed<double,3> view_direction;
      bool retval = read_calib (view_f, camera_f, center_direction_f,
                                view, camera, center, view_direction);
      if (!retval || (ib==5 && iv==1)) {
        final_retval = false;
        vcl_cerr << "Skipping file " << myprefix << "*" << vcl_endl;
        ///views.push_back(view);
        ///camera_matrices.push_back(camera);
        ///camera_center.push_back(center);
        ///camera_view_direction.push_back(view_direction);
      } 
      else {
        views.push_back(view);
        camera_matrices.push_back(camera);
        camera_center.push_back(center);
        camera_view_direction.push_back(view_direction);

        vcl_cout << "Block " << ib << " view " << iv << vcl_endl;
        vcl_cout << "Cam: " << camera << vcl_endl;
        vcl_cout << "Center: " << center << vcl_endl;
        vcl_cout << "Direction: " << view_direction<< vcl_endl << vcl_endl;
      }
    }
  return final_retval;
}

