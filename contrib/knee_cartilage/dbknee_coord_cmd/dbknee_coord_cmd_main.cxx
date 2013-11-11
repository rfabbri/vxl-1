// This is contrib/knee_cartilage/kneecoord_cmd/kneecoord_cmd_main.cxx

//:
// \file
// \brief Compute coordinate system on a knee joint
// \date Jan 1, 2007
//
// \verbatim
//  Modifications:
// \endverbatim.
 

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_math.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>


//: Note the order here!!

#include <dbknee/dbknee_coord.h>

#include <vcsl/vcsl_spatial.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>

#include <dbmsh3d/vis/dbmsh3d_process_vis.h>

//#include <dbsk3d/vis/dbsk3d_process_vis.h>
//#include <dbsk3d/vis/dbsk3d_vis_backpt.h>



#include <Inventor/SoDB.h>
#include <Inventor/Win/SoWin.h>     //<=== Specify the platform to be MS Windows
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/events/SoKeyboardEvent.h>

bool  CMD_VALIDATE = false;

//=====================================================
//: General visualization parameters.
int CMD_SHOW_GUI = 1;
int CMD_VIS = 0;
int  CMD_SHOW_ID = 0;
float CMD_SHOW_TNB = 0.0f;
float CMD_CUBE_SIZE = -1.0f;    //0.01f
float CMD_VERTEX_SIZE = -1.0f;  //0.03f;
float CMD_TRANSP = 0.0f;
float CMD_MS_TRANSP = 0.0f;
int  CMD_COLORCODE = COLOR_SILVER;
int  CMD_COLORCODE2 = COLOR_GOLD;
bool CMD_SHAPE_HINTS = true;
int  CMD_LIGHT = 0;     //default lighting.
int  CMD_BG_COL = 0; //1
int  CMD_GSIZE = 1;
int  CMD_SHOW_IDV = 0;      //draw each pointgene individually
int  CMD_VIS_INF = 0;
int  CMD_BND = 1;       //option for drawing boundary mesh/points.
float CMD_PRUNE_BOX_RATIO = 0.1f;
float CMD_VIEW_DX = -1.0f;      // -1 as un-specified value.
float CMD_VIEW_DY = -1.0f;
float CMD_VIEW_DZ = -1.0f;

bool CMD_DEL = 0;

//: Output Files
char* CMD_OP3D = NULL;
char* CMD_OIV = NULL;
char* CMD_OIV_BIN = NULL;
char* CMD_OVRML = NULL;
char* CMD_OVRML2 = NULL;

////: ================== THE SYSTEM PARAMETERS ==================
bool VIEW_P3D = false;

void eventcb (void* userdata, SoEventCallback* node)
{}


//: ================== THE MAIN FUNCTION ==================
//: Run results
#define RUN_RESULT_SUCCESS  0
#define RUN_RESULT_NO_PROCESS  -1
#define RUN_RESULT_FILE_READ_ERROR  -2

//: Ming: To enable click-selection, we need to use SoSelection instead of SoSeparator.
//:       This makes everything selectable.
SoSelection* _root;

