#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <dbul/dbul_solar_position.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_rotation_matrix.h> 

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

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

void buildBackground(SoGroup *root)
{
  SoSeparator* road = new SoSeparator;
  road->setName("Road");
  root->addChild(road);

  

  float y0 = -66;
  float w1 = 132;
  float w2 = 120;
  float w3 = 132;
  // add road lines
  static float lineVerts[8][3] = {
    { -600, y0,  0 },
    {  600, y0,  0 },
    { -600, y0+w1,  0 },
    {  600, y0+w1,  0 },
    { -600, y0+w1+w2,  0 },
    {  600, y0+w1+w2,  0 },
    { -600, y0+w1+w2+w3,  0 },
    {  600, y0+w1+w2+w3,  0 },
  };

  SoVertexProperty* vertex_list = new SoVertexProperty;
  vertex_list->vertex.setValues(0, 8, lineVerts);

  SoIndexedLineSet* road_lines = new SoIndexedLineSet;
  static int32_t index[12] = {
    0, 1, -1,              // -1 terminates the edge.
    2, 3, -1,
    4, 5, -1,
    6, 7, -1
  };

  road_lines->coordIndex.setValues(0, 12, index);

    // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // green
  road->addChild(myMaterial);
  road->addChild(vertex_list);
  road->addChild(road_lines);
}

// Construct a simple scene
void buildScene(SoGroup *root, SoNode* vehicle)
{
/*
  {
    SoSeparator *group = new SoSeparator;

    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(72.0f, 72.0f, 72.0f);
    myTransform->translation.setValue(0.0f, 0.0f, 0.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
    group->addChild(myMaterial);

    // Add a cone
    group->addChild(new SoCone);

    root->addChild(group);
  }

  {
    SoSeparator *group = new SoSeparator;

    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(48.0f, 48.0f, 48.0f);
    myTransform->translation.setValue(100.0f, 0.0f, 0.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    group->addChild(myMaterial);

    // Add a cone
    group->addChild(new SoCone);
    root->addChild(group);
  }
  */

  // draw background
  buildBackground(root);
  
  
  SoSeparator* bbox = new SoSeparator;
  bbox->setName("Bounding_Box");
  
  
  double alt, az;
  dbul_solar_position(71, 13.5 ,38.70876, -77.15572, alt, az);
  vcl_cout << "altitude = " << alt << "  azimuth = " << az <<vcl_endl;
  az = (360.0 - 16)*3.1415926/180.0 - az;
  //if(az < 0) az += 2*3.1415926;
  vcl_cout << "azimuth is "<< az * 180.0/3.1415926<<" deg -  x:"<<vcl_cos(az)<<"  y:"<<vcl_sin(az)<<vcl_endl;
  double xa = -vcl_cos(az)/vcl_tan(alt), ya = -vcl_sin(az)/vcl_tan(alt);
  
  vcl_cout << "xa:"<<xa<<"  ya:"<<ya<<" 1/tan(alt):"<<1.0/vcl_tan(alt)<<vcl_endl;
  SoTransform* shadow_transform = new SoTransform;
  shadow_transform->setMatrix(SbMatrix(1.0, 0.0, 0.0, 0.0,
                                       0.0, 1.0, 0.0, 0.0,
                                        xa,  ya, 0.01, 0.0,
                                       0.0, 0.0, 0.0, 1.0));

  SoTranslation* translation = new SoTranslation;
  translation->translation.setValue(-8.0, -24.0, 0.0);
  root->addChild(translation);
  

  float width = 72.0;
  float height = 60.0;
  float length = 180.0;
  float dx = -90;
  float dy = -36;
  float dz = 0;
  // add a box
  static float boxVerts[8][3] = {
    { dx,  dy,  dz+height },
    { dx+length,  dy,  dz+height },
    { dx,  dy+width,  dz+height },
    { dx+length,  dy+width,  dz+height },
    { dx,  dy,  dz },
    { dx+length,  dy,  dz },
    { dx,  dy+width,  dz },
    { dx+length,  dy+width,  dz }
  };

  SoVertexProperty* vertex_list = new SoVertexProperty;
  vertex_list->vertex.setValues(0, 8, boxVerts);

  SoIndexedLineSet* box_lines = new SoIndexedLineSet;
  static int32_t cubeIndex[36] = {
    0, 1, -1,              // -1 terminates the edge.
    2, 3, -1,
    0, 2, -1,
    1, 3, -1,
    4, 5, -1,
    6, 7, -1,
    4, 6, -1,
    5, 7, -1,
    0, 4, -1,
    1, 5, -1,
    2, 6, -1,
    3, 7, -1,
  };

  box_lines->coordIndex.setValues(0, 36, cubeIndex);

  bbox->addChild(vertex_list);
  bbox->addChild(box_lines);
  

  
  SoMaterialBinding* material_bind = new SoMaterialBinding;
  material_bind->value = SoMaterialBinding::OVERALL;
  root->addChild(material_bind);
    // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
  root->addChild(myMaterial);

  root->addChild(bbox);
  if(vehicle)
    root->addChild(vehicle);
  
  // Add a red material
  SoMaterial *myMaterial2 = new SoMaterial;
  myMaterial2->diffuseColor.setValue(0.0f, 0.0f, 1.0f);  // blue
  root->addChild(myMaterial2);
  root->addChild(shadow_transform);
  root->addChild(bbox);
  if(vehicle)
    root->addChild(vehicle);
 
}


