//:
// \file
// \brief utilities to run recognition experiments on MNIST data 
//        http://yann.lecun.com/exdb/mnist/
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   Oct 07, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//

#include <vcl_string.h>

void convert_mnist_files(const vcl_string& path, int cnt, const vcl_string& label_filename, const vcl_string& out_path);

void read_stat_file_gamma(const vcl_string& file, int gamma_interval, int gamma_range, const vcl_string out_path);
void read_stat_file_d_rho(const vcl_string& file, int gamma_min, int gamma_max, int gamma_range, int d_interval, int d_range, int rho_interval, int rho_range, const vcl_string out_path);

