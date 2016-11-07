#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_tableau.h>

#include <vcl_iostream.h>

#include <stdio.h>
#include <math.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPerspectiveCamera.h> 
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
//#include <windows.h>
//#include <winbase.h>

void printUsage(char *name) {
        printf("usage: ./%s <model> <X rot in degrees> <Y rot in degrees>\n",
                        name);
        printf("output file model-X-Y.rgb\n");
}

int main (int argc, char *argv[])
{
char temp[100];

if (argc < 4) {
        printUsage(argv[0]);
        return 0;
}

// initialize vgui
  //vgui::init(argc, argv);

  // initialize bgui_3d
  //bgui3d_init();

SoDB::init();
SoInput *mySceneInput = new SoInput();

SoSeparator *tree = new SoSeparator();
SoSeparator *tree2 = new SoSeparator();
tree->ref();
tree2->ref();

//open input
sprintf(temp, "%s.iv", argv[1]);
mySceneInput->openFile(temp);

SoNode *scene = SoDB::readAll(mySceneInput);
if (scene == NULL) {
exit(-1);
}
//create a viewport to render the scene into 
SbViewportRegion myViewport;
myViewport.setWindowSize(500, 500);
myViewport.setPixelsPerInch((float)200);

tree2->addChild(scene);

SoGetBoundingBoxAction bboxAction(myViewport);
bboxAction.apply(tree2);
SbVec3f *c = new SbVec3f(bboxAction.getCenter());
//c->print(stdout);
c->negate();
c->print(stdout);


// rotate 
/*
SoTransform *trans1 = new SoTransform;
SoTransform *trans2 = new SoTransform;
trans1->center.setValue(bboxAction.getCenter());
trans2->center.setValue(bboxAction.getCenter());

// rot1->angle = 2*M_PI*(atof(argv[2])/360.0);  // angle is given in degrees
trans1->rotation.setValue(-1.0, 0.0, 0.0, 2*M_PI*atof(argv[2])/360.0);
trans2->rotation.setValue(0.0, 1.0, 0.0, M_PI*atof(argv[3])/180.0);
//tree->addChild(trans1);
tree->addChild(trans2);
*/

SoRotationXYZ *rot1 = new SoRotationXYZ;
SoRotationXYZ *rot2 = new SoRotationXYZ;
rot1->angle = 2*M_PI*(atof(argv[2])/360.0);  // angle is given in degrees
                                             // convert to radians
rot2->angle = 2*M_PI*(atof(argv[3])/360.0);
printf("rotating by %f degrees about X\n", atof(argv[2]));
printf("rotating by %f degrees about Y\n", atof(argv[3]));
rot1->axis = SoRotationXYZ::X;  // this will usually be zero
rot2->axis = SoRotationXYZ::Y;  // this will be the most common
tree->addChild(rot1);
tree->addChild(rot2);

tree->addChild(scene);


 SbVec3f T=SbVec3f(atof(argv[4]), 0.0f, 0.0f);
*c=*c + T;

c->print(stdout);
// translate
//double T= atof(argv[4]); 
SoTranslation *trans = new SoTranslation;
trans->translation.setValue(*c );

tree->addChild(trans);


//add camera 
SoPerspectiveCamera *camera = new SoPerspectiveCamera();
SoPointLight *light = new SoPointLight();
tree->insertChild(light,0);
tree->insertChild(camera,0);

//camera->viewAll(tree->getChild(5), myViewport);
camera->viewAll(tree2->getChild(0), myViewport);
//camera->viewAll(tree, myViewport);
//camera->pointAt(SbVec3f(0.0, 0.0, 0.0));

// Render the scene
SoOffscreenRenderer *myRenderer = new SoOffscreenRenderer(myViewport);
myRenderer->setBackgroundColor(SbColor(1,1,1));
if (!myRenderer->render((SoNode *)tree)) {
        delete myRenderer;
        printf("not able to render offscreen\n");
        exit(-1);
}
// write output
sprintf(temp, "%s-%03d-%03d.rgb", argv[1], atoi(argv[2]), atoi(argv[3]));
//myRenderer->writeToRGB(temp);
myRenderer->writeToPostScript(argv[5]);
mySceneInput->closeFile();

// wrap the scene graph in a bgui3d tableau
  //bgui3d_tableau_new tab3d(tree);
  //tree->unref();

  // Put a shell tableau at the top of our tableau tree.
  //vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  //return vgui::run(shell, 400, 400);

return 0;

}
