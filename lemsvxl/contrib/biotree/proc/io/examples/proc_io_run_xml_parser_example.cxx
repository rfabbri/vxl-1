//: 
// \file     proc_det_filter_stats_example.cxx
// \brief    generates some statistical values on a given response file.
//            the response file's name is entered from the gui. And the 
//            statistics written to the file named xx_stats.txt and E values
//            into xx_E.txt if the given response file name is xx.txt
// \author   Gamze Tunali
// \date     2005-10-01
// 

#include <cstdio>
#include <vnl/vnl_math.h>
#include <io/proc_io_run_xml_parser.h>

#include <iostream>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

int main(int argc, char* argv[]) {
  vgui::init(argc, argv);

  vgui_dialog dlg("Load The Response File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static std::string fname = "*.txt";
  static std::string ext = "*.*";
  dlg.file("Response Filename:", ext, fname);
  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    std::FILE *xmlFile;
   // std::string filename = "C:\\Documents and Settings\\gamze\\My Documents\\LEMS\\filter\\XML\\FilterHeader.xml";
    //track_info info;
    proc_io_run_xml_parser parser;
    //unused variable int depth = 0;

    if (  fname == ""){
     std::cout << "File not specified" << std::endl;
 
    return(1);
  }
  xmlFile = std::fopen(fname.c_str(), "r");
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
   std::cout << "finished!" << std::endl;

  }
}

