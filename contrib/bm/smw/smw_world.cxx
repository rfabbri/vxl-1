//this is /contrib/bm/smw/smw_world.cxx

#include "smw/smw_world.h"

smw_world::smw_world(unsigned const& rows, unsigned const& cols,
                     time_type const& t_forget, float const& prob_thresh, 
                     float const& init_var, float const& min_var): 
    rows_(rows), cols_(cols), t_forget_(t_forget), prob_thresh_(prob_thresh),
    init_var_(init_var),min_var_(min_var),x1_(0),y1_(0),x2_(0),
    y2_(0)
{
    world_.resize(rows_);
    for(unsigned i = 0; i < rows_; ++i)
        world_[i].resize(cols_);
}//end smw_world::smw_world

smw_world::smw_world(vil_image_view<float> const& img, 
                     time_type const& t_forget, 
                     float const& prob_thresh,
                     float const& init_var, 
                     float const& min_var):
    t_forget_(t_forget),
    prob_thresh_(prob_thresh), 
    init_var_(init_var),
    min_var_(min_var), 
    rows_(img.nj()), 
    cols_(img.ni()), 
    x1_(0),y1_(0),x2_(img.ni()),y2_(img.nj())
{
    world_.resize(rows_);
    for(unsigned row = 0; row < rows_; ++row)
        for(unsigned col = 0; col < cols_; ++col)
            world_[row].push_back( smw_graph(t_forget_,prob_thresh_,
                                             init_var_,min_var_) );
     
    //we can do this in one loop but for readability we'll just do it 
    //this way.
    this->update(img);

    //world_clk_ptr_->increment_time();
}//end smw_world::smw_world

smw_world::smw_world(vil_image_view<float> const& img,
                     unsigned const& x1, unsigned const& y1,
                     unsigned const& x2, unsigned const& y2,
                     time_type const& t_forget,
                     float const& prob_thresh,
                     float const& init_var, float const& min_var):
    x1_(x1),y1_(y1),x2_(x2),y2_(y2),t_forget_(t_forget),
    prob_thresh_(prob_thresh),init_var_(init_var),min_var_(min_var)
{
    if(x2_ > x1_ && y2_ > y1_)
    {
        rows_ = y2_ - y1_;
        cols_=  x2_ - x1_;
        world_.resize(rows_);
        for(unsigned row = 0; row < rows_; ++row)
            for(unsigned col = 0; col < cols_; ++ col)
                world_[row].push_back( smw_graph(t_forget_,prob_thresh_,
                                                 init_var_,min_var_) );
    }
    else
        vcl_cerr << "ERROR: SUB-IMAGE PARAMETER FAILURE" << vcl_flush;

    this->update(img);
    
    //world_clk_ptr_->increment_time();
}//end smw_world::smw_world

bool smw_world::update(vil_image_view<float> const& grey_img)
{
        for(unsigned row = 0; row < rows_; ++row)
            for(unsigned col = 0; col < cols_; ++col)
                world_[row][col].update(grey_img(col+x1_,row+y1_,0));

        world_clk_ptr_->increment_time();
}

vil_image_view<vxl_byte> smw_world::change_map()
{
    vil_image_view<vxl_byte> change_map_img(cols_,rows_);
    for(unsigned row = 0; row < rows_; ++row)
        for(unsigned col = 0; col < cols_; ++col)
            if( world_[row][col].change() == true )
                change_map_img(col,row) = 255;
            else
                change_map_img(col,row) = 0;

    return change_map_img;
}//end smw_world::change_map

bool smw_world::write_dot_file(vcl_ostream& os, unsigned const& row, 
                               unsigned const& col)
{
    //translate the world coordinates to the subimage that the
    //world is working on
    //if we are working on the entire image this is still fine as
    //x1=y1=0
    unsigned world_col = col - x1_;
    unsigned world_row = row - y1_;
    return world_[world_row][world_col].write_dot_file(os);
}//end smw_world::write_dot_file

bool smw_world::write_dot_file_full(vcl_ostream& os, unsigned const& col,
                                    unsigned const& row)
{
    unsigned world_col = col - x1_;
    unsigned world_row = row - y1_;
    return world_[world_row][world_col].write_dot_file_full(os);
}

vil_image_view<vxl_byte> smw_world::max_prob_map()
{
    vil_image_view<float> flt_img(cols_,rows_);
    vil_image_view<vxl_byte> ret_img(cols_,rows_);

    for(unsigned row = 0; row < rows_; ++row)
        for(unsigned col = 0; col < cols_; ++ col)
            flt_img(col,row) = world_[row][col].max_prob();

    vil_convert_stretch_range_limited(flt_img,ret_img,0.0f,1.0f);

    return ret_img;

}//end smw_world::prob_map

bool smw_world::write_max_prob_map(vcl_string const& prob_path)
{
    vil_image_view<vxl_byte> prob_map = this->max_prob_map();
    vil_save(prob_map,prob_path.c_str());
}//end smw_world::write_prob_map

// bool smw_world::write_prob_map(vcl_string const& prob_path,
//                     vil_image_view<float> const& grey_img)
// {
    
// }//end smw_world::write_prob_map

vil_image_view<vil_rgb<vxl_byte> >
smw_world::highlight_region(vil_image_view<vil_rgb<vxl_byte> > const& src,
                            unsigned gx, unsigned gy,
                            unsigned side_length)
{
    unsigned ni = src.ni();
    unsigned nj = src.nj();
    unsigned nplanes = src.nplanes();
    vil_image_view<vil_rgb<vxl_byte> > ret(ni,nj,1);
    int obs_r, obs_g, obs_b;

    //if gx,gy is in the ROI (Region of Interest)
    if( ((gy > y1_) && (gy < y2_)) && ((gx > x1_) && (gx < x2_)) )
    {
        for(unsigned row = 0; row < nj; ++row)
            for(unsigned col = 0; col < ni; ++col)
            {
                //if your inside the box
                if( (row < y1_ || row > y2_) || (col < x1_ || col > x2_) )
                {
                    obs_r = src(col,row).r - 60;
                    obs_g = src(col,row).g - 60;
                    obs_b = src(col,row).b - 60;
                    if( obs_r < 0 )
                        obs_r = 0;
                    if( obs_g < 0 )
                        obs_g = 0;
                    if (obs_b < 0 )
                        obs_b = 0;
                    ret(col,row) = vil_rgb<vxl_byte>(obs_r,obs_g,obs_b);
                }
                else
                    ret(col,row) = src(col,row);

                if( ( (row >= gy - 10) && (row <= gy + 10) ) &&
                    ( (col >= gx - 10) && (col <= gx + 10) ) )
                {
                    ret(col,row) = vil_rgb<vxl_byte>(0,255,0);
                }

            }
        return ret;
    }
    else
    {
        vcl_cerr << "(gx,gy) as specified do not fall in the area in which" 
                 << " the world is operating. " << vcl_endl;
        vcl_cerr << "(x1_,y1_) = " << "(" << x1_ << ", " << y1_ << ")" 
                 << vcl_endl;
        vcl_cerr << "(x2_,y2_) = " << "(" << x2_ << ", " << y2_ << ")"
                 << vcl_endl;
        vcl_cerr << "(gx,gy) = " << "(" << gx << ", " << gy << ")" 
                 << vcl_endl;
    }
}//end smw_world::highlight_region
