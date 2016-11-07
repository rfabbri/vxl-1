#include "dbrl_id_point_2d.h"
#include "dbrl_id_point_2d_sptr.h"

void dbrl_id_point_2d::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    vsl_b_write(os, this->x_);
    vsl_b_write(os, this->y_);
    vsl_b_write(os, this->tangent_);
    vsl_b_write(os, this->id_);
    }

void dbrl_id_point_2d::b_read(vsl_b_istream &is) 
    {
    if (!is) return;
    short ver;
    vsl_b_read(is, ver);

    switch(ver)
        {
        case 1:
            vsl_b_read(is, this->x_);
            vsl_b_read(is, this->y_);
            vsl_b_read(is, this->tangent_);
            vsl_b_read(is, this->id_);

            break;

        case 2:
            vsl_b_read(is, this->x_);
            vsl_b_read(is, this->y_);
            vsl_b_read(is, this->id_);

        default:
            vcl_cerr << "I/O ERROR: dbctrk_tracker_curve::b_read(vsl_b_istream&)\n"
                << "           Unknown version number "<< ver << '\n';
            is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
            return;
        }
    }

short dbrl_id_point_2d::version() const
{
    return 1;
}


void vsl_b_write(vsl_b_ostream & os, const dbrl_id_point_2d * p)
{
    if (!p){
        vsl_b_write(os, false); // Indicate null pointer stored
    }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
    }
}
void vsl_b_read(vsl_b_istream &is,dbrl_id_point_2d * &p)
{
    delete(p);
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr){
        p = new dbrl_id_point_2d();
        p->b_read(is);
    }
    else
        p = 0;
}

//double dbrl_id_points_cmx(vcl_vector<dbrl_id_point_2d_sptr> points)
//{
//    if(points.size()==0)
//        return 0.0;
//    else
//    {
//        double xaccu=0;
//        for(unsigned i=0;i<points.size();i++)
//        {
//            xaccu+=points[i]->x();
//        }
//        xaccu/=points.size();
//        return xaccu;
//    }
//}
//double dbrl_id_points_cmy(vcl_vector<dbrl_id_point_2d_sptr> points)
//{
//    if(points.size()==0)
//        return 0.0;
//    else
//    {
//        double yaccu=0;
//        for(unsigned i=0;i<points.size();i++)
//        {
//            yaccu+=points[i]->y();
//        }
//        yaccu/=points.size();
//        return yaccu;
//    }
//}
