// This is /lemsvxl/contrib/firat/dbul_matlab/dbul_test_matlab.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jun 28, 2010
//

#include<dbul/dbul_matlab.h>
#include<iostream>

int main()
{
//    dbul_matlab m;
//    dbul_matlab_command com("ones");
//    com.add_param(2);
//    com.add_param(2);
//    std::vector<std::string> outargs;
//    outargs.push_back("x");
//    m.run("", com, outargs);
//    vnl_file_matrix<double >* matrix = m.get_matlab_matrix("/home/firat/matlab_temp", "x");
//    std::cout << "C++ says: " << (*matrix)(0,0) << std::endl;
    vnl_file_matrix<double >* matrix = new vnl_file_matrix<double >("/home/firat/matlab_temp/kSAGm1OKQNYDbLZ.txt");
    std::cout << "C++ says: " << (*matrix)(0,0) << std::endl;
    return 0;
}
