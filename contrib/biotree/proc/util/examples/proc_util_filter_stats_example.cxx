//: 
// \file     proc_det_filter_stats_example.cxx
// \brief    generates some statistical values on a given response file.
//            the response file's name is entered from the gui. And the 
//            statistics written to the file named xx_stats.txt and E values
//            into xx_E.txt if the given response file name is xx.txt
// \author   Gamze Tunali
// \date     2005-10-01
// 

#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <util/proc_det_filter_utils.h>
#include <util/proc_filter_stats.h>
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

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
    proc_det_filter_utils utils;
    proc_filter_stats stats;
    int dimx, dimy, dimz, filter_size;

    vcl_vector<xmvg_filter_response<double> > responses;
    responses = utils.read_responses(fname, dimx, dimy, dimz, filter_size);
    
    //append _stats before .txt to the file name as the file to write the results
    vcl_string out_fname;
    out_fname.assign(fname, 0, fname.find_first_of("."));
    out_fname += "_stats.txt";
    FILE *fp = vcl_fopen(out_fname.data(), "w");

    vcl_fprintf(fp,"%s\n", fname.data());
    // find the mean values
    xmvg_filter_response<double> max_resp(filter_size, 0.0);
    stats.max(responses, max_resp);
    vcl_cout << "MAX_VALUES---------->" << vcl_endl;
    vcl_fprintf(fp,"\n MAX_VALUES-------->\n");
    for (int j=0; j < filter_size; j++) {
      vcl_cout << max_resp[j] << "  ";
      vcl_fprintf(fp,"%lf\t", max_resp[j]);
    }
    xmvg_filter_response<double> mean_resp(filter_size, 0.0);
    stats.mean(responses, mean_resp);
    vcl_cout << "MEAN---------->" << vcl_endl;
    vcl_fprintf(fp,"\n MEAN-------->\n");
    for (int j=0; j < filter_size; j++) {
      vcl_cout << mean_resp[j] << " ";
      vcl_fprintf(fp,"%lf\t", mean_resp[j]);
    }

  // write the normalized values
  /* mean_resp.normalize();
  vcl_cout << "Normalized MEAN---------->" << vcl_endl;
  vcl_fprintf(fp,"\n NORMALIZED MEAN-------->\n");
  for (int j=0; j < filter_size; j++) {
      vcl_cout << mean_resp[j] << " ";
      vcl_fprintf(fp,"%lf\t", mean_resp[j]);
    }
  */
  vcl_cout << vcl_endl << "COVARIANCE-------->" << vcl_endl;
  vcl_fprintf(fp,"\n\n COVARIANCE-------->\n");
  vnl_matrix<double> covar_matrix;
  stats.covar(responses, covar_matrix);
  
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      vcl_fprintf(fp, "%lf\t", covar_matrix[i][j]);
    }
    vcl_fprintf(fp, "\n");
  }

  double max_covar = covar_matrix.max_value();
  covar_matrix /= max_covar;
  vcl_fprintf(fp,"\n MAX COVARIANCE-------->\n");
  vcl_fprintf(fp, "%lf\n\n", max_covar);

  vcl_fprintf(fp,"\n DIAGONAL COVARIANCE-------->\n");
  for (int i=0; i<filter_size; i++) {
    vcl_fprintf(fp, "%lf\t", covar_matrix[i][i]);
    vcl_cout << covar_matrix[i][i] << " ";
  }
  vcl_fprintf(fp, "\n");
  vcl_cout << vcl_endl;

  vcl_cout << vcl_endl << "NORMALIZED COVARIANCE-------->" << vcl_endl;
  vcl_fprintf(fp,"\n NORMALIZED COVARIANCE-------->\n");
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      vcl_fprintf(fp, "%lf\t", covar_matrix[i][j]);
      vcl_cout << covar_matrix[i][j] << " ";
    }
    vcl_fprintf(fp, "\n");
    vcl_cout << vcl_endl;
  }

  vcl_cout << vcl_endl << "COVARIANCE COEFFICIENT-------->" << vcl_endl;
  vcl_fprintf(fp,"\n COVARIANCE COEFFICIENT-------->\n");
  for (int i=0; i<filter_size; i++) {
    for (int j=0; j<filter_size; j++) {
      double x = covar_matrix[i][j] / sqrt(covar_matrix[i][i]*covar_matrix[j][j]);
      vcl_fprintf(fp, "%lf\t", x);
      vcl_cout << x << " ";
    }
    vcl_fprintf(fp, "\n");
    vcl_cout << vcl_endl;
  }

  // append _E before .txt
  vcl_string E_fname;
  E_fname.assign(fname, 0, fname.find_first_of("."));
  E_fname += "_E.txt";
  double max = stats.noise_threshold(responses, covar_matrix, max_covar, mean_resp,
   E_fname);
  vcl_fprintf(fp, "\nmax E=%lf\n", max);

  vcl_fclose(fp);
  }
}

