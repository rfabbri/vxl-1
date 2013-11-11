#include <dbsta/algo/dbsta_sinkhorn.h>
#include <vnl/vnl_transpose.h>

template <class T>
dbsta_sinkhorn<T>::dbsta_sinkhorn(vnl_matrix<T> M)
                                  :M_(M)
{
    maxit_=10;
}
template <class T>
dbsta_sinkhorn<T>::dbsta_sinkhorn(vnl_matrix<T> M,vnl_vector<T> m_cols,vnl_vector<T>  m_rows):M_(M),mcols_(m_cols),mrows_(m_rows)
{
    maxit_=10;
}

template <class T>
dbsta_sinkhorn<T>::dbsta_sinkhorn(vnl_matrix<T> M,T moutlier):M_(M),mcols_(M.rows(),moutlier),mrows_(M.cols(),moutlier)
{
    maxit_=10;
}

template <class T>
dbsta_sinkhorn<T>::~dbsta_sinkhorn()
{

}

template < class T > bool dbsta_sinkhorn < T >::
normalize(T tol)
{
    unsigned numrows = M_.rows();
    unsigned numcols = M_.cols();

    if (numrows != mcols_.size() || numcols != mrows_.size()) {
        vcl_cout <<
            "\n Dimension of the outliers and assignment matrix does not match ";
        return false;
    }
    //: computing error 
    vnl_vector < T > tempx(numrows, 1);
    vnl_vector < T > tempy(numcols, 1);

    int numiter = 1;
    bool flag = true;

    T *mcols_data_block = mcols_.data_block();
    T *mrows_data_block = mrows_.data_block();

    while (flag) {

        //: normalizing slacks

        vnl_vector < T > sy_slack = M_ * tempy;
        T *sy_slack_data_block = sy_slack.data_block();
        for (int i = 0; i < static_cast < int >(numrows); i++)
            if (sy_slack_data_block[i] > T(0)) {
                mcols_data_block[i] =
                    mcols_data_block[i] / (sy_slack_data_block[i] +
                                           mcols_data_block[i]);
            }

        //: sum of the columns
        vnl_vector < T > sx_slack = vnl_transpose(M_) * tempx;
        T *sx_slack_data_block = sx_slack.data_block();
        for (int i = 0; i < static_cast < int >(numcols); i++)
            if (sx_slack_data_block[i] > T(0)) {
                mrows_data_block[i] =
                    mrows_data_block[i] / (sx_slack_data_block[i] +
                                           mrows_data_block[i]);
            }


        //: sum of the rows
        vnl_vector < T > sy = M_ * tempy;
        T *sy_data_block = sy.data_block();

        for (int i = 0; i < static_cast < int >(numrows); i++)
            if (sy_data_block[i] > T(0)) {
                M_.scale_row(i,
                             1.0 / (sy_data_block[i] + mcols_data_block[i]));
                mcols_data_block[i] =
                    mcols_data_block[i] / (sy_data_block[i] +
                                           mcols_data_block[i]);
            }

        //: sum of the columns
        vnl_vector < T > sx = vnl_transpose(M_) * tempx;
        T *sx_data_block = sx.data_block();
        for (int i = 0; i < static_cast < int >(numcols); i++)
            if (sx_data_block[i] > T(0)) {
                M_.scale_column(i,
                                1.0 / (sx_data_block[i] +
                                     mrows_data_block[i]));
                mrows_data_block[i] =
                    mrows_data_block[i] / (sx_data_block[i] +
                                           mrows_data_block[i]);
            }
        //sx=vnl_transpose(M_)*tempx+mrows_;
        //sy=M_*tempy+mcols_;
        sx -= 1;
        sy -= 1;

        T err = (dot_product(sx, sx) + dot_product(sy, sy)) / (numrows + numcols);
        //: stop if the error is small or no of iteration exceed the limit
        if (err < tol)
            flag = false;
        if (numiter > maxit_)
            flag = false;
        ++numiter;
    }
    return true;
}


#define DBSTA_SINKHORN_INSTANTIATE(T) \
template class dbsta_sinkhorn<T>
