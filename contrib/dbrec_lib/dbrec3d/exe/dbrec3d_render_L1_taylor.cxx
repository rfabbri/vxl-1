//:
// \file  An executable to render the means of Taylor clusters
// \author Isabel Restrepo
// \date 27-Sept-2011

#include <dbrec3d/gui/vtk/dbrec3d_kernel_viewer.h>

#include <QApplication>
#include <QVTKWidget.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>

#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>


int main(int argc, char** argv) 
{
  vul_arg<vcl_string> taylor_basis_path("-taylor_basis_path", "file to principal components' matrix ", "");
  vul_arg<vcl_string> k_means_file("-k_means_file", "file to k-means", "");
  
  vul_arg<float> tf_min("-tf_min", "minimum value of opacity function", 0.0);
  vul_arg<float> tf_max("-tf_max", "maximum value of opacity function", 0.1);
  
  vul_arg_parse(argc, argv);
  
  //Load matrices 
  
  vnl_matrix<double> basis(125,10);
  
  vcl_string kernel_names[]={"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz"};
  for (unsigned ki = 0; ki<10; ki++) {
    vcl_string filename = taylor_basis_path() + "/" + kernel_names[ki] + ".txt";
    vcl_cout << "Reading kernel file : " << filename << vcl_endl;
    vcl_ifstream ifs(filename.c_str());
    if(!ifs.is_open())
      return -1;
    vnl_vector<double> basis_vector(125);
    unsigned i =0;
    while (!ifs.eof())
    {
      double weight;
      ifs >> weight;
      basis_vector[i] = weight;
      i++;
    }
//    if(i!=125){
//      vcl_cerr << "Wrong dimension " << i << "\n";
//      return -1;
//    }
    
    basis.set_column(ki, basis_vector);
  }
  
  
  //read the  means
  vcl_ifstream mean_ifs(k_means_file().c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open mean_ifs: " << k_means_file() <<  "\n";
    return false;
  }
  
  //vcl_vector<vnl_vector_fixed<double,10> > means;
  unsigned num_means;
  mean_ifs >> num_means;
  
  vcl_cout << "Parsing: " << num_means << " means \n";
  vcl_vector<vnl_vector<double> > features_at_means;
  
  for(unsigned i=0; i<num_means; i++){
    vnl_vector_fixed<double,10> mean_coeff;
    mean_ifs >> mean_coeff;
    //means.push_back(mean);
      
    //feature approximation for this mean
    vnl_vector<double> feature_approx = basis*mean_coeff;
    
    features_at_means.push_back(feature_approx);
  }
  
  
  QApplication app(argc, argv);
  dbrec3d_vector_kernel_viewer* grid_widget = new dbrec3d_vector_kernel_viewer(features_at_means, 5, 5, 5, tf_min(), tf_max());
  grid_widget->show();
  
  
  // Run main loop.
  return app.exec();
}