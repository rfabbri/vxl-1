//this is contrib/bm/dncn/dncn_image_point_2d.h
#ifndef dncn_image_point_2d_h_
#define dncn_image_point_2d_h_

#include<vgl/vgl_point_2d.h>

class dncn_image_point_2d
{
public:
    dncn_image_point_2d(){}

    dncn_image_point_2d( vgl_point_2d<unsigned> point, vcl_map<unsigned,unsigned> fl ):img_point(point), frame_label(fl){}

    dncn_image_point_2d( unsigned const x, unsigned const y, vcl_map<unsigned,unsigned> fl ):frame_label(fl){ img_point = vgl_point_2d<unsigned>(x,y);}

    ~dncn_image_point_2d(){}

    vgl_point_2d<unsigned> img_point;

    //the point is gt for only those frames which are indicated by the key. the value represents the class label.
    vcl_map<unsigned, unsigned> frame_label;
};

#endif //dncn_image_point_2d_h_