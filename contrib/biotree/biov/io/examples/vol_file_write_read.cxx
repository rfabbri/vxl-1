#include "../vol_file.h"

#include <vil/vil_load.h>

#include <vbl/vbl_array_3d.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <basic/bgui3d/bgui3d.h>
#include <basic/bgui3d/bgui3d_examiner_tableau.h>

#include <VolumeViz/readers/SoVRVolFileReader.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoSeparator.h>


int main(int narg, char** argv) {

//construct a cube
   vbl_array_3d<vxl_uint_8> abc(100,100,100);
   abc.resize(100,100,100);
   
   for (vxl_uint_8 i = 0; i<100; i++)
      for (vxl_uint_8 j = 0; j<100; j++)
         for (vxl_uint_8 k = 0; k<100; k++) {
            if( ((((i>50) && (i<50)) && ((j>40) && (j<60))) && ((k>30) && (k<70)))) //CUBE
               abc[k][j][i] = 255;
            else
               abc[k][j][i] = 0;
            }

//write to file
   vol_file file(abc);
   file.write("test_output.vol");


//    READ FILE

//initialize vgui
   vgui::init(narg, argv);
   bgui3d_init();

//make scene containing camera and light
   SoSeparator *root = new SoSeparator;
   root->ref();
   SoVolumeRendering::init();

//define and fill variables
   SbBox3f file_size;
   SoVolumeData::DataType file_type;
   SbVec3s file_dim;

   SoVRVolFileReader* filereader = new SoVRVolFileReader();
   char *fname ="test_output.vol"; 
   filereader->setUserData(fname);
   filereader->getDataChar(file_size,file_type,file_dim);

   SoVolumeReader *volumereader = filereader;

//Add SoVolumeData to scene graph
   SoVolumeData * volumedata = new SoVolumeData();
   volumedata->setReader(*volumereader);
   root->addChild(volumedata);

//Add TransferFunction (color map) to scene graph
   SoTransferFunction * transfunc = new SoTransferFunction();
//   transfunc->predefColorMap = SoTransferFunction::STANDARD;
   root->addChild(transfunc);
   
//Add VolumeRender to scene graph
   SoVolumeRender * volrend = new SoVolumeRender();
   root->addChild(volrend);

//display scene
   bgui3d_examiner_tableau_new tab3d(root);
   root->unref();
   vgui_shell_tableau_new shell(tab3d);
   int return_value = vgui::run(shell, 400, 400);
}
