//:
// \brief: An executaple to plot different statistics of principal components
// \file
// \author Isabel Restrepo
// \date 10-Dec-2010


#include <QApplication>
//#include <QVTKWidget.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>
#include <dbrec3d/gui/dbrec3d_line_plot.h>


void cumulative_sum(vnl_vector<double> const &v, QVector<double> &cum_sum)
{
  double sum = 0;
  double total_sum = v.sum();
  for(unsigned i = 0; i < v.size(); i++)
  {    
    sum += v[i];
    cum_sum[i] = sum/total_sum;
  }
  
  return;
}

//computes theoretical reconstruction error
void reconstruction_error(vnl_vector<double> const &weights, QVector<double> &error)
{
  double e = 0;
  
  for(unsigned i=0; i<weights.size(); i++)
  {
    error[i] = weights.extract(weights.size() - (i+1), i+1).sum();
  }
  
}

int main(int argc, char** argv) 
{
  vul_arg<vcl_string> weights_file("-weights_vector", "weights or eigen values file", "");
  vul_arg<vcl_string> r_error_file("-test_error_file", "test error file", "");


  vul_arg_parse(argc, argv);
  
  //Load vectors 
  vcl_ifstream weights_stream(weights_file().c_str());
  vnl_vector<double> weights;
  if(weights_stream)
    weights_stream >> weights;
  
  unsigned npc = weights.size();
  
  
  QVector<double> pc_index(npc);
  for(unsigned i=0; i<npc; i++)
    pc_index[i] = i+1; 
  
  QApplication app(argc, argv);

  // Create widget and register plots
  dbrec3d_choose_plot *plots = new dbrec3d_choose_plot;
  
  {
    vcl_vector<double> auxVector;
    auxVector.assign(weights.data_block(), weights.data_block() + npc);
    plots->register_plot(QString("Weights"), pc_index, QVector<double>::fromStdVector(auxVector));
    
    QVector<double> cum_var(npc);
    cumulative_sum(weights, cum_var);
    plots->register_plot(QString("Cum Variance"), pc_index, cum_var);
  }
  

  
  { 
    //t_error = 1.0 - t_error/t_error[0]; 
//    vcl_vector<double> auxVector;
//    auxVector.assign(t_error.data_block(), t_error.data_block() + npc);
//    plots->register_plot(QString("Training Error"), pc_index, QVector<double>::fromStdVector(auxVector));
  }
  
  { 
    QVector<double> theory_error(npc);
    reconstruction_error(weights, theory_error);
    plots->register_plot(QString("Training Error"), pc_index, theory_error);
  }
  
  plots->init();
  plots->show();

  // Run main loop.
  return app.exec();
}