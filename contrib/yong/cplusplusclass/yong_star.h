
#ifndef yong_star_h
#define yong_star_h


#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <cplusplusclass/yong_star_sptr.h>

class yong_star : public vbl_ref_count 
{
private:
    //Data Memmbers
    double center_x_, center_y_;
    unsigned arm_number_;
    double arm_size_;
    double orientation_;

public:    
    //Constructor, destructor, initializers
    yong_star():center_x_(0), center_y_(0), arm_number_(3), arm_size_(1.0), orientation_(0.0){}
    yong_star(unsigned const& x,unsigned const& y,unsigned const& arm_num, double const& arm_size, double const& orientaion) : center_x_(x), center_y_(y), arm_number_(arm_num), arm_size_(arm_size), orientation_(orientaion){}
    ~yong_star() {}//vcl_cout<<"destructing rectangle"<< vcl_endl;}

    double center_x(){return center_x_;}
    void set_center_x(double x){center_x_ = x;}
    double center_y(){return center_y_;}
    void set_center_y(double y){center_y_ = y;}
    unsigned arm_number(){return arm_number_;}
    void set_arm_number(unsigned arm_n){arm_number_ = arm_n;}
    double arm_size(){return arm_size_;}
    void set_arm_size(double arm_s){arm_size_ = arm_s;}
    double orientation(){return orientation_;}
    void set_orientation(double ori){orientation_ = ori;}

    bool operator< (yong_star star_1) { return (this->arm_size() < star_1.arm_size());}
    static bool less_than (yong_star_sptr star_1, yong_star_sptr star_2) { return (*star_1)<(*star_2);}


    //: output description to stream
    friend
        vcl_ostream&  operator<<(vcl_ostream& s,  yong_star &star);

};


#endif