// make the example image
vil_image_resource_sptr
make_image()
{
  vil_image_view<vxl_byte> image(400, 400, 3);
  for(int i=0; i<400; ++i){
    for(int j=0; j<400; ++j){
      image(i,j,0) = ((i+j)%2)*255;
      image(i,j,1) = ((i/2+j/2)%2)*255;
      image(i,j,2) = ((i/4+j/4)%2)*255;
    }
  }
  return vil_new_image_resource_of_view(image);
}


// Construct the camera
vnl_double_3x4 make_camera()
{
  // The calibration matrix
  vnl_double_3x3 K;
  K[0][0] = 1.2856;  K[0][1] = 0.0;      K[0][2] = 0.0;
  K[1][0] = 0.0;     K[1][1] = 1.2856;   K[1][2] = 0.0; 
  K[2][0] = 0.0;     K[2][1] = 0.0;      K[2][2] = 1.0;
  
  // The rotation about the x axis
  double angle = 3.1415926/4.0;
  vnl_double_3x3 R = vnl_rotation_matrix(angle*(vnl_double_3(1.0, 0.0, 0.0).normalize()));

  // The translation
  vnl_double_3 t(0.0, 10.0, 10.0);
  
  R = R.transpose();

  vnl_double_3x4 C;
  C.update(R);
  C.set_column(3, -R*t);
  vcl_cout << "Camera = \n" <<K*C << vcl_endl;

  return K*C;
}


int main(int argc, char** argv)
{
  if(argc <3){
    vcl_cout << "Please specify the path camera and image" << vcl_endl;
    return -1;
  }
  vcl_string camera_file(argv[1]);
  vcl_string image_file(argv[2]);
  vcl_string vehicle_file = "";
  if(argc > 3)
    vehicle_file = argv[3];
  
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();
  
    // read the file into a scene graph
  SoNode* vehicle = NULL;
  if(vehicle_file != ""){
    vehicle = bgui3d_import_file(vehicle_file);
    if(vehicle)
      vcl_cout << "loaded " << vehicle_file << vcl_endl;
  }
  buildScene(root, vehicle);

  vnl_double_3x4 camera;
  vcl_fstream fh(camera_file.c_str());
  fh >> camera;
  fh.close();
  
  //vnl_double_3x4 camera = make_camera();
  
  vil_image_resource_sptr image = vil_load_image_resource(image_file.c_str());
  
  vnl_double_3x3 A(0.0);
  A[0][2] = image->ni()/2.0;
  A[1][2] = image->nj()/2.0;
  A[0][0] = A[1][1] = 176;
  A[2][2] = 1.0;
  
  camera = A * camera;

  
  vcl_cout << camera << vcl_endl;
  // wrap the scene graph in a bgui3d tableau
  //bgui3d_project2d_tableau_new tab3d(camera, root);
  
  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();

  vgui_deck_tableau_sptr tab3d = vgui_deck_tableau_new();
  tab3d->add(exam_tab);
  tab3d->add(proj_tab);


  root->unref();

  // add an image tableau
  bgui_image_tableau_new img_tab(image);

  // add a composite tableau
  vgui_composite_tableau_new comp(img_tab, tab3d);

  // add a 2D viewer
  vgui_viewer2D_tableau_new viewer(comp);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->ni()+20, image->nj()+20);
}
