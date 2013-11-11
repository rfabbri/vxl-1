#include "dbrl_thin_plate_spline_transformation.h"
#include "dbrl_feature_sptr.h"
#include "dbrl_feature.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_vector_io.h>
#include <dbnl/algo/dbnl_matrix_multiply.h>
#include <vnl/vnl_trace.h>

dbrl_thin_plate_spline_transformation::dbrl_thin_plate_spline_transformation(vnl_matrix<double> A,vnl_matrix<double> warp_coeffs, vnl_matrix<double> tps_kernel, vcl_vector<dbrl_feature_sptr> fs)
            :A_(A),tps_kernel_(tps_kernel),warp_coeffs_(warp_coeffs),fs_(fs)
{
    tps_warp_=tps_kernel_*warp_coeffs_;
    base_pts.set_size(fs_.size(),2);
    for(unsigned i=0;i<fs_.size();i++)
    {
        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(fs_[i].ptr()))
        {
            base_pts(i,0)=pt->location()[0];
            base_pts(i,1)=pt->location()[1];

        }
        else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(fs_[i].ptr()))
        {
            base_pts(i,0)=pt->location()[0];
            base_pts(i,1)=pt->location()[1];
        }

    }
};

bool dbrl_thin_plate_spline_transformation::transform()
    {
    
    assert(from_features_.size()>0);
    to_features_.clear();
    for(int i=0;i<static_cast<int>(from_features_.size());i++)
        {
        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(from_features_[i].ptr()))
            {
            to_features_.push_back(new dbrl_feature_point(map_location(pt->location(),i)));
            }
         else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(from_features_[i].ptr()))
            {
                vnl_vector<double> pq=map_location(pt->location(),i);
                to_features_.push_back(new dbrl_feature_point_tangent(pq,pt->dir()));
            }
        }
    return true;

    }


double 
dbrl_thin_plate_spline_transformation::affine_frobenius_norm()
    {
     assert(A_.rows()>0 && A_.cols()>0);
     vnl_matrix<double> I(A_.rows(),A_.cols());
     I.set_identity();
     return (A_-I).frobenius_norm();
    }
double 
dbrl_thin_plate_spline_transformation::warp_frobenius_norm()
    {
     assert(warp_coeffs_.rows()>0 && warp_coeffs_.cols()>0);
     return warp_coeffs_.frobenius_norm();
    }
double
dbrl_thin_plate_spline_transformation::compute_energy()
{
    if(!warp_coeffs_.empty() && !tps_kernel_.empty() && !A_.empty())
    {
        vnl_matrix<double> subc=warp_coeffs_.extract(warp_coeffs_.rows(),warp_coeffs_.cols()-1,0,1);
        double energy=vnl_trace(multiply(subc,tps_kernel_,'T')*subc)+ vnl_trace(multiply(A_,A_,'T'));
        return energy;
    }
    return -1;
}
vnl_vector<double> 
dbrl_thin_plate_spline_transformation::map_location(vnl_vector<double>  loc, int index)
    {
        
        assert(base_pts.rows()==warp_coeffs_.rows());

        //: afifne part
        vnl_vector<double> homgpt(loc.size()+1,1.0);
        for(int i=1;i<static_cast<int>(loc.size())+1;i++)
            homgpt[i]=loc[i-1];
        assert(homgpt.size()==A_.cols());
        vnl_vector<double> tohomgpt= vnl_transpose(A_)*homgpt;
        
        //: tps part
        double xtps=0.0;
        double ytps=0.0;

        for(unsigned i=0;i<base_pts.rows();i++)
        {
            double d=0.0;
            double d2=(loc[0]-base_pts(i,0))*(loc[0]-base_pts(i,0))+(loc[1]-base_pts(i,1))*(loc[1]-base_pts(i,1));
            if(d2<1e-10)
                d=0;
            else
                d=d2*vcl_log(d2)/2;
                //d=vcl_exp(-d2/(2));


            xtps+=d*warp_coeffs_(i,1);
            ytps+=d*warp_coeffs_(i,2);
        }

        //vcl_cout<<vnl_transpose(A_)*homgpt<<" -> "<<tps_kernel_.get_row(index)*warp_coeffs_<<"\n";
        vnl_vector<double> tononhomgpt(loc.size(),1.0);
        tononhomgpt[0]=tohomgpt[1]+xtps;
        tononhomgpt[1]=tohomgpt[2]+ytps;

        //for(int i=1;i<static_cast<int>(tohomgpt.size());i++)
        //    tononhomgpt[i-1]=tohomgpt[i];
        return tononhomgpt;
    }

