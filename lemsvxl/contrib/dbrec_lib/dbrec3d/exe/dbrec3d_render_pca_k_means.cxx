//:
// \file  An executaple vender the means of PCA clusters
// \author Isabel Restrepo
// \date 5-April-2011

#include <dbrec3d/gui/vtk/dbrec3d_kernel_viewer.h>

#include <QApplication>
#include <QVTKWidget.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>


int main(int argc, char** argv) 
{
  vul_arg<vcl_string> pc_file("-pc_file", "file to principal components' matrix ", "");
  vul_arg<vcl_string> k_means_file("-k_means_file", "file to k-means", "");
  
  vul_arg<float> tf_min("-tf_min", "minimum value of opacity function", 0.0);
  vul_arg<float> tf_max("-tf_max", "maximum value of opacity function", 0.1);
  
  vul_arg_parse(argc, argv);
  
  //Load matrices 
  vcl_ifstream pc_stream(pc_file().c_str());
  if(!pc_stream) return -1;
  vnl_matrix<double> pc; //zero size  
  pc_stream >> pc;
  
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
    vnl_vector<double> feature_approx  = pc.extract(125, num_means) * mean_coeff;
    features_at_means.push_back(feature_approx);
  }
    
  
  QApplication app(argc, argv);
  dbrec3d_vector_kernel_viewer* grid_widget = new dbrec3d_vector_kernel_viewer(features_at_means, 5, 5, 5, tf_min(), tf_max());
  grid_widget->show();
  
  
  // Run main loop.
  return app.exec();
}