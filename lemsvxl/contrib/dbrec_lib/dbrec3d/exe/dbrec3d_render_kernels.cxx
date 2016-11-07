//:
// \file
// \author Isabel Restrepo
// \date 27-Jan-2011


//:
// \file  An executaple to read matrices, whose columns represent volume patches of PCA or data
// \author Isabel Restrepo
// \date 19-Nov-2010

#include <dbrec3d/gui/vtk/dbrec3d_kernel_viewer.h>

#include <QApplication>
#include <QVTKWidget.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>
#include <bvpl/kernels/bvpl_taylor_basis_factory.h>

int main(int argc, char** argv) 
{
  vul_arg<vcl_string> path("-path", "path to taylor kernels", "");
  vul_arg<float> tf_min("-tf_min", "minimum value of opacity function", -0.5);
  vul_arg<float> tf_max("-tf_max", "maximum value of opacity function", 0.5);
  
  vul_arg_parse(argc, argv);
  
  //Load kernels
  bvpl_taylor_basis_loader loader(path());
  vcl_map<vcl_string, bvpl_kernel_sptr> taylor_basis;
  loader.create_basis(taylor_basis);
  
  vcl_map<vcl_string, bvpl_kernel_sptr>::iterator it= taylor_basis.begin();
  it->second->print();
  
  
  QApplication app(argc, argv);
  dbrec3d_kernel_viewer* grid_widget = new dbrec3d_kernel_viewer(taylor_basis, tf_min(), tf_max());
  grid_widget->show();
  
  
  // Run main loop.
  return app.exec();
}