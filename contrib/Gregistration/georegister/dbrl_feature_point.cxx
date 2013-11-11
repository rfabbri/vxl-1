#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include "dbrl_feature_sptr.h"
#include "dbrl_affine_transformation.h"
#include "dbrl_rigid_transformation.h"
#include "dbrl_translation.h"
#include <vnl/io/vnl_io_vector.h>
#include <vcl_string.h>
dbrl_feature_point::dbrl_feature_point()
    {
        
    }

dbrl_feature_point::dbrl_feature_point(vnl_vector<double> const& loc )
    {
        location_=loc;
    }
dbrl_feature_point::dbrl_feature_point(double x,double y )
    {
        location_.set_size(2);
        location_[0]=x;
        location_[1]=y;
    }
dbrl_feature_point::~dbrl_feature_point()
    {

    }
double dbrl_feature_point::distance(const dbrl_feature_sptr & pt)
    {
        if(dbrl_feature_point *fpt=dynamic_cast<dbrl_feature_point*>(pt.ptr()))
            {
            assert(fpt->location().size()==location_.size()) ;
            return(fpt->location()-location_).two_norm();
            }
        else if(dbrl_feature_point_tangent *fpt=dynamic_cast<dbrl_feature_point_tangent*>(pt.ptr()))
            {
                vcl_cout<<"\n NYI";
                return 0;
            }
        else
            {
                return 0;
            }
    }
void dbrl_feature_point::print_feature(vcl_ostream &os)
    {
    os<<"\n point location is "<<location_;
    }


vnl_vector<double> dbrl_feature_point::location_homg() const
    {
        vnl_vector<double> homgpt(location_.size()+1,1.0);
        for(int i=0;i<static_cast<int>(location_.size());i++)
            homgpt[i]=location_[i];

        return homgpt;
    }
//: write feature
void
dbrl_feature_point::write( vcl_ostream& os ) const
{
  os << "POINT" << vcl_endl;
  os << location_.size() << vcl_endl;
  os << location_ <<vcl_endl;
}

//: read  feature
bool 
dbrl_feature_point::
read( vcl_istream& is)
{
    vcl_string str;
    vcl_getline( is, str );
    
    // The token should appear at the beginning of line
    if ( str.find( "POINT" ) != 0 ) {
      vcl_cout<< "It is not a POINT. reading is aborted.\n" ;
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
  
  if( !is )
    return false;   // cannot read location
    
  return true;
}

void
dbrl_feature_point::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, location_);
    vsl_b_write(os, id_);

    }

void
dbrl_feature_point::b_read(vsl_b_istream &is) 
    {
    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, location_);
            vsl_b_read(is, id_);
            break;
        default:
            vcl_cerr << "dbrl_feature_point: unknown I/O version " << ver << '\n';
        }
    }

short
dbrl_feature_point::version() const
    {
    return 1;
    }
void dbrl_feature_point::print_summary(vcl_ostream &os) const
{
  
}
 dbrl_feature * dbrl_feature_point::clone()const
    {
        return new dbrl_feature_point(*this);
    }

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_feature_point* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_feature_point* &p)
    {
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_feature_point();
        p->b_read(is);
        }
    else
        p = 0;

    }
