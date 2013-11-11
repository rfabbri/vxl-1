// This is brcv/seg/dbdet/vis/dbdet_image_seg_dlvwr_tool.cxx
//:
// \file

#include "dbdet_image_seg_dlvwr_tool.h"

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_command.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <bgui/bgui_image_tableau.h>
#include <bdgl/bdgl_curve_algs.h>

#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <bvis1/bvis1_manager.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>


//: A vgui command to set the active group
class dbdet_image_seg_change_params_command : public vgui_command
{
  // member variables
public:
  dbdet_image_seg_dlvwr_tool* tool_;
public:  
  dbdet_image_seg_change_params_command(dbdet_image_seg_dlvwr_tool* tool) :
    tool_(tool)
  {};
  ~dbdet_image_seg_change_params_command(){}
  dbdet_image_seg_dlvwr_tool* tool() {return this->tool_; }
  void execute()
  {
    this->tool()->set_current_state(image_seg_CHANGE_PARAMS);

    dbdet_dlvwr_params params = tool_->intsciss().params();
    osl_canny_ox_params canny_params;

    if (tool_->get_intscissors_params(params, canny_params))
    {
      this->tool_->intsciss().set_params(params);
      dbdet_image_seg_dlvwr_tool::last_used_params = params;
      // set new seed points
      vgl_point_2d<int > pt = this->tool_->intsciss().seed();
      this->tool()->intsciss().compute_optimal_paths(pt.x(), pt.y());
    }
    this->tool()->set_current_state(image_seg_NORMAL);
  }
};



//: last used livewire parameters
dbdet_dlvwr_params dbdet_image_seg_dlvwr_tool::last_used_params = dbdet_dlvwr_params();



// ------------------------------------------------------------------
//: Constructor - protected
dbdet_image_seg_dlvwr_tool::
dbdet_image_seg_dlvwr_tool() :
dbdet_image_seg_tool(),
current_state_(image_seg_NORMAL),
//tableau_(0),
seed_picked_(false),
//image_(0),
lvwr_initialzed_(false),
intsciss_()
{
  // define different gestures of the tool
  gesture_manual_points = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_clear=vgui_event_condition(vgui_key('e'),vgui_MODIFIER_NULL,true);
  gesture_smooth = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);

  gesture_start = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_freeze1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_freeze2 = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);

  gesture_reset = vgui_event_condition(vgui_key('r'), vgui_CTRL, true);
  gesture_new_frame = vgui_event_condition(vgui_key('f'), vgui_CTRL, true);

  this->contour_style_ = vgui_style::new_style(0.0f, 0.0f, 3.0f, 3.0f, 1.0f);
  this->path_style_ = vgui_style::new_style(3.0f, 0.0f, 0.0f, 3.0f, 1.0f);
  this->manual_point_style_ = vgui_style::new_style(0.0f, 3.0f, 0.0f, 3.0f, 1.0f);
}



//: Allow the tool to add to the popup menu as a tableau would
void dbdet_image_seg_dlvwr_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  menu.add("Change tool params", 
    new dbdet_image_seg_change_params_command(this) );
}



// ------------------------------------------------------------------
//: This is called when the tool is activated
void dbdet_image_seg_dlvwr_tool::
activate() 
{
  //this->select_io_storage();
  dbdet_image_seg_tool::activate();

  this->contour_.clear();
  this->manual_points_.clear();
  this->path_.clear();

  this->seed_picked_ = false;
  this->lvwr_initialzed_ = false;

  osl_canny_ox_params canny_params;
  dbdet_dlvwr_params iparams = dbdet_image_seg_dlvwr_tool::last_used_params;
  
  // pop up a window to get parameters from user
  //this->get_intscissors_params(iparams, canny_params);
  this->intsciss_.set_params(iparams);
  this->intsciss_.set_canny_params(canny_params);

  vcl_cout << "\nLivewire tool is activated!!!\n";
}


//=========================================================
//      EVENT HANDLERS
//=========================================================

