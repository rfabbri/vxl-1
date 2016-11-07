//this is /contrib/bm/smw/smw_world.h
#ifndef SMW_WORLD_H_
#define SMW_WORLD_H_


//includes node, clock, vcl etc.
#include "smw/smw_graph.h"

#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <vil/vil_save.h>

namespace world_typedefs{
    typedef vcl_vector<vcl_vector< smw_graph > > world;
};

using namespace world_typedefs;
    
class smw_world
{
public:
    smw_world(unsigned const& rows, unsigned const& cols, 
              time_type const& t_forget = 30,
              float const& prob_thresh = 0.0002,
              float const& init_var = 1.0f, 
              float const& min_var = 0.001f);

    smw_world(vil_image_view<float> const& grey_img, 
              time_type const& t_forget = 30,
              float const& prob_thresh = 0.0002, 
              float const& init_var = 1.0f,
              float const& min_var = 0.001f);

    smw_world(vil_image_view<float> const& grey_img,
              unsigned const& x1, unsigned const& y1,
              unsigned const& x2, unsigned const& y2,
              time_type const& t_forget = 30,
              float const& prob_thresh = 0.0002,
              float const& init_var = 1.0f,
              float const& min_var = 0.001f);

    ~smw_world(){}

    unsigned rows(){return rows_;}

    unsigned cols(){return cols_;}

    bool update(vil_image_view<float> const& grey_img);

    vil_image_view<vxl_byte> change_map();
    
    //give the absolute coordinates, the coordinate of the pixel
    //relative to the image frame. The function will find the 
    //correct graph in the sub image, if we made a world on a sub image.
    bool write_dot_file(vcl_ostream& os, unsigned const& col,
                        unsigned const& row);

    bool write_dot_file_full(vcl_ostream& os, unsigned const& col,
                             unsigned const& row);

    vil_image_view<vxl_byte> max_prob_map();

    bool write_max_prob_map(vcl_string const& prob_path);

    // bool write_prob_map(vcl_string const& prob_path, 
    //                     vil_image_view<float> const& grey_img);

    //give fnc an image and it will highlight the area in which the world
    //is operating. This will also create a green square with side_length 
    //(really 2 x sidelength) and position gx,gy. Origin is top left.
    vil_image_view<vil_rgb<vxl_byte> > 
    highlight_region(vil_image_view<vil_rgb<vxl_byte> > const& src,
                     unsigned gx, unsigned gy,
                     unsigned side_length);
        

    world world_;
private:
    unsigned rows_;

    unsigned cols_;

    unsigned x1_, y1_;

    unsigned x2_, y2_;
    
    float prob_thresh_;

    time_type t_forget_;
    
    float init_var_;
    
    float min_var_;
    
    smw_frame_clock* world_clk_ptr_;

};

#endif //SMW_WORLD_H_
