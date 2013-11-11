#include "dbrl_cubic_transformation.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include "dbrl_feature_point_tangent_curvature.h"
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>

//: always takes homogeneous coordinates 
dbrl_cubic_transformation::dbrl_cubic_transformation(vcl_vector<dbrl_clough_tocher_patch> patchesx,
            vcl_vector<dbrl_clough_tocher_patch> patchesy):patchesx_(patchesx),patchesy_(patchesy)
    {
    
    }
dbrl_cubic_transformation::dbrl_cubic_transformation()
    {
    
    
patchesx_.clear();
    patchesy_.clear();
}
bool dbrl_cubic_transformation::transform()
    {
        assert(from_features_.size()>0);
        to_features_.clear();
        for(int i=0;i<static_cast<int>(from_features_.size());i++)
        {
            if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(from_features_[i].ptr()))
                to_features_.push_back(new dbrl_feature_point(map_location(pt->location())));
            else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(from_features_[i].ptr()))
                to_features_.push_back(new dbrl_feature_point_tangent(map_location(pt->location()),pt->dir()));
            else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(from_features_[i].ptr()))
            {
                dbrl_feature_point_tangent_curvature *ptx= new dbrl_feature_point_tangent_curvature(map_location(pt->location()),pt->dir());                  
                if(pt->k_flag())
                    ptx->set_k(pt->k());
                to_features_.push_back(ptx);
            }

        }
        return true;
    }
//double 
//dbrl_cubic_transformation::frobenius_norm()
//    {
//     assert(A_.rows()>0 && A_.cols()>0);
//     return A_.frobenius_norm();
//    }

vnl_vector<double>
dbrl_cubic_transformation::map_location(vnl_vector<double> loc) const
    {
        vgl_point_2d<double> p(loc[0],loc[1]);
        vnl_vector<double> loct(2);
        for(int i=0;i<patchesx_.size();i++)
        {   
            if(patchesx_[i].intriangle(p))
            {
                loct[0]=p.x()+patchesx_[i].interpolate(p);
                loct[1]=p.y()+patchesy_[i].interpolate(p);

                return loct;
            }
        }
        return loc;
    }

vnl_vector<double>
dbrl_cubic_transformation::map_dir(vnl_vector<double> & dir) const
    {
       
        return dir;
    }

void dbrl_cubic_transformation::print_transformation(vcl_ostream &os)
    {
    }


void
dbrl_cubic_transformation::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    }

void dbrl_cubic_transformation::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            break;
        default:
            vcl_cerr << "dbrl_rigid_transformation: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_cubic_transformation::version() const
    {
    return 1;
    }
void dbrl_cubic_transformation::print_summary(vcl_ostream &os) const
{
  
}
dbrl_transformation * dbrl_cubic_transformation::clone()const
    {
    return new dbrl_cubic_transformation(*this);
    }

//: Binary save dbrl_cubic_transformation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_cubic_transformation* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_cubic_transformation* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_cubic_transformation* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_cubic_transformation();
        p->b_read(is);
        }
    else
        p = 0;

    }
