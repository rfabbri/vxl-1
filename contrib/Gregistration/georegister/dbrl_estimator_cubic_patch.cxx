#include <georegister/dbrl_estimator_cubic_patch.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_curvature.h>
#include <georegister/dbrl_cubic_transformation.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_sptr.h>
#include <vnl/vnl_transpose.h>

dbrl_estimator_cubic_patch::dbrl_estimator_cubic_patch()
{
}
dbrl_transformation_sptr dbrl_estimator_cubic_patch::estimate( vcl_vector<dbrl_feature_sptr> f1,
                                                               vcl_vector<dbrl_feature_sptr> f2,
                                                               dbrl_correspondence & M) const
{
   
     vcl_vector<dbrl_feature_sptr> f1f=normalize_point_set(f1,M);
        
     dbrl_cubic_transformation * cubictransform;

     vcl_vector<dbrl_feature_sptr> f1filtered;
     vcl_vector<dbrl_feature_sptr> f2filtered;

    {
        for(unsigned i=0;i<f1f.size();i++)
        {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f1f[i].ptr()))
            {
            if(pt->location()[0]!=-100 )
                {
                f1filtered.push_back(f1f[i]);
                f2filtered.push_back(f2[i]);
                }
            }
        else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f1f[i].ptr()))
            {
            if(pt->location()[0]!=-100 )
                {
                f1filtered.push_back(f1f[i]);
                f2filtered.push_back(f2[i]);
                }
            }
        }
    vnl_matrix<double> pts1(f1filtered.size(),2);
    vnl_matrix<double> pts2(f2filtered.size(),2);

    pts1.fill(1.0);
    pts2.fill(1.0);
    //: to remove the points which do not have any correspondence from the estimation part
    for(unsigned i=0;i<f1filtered.size();i++)
    {
        if(dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(f1filtered[i].ptr()))
        {
            for(int j=0;j<static_cast<int>(pt1->location().size());j++)
                pts1(i,j)=pt1->location()[j];
        }
        if(dbrl_feature_point* pt2=dynamic_cast<dbrl_feature_point*>(f2filtered[i].ptr()))
        {
            for(int j=0;j<static_cast<int>(pt2->location().size());j++)
                pts2(i,j)=pt2->location()[j];
        }
        if(dbrl_feature_point_tangent* pt1=dynamic_cast<dbrl_feature_point_tangent*>(f1filtered[i].ptr()))
        {
            for(int j=0;j<pt1->location().size();j++)
                pts1(i,j)=pt1->location()[j];
        }
        if(dbrl_feature_point_tangent* pt2=dynamic_cast<dbrl_feature_point_tangent*>(f2filtered[i].ptr()))
        {
            for(int j=0;j<pt2->location().size();j++)
                pts2(i,j)=pt2->location()[j];
        }
                if(dbrl_feature_point_tangent_curvature* pt1=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1filtered[i].ptr()))
        {
            for(int j=0;j<pt1->location().size();j++)
                pts1(i,j)=pt1->location()[j];
        }
        if(dbrl_feature_point_tangent_curvature* pt2=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2filtered[i].ptr()))
        {
            for(int j=0;j<pt2->location().size();j++)
                pts2(i,j)=pt2->location()[j];
        }

    }
    
    vnl_vector<double> zx(f1filtered.size());
    vnl_vector<double> zy(f1filtered.size());
    for(unsigned i=0;i<f1filtered.size();i++)
    {
        zx[i]=pts1(i,0)-pts2(i,0);
        zy[i]=pts1(i,1)-pts2(i,1);
        vcl_cout<<zx[i]<<" ";
    }

    vcl_vector<dbrl_clough_tocher_patch> patchx=estimate_cubic(pts2,zx);
    vcl_vector<dbrl_clough_tocher_patch> patchy=estimate_cubic(pts2,zy);


    cubictransform=new dbrl_cubic_transformation(patchx,patchy);


    }
    return cubictransform;
}
vcl_vector<dbrl_clough_tocher_patch>  
dbrl_estimator_cubic_patch::estimate_cubic(vnl_matrix<double> xy, vnl_vector<double> zs) const
{

    int n=xy.rows();
    double *x =new double[n];
    double *y =new double[n];
    double *fx =new double[n];
    double *fy =new double[n];
    double *z =new double[n];

    for(unsigned i=0;i<xy.rows();i++)
    {
        x[i]=xy(i,0);
        y[i]=xy(i,1);
        z[i]=zs[i];
    }

    compute_partial_derivatives(n,x,y,z,fx,fy);
    XYZ *points=new XYZ[n];
    for(int i=0;i<n;i++)
    {
        points[i].x=x[i];
        points[i].y=y[i];
        points[i].z=z[i];
        points[i].fx=fx[i];
        points[i].fy=fy[i];
    }

    XYZ * p = new XYZ[n + 3]; 
    for (int i = 0; i < n; i++) {
        p[i] = points[i];      
    }
    ITRIANGLE *v = new ITRIANGLE[3 * n]; 
    qsort(p, n, sizeof(XYZ), XYZCompare);
    int ntri=0;
    Triangulate(n, p, v, ntri);

    compute_normal_derivatives(n, p, v, ntri);

    vcl_vector<dbrl_clough_tocher_patch> patches;
    for(int i=0;i<ntri;i++)
    {
        dbrl_clough_tocher_patch patch;
        patch.set_points(vgl_point_3d<double>(p[v[i].p1].x,p[v[i].p1].y,p[v[i].p1].z),
            vgl_point_3d<double>(p[v[i].p2].x,p[v[i].p2].y,p[v[i].p2].z),   
            vgl_point_3d<double>(p[v[i].p3].x,p[v[i].p3].y,p[v[i].p3].z));
        patch.set_cross_bnd_derivative(v[i].f12n,v[i].f23n,v[i].f31n);
        patch.set_partials(vgl_vector_2d<double>(p[v[i].p1].fx,p[v[i].p1].fy),
            vgl_vector_2d<double>(p[v[i].p2].fx,p[v[i].p2].fy),
            vgl_vector_2d<double>(p[v[i].p3].fx,p[v[i].p3].fy));

        patch.compute_cubic_control_points();

        patches.push_back(patch);
    }

    return patches;
}
vcl_vector<dbrl_feature_sptr> dbrl_estimator_cubic_patch::normalize_point_set( vcl_vector<dbrl_feature_sptr>  f2,
                                                      dbrl_correspondence  M) const

