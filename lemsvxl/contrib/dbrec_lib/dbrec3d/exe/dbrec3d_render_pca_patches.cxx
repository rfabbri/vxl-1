//:
// \file  An executaple to read matrices, whose columns represent volume patches of PCA or data
// \author Isabel Restrepo
// \date 19-Nov-2010

#include <dbrec3d/gui/vtk/dbrec3d_pca_kernels_viewer.h>

#include <QApplication>
#include <QVTKWidget.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>

 
int main(int argc, char** argv) 
{
  vul_arg<vcl_string> pc_file("-pc_matrix", "principal components' matrix file", "");
  vul_arg<vcl_string> weights_file("-weights_vector", "weights or eigen values file", "");
  vul_arg<vcl_string> pos_file("-pos_file", "sample positions file", "");
  //vul_arg<vcl_string> t_error_file("-t_error_file", "training error file", "");
  
  vul_arg<float> tf_min("-tf_min", "minimum value of opacity function", 0.0);
  vul_arg<float> tf_max("-tf_max", "maximum value of opacity function", 0.1);

  vul_arg_parse(argc, argv);
  
  //Load matrices 
  vcl_ifstream pc_stream(pc_file().c_str());
  if(!pc_stream) return -1;
  vnl_matrix<double> pc; //zero size  
  pc_stream >> pc;
  
  vcl_ifstream weights_stream(weights_file().c_str());
  if(!weights_stream) return -1;
  vnl_vector<double> weights;
  weights_stream >> weights;
   
  QApplication app(argc, argv);
  dbrec3d_pca_kernels_viewer* grid_widget = new dbrec3d_pca_kernels_viewer(pc,weights,5,5,5, tf_min(), tf_max());
  grid_widget->show();

  
  // Run main loop.
  return app.exec();
}