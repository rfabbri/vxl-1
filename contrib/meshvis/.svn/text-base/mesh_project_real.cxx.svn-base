#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vcl_cmath.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_file_io.h>

#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_rotation_matrix.h> 

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <vcl_iostream.h>

#include <dbul/dbul_dir_file.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMatrixTransform.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_grid_tableau.h>

// Construct a simple scene
void buildScene(SoGroup *root)
{
  {
    SoSeparator *group = new SoSeparator;
    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    myTransform->translation.setValue(-20.0f, -20.0f, 10.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    group->addChild(myMaterial);

    // Add a cone
    group->addChild(new SoCylinder);

    root->addChild(group);
  }

}

int main(int argc, char** argv)
{
  if (argc != 4) {
    vcl_cout << vcl_endl 
             << " Usage: mesh_project_real mesh.ply2 imagefile camerafile " 
             << vcl_endl;
    return 0;
  }

  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();


  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();
#if 0
   SoTransform *myTransform = new SoTransform;
   myTransform->ref();
   SoTransform *S = new SoTransform;
   S->ref();
  S->scaleFactor.setValue(100.0f, 21.2f, 1.0f);
  SoTransform *R = new SoTransform;
  R->ref();
  R->rotation.setValue(SbVec3f(0.0f,0.0f,1.0f),0.03744f);
  SoTransform *T = new SoTransform;
  T->ref();
  T->translation.setValue(-136.0f, -52.35f, 0.0f);
 
  //myTransform->scaleFactor.setValue(10.0f, 21.2f, 1.0f);
//  myTransform->combineRight(S);
  myTransform->combineRight(T);
  myTransform->combineRight(R);
  //S->unref();
  R->unref();
  T->unref();
  root->addChild(myTransform);
  myTransform->unref();


  //  float c = vcl_cos(0.03744f), s = vcl_sin(0.03744f);
  float c = vcl_cos(-0.01f), s = vcl_sin(-0.01f);
  SbMatrix S,R,T;
  S[0][0] = 1.0f;  S[0][1] = 0.0f; S[0][2] = 0.0f; S[0][3] = 0.0f;
  S[1][0] = 0.0f;  S[1][1] = 1.0; S[1][2] = 0.0f; S[1][3] = 0.0f;
  S[2][0] = 0.0f;  S[2][1] = 0.0f; S[2][2] = 1.0f; S[2][3] =  0.0f;
  S[3][0] = 0.0f;  S[3][1] = 0.0f; S[3][2] = 0.0f; S[3][3] = 1.0f;

  R[0][0] = c;  R[0][1] = s; R[0][2] = 0.0f; R[0][3] = 0.0f;
  R[1][0] = -s;  R[1][1] = c; R[1][2] = 0.0f; R[1][3] = 0.0f;
  R[2][0] = 0.0f;  R[2][1] = 0.0f; R[2][2] = 1.0f; R[2][3] =  0.0f;
  R[3][0] = 0.0f;  R[3][1] = 0.0f; R[3][2] = 0.0f; R[3][3] = 1.0f;

  T[0][0] = 1.0f;  T[0][1] = 0.0f; T[0][2] = 0.0f; T[0][3] = 0.0f;
  T[1][0] = 0.0f;  T[1][1] = 1.0f; T[1][2] = 0.0f; T[1][3] = 0.0f;
  T[2][0] = 0.0f;  T[2][1] = 0.0f; T[2][2] = 1.0f; T[2][3] =  0.0f;
  //T[3][0] = -100.0f;  T[3][1] = -55.0f; T[3][2] = 0.0f; T[3][3] = 1.0f;
  //T[3][0] = -136.0f;  T[3][1] = -52.32f; T[3][2] = 0.0f; T[3][3] = 1.0f;
  //  T[3][0] = -1036.0f;  T[3][1] = -21.15f; T[3][2] = 0.0f; T[3][3] = 1.0f;
  //  T[3][0] = -1036.0f;  T[3][1] = -51.15f; T[3][2] = 0.0f; T[3][3] = 1.0f;
  T[3][0] = 0.0f;  T[3][1] = 0.0f; T[3][2] = 0.0f; T[3][3] = 1.0f;
//for some reason the transform order has to be reversed for the security cam
//  myTransform->setMatrix(T*R);
  //myTransform->setMatrix(S*R*T);
  //  myTransform->setMatrix(T*R);
  myTransform->setMatrix(T);
  root->addChild(myTransform);
  myTransform->unref();
  //  buildScene(root);
#endif
  // #################################################
  // Load and display a .PLY2 mesh file
  dbmsh3d_mesh_mc M;
  vcl_string input_file = argv[1];
  vcl_string suffix = dbul_get_suffix (input_file);
  if (suffix == ".ply2")
    dbmsh3d_load_ply2 (&M, input_file.c_str());
  else if (suffix == ".ply")
    dbmsh3d_load_ply (&M, input_file.c_str());
  else if (suffix == ".off")
    dbmsh3d_load_off (&M, input_file.c_str());
  else if (suffix == ".m")
    dbmsh3d_load_m (&M, input_file.c_str());
  
  M.IFS_to_MHE();
  M.build_face_IFS ();
  merge_mesh_coplanar_faces (&M);
  M.build_face_IFS ();
  M.orient_face_normals();
  M.build_face_IFS ();
  //Draw the whole mesh to a single object
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
  root->addChild(myMaterial);
  root->addChild (draw_M (&M, true)); 
#if 0
    SoNode* n = root->getByName(SbName("boundary_mesh_style"));
    SoDrawStyle* sty = (SoDrawStyle*)n;
    if(sty)
      {
        sty->style=SoDrawStyleElement::LINES;
        sty->lineWidth = 2;
      }

    SoNode* m = root->getByName(SbName("boundary_mesh_light_model"));
    SoLightModel* md = (SoLightModel*)m;
    if(md)
      md->model=SoLightModel::BASE_COLOR;

#endif
  // ################################################
  // Load the image
 vcl_string image_file = argv[2];
    vil_image_resource_sptr image = vil_load_image_resource(image_file.c_str());
    if(!image)
      {
        vcl_cout << "couldn't load image\n";
        return 0;
      }

  // ################################################
  // Load the camera
    vnl_double_3x4 camera;
 vcl_string cam_file = argv[3];
    vcl_ifstream cam_istr(cam_file.c_str());
    cam_istr >> camera;

  // wrap the scene graph in a bgui3d tableau
  //bgui3d_project2d_tableau_new tab3d(camera, root);
  
  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();
  
#if 0
  vgui_deck_tableau_sptr tab3d = vgui_deck_tableau_new();
   tab3d->add(exam_tab);
  tab3d->add(proj_tab);
#endif
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(2,1);
  root->unref();

  // add an image tableau
  vgui_image_tableau_new img_tab(image);

  // add a composite tableau
  vgui_composite_tableau_new comp(img_tab, proj_tab);

  // add a 2D viewer
  vgui_viewer2D_tableau_new viewer(comp);

  // put into grid
  grid->add_at(viewer, 0, 0);
  grid->add_at(exam_tab, 1, 0);
  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 1500, 1000);
}
