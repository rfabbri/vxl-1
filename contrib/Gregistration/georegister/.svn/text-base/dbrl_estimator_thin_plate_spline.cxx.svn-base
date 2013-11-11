#include "dbrl_estimator_thin_plate_spline.h"
#include "dbrl_feature.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_sptr.h"
#include "dbrl_thin_plate_spline_transformation.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_qr.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_matlab_filewrite.h>
#include <vnl/vnl_fastops.h>
#include <vul/vul_timer.h>
#include <dbnl/algo/dbnl_matrix_multiply.h>
#include <georegister/dbrl_feature_point_tangent.h>

dbrl_transformation_sptr dbrl_estimator_point_thin_plate_spline::estimate(vcl_vector<dbrl_feature_sptr> f1,
                                                                          vcl_vector<dbrl_feature_sptr> f2,
                                                                          dbrl_correspondence & M) const
{
dbrl_transformation_sptr  tps= estimate_tps(f1,f2);
return tps;
}
dbrl_transformation_sptr dbrl_estimator_point_thin_plate_spline::estimate_tps(vcl_vector<dbrl_feature_sptr> f1,
                                                                          vcl_vector<dbrl_feature_sptr> f2) const
        
{
    assert(f1.size()>0);
    assert(f2.size()>0);
    
    vcl_vector<dbrl_feature_sptr> f1filtered;
    vcl_vector<dbrl_feature_sptr> f2filtered;

    for(unsigned i=0;i<f1.size();i++)
        {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f1[i].ptr()))
            {
            if(pt->location()[0]!=-100 )
                {
                f1filtered.push_back(f1[i]);
                f2filtered.push_back(f2[i]);
                }
            }
        else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
            {
            if(pt->location()[0]!=-100 )
                {
                f1filtered.push_back(f1[i]);
                f2filtered.push_back(f2[i]);
                }
            }
        }
    vnl_matrix<double> pts1(f1filtered.size(),3);
    vnl_matrix<double> pts2(f2filtered.size(),3);

    pts1.fill(1.0);
    pts2.fill(1.0);
    //: to remove the points which do not have any correspondence from the estimation part
    for(unsigned i=0;i<f1filtered.size();i++)
        {
        if(dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(f1filtered[i].ptr()))
            {
            for(int j=0;j<static_cast<int>(pt1->location().size());j++)
                pts1(i,j+1)=pt1->location()[j];
            }
        if(dbrl_feature_point* pt2=dynamic_cast<dbrl_feature_point*>(f2filtered[i].ptr()))
            {
            for(int j=0;j<static_cast<int>(pt2->location().size());j++)
                pts2(i,j+1)=pt2->location()[j];
            }
        if(dbrl_feature_point_tangent* pt1=dynamic_cast<dbrl_feature_point_tangent*>(f1filtered[i].ptr()))
            {
            for(int j=0;j<pt1->location().size();j++)
                pts1(i,j+1)=pt1->location()[j];
            }
        if(dbrl_feature_point_tangent* pt2=dynamic_cast<dbrl_feature_point_tangent*>(f2filtered[i].ptr()))
            {
            for(int j=0;j<pt2->location().size();j++)
            {
                pts2(i,j+1)=pt2->location()[j];
                //vcl_cout<<pt2->location()[j]<<" ";
            }
        }
            //vcl_cout<<"\n";
        }

    //vcl_cout<<"pts1 :\n"<<pts1;
    //vcl_cout<<"pts2 :\n"<<pts2;
    //: QR-decomposition of pts1
    vnl_qr<double> qr(pts2);
    vnl_matrix<double> Q=qr.Q();
    vnl_matrix<double> Rtemp=qr.R();
    vnl_matrix<double> R=Rtemp.extract(pts2.cols(),pts2.cols());
    vnl_matrix<double> q1=Q.extract(pts2.rows(),3);
    vnl_matrix<double> q2=Q.extract(pts2.rows(),pts2.rows()-3,0,3);
    //: compute tps_kernel for pts1
    vnl_matrix<double> K=build_K(f2filtered,f2filtered);

    vnl_matrix<double> identity1(pts1.rows()-pts1.cols(),pts1.rows()-pts1.cols());
    identity1.set_identity();
    vnl_matrix<double> identity2(R.rows(),R.rows());
    identity2.set_identity();
    vnl_matrix<double> identity3(3,3);
    identity3.set_identity();
    vnl_matrix<double> dre;

    vnl_matrix<double> C1=multiply(K,q2);
    if(lambda1_!=0.0)
        dre=multiply(q2,C1,'T')+lambda1_*identity1;
    else
        dre=multiply(q2,C1,'T');
    vnl_cholesky ch(dre);
    vnl_matrix<double> temp=vnl_transpose(q2)*pts1;
    vnl_matrix<double> tempx(temp.rows(),temp.cols());
    tempx.fill(0.0);

    for(int i=0;i<static_cast<int>(temp.cols());i++)
        {
        vnl_vector<double> *x=new vnl_vector<double>(temp.get_column(i));
        ch.solve(temp.get_column(i),x);
        tempx.set_column(i,*x);
        }
    vnl_matrix<double> gamma=tempx;
    vnl_matrix<double> c=q2*gamma;
    vnl_matrix<double> A;

    //: computing Affine component
    if(lambda2_!=0.0)
        A=vnl_inverse(R.transpose()*R+lambda2_*identity2 )*(R.transpose()*q1.transpose()*(pts1-K*c)-R.transpose()*R);
    else
        A=vnl_inverse(R.transpose()*R)*(R.transpose()*q1.transpose()*(pts1-K*c)-R.transpose()*R);

    vnl_matrix<double> d=A+identity3;


    K=build_K(f2,f2filtered);

    
    dbrl_thin_plate_spline_transformation * tps=new dbrl_thin_plate_spline_transformation(d,c,K,f2filtered);
    
    tps->set_lambda1(lambda1_);
    tps->set_lambda2(lambda2_);

    return tps;

    }
