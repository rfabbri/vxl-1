#ifndef SMW_FREQ_MATRIX_H_
#define SMW_FREQ_MATRIX_H_

#include<vnl/vnl_matrix.h>

template<class T>
class smw_freq_matrix : public vnl_matrix<T>
{
public:
    //construct matrix with nrows and  ncols.
    smw_freq_matrix(unsigned const& nrows = 1, unsigned const& ncols = 1):
        vnl_matrix<T>(nrows,ncols) {} 

    //add a row past the last row of the matrix and fill it with zeros
    bool add_row();

    //add a column past the last column of the matrix and fill it with zeros
    bool add_column();

    bool remove_row(unsigned const& row);

    bool remove_col(unsigned const& col);

};

#endif //SMW_FREQ_MATRIX_H_
