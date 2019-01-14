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
#include <util/proc_det_filter_utils.h>
#include <util/proc_filter_stats.h>
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
    proc_det_filter_utils utils;
    proc_filter_stats stats;
    int dimx, dimy, dimz, filter_size;

    std::vector<xmvg_filter_response<double> > responses;
    responses = utils.read_responses(fname, dimx, dimy, dimz, filter_size);
    
    //append _stats before .txt to the file name as the file to write the results
    std::string out_fname;
    out_fname.assign(fname, 0, fname.find_first_of("."));
    out_fname += "_stats.txt";
    FILE *fp = std::fopen(out_fname.data(), "w");

    std::fprintf(fp,"%s\n", fname.data());
    // find the mean values
    xmvg_filter_response<double> max_resp(filter_size, 0.0);
    stats.max(responses, max_resp);
    std::cout << "MAX_VALUES---------->" << std::endl;
    std::fprintf(fp,"\n MAX_VALUES-------->\n");
    for (int j=0; j < filter_size; j++) {
      std::cout << max_resp[j] << "  ";
      std::fprintf(fp,"%lf\t", max_resp[j]);
    }
    xmvg_filter_response<double> mean_resp(filter_size, 0.0);
    stats.mean(responses, mean_resp);
    std::cout << "MEAN---------->" << std::endl;
    std::fprintf(fp,"\n MEAN-------->\n");
    for (int j=0; j < filter_size; j++) {
      std::cout << mean_resp[j] << " ";
      std::fprintf(fp,"%lf\t", mean_resp[j]);
    }

  // write the normalized values
  /* mean_resp.normalize();
  std::cout << "Normalized MEAN---------->" << std::endl;
  std::fprintf(fp,"\n NORMALIZED MEAN-------->\n");
  for (int j=0; j < filter_size; j++) {
      std::cout << mean_resp[j] << " ";
      std::fprintf(fp,"%lf\t", mean_resp[j]);
    }
  */
  std::cout << std::endl << "COVARIANCE-------->" << std::endl;
  std::fprintf(fp,"\n\n COVARIANCE-------->\n");
  vnl_matrix<double> covar_matrix;
  stats.covar(responses, covar_matrix);
  
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      std::fprintf(fp, "%lf\t", covar_matrix[i][j]);
    }
    std::fprintf(fp, "\n");
  }

  double max_covar = covar_matrix.max_value();
  covar_matrix /= max_covar;
  std::fprintf(fp,"\n MAX COVARIANCE-------->\n");
  std::fprintf(fp, "%lf\n\n", max_covar);

  std::fprintf(fp,"\n DIAGONAL COVARIANCE-------->\n");
  for (int i=0; i<filter_size; i++) {
    std::fprintf(fp, "%lf\t", covar_matrix[i][i]);
    std::cout << covar_matrix[i][i] << " ";
  }
  std::fprintf(fp, "\n");
  std::cout << std::endl;

  std::cout << std::endl << "NORMALIZED COVARIANCE-------->" << std::endl;
  std::fprintf(fp,"\n NORMALIZED COVARIANCE-------->\n");
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      std::fprintf(fp, "%lf\t", covar_matrix[i][j]);
      std::cout << covar_matrix[i][j] << " ";
    }
    std::fprintf(fp, "\n");
    std::cout << std::endl;
  }

  std::cout << std::endl << "COVARIANCE COEFFICIENT-------->" << std::endl;
  std::fprintf(fp,"\n COVARIANCE COEFFICIENT-------->\n");
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      double x = covar_matrix[i][j] / sqrt(covar_matrix[i][i]*covar_matrix[j][j]);
      std::fprintf(fp, "%lf\t", x);
      std::cout << x << " ";
    }
    std::fprintf(fp, "\n");
    std::cout << std::endl;
  }

  // append _E before .txt
  std::string E_fname;
  E_fname.assign(fname, 0, fname.find_first_of("."));
  E_fname += "_E.txt";
  double max = stats.noise_threshold(responses, covar_matrix, max_covar, mean_resp,
   E_fname);
  std::fprintf(fp, "\nmax E=%lf\n", max);

  std::fclose(fp);
  }
}

