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

#include <string>

void convert_mnist_files(const std::string& path, int cnt, const std::string& label_filename, const std::string& out_path);

void read_stat_file_gamma(const std::string& file, int gamma_interval, int gamma_range, const std::string out_path);
void read_stat_file_d_rho(const std::string& file, int gamma_min, int gamma_max, int gamma_range, int d_interval, int d_range, int rho_interval, int rho_range, const std::string out_path);

