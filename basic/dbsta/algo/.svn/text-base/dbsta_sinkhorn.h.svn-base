#ifndef dbsta_sinkhorn_h_
#define dbsta_sinkhorn_h_
//: Author: vishal jain (vj@lems.brown.edu)
//  Aug 2005

//Imlementation of Sinkhonr algorithm 
//to convert a matrix into a doubly stochastic matrix
// The class is templated over data type
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
template <class T>
class dbsta_sinkhorn{

public:
    dbsta_sinkhorn<T>(vnl_matrix<T>  M);
    dbsta_sinkhorn<T>(vnl_matrix<T> M,vnl_vector<T> m_cols, vnl_vector<T> m_rows);
    dbsta_sinkhorn(vnl_matrix<T> M,T moutlier);
    ~dbsta_sinkhorn();

    void set_maxiter(int maxit){maxit_=maxit;}
    bool normalize(T tol);

    vnl_matrix<T> M(){return M_;}
    vnl_vector<T> mcols(){return mcols_;}
    vnl_vector<T> mrows(){return mrows_;}
    
protected:

    vnl_matrix<T> M_;
    vnl_vector<T> mcols_;
    vnl_vector<T> mrows_;
    int maxit_;

};
#endif
