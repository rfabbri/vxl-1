
#include "dbrl_correspondence.h"
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <dbsta/algo/dbsta_sinkhorn.h>
#include <vcl_cassert.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
dbrl_correspondence::dbrl_correspondence()
    {

    }
dbrl_correspondence::dbrl_correspondence(vnl_matrix<double> M):M_(M)
    {
    assert(M_.rows()>0 && M_.cols()>0);
    initm=1/(double)vcl_max(M_.rows(),M_.cols());
    moutlier=initm*initm;

    Mold_=M_;


    }
dbrl_correspondence::dbrl_correspondence(int m,int n):M_(m,n)
    {
        assert(m>0 && n>0);    
        initm=1/(double)vcl_max(m,n);
        moutlier=initm*initm;
        M_.fill(initm);
        Mold_=M_;
    }
dbrl_correspondence::~dbrl_correspondence()
    {
    }
//: intial value for the correspondence matrix
void dbrl_correspondence::setinitialvalue(double init)
    {
        initm=init;
    }
void dbrl_correspondence::setinitialoutlier(double outlier)
    {
        moutlier=outlier;
    }

bool dbrl_correspondence::normalize()
    {
           dbsta_sinkhorn<double> sinkhorn(M_,moutlier);
           sinkhorn.normalize(1e-2);
           mcols_=sinkhorn.mcols();
           mrows_=sinkhorn.mrows();
           M_=sinkhorn.M();
           return true;

    }
double dbrl_correspondence::operator() (unsigned i,unsigned j) const
    {
        assert(i<M_.rows());
        assert(j<M_.cols());

        return M_(i,j);
    
    }
bool dbrl_correspondence::updateM(vnl_matrix<double> & M){
        Mold_=M_;
        M_=M;
        return normalize();
    }
bool dbrl_correspondence::updateM(vnl_matrix<double> & M, double annealrate){
        Mold_=M_;
        M_=M;
        moutlier*=annealrate;
        vcl_cout<<"\n outlier val is :"<<moutlier<<"\n";
        return normalize();
    }

double dbrl_correspondence::errdist()
    {
        return (Mold_-M_).array_two_norm();
    }
bool dbrl_correspondence::binarize(double threshold)
    {   
        for(unsigned i=0;i<M_.rows();i++)
            {
            for(unsigned j=0;j<M_.cols();j++)
                {
                    if(M_(i,j)>threshold)
                        M_(i,j)=1;
                    else
                        M_(i,j)=0;
                }
            }

        for(unsigned i=0;i<mcols_.size();i++)
            if(mcols_[i]>threshold)
                mcols_[i]=1;
            else
                mcols_[i]=0;
        for(unsigned i=0;i<mrows_.size();i++)
            if(mrows_[i]>threshold)
                mrows_[i]=1;
            else
                mrows_[i]=0;
        return true;
    }

void
dbrl_correspondence::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, M_);
    vsl_b_write(os, mcols_);
    vsl_b_write(os, mrows_);
    vsl_b_write(os, initm);
    vsl_b_write(os, moutlier);
    }

void
dbrl_correspondence::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, M_);
            vsl_b_read(is, mcols_);
            vsl_b_read(is, mrows_);
            vsl_b_read(is, initm);
            vsl_b_read(is, moutlier);
            break;
        default:
            vcl_cerr << "dbrl_feature_point: unknown I/O version " << ver << '\n';
        }
    }
short
dbrl_correspondence::version() const
    {
    return 1;
    }
void dbrl_correspondence::print_summary(vcl_ostream &os) const
{
for (unsigned i=0;i<M_.rows();i++)
    {
    for(unsigned j=0;j<M_.cols();j++)
        os<<M_(i,j)<<" ";
    os<<"\n";
    }
}
//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_correspondence p)
    {
        p.b_write(os);
    }

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_correspondence &p)
    {
        p.b_read(is);
    }

