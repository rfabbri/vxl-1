//Brandon Mayer
//this is /contrib/bm/ncn1/scratch.cxx
//executable for testing

#include < vcl_iostream.h >
#include < vnl/vnl_matrix.h >

int main()
{
    vnl_matrix<unsigned> mat;
    unsigned nrows = 3;
    unsigned ncols = 4;
    mat.set_size(nrows,ncols);
    for(unsigned i = 0; i < nrows; ++i)
        for(unsigned j = 0; j < ncols; ++j)
        {
            mat(i,j) = j*nrows + i;
        }
 
        vcl_cout << mat << vcl_endl;
    return 0;
}