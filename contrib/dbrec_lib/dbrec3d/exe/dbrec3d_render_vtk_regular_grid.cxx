//:
// \file
// \author Isabel Restrepo
// \date 10-Nov-2010

#include <dbrec3d/gui/vtk/dbrec3d_pca_kernels_viewer.h>
#include <dbrec3d/gui/vtk/dbrec3d_kernel_viewer.h>
#include <bvpl/bvpl_octree/bvpl_gauss3D_steerable_filters.h> 
#include <QApplication>
#include <QVTKWidget.h>


void generate_debug_matrix(vnl_matrix<double> &mat)
{
  for(unsigned i = 0; i < mat.columns(); i++)
    mat.set_column(i, i*20);
}

int main(int argc, char** argv) 
{
  
//  vnl_matrix<double> mat(60,10);   
//  generate_debug_matrix(mat);
//  vnl_vector<double> vec(10, 10);
//  QApplication app(argc, argv);
//  dbrec3d_pca_kernels_viewer* grid_widget = new dbrec3d_pca_kernels_viewer(mat, vec, 3,4,5, 0,255);
//  grid_widget->show();
  
  bvpl_gauss3D_steerable_filters filters;
  filters.assemble_basis_size_5();
  vcl_vector<vcl_vector<vcl_string> > basis = filters.basis();
  vcl_map<vcl_string, vnl_vector_fixed<float,5> > separable_taps = filters.separable_taps();
  vcl_vector<vcl_string> basis_names = filters.basis_names();
  
  QApplication app(argc, argv);
  dbrec3d_steerable_basis_viewer* grid_widget = new dbrec3d_steerable_basis_viewer(basis, basis_names, separable_taps, -5.0, 5.0);
  grid_widget->show();
  
  
  
  // Run main loop.
  return app.exec();
}