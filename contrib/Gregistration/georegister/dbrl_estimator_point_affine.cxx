#include "dbrl_estimator_point_affine.h"
#include "dbrl_feature.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include "dbrl_feature_point_tangent_curvature.h"

#include "dbrl_feature_point_sptr.h"
#include "dbrl_affine_transformation.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <dbnl/algo/dbnl_matrix_multiply.h>

dbrl_transformation_sptr dbrl_estimator_point_affine::estimate(vcl_vector<dbrl_feature_sptr> f1,
                                                               vcl_vector<dbrl_feature_sptr> f2,
                                                               dbrl_correspondence & M) const
    {
    assert(f1.size()>0);
    assert(f2.size()>0);

    dbrl_feature_point * p1;
    dbrl_feature_point * p2;

    dbrl_feature_point_tangent * pt1;
    dbrl_feature_point_tangent * pt2;

        dbrl_feature_point_tangent_curvature * ptk1;
    dbrl_feature_point_tangent_curvature * ptk2;


    if((p1=dynamic_cast<dbrl_feature_point*>(f1[0].ptr()))
        && (p2=dynamic_cast<dbrl_feature_point*>(f2[0].ptr())))
        {
        vnl_matrix<double> fpt1(f1.size(),3);
        vnl_matrix<double> fpt2(f2.size(),3);

        for(unsigned i=0;i<f1.size();i++)
            {
                //pts1.push_back(dynamic_cast<dbrl_feature_point*>(f1[i].ptr()));
                if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f1[i].ptr()))
                    fpt1.set_row(i,pt->location_homg());
                //else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
                //    fpt1.set_row(i,pt->location_homg());
                //else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
                //    fpt1.set_row(i,pt->location_homg());


            }

        for(unsigned i=0;i<f2.size();i++)
            {
                //pts2.push_back(dynamic_cast<dbrl_feature_point*>(f2[i].ptr()));
                if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
                    fpt2.set_row(i,pt->location_homg());
                //else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
                //    fpt2.set_row(i,pt->location_homg());

                //else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
                //    fpt2.set_row(i,pt->location_homg());
            }

        vnl_matrix<double> numer(3,3,0.0);
        vnl_matrix<double> component2(3,3,0.0);
        vnl_matrix<double> denom(3,3,0.0);
        vnl_matrix<double> identity(3,3,0.0);
        identity.fill_diagonal(1.0);
        vnl_matrix<double> Mcorr=M.M();

        vnl_matrix<double> temp1=multiply(Mcorr,fpt2);
        vnl_matrix<double> component1=multiply(fpt1,temp1,'T');
        for(unsigned i=0;i<fpt2.rows();i++)
        {
            component2+=(outer_product<double>(fpt2.get_row(i),fpt2.get_row(i))*(Mcorr.get_column(i).sum()));
        }

        vnl_matrix<double> A=(component1-component2)*vnl_inverse(component2+identity*lambda_)+identity;
        //A+I
        //vnl_matrix<double> A=numer*vnl_inverse(denom+identity*lambda_)+identity;
        dbrl_affine_transformation * affinet=new dbrl_affine_transformation(A);
        return affinet;
    }
    else if((pt1=dynamic_cast<dbrl_feature_point_tangent*>(f1[0].ptr()))
        &&  (pt2=dynamic_cast<dbrl_feature_point_tangent*>(f2[0].ptr())))
    {

        vnl_matrix<double> fpt1(f1.size(),3);
        vnl_matrix<double> fpt2(f2.size(),3);

        for(unsigned i=0;i<f1.size();i++)
        {
            if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
                fpt1.set_row(i,pt->location_homg());
        }

        for(unsigned i=0;i<f2.size();i++)
        {
            if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
                fpt2.set_row(i,pt->location_homg());
        }

        vnl_matrix<double> numer(3,3,0.0);
        vnl_matrix<double> component2(3,3,0.0);
        vnl_matrix<double> denom(3,3,0.0);
        vnl_matrix<double> identity(3,3,0.0);
        identity.fill_diagonal(1.0);
        vnl_matrix<double> Mcorr=M.M();
        vnl_matrix<double> temp1=multiply(Mcorr,fpt2);
        vnl_matrix<double> component1=multiply(fpt1,temp1,'T');
        for(unsigned i=0;i<fpt2.rows();i++)
            component2+=(outer_product<double>(fpt2.get_row(i),fpt2.get_row(i))*(Mcorr.get_column(i).sum()));

        vnl_matrix<double> A=(component1-component2)*vnl_inverse(component2+identity*lambda_)+identity;
        //A+I
        //vnl_matrix<double> A=numer*vnl_inverse(denom+identity*lambda_)+identity;
        dbrl_affine_transformation * affinet=new dbrl_affine_transformation(A);
        return affinet;

        }
        else if((ptk1=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[0].ptr()))
        &&  (ptk2=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[0].ptr())))
        {

        vnl_matrix<double> fpt1(f1.size(),3);
        vnl_matrix<double> fpt2(f2.size(),3);

        for(unsigned i=0;i<f1.size();i++)
            {
                if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
                    fpt1.set_row(i,pt->location_homg());
            }

        for(unsigned i=0;i<f2.size();i++)
            {
               if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
                    fpt2.set_row(i,pt->location_homg());
            }

        vnl_matrix<double> numer(3,3,0.0);
        vnl_matrix<double> component2(3,3,0.0);
        vnl_matrix<double> denom(3,3,0.0);
        vnl_matrix<double> identity(3,3,0.0);
        identity.fill_diagonal(1.0);
        vnl_matrix<double> Mcorr=M.M();
        vnl_matrix<double> temp1=multiply(Mcorr,fpt2);
        vnl_matrix<double> component1=multiply(fpt1,temp1,'T');
        for(unsigned i=0;i<fpt2.rows();i++)
            component2+=(outer_product<double>(fpt2.get_row(i),fpt2.get_row(i))*(Mcorr.get_column(i).sum()));

        vnl_matrix<double> A=(component1-component2)*vnl_inverse(component2+identity*lambda_)+identity;

        //vcl_cout<<A;
        //A+I
        //vnl_matrix<double> A=numer*vnl_inverse(denom+identity*lambda_)+identity;
        dbrl_affine_transformation * affinet=new dbrl_affine_transformation(A);
        return affinet;

        }


      return 0;
    }


