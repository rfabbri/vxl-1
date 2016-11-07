//: 
// \file      det_detect_cylinder_example.cxx
// \brief     example program of finding cylinder on icosahedron filter responses 
// \          of hair. It writes some data into the folder where the responses are 
//\           saved.
// \author    Gamze D. Tunali 
// \date      2005-09-20
// 

#include <xmvg/xmvg_icosafilter_response.h>
#include <det/det_cylinder_detect.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <det/det_cylinder_map.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <util/proc_det_filter_utils.h>
#include <io/proc_io_filter_xml_parser.h>

int main(int argc, char *argv[])
{
  // read covariance matrix from txt file
  FILE* cov_file = fopen("C:\\test_images\\filters\\june8_negatives\\noise\\noise_cov.txt", "r");
  int cov_rows=6, cov_cols=6;

  vnl_matrix<double> covar_matrix(cov_rows, cov_cols ,0.0);
  for (int i=0; i < cov_rows; i++) {
    for (int j=0; j < cov_cols; j++) {
      double val;
      fscanf(cov_file, "%lf\t", &val);
      covar_matrix[i][j] = val;
    }
    fscanf(cov_file, "\n");
  }

  vnl_symmetric_eigensystem<double> eign_solver(covar_matrix);
  double i0 = eign_solver.get_eigenvalue(0);

  double i1 = eign_solver.get_eigenvalue(1);
  double i2 = eign_solver.get_eigenvalue(2);
  double i3 = eign_solver.get_eigenvalue(3);
  double i4 = eign_solver.get_eigenvalue(4);
  double i5 = eign_solver.get_eigenvalue(5);
  vnl_matrix<double> C_inv = vnl_matrix_inverse<double>(covar_matrix);

  //double max_covar = 25124510453052.109000;
  double max_covar;
  fscanf(cov_file, "%lf\n", &max_covar);

  xmvg_icosafilter_response mean(6, 0.0);
  for (int i=0; i < cov_rows; i++) {
    double val;
    fscanf(cov_file, "%lf\t", &val);
    mean[i] = val;
  }

  // test the reading responses
  FILE *fp, *fp_viz, *fp_cyl, *fp_cos, *fp_nonmax;
  int dimx, dimy, dimz, filter_size;

  proc_det_filter_utils utils;

  vcl_string folder = "C:\\test_images\\filters\\aug8_straight_and_angled_hairs\\hair_negatives_XML\\";//; bigger_box20x20x20\\";

  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  dimx = parser.dim_x();
  dimy = parser.dim_y();
  dimz = parser.dim_z();
  filter_size = parser.filter_num();
  vcl_vector<xmvg_filter_response<double> > responses = parser.responses();
  vcl_vector<vgl_vector_3d<double> > orientations = parser.filter_orientations();


  //responses = utils.read_responses(folder + "responses_zfilter_gauss_box1_bigger_rad40L160res10.txt",// "icosa_gaussian_parallel_r40_h160_res15.txt", 
   // dimx, dimy, dimz, filter_size);

  fp = fopen((folder + "filter_resp.txt").data(),"w");
  fp_viz = fopen((folder + "ratios.txt").data(),"w");
  fp_cyl = fopen((folder + "cyl.txt").data(),"w");
  fp_cos = fopen((folder + "cosalphas.txt").data(),"w");
  fp_nonmax = fopen((folder + "non_max_sup.txt").data(),"w");
  fprintf(fp_viz,"%d %d %d %d\n", dimx, dimy, dimz, filter_size);
  fprintf(fp_cyl,"%d %d %d %d\n", dimx, dimy, dimz, filter_size);
  fprintf(fp,"%d %d %d %d\n", dimx, dimy, dimz, filter_size);
  fprintf(fp_nonmax,"%d %d %d %d\n", dimx, dimy, dimz, filter_size);
  vcl_vector<int> counts(6, 0);
  det_cylinder_detect detector;

  double min_E = 1e29; //10000000000.0;
  for (int i=0; i < dimz; i++) {
    for (int j=0; j < dimy; j++) {
      //if (i==6) fprintf(fp_viz,"{");
       for (int k=0; k < dimx; k++) {
          int index = dimy * dimx * i + dimx * j + k;
          xmvg_icosafilter_response ptr= responses[index];
          double E;

          cme_t type = detector.det_type(ptr, orientations);
          
          if (((j >= 4) && (j <=7)) && ((k >= 4) && (k <=7))) {
            if (min_E > E) min_E = E;
          }
          //if (i == 6) fprintf(fp_viz,"%lf,", E);
          fprintf(fp_viz,"%lf ", E);
          //if (type == CYLINDER) {
          fprintf(fp,"\nz=%d y=%d x=%d E=%.3lf|| ", i, j, k, E);   
          for (int f=0; f < filter_size; f++) {
             double resp = ptr.get(f);
             fprintf(fp,"%lf ", resp);
             //fprintf(fp_viz,"1 ");
           }
           if (type.strength_ > 0) {// CYLINDER) 
             fprintf(fp," [CYLINDER]\n");
             fprintf(fp_cyl,"%lf ", E);
             fprintf(fp_cos,"\n---> %lf %lf %lf-->\n", type.dir_.x(), type.dir_.y(), type.dir_.z());   
          }
          else {
            fprintf(fp," [NOISE]\n");
            fprintf(fp_cyl,"0.0 ");
            //fprintf(fp_cos,"noise ");
          }
          
       }
        //if (i == 6) fprintf(fp_viz,"},\n");
        fprintf(fp_viz,"\n");
        fprintf(fp_cyl,"\n");
    }
    fprintf(fp_viz,"\n");
    fprintf(fp_cyl,"\n");
  }
  for (unsigned i=0; i<counts.size(); i++)
    fprintf(fp_cos,"%d: %d\n", i, counts[i]);
  vcl_cout << "MIN E=" << min_E << vcl_endl;

}
