#include "dbrl_rigid_transformation.h"
#include "dbrl_feature_point.h"
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>
bool dbrl_rigid_transformation::transform()
    {
        assert(from_features_.size()>0);
        to_features_.clear();
        for(int i=0;i<static_cast<int>(to_features_.size());i++)
            {
                if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(to_features_[i].ptr()))
                    {
                        to_features_.push_back(new dbrl_feature_point(map_location(pt->location())));
                    }
            }
        return true;
    }
vnl_vector<double>
dbrl_rigid_transformation::map_location(vnl_vector<double> frompt)
    {
        assert(T_.size()==R_.cols());
        assert(T_.size()==R_.rows());
        assert(frompt.size()==T_.size());
        
        return (R_*frompt+T_)*scale_;
    }

vnl_vector<double>
dbrl_rigid_transformation::map_dir(vnl_vector<double> & fromdir)
    {
        assert(fromdir.size()==T_.size());
        return R_*fromdir;
    }


void
dbrl_rigid_transformation::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, T_);
    vsl_b_write(os, R_);
    vsl_b_write(os, scale_);
    }

void dbrl_rigid_transformation::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, T_);
            vsl_b_read(is, R_);
            vsl_b_read(is, scale_);
            break;
        default:
            vcl_cerr << "dbrl_rigid_transformation: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_rigid_transformation::version() const
    {
    return 1;
    }
void dbrl_rigid_transformation::print_summary(vcl_ostream &os) const
{
  
}
dbrl_transformation * dbrl_rigid_transformation::clone()const
    {
    return new dbrl_rigid_transformation(*this);
    }

//: Binary save dbrl_rigid_transformation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_rigid_transformation* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_rigid_transformation* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_rigid_transformation* &p)
    {
    if(p)
        delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_rigid_transformation();
        p->b_read(is);
        }
    else
        p = 0;

    }