//: the handle function
bool dbdet_image_seg_dlvwr_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view)
{
  if (this->gesture_reset(e))
  {
    this->handle_reset();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (this->gesture_new_frame(e))
  {
    vgui_dialog dialog("dLivewire");
    dialog.message("Clear all data and start a new frame?");
    if (dialog.ask())
    {
      this->activate();
    }
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  int mouse_x = int(ix+0.5);
  int mouse_y = int(iy+0.5);

  // Event: manually select edge points
  if (!this->seed_picked_ && this->gesture_manual_points(e))
  {
    this->handle_manual_points(mouse_x, mouse_y);
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // Event: choose initial seed point)
  if (!this->seed_picked_ && this->gesture_start(e)) 
  { 
    this->handle_pick_initial_seed(mouse_x, mouse_y);
    vgui::out << "Initial seed selected" << vcl_endl;

    //do the following no matter what mode we are in
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  } // end of first event - choose initial seed point


  // Event: mouse motions
  if (this->seed_picked_ && (e.type == vgui_MOTION)) 
  {         
    int mode = 0;
    if (e.modifier == vgui_CTRL) mode = 1;
    if (e.modifier == vgui_SHIFT) mode = 2;
    this->handle_mouse_motion(mouse_x, mouse_y, mode );
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // Event: mouse clicks
  if (this->seed_picked_ && 
    (this->gesture_freeze1(e) || this->gesture_freeze2(e))) 
  {         
    this->handle_freeze_path();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // Event: smooth the curves for nicer results
  if (this->seed_picked_ && this->gesture_smooth(e)) 
  { 
    this->handle_smooth_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  // Event: delete end parts of contour
  if (this->seed_picked_ && this->gesture_clear(e)) 
  {
    this->handle_delete_end_segments();
    vgui::out << "last contour pt(x,y)=(" << this->contour_.back().x() <<
         "," << this->contour_.back().y() << ")" << vcl_endl;
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  
  // Event: Redraws all curves, windows, etc
  if (e.type == vgui_DRAW_OVERLAY) 
  {
    this->handle_draw_overlay();
    return false;
  }


  // Event: save the contours as a .con file
  if (this->seed_picked_ && e.type == vgui_MOUSE_PRESS && e.button == vgui_RIGHT) 
  {
    this->handle_save_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    vcl_cout<<"We are at the end of the Save event"<<"\n";
    return false;
  }   


  return false;
}    // end handle()


//: Handle reset event
bool dbdet_image_seg_dlvwr_tool::
handle_reset()
{
  this->seed_picked_ = false;
  this->contour_.clear();
  this->path_.clear();
  return false;
}



//: Handle manually select edge points
bool dbdet_image_seg_dlvwr_tool::
handle_manual_points(int mouse_x, int mouse_y)
{
  if (mouse_x < 0 || mouse_x >= (int)this->image_->ni() || 
    mouse_y < 0 || mouse_y >= (int)this->image_->nj())
    return false;
  this->manual_points_.push_back(vgl_point_2d<int >(mouse_x, mouse_y));
  return true;
}


//: Handle pick initial seed
bool dbdet_image_seg_dlvwr_tool::
handle_pick_initial_seed(int mouse_x, int mouse_y)
{
  vcl_cout<<"We are in Event 1: picking initial seed pt"<<"\n";

  if (! this->lvwr_initialzed_)
  {
    this->init_livewire();
    this->lvwr_initialzed_ = true;
  }

  // range check of mouse position
  if (!(mouse_x >= 0 && mouse_x < (int)this->image_->ni() && 
    mouse_y >= 0 && mouse_y < (int)this->image_->nj())) 
  {
    vcl_cout << "Not a valid seed point on image!!\n";
    vcl_cout << "Select a new seed point inside the image!!\n"; 
    return false;
  }
  
  //start over, with a new set of mouse coor
  this->contour_.clear();
  this->contour_.push_back(vgl_point_2d<double >(mouse_x, mouse_y));
  this->intsciss_.compute_optimal_paths(mouse_x, mouse_y);
  this->seed_picked_ = true;
  
  vcl_cout << "Paths are computed...\n";
  return false;
}


//------------------------------------------------------------------------
//: Handle mouse motion
bool dbdet_image_seg_dlvwr_tool::
handle_mouse_motion(double newx, double newy, int mode)
{   
  int free_x = (int)(newx+0.5);
  int free_y = (int)(newy+0.5);

    // check to ensure new pts are not the same as the seed pt!
  if (free_x ==this->intsciss_.seed().x() && 
    free_y ==this->intsciss_.seed().y())
  {
      return false;
  }

  // ignore if points is outside the search box
  if (!this->intsciss_.explored_region().contains(free_x, free_y))
  {
    return false;
  }

  // available modes: 
  // 0 - livewire with cursor snapped to best point in neighborhood
  // 1 - livewire but using mouse position directly
  // 2 - draw straight lines
  vgl_point_2d<int > endpoint;
  if (mode == 0 || mode == 1)
  {
    endpoint = (mode==0) ? 
      this->intsciss().cursor_snap(free_x, free_y) : vgl_point_2d<int >(free_x, free_y);

    this->intsciss_.get_path(endpoint.x(), endpoint.y(), this->path_);
  }
  // draw straight lines
  else if (mode == 2)
  {
    endpoint.set(free_x, free_y);
    vgl_vector_2d<float > v(newx-this->intsciss().seed().x(), 
      newy - this->intsciss().seed().y());
    int len = (int) v.length();
    normalize(v);

    // sample points along the line segment
    this->path_.clear();
    for (int s=0; s<len; ++s)
    {
      this->path_.push_back(vgl_point_2d<int >((int)(free_x-s*v.x()+0.5), 
        (int)(free_y-s*v.y()+0.5)));
      
    }
  }

  
  vgui::out << "(x,y)=(" << free_x << "," << free_y << 
    ") endpoint=(" << endpoint.x() << "," << endpoint.y() <<") grad_cost=" << this->intsciss().grad_mag_cost(endpoint.x(), endpoint.y()) << 
    " log_cost= " << this->intsciss().log_cost(endpoint.x(), endpoint.y()) << 
    " accum_cost= " << this->intsciss().accum_cost(endpoint.x(), endpoint.y()) << vcl_endl;
  

  return false;
}





//: Handle freeze path
bool dbdet_image_seg_dlvwr_tool::
handle_freeze_path()
{
  if (this->path_.size() < 2) return false;
  if (this->current_state() == image_seg_CHANGE_PARAMS) return false;
  
  // move points in `path_' to contour
  vgl_point_2d<int > pt;
  for (int m=this->path_.size()-2; m >=0; --m)
  {
    pt = this->path_[m];
    this->contour_.push_back(vgl_point_2d<double >(pt.x(), pt.y()));
  }
  this->path_.clear();

  // set new seed points
  this->intsciss_.compute_optimal_paths(pt.x(), pt.y());
  return false;
}

///// FIX TANGENT COMPUTATION !!!!!!!!!!!!!!!!






//: Handle smoothing contours
bool dbdet_image_seg_dlvwr_tool::
handle_smooth_contour()
{ 
  // smooth the contours
  bdgl_curve_algs::smooth_curve(this->contour_, 1.0);

  // the smoothing algo added 6 more points to the curve (3 at beginning and 
  // 3 at the end). Need to remove them
  //this->contour_.pop_front();
  this->contour_.pop_back();
  this->contour_.pop_back();
  this->contour_.pop_back();
  this->contour_.erase(this->contour_.begin());
  this->contour_.erase(this->contour_.begin());
  this->contour_.erase(this->contour_.begin());

  // update seed points
  int new_seed_x = int(0.5+this->contour_.back().x());
  int new_seed_y = int(0.5+this->contour_.back().y());
  this->intsciss_.compute_optimal_paths(new_seed_x, new_seed_y);
  return false;
}





//: Handle delete end segments on contours and medial curve
bool dbdet_image_seg_dlvwr_tool::
handle_delete_end_segments()
{
  unsigned int num_pts = 10;
  // if the number of points in contour is <= num_pts+3, delete the whole curve
  // except for the initial seed
  if (this->contour_.size() <= num_pts+3)
  {
    vgl_point_2d<double > init_seed(this->contour_.front());
    this->contour_.clear();
    this->contour_.push_back(init_seed);
  }
  // the mouse_curve is long enough
  else
  {
    // in general case, remove num_pts from mouse_curve and 
    // shorten the two contours on two sides accordingly
    
    // remove num_pts from mouse_curve
    for (unsigned int i = 0; i < num_pts; ++i)
    {
      this->contour_.pop_back();
    }
  
    // reset the seed points
    vgl_point_2d<double > seed = this->contour_.back();
    
    // reset seed points in livewire
    this->intsciss_.compute_optimal_paths((int)seed.x(), (int)seed.y());
  }
  return false;
}


//// ------------------------------------------------------------------------
////: Handle save contour event
//bool dbdet_image_seg_dlvwr_tool::
//handle_save_contour()
//{
//  vcl_cout<<"\nIn the Save event " <<"\n";
//  
//  
//  vcl_string image_filename = ".con";
//  
//  // pop up a dialog to ask for output file name
//  static vcl_string regexp = "*.*";
//  vgui_dialog save_dl("Save contour");
//  save_dl.inline_file("Filename: ", regexp, image_filename);
//
//  if (save_dl.ask()) 
//  {
//    //open the file
//    vcl_ofstream fs2(image_filename.c_str()); 
//    if (!fs2) 
//    {
//      vcl_cout << "Problems in opening file: " << image_filename << "\n";
//      return false;
//    }
//    else 
//    {
//      // save data to the ".con" file.
//      fs2 << "CONTOUR\nOPEN\n" << this->contour_.size() << "\n";
//      
//      // first (left) contour
//      for (unsigned int i=0; i<this->contour_.size(); ++i) 
//      {
//        fs2 << this->contour_[i].x() << " ";
//        fs2 << this->contour_[i].y() << "\n";
//      }
//
//      // finish saving the .con file
//      fs2.close();
//
//      vcl_cout << "Contour written to the specified files!\n";    
//      this->seed_picked_ = false;
//
//      // clear old data
//      this->contour_.clear();
//    }
//  }
//  return false;
//}




// ------------------------------------------------------------------------
//: Handle save contour event
bool dbdet_image_seg_dlvwr_tool::
handle_save_contour()
{
  vcl_cout<<"\nIn the Save event " <<"\n";
   
  // pop up a dialog to ask whether user wantst to save the contour
  //bool save_contour = false;
  vgui_dialog save_dl("Save contour?");
  //save_dl.checkbox("save the contour?", save_contour);

  if (save_dl.ask()) 
  {
    vcl_vector<vsol_point_2d_sptr > pts;
    
    for (unsigned int i=0; i<this->contour_.size(); ++i) 
    {
      pts.push_back(new vsol_point_2d(this->contour_[i].x(),
        this->contour_[i].y()));
    }

    vsol_polyline_2d_sptr polyline = new vsol_polyline_2d(pts);

    // push the polyline to the storage
    bpro1_storage_sptr storage = 
      bvis1_manager::instance()->repository()->get_data_by_name(this->vsol2d_name());

    assert(storage->type() == "vsol2D");
    
    vidpro1_vsol2D_storage_sptr vsol_storage = 
      static_cast<vidpro1_vsol2D_storage* >(storage.ptr());

    vsol_storage->add_object(polyline->cast_to_spatial_object(), this->image_name());

    bvis1_manager::instance()->display_current_frame();
    

    // reset seed
    this->seed_picked_ = false;

    // clear old data
    this->contour_.clear();
  }
  return false;
}









// ------------------------------------------------------------------------
//: Handle draw overlay event
bool dbdet_image_seg_dlvwr_tool::
handle_draw_overlay()
{
  // draw the manual edge points
  
  this->manual_point_style_->apply_all();;
  vgui_soview2D_point soview_point;
  vgui_soview2D_lineseg soview_line;
  for (unsigned int m=0; m < this->manual_points_.size(); ++m)
  {
    soview_point.x = float(this->manual_points_[m].x());
    soview_point.y = float(this->manual_points_[m].y());
    soview_point.draw();
  }


  if (this->seed_picked_) 
  {
    this->contour_style_->apply_all();

    // draw livewire search box
    // box dimensions
    vgl_box_2d<int > box = this->intsciss_.explored_region(); 

    // construct 4 points of the box
    float points_x[] = {box.min_x(), box.max_x(), box.max_x(), box.min_x()};
    float points_y[] = {box.min_y(), box.min_y(), box.max_y(), box.max_y()};

    vgui_soview2D_polygon box_vis(4, points_x, points_y);
    
    // draw the box
    box_vis.draw();

    // draw the contour
    this->contour_style_->apply_all();
  
    for (unsigned int m = 0; m<this->contour_.size(); m++)
    {
      soview_point.x = (float)this->contour_[m].x();
      soview_point.y = (float)this->contour_[m].y();
      soview_point.draw();
    }  

    // draw the optimized path return livewire
    this->path_style_->apply_all();
    for (unsigned int m=0; m<this->path_.size(); ++m)
    {
      
      soview_point.x = (float)this->path_[m].x();
      soview_point.y = (float)this->path_[m].y();
      soview_point.draw();

      soview_line.x0 = soview_point.x;
      soview_line.y0 = soview_point.y;
      ;
      vgl_vector_2d<float > v = this->intsciss().grad_at(this->path_[m].x(), this->path_[m].y());
      normalize(v);
      soview_line.x1 = soview_point.x + v.x()/2;
      soview_line.y1 = soview_point.y + v.y()/2;
      soview_line.draw();
    } 

    
  }
  return false;
}


//-----------------------------------------------------------------
//: intialize livewires
bool dbdet_image_seg_dlvwr_tool::
init_livewire()
{ 
  // convert to gray scale if necessary
  vil_image_view_base_sptr view_sptr = 
    vil_convert_cast(float(), this->image_->get_view());
  vil_image_view< float > image_view = *view_sptr;
  

  vil_image_view< float > greyscale_view;
  if( image_view.nplanes() == 3 ) 
  {
    vil_convert_planes_to_grey( image_view , greyscale_view );
  }
  else if ( image_view.nplanes() == 1 ) 
  {
    greyscale_view = image_view;
  }
  else 
  {
    vcl_cerr << "Returning false. nplanes(): " << image_view.nplanes() << vcl_endl;
    return false;
  }

  this->intsciss_.set_image(greyscale_view);
  this->intsciss_.compute_static_cost_components();
  this->intsciss_.force_static_cost(this->manual_points_, 0);
  //this->intsciss_.compute_cursor_snap();
  return true;
}






//----------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------
bool dbdet_image_seg_dlvwr_tool::
get_intscissors_params(dbdet_dlvwr_params& iparams, osl_canny_ox_params& params)
{
  vgui_dialog dialog("dLivewire");
  dialog.message("Start: press SHIFT+left-click to select initial seed");
  dialog.message("Press 'e' to delete the last 10 points of the contours");
  dialog.message("Press 's' to smooth the current contours");
  
  dialog.message("Press Shift and right-click to save the contours to the specified file and restart the tool");
  dialog.message("\n");

  dialog.field("Weight for Laplacian Zero-Crossing", iparams.weight_z);
  dialog.field("Weight for Gradient Magnitude", iparams.weight_g);
  dialog.field("Weight for Gradient Direction", iparams.weight_d);
  ////intscissors_dialog->field("Sigma for Gaussian smoothing", iparams->gauss_sigma);
  dialog.field("Half window width for optimum paths", iparams.window_w_over_2);
  dialog.field("Half window height for optimum paths", iparams.window_h_over_2);
  dialog.field("Half side of cursor snap window", iparams.cursor_snap_side_over_2);

  ////intscissors_dialog->field("Weight for Path Length penalty", iparams->weight_l);
  ////intscissors_dialog->field("Normalization factor for path length", iparams->path_norm);
  //intscissors_dialog->checkbox("Use canny", iparams->canny);

  ////intscissors_dialog->field("Weight for costs from canny edges", iparams->weight_canny);
  ////intscissors_dialog->field("Weight for path length on using canny", iparams->weight_canny_l);
  return dialog.ask();

  //dbdet_image_seg_dlvwr_tool::last_used_params = iparams;
}
