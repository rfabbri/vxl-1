//this is contrib/bm/ncn/ncn_image_point.h
#ifndef ncn_image_point_h_
#define ncn_image_point_h_
//:
// \file
// \brief A simple class to store an image location
// \Author Brandon A. Mayer
// \date June 2, 2010
//
// \A simple class to store an image location
//
// \verbatim
//  No modifications Yet.
// \endverbatim
#include<vnl/vnl_math.h>

#include<vcl_ostream.h>
#include<vcl_set.h>

class ncn_image_point
{
public:
    ncn_image_point(): x_(unsigned(0)),y_(unsigned(0)){}
    ncn_image_point(unsigned row, unsigned col): x_(row),y_(col){}
    unsigned x() const {return x_;}
    unsigned y() const {return y_;}
    void set_x(unsigned& x){x_ = x;}
    void set_y(unsigned& y){y_ = y;}
    // will sort by row major location in image
    bool operator < (ncn_image_point const& p) const
    {
        if(y_ != p.y())
            return y_ < p.y();
        else
            return x_ < p.x();
    }
    
   //friend vcl_ostream& operator << (vcl_ostream& os, ncn_image_point const& p);

private:
    friend vcl_ostream& operator << (vcl_ostream& os, ncn_image_point const& p);
    unsigned x_;
    unsigned y_;
    
};



#endif //ncn_image_point_h_
