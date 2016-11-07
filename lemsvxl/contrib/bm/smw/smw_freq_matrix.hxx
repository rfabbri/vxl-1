#ifndef smw_freq_histogram_txx_
#define smw_freq_histogram_txx_

#include "smw_freq_matrix.h"
#include <vcl_iostream.h>

//add a row and fill it with zeros
template<class T>
bool smw_freq_matrix<T>::add_row()
{
    unsigned nrows = this->rows() + 1;
    unsigned ncols = this->cols();
    smw_freq_matrix temp(nrows,ncols);
    
    unsigned last_row = this->rows(); //==nrows - 1
    for(unsigned r = 0; r < nrows; ++r)
        for(unsigned c = 0; c < ncols; ++c)
        {
            if( r == last_row )
                temp(r,c) = 0;
            else
                temp(r,c) = this->get(r,c);
        }
    this->set_size(nrows,ncols);
    this->update(temp);
    return true;
}

//add a column and fill it with zeros
template<class T>
bool smw_freq_matrix<T>::add_column()
{
    
    unsigned nrows = this->rows();
    unsigned ncols = (this->cols()) + 1;
    smw_freq_matrix temp(nrows,ncols);
    
    unsigned last_row = this->cols(); //==ncols - 1
    for(unsigned r = 0; r < nrows; ++r)
        for(unsigned c = 0; c < ncols; ++c)
        {
            if(c == last_row)
                temp(r,c) = 0;
            else
                temp(r,c) = this->get(r,c);
                
        }
    this->set_size(nrows,ncols);
    this->update(temp);
    return true;
}

template<class T>
bool smw_freq_matrix<T>::remove_row(unsigned const& row)
{
    if( row > this->rows() )
    {
        vcl_cerr << "Error: smw_freq_matrix<T>::remove_col(unsigned const& col)"
                 << '\n' << "Attempted to remove a column past the last"
                 << " in the matrix" << '\n' << vcl_flush;
        return 0;
    }

    unsigned nrows = this->rows() - 1;
    unsigned ncols = this->cols();
    smw_freq_matrix temp(nrows,ncols);
    
    
    for(unsigned r = 0, t_row = 0; r < this->rows(); ++r)
    {
        if( r != row )
        {
            for(unsigned c = 0; c < ncols; ++c)
                temp(t_row,c) = this->get(r,c);
            ++t_row;
        }
    }
    
    this->set_size(nrows,ncols);
    this->update(temp);
    return true;
}

template<class T>
bool smw_freq_matrix<T>::remove_col(unsigned const& col)
{
    if( col > this->cols() )
    {
        vcl_cerr << "Error: smw_freq_matrix<T>::remove_col(unsigned const& col)"
                 << '\n' << "Attempted to remove a colunm past the last"
                 << " in the matrix " << '\n' << vcl_flush;
        return 0;
    }

    unsigned nrows = this->rows();
    unsigned ncols = this->cols() - 1;
    smw_freq_matrix temp(nrows,ncols);
    
    
    unsigned t_col;
    for(unsigned r = 0; r < this->rows(); ++r)
    {
        t_col = 0;
        for(unsigned c = 0; c < this->cols(); ++c)
            if( c != col )
            {
                temp(r,t_col) = this->get(r,c);
                ++t_col;
            }
    }

    this->set_size(nrows,ncols);
    this->update(temp);
    return true;
}

#undef SMW_FREQ_MATRIX_INSTANTIATE
#define SMW_FREQ_MATRIX_INSTANTIATE(T)\
    template class smw_freq_matrix<T >

#endif //smw_freq_matrix_txx_
