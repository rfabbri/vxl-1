//this is contrib/bm/smw/tests/test_smw_freq_matrix.cxx
#include"smw_freq_matrix.h"
#include<testlib/testlib_test.h>
#include<vcl_iostream.h>
static void test_smw_freq_matrix()
{
    //create a freq_matrix
    typedef smw_freq_matrix<unsigned> freq_matrix;
    freq_matrix a(5,5); //create 5x5 matrix

    for(unsigned r = 0; r < a.rows(); ++r)
        for(unsigned c = 0; c < a.cols(); ++c)
            a[r][c] = r+c;
      

    //output original matrix
    vcl_cout << "Original Matrix: " << vcl_endl;
    a.print(vcl_cout);
    
    //add a row and print
    vcl_cout << '\n' << "Matrix with row added" << vcl_endl;
    a.add_row();
    a.print(vcl_cout);

    //add a column and print
    vcl_cout << '\n' << "Matrix with column added" << vcl_endl;
    a.add_column();
    a.print(vcl_cout);

    //remove the third column (index 2)
    vcl_cout << '\n' << "Matrix with third column deleted" << vcl_endl;
    a.remove_col(2);
    a.print(vcl_cout);

    //try to delete a column past the last in the current matrix
    vcl_cout << '\n' << "Attempt to remove a column past" 
             << " the last in the matrix. " << vcl_endl;
    a.remove_col(10);
    a.print(vcl_cout);

    //remove a row 4 (index 3)
    vcl_cout << '\n' << "Matrix with 4th row deleted" << vcl_endl;
    a.remove_row(3);
    a.print(vcl_cout);

    //try to delete a row past the last
    vcl_cout << '\n';
    a.remove_row(20);
    a.print(vcl_cout);

    //test assignment operator
    vcl_cout << '\n' << "Assign 9 to bottom right corner" << vcl_endl;
    a[4][4] = 9;
    a.print(vcl_cout);

    //try assigning out of bounds
    vcl_cout << '\n' << "Try assigning element out of bounds" << vcl_endl;
    a[5][3] = 10;

    
}


TESTMAIN( test_smw_freq_matrix )
