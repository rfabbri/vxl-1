//: 
// \file     proc_det_filter_stats_example.cxx
// \brief    generates some statistical values on a given response file.
//            the response file's name is entered from the gui. And the 
//            statistics written to the file named xx_stats.txt and E values
//            into xx_E.txt if the given response file name is xx.txt
// \author   Gamze Tunali
// \date     2005-10-01
// 

#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <io/proc_io_run_xml_parser.h>

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

int main(int argc, char* argv[]) {
  vgui::init(argc, argv);

  vgui_dialog dlg("Load The Response File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static vcl_string fname = "*.txt";
  static vcl_string ext = "*.*";
  dlg.file("Response Filename:", ext, fname);
  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    vcl_FILE *xmlFile;
   // vcl_string filename = "C:\\Documents and Settings\\gamze\\My Documents\\LEMS\\filter\\XML\\FilterHeader.xml";
    //track_info info;
    proc_io_run_xml_parser parser;
    //unused variable int depth = 0;

    if (  fname == ""){
     vcl_cout << "File not specified" << vcl_endl;
 
    return(1);
  }
  xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile){
    fprintf(stderr, " %s error on opening", fname.c_str() );
    return(1);
  }
  if (!parser.parseFile(xmlFile)) {
    fprintf(stderr,
      "%s at line %d\n",
      XML_ErrorString(parser.XML_GetErrorCode()),
      parser.XML_GetCurrentLineNumber()
      );
     return 1;
   }
   vcl_cout << "finished!" << vcl_endl;

  }
}

