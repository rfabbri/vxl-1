#include <QApplication>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

#include <dbrec3d/gui/dbrec3d_line_plot.h>

int main(int argc, char** argv)
{
  vul_arg<vcl_string> curve_file("-curve", "curve filename", "");
  vul_arg_parse(argc, argv);
  
  
  vcl_ifstream vec_ofs(curve_file().c_str());
  vnl_vector<double> curve;
  vec_ofs >> curve;
  
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_line_plot *line_plot = new dbrec3d_line_plot(curve.data_block(), curve.size());
  
  line_plot->show();
  
  // Run main loop.
  return application.exec();
}