//: function to build the kernel matrix 
//:x and y would be different in order  to interpolate the warpping coefficients on the grid points
vnl_matrix<double> dbrl_estimator_point_thin_plate_spline::build_K(vcl_vector<dbrl_feature_sptr> &x , vcl_vector<dbrl_feature_sptr> &y) const
    {
    vnl_matrix<double> K(x.size(),y.size());
    double *f1x=new double[x.size()];
    double *f1y=new double[x.size()];
    double *f2x=new double[y.size()];
    double *f2y=new double[y.size()];
    for(unsigned i=0;i<x.size();i++)
    {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(x[i].ptr()))
            {
                f1x[i]=pt->location()[0];
                f1y[i]=pt->location()[1];
            }
        if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(x[i].ptr()))
            {
                f1x[i]=pt->location()[0];
                f1y[i]=pt->location()[1];
            }
    }
        for(unsigned i=0;i<y.size();i++)
        {
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(y[i].ptr()))
                {
                f2x[i]=pt->location()[0];
                f2y[i]=pt->location()[1];
                }
                        if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(y[i].ptr()))
                {
                f2x[i]=pt->location()[0];
                f2y[i]=pt->location()[1];
                }

        }
            for(unsigned i=0;i<K.rows();i++)
                {
                for(unsigned j=0;j<K.cols();j++)
                    {
                    double d2=(f1x[i]-f2x[j])*(f1x[i]-f2x[j])+(f1y[i]-f2y[j])*(f1y[i]-f2y[j]);
                    double d=0;

                    //d=vcl_exp(-d2);

                    if(d2<1e-10)
                        d=0;
                    else
                        d=d2*vcl_log(d2)/2;
                        //d=vcl_exp(-d2/(2));
                    K(i,j)=d;
                    }
                }

            delete f1x;
            delete f1y;
            delete f2x;
            delete f2y;
            return K;
    }

double dbrl_estimator_point_thin_plate_spline::residual(const vcl_vector<dbrl_feature_sptr>& f1,
                                                        const vcl_vector<dbrl_feature_sptr>& f2,
                                                        const dbrl_correspondence & M,
                                                        const dbrl_transformation_sptr& tform) const

    {
    dbrl_thin_plate_spline_transformation * t;
    assert(t=dynamic_cast<dbrl_thin_plate_spline_transformation *>(tform.ptr()));
    assert(f1.size()>0);
    assert(f2.size()>0);
    t->set_from_features(f1);
    t->transform();
    vcl_vector<dbrl_feature_sptr> tformed_f1=t->get_to_features();
    double residual=0;
    for(unsigned i=0;i<M.rows();i++)
        for(unsigned j=0;j<M.cols();j++)        
            residual+=(tformed_f1[i]->distance(f2[j])*M(i,j));
    return residual;//+lambda1_*t->warp_frobenius_norm()+lambda2_*t->affine_frobenius_norm();
    }

void
dbrl_estimator_point_thin_plate_spline::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, lambda1_);
    vsl_b_write(os, lambda2_);
    }

void dbrl_estimator_point_thin_plate_spline::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, lambda1_);
            vsl_b_read(is, lambda2_);
            break;
        default:
            vcl_cerr << "dbrl_estimator_point_thin_plate_spline: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_estimator_point_thin_plate_spline::version() const
    {
    return 1;
    }
void dbrl_estimator_point_thin_plate_spline::print_summary(vcl_ostream &os) const
{
  
}
dbrl_estimator * dbrl_estimator_point_thin_plate_spline::clone()const
    {
    return new dbrl_estimator_point_thin_plate_spline(*this);
    }

//: Binary save dbrl_estimator_point_thin_plate_spline* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_point_thin_plate_spline* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_estimator_point_thin_plate_spline* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_point_thin_plate_spline* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_estimator_point_thin_plate_spline();
        p->b_read(is);
        }
    else
        p = 0;

    }
    
