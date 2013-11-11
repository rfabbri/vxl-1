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

#include <vgui/vgui_dialog.h>
#include <vcl_cstddef.h>



int main(int narg, char** argv) {

  static vcl_string fname = "*.vol";
  static vcl_string ext = "*.*";

//find filename
  vgui::init(narg, argv);
  if(narg==2)
    fname=argv[1];
  else {

  vgui_dialog dlg("Load VOL File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  dlg.file("vol Filename:", ext, fname);

  if (!dlg.ask())
    return 0;
  else
  {
//    vgui::quit();
    vcl_size_t dot_pos = fname.find_first_of(".");
    if(vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "vol") != 0 && vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "VOL") != 0)
    {
      vcl_cout << "**************************************" << vcl_endl;
      vcl_cout << "The file does not have a vol extension" << vcl_endl;
      vcl_cout << "**************************************" << vcl_endl;
      return 0;
    }
   }
//      vgui::quit();
   }


//    READ FILE

//initialize vgui
   bgui3d_init();
   SoVolumeRendering::init();

//make scene containing camera and light
   SoSeparator *root = new SoSeparator;
   root->ref();

//define and fill variables
   SbBox3f file_size;
   SoVolumeData::DataType file_type;
   SbVec3s file_dim;

   SoVRVolFileReader* filereader = new SoVRVolFileReader();
   filereader->setUserData((char*) fname.c_str());
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
   vgui_shell_tableau_new shell(tab3d);
   vgui::run(shell, 400, 400);
   root->unref();
}
