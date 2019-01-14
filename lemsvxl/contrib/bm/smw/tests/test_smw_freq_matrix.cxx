//this is contrib/bm/smw/tests/test_smw_freq_matrix.cxx
#include"smw_freq_matrix.h"
#include<testlib/testlib_test.h>
#include<iostream>
static void test_smw_freq_matrix()
{
    //create a freq_matrix
    typedef smw_freq_matrix<unsigned> freq_matrix;
    freq_matrix a(5,5); //create 5x5 matrix

    for(unsigned r = 0; r < a.rows(); ++r)
        for(unsigned c = 0; c < a.cols(); ++c)
            a[r][c] = r+c;
      

    //output original matrix
    std::cout << "Original Matrix: " << std::endl;
    a.print(std::cout);
    
    //add a row and print
    std::cout << '\n' << "Matrix with row added" << std::endl;
    a.add_row();
    a.print(std::cout);

    //add a column and print
    std::cout << '\n' << "Matrix with column added" << std::endl;
    a.add_column();
    a.print(std::cout);

    //remove the third column (index 2)
    std::cout << '\n' << "Matrix with third column deleted" << std::endl;
    a.remove_col(2);
    a.print(std::cout);

    //try to delete a column past the last in the current matrix
    std::cout << '\n' << "Attempt to remove a column past" 
             << " the last in the matrix. " << std::endl;
    a.remove_col(10);
    a.print(std::cout);

    //remove a row 4 (index 3)
    std::cout << '\n' << "Matrix with 4th row deleted" << std::endl;
    a.remove_row(3);
    a.print(std::cout);

    //try to delete a row past the last
    std::cout << '\n';
    a.remove_row(20);
    a.print(std::cout);

    //test assignment operator
    std::cout << '\n' << "Assign 9 to bottom right corner" << std::endl;
    a[4][4] = 9;
    a.print(std::cout);

    //try assigning out of bounds
    std::cout << '\n' << "Try assigning element out of bounds" << std::endl;
    a[5][3] = 10;

    
}


TESTMAIN( test_smw_freq_matrix )
