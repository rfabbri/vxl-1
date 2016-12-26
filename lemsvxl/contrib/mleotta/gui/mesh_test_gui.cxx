#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_rotation_matrix.h> 

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>


#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <imesh/algo/imesh_project.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

#include <Inventor/nodes/SoMaterialBinding.h> 
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <vgui/vgui_deck_tableau.h>

// This is a very simple example of how to use the
// basic bgui3d_project2d_tableau to render an Open Inventor
// scene graph with a 3x4 camera matrix onto an image plane
// compatible with vgui_viewer2d_tableau.
//

SoNode* build_SoNode(const imesh_mesh& mesh)
{
  SoSeparator *root = new SoSeparator;
  root->ref();

  //root->addChild(mesh_xform_);

  SoSeparator *group = new SoSeparator;
  root->addChild(group);
  
  SoShapeHints * hints = new SoShapeHints;
  group->addChild( hints );
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->shapeType = SoShapeHints::SOLID;
  hints->faceType = SoShapeHints::CONVEX;


  // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
  myMaterial->transparency.setValue(0.5);
  group->addChild(myMaterial);

  // coordset
  SoCoordinate3 * coords = new SoCoordinate3;
  group->addChild( coords );
  typedef imesh_vertex_array<3>::const_iterator vitr;
  const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();
  unsigned int idx = 0;
  for(vitr v = verts3d.begin(); v!=verts3d.end(); ++v)
  {
    coords->point.set1Value(idx++, SbVec3f((*v)[0], (*v)[1], (*v)[2]));
  }


  // indexed face set
  SoIndexedFaceSet * ifs = new SoIndexedFaceSet;
  group->addChild( ifs );
  const imesh_face_array_base& faces = mesh.faces();
  idx = 0;
  for(unsigned int f=0; f<faces.size(); ++f)
  {
    for(unsigned int i=0; i<faces.num_verts(f); ++i)
      ifs->coordIndex.set1Value(idx++, faces(f,i));
    ifs->coordIndex.set1Value(idx++, -1);
  }

  return root;
}






int main(int argc, char** argv)
{
  if(argc <4){
    vcl_cout << "Please specify the camera, image, and mesh" << vcl_endl;
    return -1;
  }
  vcl_string camera_file(argv[1]);
  vcl_string image_file(argv[2]);
  vcl_string mesh_file(argv[3]);
  
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();

  imesh_mesh v_mesh;
  if(!imesh_read_ply2(mesh_file,v_mesh))
    return -1;

  vcl_cout << "loaded " << mesh_file << vcl_endl;

  SoNode* obj = build_SoNode(v_mesh);
  root->addChild(obj);
  obj->unref();

  vnl_double_3x4 camera;
  vcl_fstream fh(camera_file.c_str());
  fh >> camera;
  fh.close();

//#if 0
  vpgl_perspective_camera<double> P;
  vpgl_perspective_decomposition(camera, P);
  vcl_cout << "K = \n"<<P.get_calibration().get_matrix() << vcl_endl;
  vcl_cout << "R = \n" << P.get_rotation().as_matrix() << vcl_endl;
  vcl_cout << "c = \n" << P.get_camera_center() << vcl_endl;
//#endif
  
  vil_image_resource_sptr image = vil_load_image_resource(image_file.c_str());

  
  vcl_cout << camera << vcl_endl;

  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();

  vgui_deck_tableau_sptr tab3d = vgui_deck_tableau_new();
  tab3d->add(exam_tab);
  tab3d->add(proj_tab);

  root->unref();

  // add an image tableau
  vgui_image_tableau_new img_tab(image);

  // add a composite tableau
  vgui_composite_tableau_new comp(img_tab, tab3d);

  // add a 2D viewer
  vgui_viewer2D_tableau_new viewer(comp);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->ni()+20, image->nj()+20);
}