void dbrl_thin_plate_spline_transformation::print_transformation(vcl_ostream &os)
    {
    os<<"Affine Transformation is : \n"<<A_;
    
    os<<"The warping coefficients are \n";
    for(int i=0;i<static_cast<int>(warp_coeffs_.rows());i++)
        {
        for(int j=0;j<static_cast<int>(warp_coeffs_.cols());j++)
            os<<warp_coeffs_[i][j]<<" ";
        os<<"\n";
        }

    }


void dbrl_thin_plate_spline_transformation::build_K(vcl_vector<dbrl_feature_sptr> &x ) 
    {


    tps_kernel_.set_size(x.size(),fs_.size());

    double *f1x=new double[x.size()];
    double *f1y=new double[x.size()];
    double *f2x=new double[fs_.size()];
    double *f2y=new double[fs_.size()];
    for(unsigned i=0;i<x.size();i++)
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(x[i].ptr()))
            {
            for(int j=0;j<static_cast<int>(pt->location().size());j++)
                {
                f1x[i]=pt->location()[0];
                f1y[i]=pt->location()[1];
                }
            }
        for(unsigned i=0;i<fs_.size();i++)
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(fs_[i].ptr()))
                {
                f2x[i]=pt->location()[0];
                f2y[i]=pt->location()[1];
                }
            for(unsigned i=0;i<tps_kernel_.rows();i++)
                {
                for(unsigned j=0;j<tps_kernel_.cols();j++)
                    {
                    double d2=(f1x[i]-f2x[j])*(f1x[i]-f2x[j])+(f1y[i]-f2y[j])*(f1y[i]-f2y[j]);
                    double d=0;
                    if(d2<1e-10)
                        d=0;
                    else
                        d=d2*vcl_log(d2)/2;
                    tps_kernel_(i,j)=d;
                    }
                }

            delete [] f1x;
            delete [] f2x;
            delete [] f1y;
            delete [] f2y;

    }

void
dbrl_thin_plate_spline_transformation::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, A_);
    vsl_b_write(os, tps_kernel_);
    vsl_b_write(os, warp_coeffs_);
    vsl_b_write(os, tps_warp_);
    vsl_b_write(os, fs_);
    }

void dbrl_thin_plate_spline_transformation::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:

            vsl_b_read(is, A_);
            vsl_b_read(is, tps_kernel_);
            vsl_b_read(is, warp_coeffs_);
            vsl_b_read(is, tps_warp_);
            vsl_b_read(is, fs_);
            
            break;
        default:
            vcl_cerr << "dbrl_rigid_transformation: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_thin_plate_spline_transformation::version() const
    {
    return 1;
    }
void dbrl_thin_plate_spline_transformation::print_summary(vcl_ostream &os) const
{
  
}
dbrl_transformation * dbrl_thin_plate_spline_transformation::clone()const
    {
    return new dbrl_thin_plate_spline_transformation(*this);
    }

//: Binary save dbrl_thin_plate_spline_transformation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_thin_plate_spline_transformation* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_thin_plate_spline_transformation* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_thin_plate_spline_transformation* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_thin_plate_spline_transformation();
        p->b_read(is);
        }
    else
        p = 0;

    }
