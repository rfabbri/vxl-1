//---------------------------------------------------------------------
// This is contrib/ozge/iv2ras.cxx
//:
// \file
// \brief Model Registration Application
//
// \author
//  O.C. Ozcanli - February 09, 2005
//
// \verbatim
//  Modifications
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_iostream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_tableau.h>

#include <Inventor/SbBasic.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoOffscreenRenderer.h>

#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSphere.h>

#include <Inventor/nodes/SoShapeHints.h>

//image params
const short SCREEN_DPI = 144;
const short IMAGE_RES_WIDTH = 300;
const short IMAGE_RES_HEIGHT= 300;

void usage(char *program)
{
  printf("%s <infile> <outfile>\n",program);
}

#define LEN 1000
#define RES 1000
#define SIZE 1000000    // RES*RES
#define NUM  4000000

//normals for each plane
//static float norms[NUM][3]; //= 
/*{
  {0, 1, 0}
};*/

static float vertices[NUM][3];// =
/*{
  {LEN, 0, LEN}, {LEN, 0,  -LEN}, { -LEN, 0, -LEN}, {-LEN, 0, LEN}//,
};*/

//float **norms;
//float **vertices;

static int numvertices[SIZE];// = {4};
//int *numvertices;

SoSeparator *
makePlane()
{

  //int *numvertices = new int[1];
  for (int i = 0; i<SIZE; i++)
    numvertices[i] = 4;

  //float norms[][] = new float[SIZE][3];
  /*float **norms = (float **) malloc (sizeof(float *)*SIZE);
  for (int i = 0; i<SIZE; i++) 
    norms[i] = (float *) malloc(sizeof(float)*3);
*/

  /*for (int i = 0; i<NUM; i++) {
    norms[i][0] = 0;
    norms[i][1] = 1;
    norms[i][2] = 0;
  }*/

  //float **vertices = new float[SIZE][3];
  /*float **vertices = (float **) malloc (sizeof(float *)*SIZE);
  for (int i = 0; i<SIZE; i++) 
    vertices[i] = (float *) malloc(sizeof(float)*3);
*/
 
  int k = 0;
  for (int i = 0; i<RES; i++)
    for (int j = 0; j<RES; j++) {
      vertices[k][0] = i;     
      vertices[k][1] = 0;
      vertices[k][2] = j;

      vertices[k+1][0] = i+1;     
      vertices[k+1][1] = 0;
      vertices[k+1][2] = j;

      vertices[k+2][0] = i+1;     
      vertices[k+2][1] = 0;
      vertices[k+2][2] = j+1;

      vertices[k+3][0] = i;     
      vertices[k+3][1] = 0;
      vertices[k+3][2] = j+1;
      k = k+4;
    }

  vcl_cout << "k is " << k << " should be " << NUM << "\n";

  SoSeparator *plane = new SoSeparator();
  plane->ref();

  //Define the normals
  /*SoNormal *norm = new SoNormal;
  norm->vector.setValues(0, NUM, norms);
  plane->addChild(norm);
  
  SoNormalBinding *bind = new SoNormalBinding;
  bind->value = SoNormalBinding::PER_VERTEX;
  plane->addChild(bind);
  */

  SoShapeHints *hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  plane->addChild(hints);

  // Define material
  SoMaterial *mat = new SoMaterial;
  mat->diffuseColor.setValue(1, 1, 1);
  plane->addChild(mat);

  // Define coords of vertices
  SoCoordinate3 *coords = new SoCoordinate3;
  coords->point.setValues(0, NUM, vertices);
  plane->addChild(coords);

  //SoTranslation *myTrans = new SoTranslation();
  //myTrans->translation.setValue(-RES,0,-RES);
  //plane->addChild(myTrans);

  // Define the faceset
  SoFaceSet *face = new SoFaceSet;
  face->numVertices.setValues(0, SIZE, numvertices);
  plane->addChild(face);

  plane->unrefNoDelete();
  return plane;
}

int main(int argc, char *argv[])
{

  //outputs in BMP format
  //FILE *fpOut = NULL;

  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  SoInput *mySceneInput = new SoInput();

  SoSeparator *tree = new SoSeparator();
  tree->ref();

  if (argc != 3) { usage(argv[0]); exit(-1); }

  
  //open output
  //fpOut = fopen(argv[2],"wb");

  // add camera
  SoPerspectiveCamera *camera = new SoPerspectiveCamera();
  camera->position.setValue(0, 100, 100);
  //SbVec3f origin(0,0,0);
  //SbVec3f up(0, 1, -1);
  //camera->pointAt(origin, up);

  //SoDirectionalLight *light2 = new SoDirectionalLight();
  //light2->color.setValue(1,1,1);
  //light2->direction.setValue(0,-1,0);

  SoSpotLight *light = new SoSpotLight();
  light->location.setValue(50, 10, 0);
  light->direction.setValue(-1,0,0);

  //SoDirectionalLight *light3 = new SoDirectionalLight();
  //light3->location.setValue(0, 50, 0);
  //light3->direction.setValue(-1,-1,0);
  
  //tree->addChild(light2);
  //tree->addChild(light3);
  tree->addChild(light);
  tree->addChild(camera);


  //open input
  SbBool ok = mySceneInput->openFile(argv[1]);

  if (!ok) {
    return -1;
  }

  // close input and output
  mySceneInput->closeFile();

    //read input
  SoNode* scene = SoDB::readAll(mySceneInput);
  if (scene == NULL) {
    exit(-1);
  }


  //tree->addChild(makePlane());

  //SoSeparator *s = new SoSeparator();
  //s->ref();
  //SoSphere *w = new SoSphere;
  //w->radius.setValue(1000);
  //SoTranslation *trans2 = new SoTranslation();
  //trans2->translation.setValue(0,-1000,0);
  //s->addChild(trans2);
  //s->addChild(w);
  //s->unrefNoDelete();
  //tree->addChild(s);


  //SoTranslation *myTransform = new SoTranslation();
  //Transform->translation.setValue(0,0,0);
  //tree->addChild(myTransform);
  //tree->addChild(new SoCone);
  tree->addChild(scene);


  // Create a viewport to render the scene into.
  SbViewportRegion myViewport;

  myViewport.setWindowSize(IMAGE_RES_WIDTH,IMAGE_RES_HEIGHT);
  myViewport.setPixelsPerInch((float)SCREEN_DPI);

//  SoGetBoundingBoxAction bbox(myViewport);
//  bbox.apply(scene);
//  SbVec3f center = bbox.getCenter();
//  vcl_cout << center[0] << " " << center[1] << " " << center[2] << vcl_endl;
  //myTransform->translation.setValue(-center[0],-center[1],-center[2]);
  //myTransform->center = bbox.getCenter();

  camera->viewAll(tree->getChild(3),myViewport);
  
  // Render the scene
  //SoOffscreenRenderer *myRenderer = new SoOffscreenRenderer(myViewport);
  //SbColor backCol(0,0,1);
  //myRenderer->setBackgroundColor(backCol);
  //if (!myRenderer->render((SoNode *)tree)) {
  //  delete myRenderer;
  //  exit(-1);
  //}

  //write in windows BMP format
  //myRenderer->writeToPostScript(fpOut);

  
  
  //fclose(fpOut);
  
  // wrap the scene graph in a bgui3d tableau
  bgui3d_tableau_new tab3d(tree);
  tree->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
  return 0;

}
