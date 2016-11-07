

#ifndef _dbrl_id_point_2d_
#define _dbrl_id_point_2d_

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

class dbrl_id_point_2d:public vbl_ref_count
    {
    public:
        dbrl_id_point_2d(){};
        dbrl_id_point_2d(double x,double y, int id){x_=x;y_=y;id_=id;weight_=0;}
        dbrl_id_point_2d(double x,double y, double tangent,int id){x_=x;y_=y;tangent_=tangent;id_=id;weight_=0;}
        ~dbrl_id_point_2d(){};
        double x(){return x_;}
        double y(){return y_;}
        double tangent(){return tangent_;}
        double weight(){return weight_;}

        void set_weight(double w){weight_=w;};
        int id(){return id_;}

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    //: Return IO version number;
    short version() const;
    private:
        double x_;
        double y_;
        double tangent_;
        double weight_;
        int id_;
    };

//: Binary save dbctrk_tracker_curve_sptr to stream.
void vsl_b_write(vsl_b_ostream & os, const dbrl_id_point_2d * p);
//: Binary load dbctrk_tracker_curve_sptr to stream
void vsl_b_read(vsl_b_istream &is,dbrl_id_point_2d * &p);


//double cmx(vcl_vector<dbrl_id_point_2d_sptr> points);
//double cmy(vcl_vector<dbrl_id_point_2d_sptr> points);

#endif
