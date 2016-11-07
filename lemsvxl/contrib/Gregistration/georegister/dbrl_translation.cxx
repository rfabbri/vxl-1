#include "dbrl_translation.h"
#include "dbrl_feature_point.h"
#include <vnl/io/vnl_io_vector.h>

vnl_vector<double>
dbrl_translation::map_location(vnl_vector<double> frompt)
    {
        assert(frompt.size()==T_.size());
        return frompt+T_;
    }

vnl_vector<double>
dbrl_translation::map_dir(vnl_vector<double> & fromdir)
    {
        assert(fromdir.size()==T_.size());
        return fromdir;
    }


bool dbrl_translation::transform()
    {
        assert(from_features_.size()>0);
        to_features_.clear();
        for(int i=0;i<static_cast<int>(from_features_.size());i++)
            {
                if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(from_features_[i].ptr()))
                    {
                        to_features_.push_back(new dbrl_feature_point(map_location(pt->location())));
                    }
            }
        return true;
    }

void
dbrl_translation::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, T_);
    }

void dbrl_translation::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, T_);
            break;
        default:
            vcl_cerr << "dbrl_translation: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_translation::version() const
    {
    return 1;
    }
void dbrl_translation::print_summary(vcl_ostream &os) const
{
  
}
dbrl_transformation * dbrl_translation::clone()const
    {
    return new dbrl_translation(*this);
    }

//: Binary save dbrl_translation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_translation* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_translation* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_translation* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_translation();
        p->b_read(is);
        }
    else
        p = 0;

    }
