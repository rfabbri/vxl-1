#include "dbrl_feature_sptr.h"
#include "dbrl_feature_point_tangent_curvature.h"

#include "dbrl_affine_transformation.h"
#include "dbrl_rigid_transformation.h"
#include "dbrl_translation.h"
#include <vnl/io/vnl_io_vector.h>
#include <vcl_string.h>
dbrl_feature_point_tangent_curvature::dbrl_feature_point_tangent_curvature():/*circarc_(),*/is_k_set_(false)
    {
        location_(0);
    }

dbrl_feature_point_tangent_curvature::dbrl_feature_point_tangent_curvature(vnl_vector<double> const loc, double const  dir): dir_(dir),/*circarc_(),*/is_k_set_(false)
    {
        location_=loc;
    }
dbrl_feature_point_tangent_curvature::dbrl_feature_point_tangent_curvature(double x,double y, double dir ): dir_(dir)/*,circarc_()*/,is_k_set_(false)
    {
        location_.set_size(2);
        location_[0]=x;
        location_[1]=y;
    }
dbrl_feature_point_tangent_curvature::~dbrl_feature_point_tangent_curvature()
    {

    }
bool dbrl_feature_point_tangent_curvature::compute_arc(double arclength)
{
 if(is_k_set_)
 {
     //dbgl_circ_arc cright;
     //dbgl_circ_arc cleft;

     //cright.set_from(vgl_point_2d<double>(location_[0],location_[1]),vgl_vector_2d<double>(vcl_cos(dir_),vcl_sin(dir_)),k_,arclength);
     //cleft.set_from(vgl_point_2d<double>(location_[0],location_[1]),vgl_vector_2d<double>(-vcl_cos(dir_),-vcl_sin(dir_)),-k_,arclength);

     //circarc_.set(cright.point_at_length(arclength),cleft.point_at_length(arclength),-k_);


     return true;
 }
    // return circarc_.set_from(vgl_point_2d<double>(location_[0],location_[1]),vgl_vector_2d<double>(vcl_cos(dir_),vcl_sin(dir_)),k_,arclength);
    //return circarc_.set_from(vgl_point_2d<double>(location_[0],location_[1]),vgl_vector_2d<double>(vcl_cos(dir_),vcl_sin(dir_)),-k_,arclength,arclength);
 else
     return false;
}
double dbrl_feature_point_tangent_curvature::distance(const dbrl_feature_sptr & pt)
    {
        if(dbrl_feature_point_tangent_curvature *fpt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(pt.ptr()))
            {
            assert(fpt->location().size()==location_.size()) ;
            return(fpt->location()-location_).two_norm();
            }
        //else if(dbrl_feature_point_tangent_curvature_tangent *fpt=dynamic_cast<dbrl_feature_point_tangent_curvature_tangent*>(pt.ptr()))
        //    {
        //        vcl_cout<<"\n NYI";
        //        return 0;
        //    }
        else
            {
                return 0;
            }
    }
void dbrl_feature_point_tangent_curvature::print_feature(vcl_ostream &os)
    {
    os<<"\n point location is "<<location_;
    os<<"\n direction is "<<dir_;
    os<<"\n curvature is "<<k_;
    }


vnl_vector<double> dbrl_feature_point_tangent_curvature::location_homg() const
    {
        vnl_vector<double> homgpt(location_.size()+1,1.0);
        for(int i=0;i<location_.size();i++)
            homgpt[i]=location_[i];

        return homgpt;
    }
double dbrl_feature_point_tangent_curvature::dir() const
    {
        return dir_;
    }


//: write feature
void
dbrl_feature_point_tangent_curvature::write( vcl_ostream& os ) const
{
    os << "POINT-TANGENT-CURVATURE" << vcl_endl;
    os << location_.size() << vcl_endl;
    os << location_ <<vcl_endl;
    os << dir_ <<vcl_endl;
    os << k_ <<vcl_endl;

}

//: read  feature
bool 
dbrl_feature_point_tangent_curvature::
read( vcl_istream& is)
{
    vcl_string str;
    vcl_getline( is, str );
    
    // The token should appear at the beginning of line
    if ( str.find( "POINT-TANGENT-CURVATURE" ) != 0 ) {
      vcl_cout<< "It is not a POINT-TANGENT-CURVATURE. reading is aborted.\n" ;
      return false;
    }
  
  // get dim
  int dim=-1;
  is >> dim;
  
  if( !is || dim<=0 ) 
    return false;    // cannot get dimension
    
  // get location
  location_.set_size( dim );
  is >> location_;
  is>> dir_;
  is>> k_;
  if( !is )
    return false;   // cannot read location
    
  return true;
}

void
dbrl_feature_point_tangent_curvature::b_write(vsl_b_ostream &os) const
    {
        vsl_b_write(os, version());
        vsl_b_write(os, location_);
        vsl_b_write(os, dir_);
        vsl_b_write(os, k_);
        vsl_b_write(os, id_);
    }

void
dbrl_feature_point_tangent_curvature::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, location_);
            vsl_b_read(is, dir_);
            vsl_b_read(is, k_);
            vsl_b_read(is, id_);
            break;
        default:
            vcl_cerr << "dbrl_feature_point_tangent_curvature: unknown I/O version " << ver << '\n';
        }
    }

short
dbrl_feature_point_tangent_curvature::version() const
    {
    return 1;
    }
void dbrl_feature_point_tangent_curvature::print_summary(vcl_ostream &os) const
{
  
}
 dbrl_feature * dbrl_feature_point_tangent_curvature::clone()const
    {
        return new dbrl_feature_point_tangent_curvature(*this);
    }

//: Binary save dbrl_feature_point_tangent_curvature* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_feature_point_tangent_curvature* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }
    }

//: Binary load dbrl_feature_point_tangent_curvature* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_feature_point_tangent_curvature* &p)
    {
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_feature_point_tangent_curvature();
        p->b_read(is);
        }
    else
        p = 0;

    }
