#include "mygui_yong_star_tableau.h"


#include <vcl_string.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_style.h>
#include <math.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

#define MY_PI 3.1415926
#define DISTANCE_SQUARE_THRESHOLD 100


//-----------------------------------------------------------------------------
mygui_yong_star_tableau::mygui_yong_star_tableau(const char* n) : name_(n), style_(vgui_style::new_style())
{
  style_->rgba[0] = 1.0f;
  style_->rgba[1] = 1.0f;
  style_->rgba[2] = 0.0f;

  style_->line_width = 1;
  style_->point_size = 3;

  current_selected_star_ = -1;    // no selected star;
  current_highlighted_star_ = -1; 

}


//-----------------------------------------------------------------------------

vcl_string mygui_yong_star_tableau::type_name() const
{
  return "mygui_yong_star_tableau";
}


//-----------------------------------------------------------------------------
bool mygui_yong_star_tableau::handle(vgui_event const& e)
{

    if (e.type == vgui_MOTION && e.type != vgui_BUTTON_DOWN)
    {
        float pointx, pointy;
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
        int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);

        // find out the highlighted star
        unsigned i = 0;
        
        for (vcl_vector<yong_star_sptr>::iterator vit = this->star_list_.begin();    vit != this->star_list_.end(); vit++, i++)
        {
            double x = (*vit)->center_x();
            double y = (*vit)->center_y();

            double distance_square = (intx - x)*(intx-x) + (inty - y)*(inty - y);
            if( distance_square <= DISTANCE_SQUARE_THRESHOLD)
            {
                current_highlighted_star_ = i;
                break;
            }
        }

        if( i >= star_list_.size())
        {
            if(current_highlighted_star_ != -1)
            {
                current_highlighted_star_ = -1;
                this->draw();
            }
            current_highlighted_star_ = -1;
        }
        else
        {
            this->draw();
        }

        return true; // event has been used
    }

    if (e.type == vgui_BUTTON_DOWN)
    {
        float pointx, pointy;
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
        int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);

        if(current_selected_star_ == -1)
        {
            if(    current_highlighted_star_ != -1)
            {
                current_selected_star_ = current_highlighted_star_;
            }
        }
        else
        {
            this->star_list_[current_selected_star_]->set_center_x(intx);
            this->star_list_[current_selected_star_]->set_center_y(inty);
            current_selected_star_ = -1;
        }

        this->draw();

        return true; // event has been used
    }

    if(e.type ==  vgui_KEY_DOWN && e.key == 'r' && e.modifier == vgui_CTRL)
    {
        if(current_selected_star_ != -1)
        {
            double ori = this->star_list_[current_selected_star_]->orientation();
            this->star_list_[current_selected_star_]->set_orientation(ori + 0.3);
            this->draw();
        }    
    }
    if(e.type ==  vgui_KEY_DOWN && e.key == 'l' && e.modifier == vgui_CTRL)
    {
        if(current_selected_star_ != -1)
        {
            double arm_s = this->star_list_[current_selected_star_]->arm_size();
            this->star_list_[current_selected_star_]->set_arm_size(arm_s + 3);
            this->draw();
        }    
    }
    if(e.type ==  vgui_KEY_DOWN && e.key == 's' && e.modifier == vgui_CTRL)
    {
        if(current_selected_star_ != -1)
        {
            double arm_s = this->star_list_[current_selected_star_]->arm_size();
            if (arm_s > 3)
            {
                this->star_list_[current_selected_star_]->set_arm_size(arm_s - 3);
            }

            this->draw();
        }    
    }

    //  We are not interested in other events,
    //  so pass event to base class:
    return vgui_easy2D_tableau::handle(e);
}

bool mygui_yong_star_tableau::draw()
{
    // iterate all the stars and draw them
    this->clear();

    unsigned j = 0;
    for (vcl_vector<yong_star_sptr>::iterator vit = this->star_list_.begin();    vit != this->star_list_.end(); vit++, j++)
    {
        if(j == this->current_selected_star_)
        {
//            this->style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3, 3, 1.0);
            this->set_foreground(1,0,0);
            this->set_line_width(3);
        }
        else if(j == this->current_highlighted_star_)
        {
//            this->style_ = vgui_style::new_style(0.0, 0.0, 1.0, 3, 2, 1.0);
            this->set_foreground(0,0,1);
            this->set_line_width(2);
        }
        else
        {
//            this->style_ = vgui_style::new_style(0.0, 1.0, 0.0, 3, 1, 1.0);
            this->set_foreground(0,1,0);
            this->set_line_width(1);            
        }

        double x = (*vit)->center_x();
        double y = (*vit)->center_y();
        unsigned arm_n = (*vit)->arm_number();
        double ori = (*vit)->orientation();
        double arm_s = (*vit)->arm_size();

        for(unsigned i=0; i<arm_n; i++)
        {
            double x1 = x + cos(ori+i*(2*MY_PI/arm_n))*arm_s;
            double y1 = y + sin(ori+i*(2*MY_PI/arm_n))*arm_s;
            this->add_line(x, y, x1, y1);
        }
        //vcl_cout << "redraw: "<< j << vcl_endl;
    }

    this->observers.notify();

    return true;
}

bool mygui_yong_star_tableau::set_star_list(vcl_vector<yong_star_sptr> list)
{
    star_list_ = list;
    this->draw();
    return true;

}

