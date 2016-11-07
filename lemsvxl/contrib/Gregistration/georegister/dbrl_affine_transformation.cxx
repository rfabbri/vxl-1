#include "dbrl_affine_transformation.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include "dbrl_feature_point_tangent_curvature.h"
#include "dbrl_feature_point_tangent_curvature_groupings.h"

#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>

//: always takes homogeneous coordinates 


dbrl_affine_transformation::dbrl_affine_transformation()
    {
    A_.set_identity();
    }

bool dbrl_affine_transformation::transform()
    {
        assert(from_features_.size()>0);
        to_features_.clear();
        for(int i=0;i<static_cast<int>(from_features_.size());i++)
            {
                if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(from_features_[i].ptr()))
                {
                    to_features_.push_back(new dbrl_feature_point(map_location(pt->location())));
                }
                else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(from_features_[i].ptr()))
                {
                    to_features_.push_back(new dbrl_feature_point_tangent(map_location(pt->location()),pt->dir()));
                }
                else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(from_features_[i].ptr()))
                {
                    if(dbrl_feature_point_tangent_curvature_groupings * ptg=dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(pt))
                    {
                        dbrl_feature_point_tangent_curvature_groupings *ptx= new dbrl_feature_point_tangent_curvature_groupings(map_location(ptg->location()),ptg->dir(),ptg->feature_neighbor_map_);                  
                        if(ptg->k_flag())
                            ptx->set_k(ptg->k());
                        to_features_.push_back(ptx);
                    }
                    else
                    {
                        dbrl_feature_point_tangent_curvature *ptx= new dbrl_feature_point_tangent_curvature(map_location(pt->location()),pt->dir());      
                        if(pt->k_flag())
                            ptx->set_k(pt->k());
                        to_features_.push_back(ptx);
                    }
                }

            }
        return true;
    }
double 
dbrl_affine_transformation::frobenius_norm()
    {
     assert(A_.rows()>0 && A_.cols()>0);
     return A_.frobenius_norm();
    }

vnl_vector<double>
dbrl_affine_transformation::map_location(vnl_vector<double> loc) const
    {
        vnl_vector<double> homgpt(loc.size()+1,1.0);
        for(int i=0;i<static_cast<int>(loc.size());i++)
            homgpt[i]=loc[i];
        assert(homgpt.size()==A_.cols());
        vnl_vector<double> tohomgpt= A_*homgpt;
        vnl_vector<double> nonhomgpt(tohomgpt.size()-1,0.0);
        for(unsigned i=0;i<tohomgpt.size()-1;i++)
            nonhomgpt[i]=tohomgpt[i];///tohomgpt[tohomgpt.size()-1];

        return nonhomgpt;
    }

double
dbrl_affine_transformation::energy() 
    {
        return frobenius_norm();
    }


vnl_vector<double>
dbrl_affine_transformation::map_dir(vnl_vector<double> & dir) const
    {
        assert(dir.size()==A_.cols());
        return A_*dir;
    }

void dbrl_affine_transformation::print_transformation(vcl_ostream &os)
    {
        os<<"Affine MAtrix\n"<<A_;
    }


void
dbrl_affine_transformation::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, A_);
    }

void dbrl_affine_transformation::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, A_);
            break;
        default:
            vcl_cerr << "dbrl_rigid_transformation: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_affine_transformation::version() const
    {
    return 1;
    }
void dbrl_affine_transformation::print_summary(vcl_ostream &os) const
{
    A_.print(os);
}
dbrl_transformation * dbrl_affine_transformation::clone()const
    {
    return new dbrl_affine_transformation(*this);
    }

//: Binary save dbrl_affine_transformation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_affine_transformation* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_affine_transformation* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_affine_transformation* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_affine_transformation();
        p->b_read(is);
        }
    else
        p = 0;

    }
