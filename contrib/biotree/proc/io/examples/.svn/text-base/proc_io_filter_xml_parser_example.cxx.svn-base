//: 
// \file     proc_io_filter_parser_example.cxx
// \brief    creates a parser to read filter response file and writes out some 
// \         filter values
// \author   Gamze Tunali
// \date     2005-10-01
// 

#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <io/proc_io_filter_xml_parser.h>
#include <xmvg/xmvg_filter_response.h>
#include <splr/splr_pizza_slice_symmetry.h>
#include <splr/splr_symmetry.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
using namespace std;
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
    proc_io_filter_xml_parser parser;
    parse(fname, parser);

    // write some example filter results
    vcl_vector<xmvg_filter_response<double> > responses = parser.responses();
    xmvg_composite_filter_descriptor d = parser.composite_filter_descr();

    // retrieve y and z values from the arguments
    int z, filter_index=0;
    if (argc < 4) {
      vcl_cout << "Usage: " << argv[0] << " z filter_num fname " << vcl_endl;
      return 1;
    }

    // minus 1 is due to the index starting from 0
    //x = atoi(argv[1])-1;
    z = atoi(argv[1])-1;
    filter_index = atoi(argv[2])-1;
    vcl_string fname = argv[3];
    //unused variable int filter_num = parser.filter_num();
    int dimx = parser.dim_x();
    int dimy = parser.dim_y();
    int dimz = parser.dim_z();
    vcl_ofstream s(fname.c_str());
    

    // hold x and z and change y while writing

    // first write a row of filter names
    /*s << " " << "\t";
    for (int i=0; i < filter_num; i++) {
      s << "filter" << i+1 << "\t";
    }
    s << "\n";

    // write the rows of filter values for each y
    for (int y=0; y < dimy ; y++) {
      s << "y=" << y << "\t";
      for (int i=0; i < filter_num; i++) {
        int voxel_index = dimy*dimx*z + dimx*y + x;
        s << responses[voxel_index][i] << "\t";
      }
      s << "\n";
    }*/
  
  //write out the the filter values for a given z slice (x and y varies)
  s << "{";
  for (int x=0; x<dimz; x++) {
    s << "{";
    for (int y=0; y < dimy ; y++) {
     // for (int i=0; i < filter_num; i++) {
        int voxel_index = dimy*dimx*z + dimx*y + x;
        double d = responses[voxel_index][filter_index];
        s  << fixed << d ;//<< " ";
     // }
      if (y!=dimy-1)
        s << ", " ;
  
    }
    //s << "\n";
    s << "}";
  if (z!=dimz-1)
     s << ",\n" ;
  }
  s << "}\n";
  }
}

