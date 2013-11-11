#include "dbrl_feature_sptr.h"
#include "dbrl_feature_point_tangent_curvature_groupings.h"

#include "dbrl_affine_transformation.h"
#include "dbrl_rigid_transformation.h"
#include "dbrl_translation.h"
#include <vnl/io/vnl_io_vector.h>
#include <vcl_string.h>
dbrl_feature_point_tangent_curvature_groupings::dbrl_feature_point_tangent_curvature_groupings():dbrl_feature_point_tangent_curvature()
    {
    }

dbrl_feature_point_tangent_curvature_groupings::
dbrl_feature_point_tangent_curvature_groupings(vnl_vector<double> const loc, double const  dir,vcl_vector<unsigned> ns):dbrl_feature_point_tangent_curvature(loc,dir)
    {
        feature_neighbor_map_=ns;
    }
dbrl_feature_point_tangent_curvature_groupings::dbrl_feature_point_tangent_curvature_groupings(double x,double y, double dir, vcl_vector<unsigned> ns ):dbrl_feature_point_tangent_curvature(x,y,dir)
    {
        feature_neighbor_map_=ns;
    }
dbrl_feature_point_tangent_curvature_groupings::~dbrl_feature_point_tangent_curvature_groupings()
    {

    }
void dbrl_feature_point_tangent_curvature_groupings::print_feature(vcl_ostream &os)
    {
    os<<"\n point location is "<<location_;
    os<<"\n direction is "<<dir_;
    os<<"\n curvature is "<<k_;
    }




//: write feature
void
dbrl_feature_point_tangent_curvature_groupings::b_write(vsl_b_ostream &os) const
    {
        vsl_b_write(os, version());
    }

void
dbrl_feature_point_tangent_curvature_groupings::b_read(vsl_b_istream &is) 
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
            vcl_cerr << "dbrl_feature_point_tangent_curvature_groupings: unknown I/O version " << ver << '\n';
        }
    }

short
dbrl_feature_point_tangent_curvature_groupings::version() const
    {
    return 1;
    }
void dbrl_feature_point_tangent_curvature_groupings::print_summary(vcl_ostream &os) const
{
  
}
 dbrl_feature * dbrl_feature_point_tangent_curvature_groupings::clone()const
    {
        return new dbrl_feature_point_tangent_curvature_groupings(*this);
    }

//: Binary save dbrl_feature_point_tangent_curvature_groupings* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_feature_point_tangent_curvature_groupings* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }
    }

//: Binary load dbrl_feature_point_tangent_curvature_groupings* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_feature_point_tangent_curvature_groupings* &p)
    {
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_feature_point_tangent_curvature_groupings();
        p->b_read(is);
        }
    else
        p = 0;

    }