double dbrl_estimator_point_affine::residual(const vcl_vector<dbrl_feature_sptr>& f1,
                                             const vcl_vector<dbrl_feature_sptr>& f2,
                                             const dbrl_correspondence & M,
                                             const dbrl_transformation_sptr& tform) const

    {
    dbrl_affine_transformation * t;
    assert(t=dynamic_cast<dbrl_affine_transformation *>(tform.ptr()));
    assert(f1.size()==0);
    assert(f2.size()==0);

    t->set_from_features(f2);
    t->transform();
    vcl_vector<dbrl_feature_sptr> tformed_f2=t->get_to_features();


    double residual=0;
    for(unsigned i=0;i<M.rows();i++)
        for(unsigned j=0;j<M.cols();j++)        
            residual+=(f1[i]->distance(tformed_f2[j])*M(i,j));
            
    //: TODO A-I rather than A        
    return residual+lambda_*t->frobenius_norm();

    }

void
dbrl_estimator_point_affine::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, lambda_);
    }

void dbrl_estimator_point_affine::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, lambda_);
            break;
        default:
            vcl_cerr << "dbrl_estimator_point_affine: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_estimator_point_affine::version() const
    {
    return 1;
    }
void dbrl_estimator_point_affine::print_summary(vcl_ostream &os) const
{
  
}
dbrl_estimator * dbrl_estimator_point_affine::clone()const
    {
    return new dbrl_estimator_point_affine(*this);
    }

//: Binary save dbrl_estimator_point_affine* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_point_affine* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_estimator_point_affine* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_point_affine* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_estimator_point_affine();
        p->b_read(is);
        }
    else
        p = 0;

    }