{
    vnl_matrix<double> pts(f2.size(),2,0.0);
    for(unsigned i=0;i<f2.size();i++)
    {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
        {
            pts(i,0)=pt->location()[0];
            pts(i,1)=pt->location()[1];
        }
        else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
        {
            pts(i,0)=pt->location()[0];
            pts(i,1)=pt->location()[1];
        }
                else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
        {
            pts(i,0)=pt->location()[0];
            pts(i,1)=pt->location()[1];
        }

    }
    vnl_transpose Mtrans(M.M());
    vnl_matrix<double> Mt=Mtrans.asMatrix();
    vnl_matrix<double> vpts=Mtrans*pts;
    vcl_vector<dbrl_feature_sptr>  f2filtered;
    for(int i=0;i<Mt.rows();i++)
    {
        if(Mt.get_row(i).sum()<0.75)
            vpts.set_row(i,-100);
        else
        {
            vpts.scale_row(i,1/Mt.get_row(i).sum());
        }
        f2filtered.push_back(new dbrl_feature_point(vpts.get_row(i)));
    }
    return f2filtered;

}


double dbrl_estimator_cubic_patch::residual(const vcl_vector<dbrl_feature_sptr>& f1,
                                            const vcl_vector<dbrl_feature_sptr>& f2,
                                            const dbrl_correspondence & M,
                                            const dbrl_transformation_sptr& tform) const

    {
    dbrl_cubic_transformation * t;
    assert(t=dynamic_cast<dbrl_cubic_transformation *>(tform.ptr()));
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
    return residual;

    }

void
dbrl_estimator_cubic_patch::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    //vsl_b_write(os, lambda_);
    }

void dbrl_estimator_cubic_patch::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            //vsl_b_read(is, lambda_);
            break;
        default:
            vcl_cerr << "dbrl_estimator_cubic_patch: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_estimator_cubic_patch::version() const
    {
    return 1;
    }
void dbrl_estimator_cubic_patch::print_summary(vcl_ostream &os) const
{
  
}
dbrl_estimator * dbrl_estimator_cubic_patch::clone()const
    {
    return new dbrl_estimator_cubic_patch(*this);
    }

//: Binary save dbrl_estimator_cubic_patch* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_cubic_patch* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_estimator_cubic_patch* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_cubic_patch* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_estimator_cubic_patch();
        p->b_read(is);
        }
    else
        p = 0;

    }
