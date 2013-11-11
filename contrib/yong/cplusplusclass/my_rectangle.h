
#ifndef my_rectangle_h
#define my_rectangle_h
// This is 
//:
// \file
// \brief  
// \author Isa Restrepo (mir@lems.brwon.edu)
// \date 1/16/2005
//
// \verbatim
//  Modifications
// 
// \endverbatim
/********************************************/

# include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <cplusplusclass/my_rectangle_sptr.h>

class my_rectangle : public vbl_ref_count 
{
    //Data Memmbers
    unsigned width_, height_;

public:    
    //Constructor, destructor, initializers
    my_rectangle():width_(0), height_(0){}
    my_rectangle(unsigned const& w,unsigned const& h) : width_(w), height_(h){}
    ~my_rectangle() {}//vcl_cout<<"destructing rectangle"<< vcl_endl;}

public:
    unsigned height(){return height_;}
    void set_height(unsigned h){height_ = h;}
    unsigned width(){return width_;}
    void set_width(unsigned w){width_ = w;}
    unsigned area(){return height_*width_;}

    // when is it better to use const, & and inline

    bool operator< (my_rectangle r1) { return (this->area() < r1.area());}
    static bool less_than (my_rectangle_sptr r1, my_rectangle_sptr r2) { return (*r1)<(*r2);}


    //: output description to stream
    friend
        vcl_ostream&  operator<<(vcl_ostream& s,  my_rectangle &r);



};


#endif

