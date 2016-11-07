//:
// \brief An executable to plot functions
// \file
// \author Isabel Restrepo
// \date 9-Dec-2010

#include <QApplication>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

#include <dbrec3d/gui/dbrec3d_line_plot.h>

int main(int argc, char** argv)
{
  vul_arg<vcl_string> y_file("-y_file", "filename of y values", "");

  vul_arg_parse(argc, argv);
  
  vcl_ifstream y_ofs(y_file().c_str());
  if(!y_ofs)
  {
    vcl_cerr << "Y values file is not valid" << vcl_endl;
    return -1;
  }
  vnl_vector<double> y_vals;
  y_ofs >> y_vals;

  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_line_plot *line_plot = new dbrec3d_line_plot(y_vals.data_block(), y_vals.size());
  
  line_plot->show();
  
  // Run main loop.
  return application.exec();
}