int main (int argc, char **argv)
{
  // ==========================================================================
  // 1) Set up the command line variables
  // ==========================================================================
  

  // task-specific params

  
  // Compute coordinate system
  vul_arg<int > compute_cs("-compute_cs", 
    "Set to 1 to compute cylinder based coordinate system", 0); 
  vul_arg<vcl_string > data_file("-data_file", "Input data file", "");
  vul_arg<vcl_string > input_file_format("-format", "Input file format", "");
  vul_arg<vcl_string > param_file("-param_file", "Parameter file", "");
  vul_arg<vcl_string > output_file("-out_file", "Output file", "");
  vul_arg<int > show_gui("-show_gui", "Visualize results", 1);

  

  // compute points of center bands on the mesh
  vul_arg<int > compute_bands("-compute_bands", 
    "Compute points of the center bands", 0);
  vul_arg<vcl_string > band_file("-band_file",
    "Name of file to save band points.",
    "cartilage_band_points_local_coord.pc");


  // separate cartilage mesh into inner and outer surface
  vul_arg<int > inner_outer("-inner_outer", 
    "Separate mesh into inner and outer surfaces", 0);
  vul_arg<vcl_string > inner_mesh_file("-inner_mesh_file", 
    "Name of inner mesh file", "inner_surface.ply");
  vul_arg<vcl_string > outer_mesh_file("-outer_mesh_file", 
    "Name of outer mesh file", "outer_surface.ply");
 



  // crop a mesh using cylinder coordinates
  vul_arg<int > crop_cyl("-crop_cyl", 
    "Crop a mesh using cylinder coordinate system", 0);
  vul_arg<vcl_string > cs_file("-cs_file", "Coordinate system parameter file", "");
  vul_arg<vcl_vector<double > > cyl_angle_list("-cyl_angle_list", 
    "List of cylindrical angles, in CCW order, determing the regions", 
    vcl_vector<double >());
  vul_arg<double > band_width_ratio("-band_width_ratio", 
    "Ratio of the cartilage bands' width compared to overall width", 0.2);


  // take in a mesh with thickness attached and a coordinate system and output
  // a 2D table encoding the euclidean coordinates (on the cylinder) of the points
  // and their thickness
  vul_arg<int > thickness2d("-thickness2d", 
    "Flatten a cartilage surface using a cylider and export their 2d coordinates and thickness",
    0);


  // pure visualization commands
  vul_arg<vcl_string > view_mesh("-view_mesh", "Mesh to visualize", "");
  vul_arg<vcl_string > view_cs("-view_cs", "View cylindrical coordinate system", "");
  


  // Parse
   vul_arg_parse(argc, argv);
  


  // ================== SOME APPLICATION RELATED VARIABLES ==================
  vcl_string ApplicationTitle = "LEMS 3D Visualizer";

  //: ================== VARIABLES ==================
  vcl_string dir_prefix = "";      //File prefix
  
  VIS_COLOR_CODE bg_colorcode = COLOR_BLACK;
  VIS_COLOR_CODE gene_colorcode = COLOR_WHITE;
  
  // ==========================================================================
  // 2) Initializes SoWin library (and implicitly also the Coin
  //   library). Returns a top-level / shell window to use.
  
  // Ming: To register my own type into the system.
  //       See http://doc.coin3d.org/Coin/classSoType.html#d2 for help.
  // ==========================================================================

  SoDB::init();

  //Initialize homemade Coin3d OpenInventor classes.
  dbmsh3dvis_init_vispt_OpenInventor_classes();
  //dbsk3dvis_init_vispt_OpenInventor_classes();
  
  HWND mainwin = SoWin::init(argc, argv, argv[0]);

  //: Ming: To enable click-selection, we need to use SoSelection instead of SoSeparator.
  //:       This makes everything selectable.
  _root = new SoSelection;
  _root->policy = SoSelection::SHIFT;

  _root->ref();

  // Nhon: increase line drawing width
  SoDrawStyle* style = new SoDrawStyle();
  style->lineWidth.setValue(2.0f);
  _root->addChild(style);


  // ---------- Setting General Processing Parameters -------------------------
  
  SoSeparator* vis;

  // ==========================================================================
  // 3) PROCESS TASKS
  // ==========================================================================
  
  // --------------------------------------------------------------------------
  // A. Compute the cylinder-based coordinate system
  // --------------------------------------------------------------------------

  if (compute_cs())
  {
    // i. Load input mesh file
    // --------------

    // determine file name
    if (data_file() == "")
    {
      vcl_cerr << "An input data file is required to proceed."
        << "Use '-data_file' option for input data file.\n";
      return RUN_RESULT_NO_PROCESS;
    }

    vcl_cout << "Input data file = " << data_file() << vcl_endl;

    // determine the file format
    if (input_file_format() == "")
    {
      input_file_format.value_ = vul_file::extension(data_file());
    }
    else
    {
      input_file_format.value_ = "." + input_file_format();
    }

    dbmsh3d_pro_vis* bndvis = new dbmsh3d_pro_vis;
    bndvis->reset_data(); 
    bndvis->reset_mesh();

    // Load the point cloud depending on the format
    if (input_file_format() == ".p3d")
    {
      if ( !bndvis->load_p3d(data_file()) )
      {
        vcl_cerr << "ERROR: Could not load .p3d file " 
          << data_file() << ".\n";
        return RUN_RESULT_FILE_READ_ERROR;
      }
    }
    else if (input_file_format() == ".ply")
    {
      if ( !bndvis->load_ply(data_file()) )
      {
        vcl_cerr << "ERROR: Could not load .ply file " 
          << data_file() << ".\n";
        return RUN_RESULT_FILE_READ_ERROR;
      }
    }
    else // Unknow format --> Stop the program
    {
      vcl_cerr << "ERROR: Invalid file format. Quit now.\n";
      return RUN_RESULT_NO_PROCESS;
    } 

    
    
    // ii. Load the parameters neccessary to build the coordinate system
    // -----------------------------------------------------------------
    
    if (param_file() == "")
    {
      vcl_cerr << "ERROR: No parameter file specified.\n";
      return RUN_RESULT_NO_PROCESS;
    }
    vcl_cout << "Parameter file = " << param_file() << vcl_endl;


    /////////////////////////////////////////////////////////////
    vcl_map<vcl_string, double > param_map;
    dbknee_read_coord_param_file(param_file(), param_map);

    if (param_map.empty())
    {
      vcl_cerr << "ERROR: parameter file is either non-existent or empty.\n";
      return RUN_RESULT_NO_PROCESS;
    }

    // iii. Construct the coordinate system
    // ------------------------------------

    vgl_box_3d<double> bounding_box;
    detect_bounding_box (bndvis->mesh(), bounding_box);
    dbknee_cylinder_based_coord_params coord_params(bounding_box, param_map);
    
    dbknee_cylinder_based_coord coord;
    coord.set_point_set(bndvis->mesh());
    vcl_cout << "Cropping box = " << coord_params.cropping_box << vcl_endl;
    coord.set_cropping_box(coord_params.cropping_box);

    vcl_cout << "Notch point = " << coord_params.notch_point << vcl_endl;
    coord.set_notch_point(coord_params.notch_point);


    // Execution !
    coord.build();
    coord.compute_z_of_band_centroids();


    // iv. Save results
    // -----------------

    if (output_file() == "")
    {
      output_file.value_ = vul_file::strip_extension(param_file()) + "_output.txt";
    }
    // open file for writing
    vcl_ofstream outfp(output_file().c_str(), vcl_ios_out);
    coord.print(outfp);
    outfp.close();  

    // v. Visualize the system
    // ------------------------

    // Add point set to visualize
    vis = bndvis->vis_ptset(); //vis_pt_set();
    _root->addChild (vis);

    delete bndvis;
    
    ///////////////////////////////////////////////////////////////////////////
    // temporary - a hack to save the cropped point set to a file
    // vcl_string cropped_point_file = "cropped_point_set_local_coords.pc";
    // dbknee_compute_write_local_coords_to_file(coord, bndshockvis->bnd_set(),
    //  cropped_point_file);
    ///////////////////////////////////////////////////////////////////////////
    

    // visualize the cylinder
    vgl_cylinder<double > cylinder = coord.cylinder();
    vis = draw_cylinder(cylinder.center(), 
      cylinder.center() + cylinder.length()*cylinder.orientation(),
      (float)cylinder.radius(), color_from_code(COLOR_CYAN), 0.5f);
    _root->addChild (vis);

    // draw the 3 axes
    vis = draw_line(0.0f, 0.0f, 0.0f, 100.0f, 0.0f, 0.0f, color_from_code(COLOR_RED));
    _root->addChild(vis);
    vis = draw_line(0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 0.0f, color_from_code(COLOR_GREEN));
    _root->addChild(vis);
    vis = draw_line(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f, color_from_code(COLOR_BLUE));
    _root->addChild(vis);

    // draw the notch point
    vis = draw_sphere(coord.notch_point(), 1.0f, color_from_code(COLOR_RED));
    _root->addChild(vis);

    vis = draw_sphere(coord.origin(), 1.0f, color_from_code(COLOR_GREEN));
    _root->addChild(vis);



    /////////////////////////////////////////////////////////////////////////////
    // a temporary hack to display all 7 sets of axes simultaneously
    //vnl_file_matrix<double > pts("coord_pts_7_trials.txt");

    //for (int k=0; k<7; ++k)
    //{
    //  int row = k*4;
    //  vgl_point_3d<double > pt(pts(row, 0), pts(row, 1), pts(row, 2));
    //  vgl_point_3d<double > pt_x(pts(row+1, 0), pts(row+1, 1), pts(row+1, 2));
    //  vgl_point_3d<double > pt_y(pts(row+2, 0), pts(row+2, 1), pts(row+2, 2));
    //  vgl_point_3d<double > pt_z(pts(row+3, 0), pts(row+3, 1), pts(row+3, 2));

    //  // draw the axes of the new coordinate system
    //  vis = draw_line(pt, pt_x, color_from_code(COLOR_RED));
    //  _root->addChild(vis);
    //  vis = draw_line(pt, pt_y, color_from_code(COLOR_GREEN));
    //  _root->addChild(vis);
    //  vis = draw_line(pt, pt_z, color_from_code(COLOR_BLUE));
    //  _root->addChild(vis);
    //}
    /////////////////////////////////////////////////////////////////////////////





    // draw the axes of the new coordinate system
    vis = draw_line(coord.origin(), coord.origin()+70*coord.x_axis(), 
      color_from_code(COLOR_RED));
    _root->addChild(vis);
    vis = draw_line(coord.origin(), coord.origin()+70*coord.y_axis(), 
      color_from_code(COLOR_GREEN));
    _root->addChild(vis);
    vis = draw_line(coord.origin(), coord.origin()+70*coord.z_axis(), 
      color_from_code(COLOR_BLUE));
    _root->addChild(vis);
  }

  // --------------------------------------------------------------------------
  // B. Separate the cartilage mesh into inner and outer mesh
  // --------------------------------------------------------------------------

  if (inner_outer())
  {
    dbmsh3d_mesh inner_mesh;
    dbmsh3d_mesh outer_mesh;

    dbmsh3d_mesh mesh;

    dbmsh3d_load_ply(&mesh, data_file().c_str());
    mesh.print_summary(vcl_cout);

    dbknee_cylinder_based_coord coord;
    coord.load_from_cs_file(cs_file());
    vgl_point_3d<double > axis_origin = coord.origin();
    vgl_vector_3d<double > axis_dir = coord.z_axis();

    vcl_cout << "Separating the cartilage into inner and outer meshes.\n";
    dbknee_separate_inner_outer_surfaces(mesh, 
                                          axis_origin,
                                          axis_dir,
                                          inner_mesh,
                                          outer_mesh);

    vcl_cout << "Writing inner and outer meshes to files.\n";
    dbmsh3d_save_ply(&inner_mesh, inner_mesh_file().c_str(), false);
    dbmsh3d_save_ply(&outer_mesh, outer_mesh_file().c_str(), false);
  }

  // --------------------------------------------------------------------------
  // C. Crop a mesh using cylinder coordinates
  // --------------------------------------------------------------------------

  if (crop_cyl())
  {
    // load the coordinate system
    dbknee_cylinder_based_coord coord;
    coord.load_from_cs_file(cs_file());

    // load the mesh
    dbmsh3d_mesh mesh;
    dbmsh3d_load(&mesh, data_file().c_str());

    // retrieve the angles
    vcl_vector<double > crop_angles = cyl_angle_list();

    // crop the mesh into regions and save the mesh

    // for each angle range
    for (int i=0; i< (int)crop_angles.size()-1; ++i)
    {
      double theta_min_degree = crop_angles[i];
      double theta_min = theta_min_degree*vnl_math::pi / 180;

      double theta_max_degree = crop_angles[i+1];
      double theta_max = theta_max_degree * vnl_math::pi / 180;

      
      // for each band
      
      // top band
      dbmsh3d_mesh top_mesh;
      double top_z_min = coord.z_top_band_centroid() - 
        0.5*band_width_ratio()*coord.cylinder().length();
      double top_z_max = coord.z_top_band_centroid() +
        0.5*band_width_ratio()*coord.cylinder().length();

      coord.crop_mesh_cylindrical(mesh, top_mesh, 
        top_z_min, top_z_max, theta_min, theta_max);

      // bottom band
      dbmsh3d_mesh bot_mesh;
      double bot_z_min = coord.z_bot_band_centroid() - 
        0.5*band_width_ratio()*coord.cylinder().length();
      double bot_z_max = coord.z_bot_band_centroid() +
        0.5*band_width_ratio()*coord.cylinder().length();

      coord.crop_mesh_cylindrical(mesh, bot_mesh, 
        bot_z_min, bot_z_max, theta_min, theta_max);

      // write them out
      vcl_string base_name = vul_file::strip_extension(data_file().c_str());
      vcl_stringstream top_stream;
      top_stream << base_name 
        << "_top_width_" << band_width_ratio()
        << "_theta_" << vcl_abs(theta_min_degree) << "_" << vcl_abs(theta_max_degree)
        << ".ply";
      dbmsh3d_save_ply(&top_mesh, top_stream.str().c_str(), false);

      vcl_stringstream bot_stream;
      bot_stream << base_name 
        << "_bot_width_" << band_width_ratio()
        << "_theta_" << vcl_abs(theta_min_degree) << "_" << vcl_abs(theta_max_degree)
        << ".ply";
      dbmsh3d_save_ply(&bot_mesh, bot_stream.str().c_str(), false);
    }  
  }







  // --------------------------------------------------------------------------
  // D. Load a coordinate system and a mesh with thickness attached and 
  // output a table with coordinates of the points (on the cylinder) its thickness 
  // --------------------------------------------------------------------------

  if (thickness2d())
  {
    // load the coordinate system
    dbknee_cylinder_based_coord coord;
    coord.load_from_cs_file(cs_file());

    // i. Load the meshes
    dbmsh3d_richmesh richmesh;

    vcl_cout << "\n\nLoad mesh file = " << data_file() << vcl_endl;

    // list of properties to load
    vcl_vector<vcl_string > vertex_prop_list;
    vertex_prop_list.push_back("verror_abs");

    vcl_vector<vcl_string > face_prop_list;
    face_prop_list.clear();

    ///////////////////////////////
    dbmsh3d_load_ply(&richmesh, data_file().c_str(), vertex_prop_list, face_prop_list);
    ///////////////////////////////

    vcl_cout << "done.\n";
    vcl_cout << "#v[ " << richmesh.num_vertices() 
      << " ], #f[ " << richmesh.facemap().size() << " ]\n";


    // iterate thru the vertices and output their cylindrical coordinates
    // and thickness
    vnl_matrix<double > output_table(richmesh.num_vertices(), 5);
    richmesh.reset_vertex_traversal();
    int vertex_count = 0;
    for (dbmsh3d_vertex* vb=0; richmesh.next_vertex(vb); )
    {
      if (vb->is_a() != "dbmsh3d_richvertex")
      {
        vcl_cerr << "Error: loaded mesh needs to have vertices of type dbmsh3d_richvertex \n";
        vcl_cerr << "Current vertex type: " << vb->is_a() << vcl_endl;
        vcl_cerr << "Quit now. \n";
        return 1;
      }
      dbmsh3d_richvertex* v = static_cast<dbmsh3d_richvertex*>(vb);

      double thickness = -1;
      v->get_scalar_property("verror_abs", thickness);
      assert(thickness>=0);

      double radius = 0;
      double theta = 0;
      double z = 0;
      coord.wc_to_local_cyl(v->pt(), radius, theta, z);

      output_table(vertex_count, 0) = v->id();
      output_table(vertex_count, 1) = radius;
      output_table(vertex_count, 2) = theta;
      output_table(vertex_count, 3) = z;
      output_table(vertex_count, 4) = thickness;
      ++vertex_count;
    }      

    // write output table to a file
    vcl_fstream outfile(output_file().c_str(), vcl_ios_out);
    outfile << "id radius theta z thickness\n";
    output_table.print(outfile);
    outfile.close();
  }
 

  
  // pure visualization commands

  // view mesh
  if (view_mesh() != "")
  {
    // load the mesh
    dbmsh3d_pro_vis bndvis2; // = new dbmsh3d_pro_vis;
    bndvis2.reset_data();
    bndvis2.reset_mesh();

    bndvis2.load_ply(view_mesh());;
    //bndvis2.set
    vis = bndvis2.vis_mesh(false, true, COLOR_RED);
    _root->addChild(vis);
  }

  // view coordinate system
  if (view_cs() != "")
  {
    // visualize the cylinder
    // load the coordinate system
    dbknee_cylinder_based_coord coord;
    coord.load_from_cs_file(view_cs());

    vgl_cylinder<double > cylinder = coord.cylinder();
    vis = draw_cylinder(cylinder.center(), 
      cylinder.center() + cylinder.length()*cylinder.orientation(),
      (float)cylinder.radius(), color_from_code(COLOR_CYAN), 0.5f);
    _root->addChild (vis);

    // draw the axes of the new coordinate system
    vis = draw_line(coord.origin(), 
      coord.origin()+ cylinder.length()*coord.x_axis(), 
      color_from_code(COLOR_RED));
    _root->addChild(vis);

    vis = draw_line(coord.origin(), 
      coord.origin() + cylinder.length()*coord.y_axis(), 
      color_from_code(COLOR_GREEN));
    _root->addChild(vis);
    
    vis = draw_line(coord.origin(), 
      coord.origin() + cylinder.length()*coord.z_axis(), 
      color_from_code(COLOR_BLUE));
    _root->addChild(vis);

    //// draw the notch point
    //vis = draw_sphere(coord.notch_point(), 1.0f, color_from_code(COLOR_RED));
    //_root->addChild(vis);

    vis = draw_sphere(coord.origin(), 1.0f, color_from_code(COLOR_GREEN));
    _root->addChild(vis);
  }





  // ==========================================================================
  // 4) VIEW RESULTS. 
  // ==========================================================================
  
  if (show_gui() == 0) {
    vcl_cout << "Show gui = " << show_gui() << vcl_endl;
    //: Don't show the GUI Window.
    return RUN_RESULT_SUCCESS;
  }

  // --------------------------------------------------------------------------
  // i. Use SoWinExaminerViewer as our main viewer
  // --------------------------------------------------------------------------
  ApplicationTitle += " -- ";
  ApplicationTitle += data_file();

  fprintf (stderr, "\nmain(): starting the Coin3D GUI...\n");
  
  SoWinExaminerViewer* eviewer = new SoWinExaminerViewer (mainwin);
  ///eviewer->setTransparencyType(SoGLRenderAction::ADD);
  eviewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  eviewer->setSceneGraph(_root);
  eviewer->setTitle(ApplicationTitle.c_str());
  eviewer->setBackgroundColor (color_from_code (bg_colorcode));
  eviewer->show();

  // --------------------------------------------------------------------------
  // ii. Handle HOTKEY & Event Callbacks.
  // --------------------------------------------------------------------------

  SoEventCallback * cb = new SoEventCallback;
  //Here, pass the main SoWinExaminerViewer* object as parameter.
  cb->addEventCallback(SoKeyboardEvent::getClassTypeId(), eventcb, eviewer);
  _root->insertChild(cb, 0);

  // --------------------------------------------------------------------------
  // iii. Initialize the camera viewpoint
  // --------------------------------------------------------------------------
  
  //SoCamera *camera = eviewer->getCamera();
  //camera->orientation.setValue( SbRotation(SbVec3f(1, 0, 0), (M_PI/3.0f)) *
  //                              SbRotation(SbVec3f(0, 0, 1), (M_PI/4.0f)) );

  
  // --------------------------------------------------------------------------
  // iv. Show the XYZ Axis Cross.
  // --------------------------------------------------------------------------
  eviewer->setFeedbackVisibility(TRUE);
  
  

  // --------------------------------------------------------------------------
  // v. Pop up the main window. Loop until exit.
  // --------------------------------------------------------------------------
  SoWin::show(mainwin);
  SoWin::mainLoop();
    
  // ==========================================================================
  // 5) Clean up resources. Exit.
  // ==========================================================================
  _root->unref();
  return RUN_RESULT_SUCCESS;

}


