// This is brcv/seg/dbdet/vis/dbdet_dmedial_wire_tool.cxx
//:
// \file

#include "dbdet_dmedial_wire_tool.h"
//
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>


//
#include <vil/vil_convert.h>
//#include <vil1/vil1_vil.h>
//
#include <bvis1/bvis1_manager.h>
//#include <vidpro1/vidpro1_repository.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
//
//#include <vsol/vsol_polyline_2d.h>
//#include <vsol/vsol_polyline_2d_sptr.h>
//#include <vsol/vsol_polygon_2d.h>
//#include <vsol/vsol_polygon_2d_sptr.h>
//#include <vsol/vsol_point_2d.h>

#include <bdgl/bdgl_curve_algs.h>
#include <bgui/bgui_image_tableau.h>

//
//

//: Constructor - protected
dbdet_dmedial_wire_tool::
dbdet_dmedial_wire_tool() : 
max_search_radius(10.0),
tableau_(0), 
image_(0),
seed_picked_(false), 
livewires_initialzed_(false),
radius_(3.3)
{
  // define different gestures of the tool
  gesture_start=vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_clear=vgui_event_condition(vgui_key('e'),vgui_MODIFIER_NULL,true);
  gesture_smooth = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  gesture_increase_radius=vgui_event_condition(vgui_key('r'),vgui_MODIFIER_NULL,true);
  gesture_decrease_radius=vgui_event_condition(vgui_key('r'),vgui_SHIFT,true);
  gesture_reset = vgui_event_condition(vgui_key('0'), vgui_MODIFIER_NULL, true);
  gesture_freeze = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);


  gesture_up=vgui_event_condition(vgui_CURSOR_UP,vgui_MODIFIER_NULL,true);
  gesture_down=vgui_event_condition(vgui_CURSOR_DOWN,vgui_MODIFIER_NULL,true);
  gesture_left=vgui_event_condition(vgui_CURSOR_LEFT,vgui_MODIFIER_NULL,true);
  gesture_right=vgui_event_condition(vgui_CURSOR_RIGHT,vgui_MODIFIER_NULL,true);
  gesture_run_fastmode=vgui_event_condition(vgui_key('f'),vgui_MODIFIER_NULL,true);


  gesture_scale_up=vgui_event_condition(vgui_CURSOR_UP, vgui_SHIFT, true);
  gesture_scale_down=vgui_event_condition(vgui_CURSOR_DOWN, vgui_SHIFT, true);

  this->medial_style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  this->contour_style_ = vgui_style::new_style(0.0f, 0.0f, 3.0f, 3.0f, 0.0f);

  this->path_style_ = vgui_style::new_style(3.0f, 0.0f, 0.0f, 3.0f, 1.0f);
  this->manual_point_style_ = vgui_style::new_style(0.0f, 3.0f, 0.0f, 3.0f, 1.0f);
}


//: This is called when the tool is activated
void dbdet_dmedial_wire_tool::
activate() 
{
  // clear existing data
  this->clear_all_contours();

  this->seed_picked_ = false;
  this->livewires_initialzed_ = false;

  osl_canny_ox_params canny_params;
  dbdet_dlvwr_params iparams;

  // pop up window to get parameters from user
  this->get_intscissors_params(iparams, canny_params);

  for(unsigned int i=0; i<2; i++)
  {
    this->intsciss_[i].set_params(iparams);
    this->intsciss_[i].set_canny_params(canny_params);
  }
  vcl_cout << "\nMedial wire tool is activated!!!\n";
}


// ------------------------------------------------------------------
//: Set the tableau to work with
bool dbdet_dmedial_wire_tool::
set_tableau ( const vgui_tableau_sptr& tableau)
{
  if( tableau.ptr() != 0 && tableau->type_name() == "vgui_image_tableau" )
  {
    this->tableau_.vertical_cast(tableau);
    this->image_ = this->tableau_->get_image_resource();
    bgui_image_tableau_sptr bgui_tab;
    bgui_tab.vertical_cast(this->tableau_);
    bgui_tab->unset_handle_motion();
    return true;
  }
  
  vcl_cout << "Error : Invalid tableau type \n";
  this->tableau_ = 0;
  return false;
}



// ------------------------------------------------------------------
//: Clear all contours
void dbdet_dmedial_wire_tool::
clear_all_contours()
{
  this->mouse_curve_.clear();
  for (unsigned int i=0; i<2; ++i)
  {
    this->contour_[i].clear();
    this->path_[i].clear();
  }
  return;
}


//=========================================================
//      EVENT HANDLERS
//=========================================================

//: the handle function
bool dbdet_dmedial_wire_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view)
{
  if (this->gesture_reset(e))
  {
    this->handle_reset();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  float mouse_x = ix;
  float mouse_y = iy;

  // Event: choose initial seed point
  if (! this->seed_picked() && this->gesture_start(e)) 
  { 
    vgui::out << "Initial seed selected" << vcl_endl;
    this->handle_pick_initial_seed(mouse_x, mouse_y);
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  } // end of first event - choose initial seed point
  

  // Event: change radius allows user to input a new search radius, 
  // depending on cartilage thickness
  // Livewire will use this radius to compute new seed points
  if(gesture_increase_radius(e))
  {
    this->handle_change_radius(0.1);
    vgui::out << "r =" << this->radius() << vcl_endl;
    return false;
  }

  if(gesture_decrease_radius(e))
  {
    this->handle_change_radius(-0.1);
    vgui::out << "r =" << this->radius() << vcl_endl;
    return false;
  }

  
  // Event: mouse motions
  if (this->seed_picked() && (e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) 
  {
    this->handle_mouse_motion(mouse_x, mouse_y);
  // vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
  //   "," << this->mouse_curve.back().y() << ")" << vcl_endl;
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // Event: mouse clicks
  if (this->seed_picked() && this->gesture_freeze(e)) 
  {         
    this->handle_freeze_path();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // Event: smooth the curves for nicer results
  if (this->seed_picked() && this->gesture_smooth(e)) 
  { 
    this->handle_smooth_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  // Event: delete end parts of contour
  if (this->seed_picked_ && gesture_clear(e)) 
  {
    this->handle_delete_end_segments();
    //vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
    //     "," << this->mouse_curve.back().y() << ")" << vcl_endl;
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
  if (this->seed_picked() && e.type == vgui_MOUSE_PRESS && e.button == vgui_RIGHT) 
  {
    this->handle_save_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    //vcl_cout<<"Save event"<<"\n";
    return false;
  }   


  return false;
} // end handle()


// ------------------------------------------------------------------
//: Handle reset event
bool dbdet_dmedial_wire_tool::
handle_reset()
{
  this->seed_picked_ = false;
  this->contour_[0].clear();
  this->contour_[1].clear();
  this->mouse_curve_.clear();
  return false;
}


// ------------------------------------------------------------------
//: Handle pick initial seed
bool dbdet_dmedial_wire_tool::
handle_pick_initial_seed(float mouse_x, float mouse_y)
{
  vcl_cout<<"Event: pick initial seed pt"<<"\n";

  if (! this->livewires_initialzed_)
  {
    this->init_livewires();
    this->livewires_initialzed_ = true;
  }

  // range check of mouse position
  mouse_x = (int)mouse_x;
  mouse_y = (int)mouse_y;
  if (!(mouse_x >= 0 && mouse_x < (int)image_->ni() && 
    mouse_y >= 0 && mouse_y < (int)image_->nj())) 
  {
    vcl_cout << "Not a valid seed point on image!!\n";
    vcl_cout << "Select a new seed point inside the image!!\n"; 
    return false;
  }
  

  //start over, with a new set of mouse coor
  vgl_point_2d<double > pt(mouse_x, mouse_y);

  this->mouse_curve_.clear();
  this->mouse_curve_.push_back(pt);
  
  for (unsigned int i=0; i<2; ++i)
  {
    // reset contours
    this->contour_[i].clear();
    this->contour_[0].push_back(pt);

    // set seed for the livewires
    this->intsciss_[i].compute_optimal_paths(
      (int)mouse_x, (int)mouse_y);
  }
  
  this->seed_picked_ = true;
  vcl_cout << "Paths are computed...\n";
  return false;
}




//------------------------------------------------------------------------
//: Handle change radius
bool dbdet_dmedial_wire_tool::
handle_change_radius(double dr)
{
  vcl_cout << "\n---------------------------------------------\n";
  vcl_cout << "Current radius = " << this->radius() << vcl_endl;
  if ((this->radius_ + dr > 0)&&(this->radius_ + dr < this->max_search_radius))
    this->radius_ += dr;
  vcl_cout << "New radius = " << this->radius() << vcl_endl;
  vcl_cout << "---------------------------------------------\n";
  return false;
}


//------------------------------------------------------------------------
//: Handle mouse motion
bool dbdet_dmedial_wire_tool::
handle_mouse_motion(double newx, double newy)
{

  // compute distance relative to previous mouse point
  // find distance between the 2 pts
  this->mouse_pt_.set(newx, newy);
  vgl_vector_2d<double > t = this->mouse_pt_ - this->mouse_curve_.back();


  // ignore event if mouse_pt is too close or too far from the previous mouse point
  if ( t.length()<1 )
  {
    return false;
  }

  // free points for the 2 livewires
  vgl_vector_2d<double > n(-t.y(), t.x());
  normalize(n);
  vgl_point_2d<double > free_pt[2];
  free_pt[0] = this->mouse_pt_ + n * this->radius();
  free_pt[1] = this->mouse_pt_ - n * this->radius();

  // make sure the new free points are valid
  for (unsigned int i=0; i<2; ++i)
  {
    int free_x = (int)(free_pt[i].x()+0.5);
    int free_y = (int)(free_pt[i].y()+0.5);
    if (!this->intsciss_[i].explored_region().contains(free_x, free_y))
      return false;
    vgl_point_2d<int > newpt = 
      this->intsciss_[i].cursor_snap(free_x, free_y);
    this->intsciss_[i].get_path(newpt.x(), newpt.y(), this->path_[i]);
  }
  
  return false;
}



//: Handle freeze path
bool dbdet_dmedial_wire_tool::
handle_freeze_path()
{
  // move points in `path_' to contour
  vgl_point_2d<int > pt(0, 0);
  for (unsigned int i=0; i<2; ++i)
  {
    if (this->path_[i].size() < 2) continue;
    // ignore the last point to avoid duplicate between two `freezes'
    for (int m=this->path_[i].size()-2; m >=0; --m)
    {
      pt = this->path_[i][m];
      this->contour_[i].push_back(vgl_point_2d<double >(pt.x(), pt.y()));
    }
    this->path_[i].clear();
      
    // set new seed points
    this->intsciss_[i].compute_optimal_paths(pt.x(), pt.y());
  }

  // move latest mouse point to mouse curve
  this->mouse_curve_.push_back(this->mouse_pt_);

  return false;
}









// -----------------------------------------------------------------
//: Handle smoothing contours
bool dbdet_dmedial_wire_tool::
handle_smooth_contour()
{ 
  //bdgl_curve_algs::smooth_curve(this->mouse_curve_, 1.0);
  for(unsigned int i=0; i<2; i++)
  {
    // smooth the contours
    bdgl_curve_algs::smooth_curve(this->contour_[i], 1.0);
    // update seed points
    int seed_x = int(0.5 + this->contour_[i].back().x());
    int seed_y = int(0.5 + this->contour_[i].back().y());
    this->intsciss_[i].compute_optimal_paths(seed_x, seed_y);
  }
  return false;
}





//: Handle delete end segments on contours and medial curve
bool dbdet_dmedial_wire_tool::
handle_delete_end_segments()
{
//  unsigned int num_pts = 10;
//  // if the number of points in mouse_curve is <= num_pts+3, delete the whole curve
//  // except for the initial seed
//  if (this->mouse_curve.size() <= num_pts+3)
//  {
//    vgl_point_2d<double > init_seed(this->mouse_curve.front());
//    this->mouse_curve.clear();
//    this->contour[0].clear();
//    this->contour[1].clear();
//
//    // add back the initial seed
//    this->mouse_curve.push_back(init_seed);
//    this->contour[0].push_back(init_seed);
//    this->contour[1].push_back(init_seed);
//  }
//  // the mouse_curve is long enough
//  else
//  {
//    // in general case, remove num_pts from mouse_curve and 
//    // shorten the two contours on two sides accordingly
//    
//    // remove num_pts from mouse_curve
//    for (unsigned int i = 0; i < num_pts; ++i)
//    {
//      this->mouse_curve.pop_back();
//    }
//    
//    
//    // compute tangent vector at the end of mouse_curve
//    // choose 2 points far apart for reliability
//    vgl_point_2d<double > pt_s = this->mouse_curve.at(this->mouse_curve.size()-3);
//    vgl_point_2d<double > pt_e = this->mouse_curve.at(this->mouse_curve.size()-1);
//    
//    vgl_vector_2d<double > v_tangent(pt_e.x()-pt_s.x(), pt_e.y()-pt_s.y());
//
//    // shorten the two contours
//    for (int i = 0; i < 2; ++i)
//    {
//      while( this->contour[i].size()>0 )
//      {
//        vgl_point_2d<double > last_pt = this->contour[i].back();
//        vgl_vector_2d<double > v(last_pt.x()-pt_e.x(), last_pt.y()-pt_e.y());
//        // check relative position of the last point to the tangent
//        // if angle is < 2, i.e. the last point is ahead of the last poin
//        // on mouse_curve, remove the last point
//        if (dot_product<double >(v_tangent, v) > 0)
//        {
//          this->contour[i].pop_back();
//        }
//        else
//          break;
//      }
//      // safety check
//      if (this->contour[i].size() == 0)
//      {
//        this->contour[i].push_back(this->mouse_curve.front());
//      }
//    }
//  }
//  // reset the seed points
//  for (int i = 0; i < 2; ++i)
//  {
//    this->seed_x[i] = static_cast<int>(this->contour[i].back().x());
//    this->seed_y[i] = static_cast<int>(this->contour[i].back().y());
//
//    // reset seed points in livewire
//    this->intsciss[i].compute_directions(seed_x[i], seed_y[i]);
//  }
  
  return false;
}

// ------------------------------------------------------------------------
//: Handle save contour event
bool dbdet_dmedial_wire_tool::
handle_save_contour()
{
  vcl_cout<<"\nIn the Save event " <<"\n";
  
  
  vcl_string image_filename = ".con";
  
  // pop up a dialog to ask for output file name
  static vcl_string regexp = "*.*";
  vgui_dialog save_dl("Save contour");
  save_dl.inline_file("Filename: ", regexp, image_filename);

  if (save_dl.ask()) 
  {
    //open the file
    vcl_ofstream fs2(image_filename.c_str()); 
    if (!fs2) 
    {
      vcl_cout << "Problems in opening file: " << image_filename << "\n";
      return false;
    }
    else 
    {
      // save data to the ".con" file.
      fs2 << "CONTOUR\nOPEN\n" << this->contour_[0].size()+ 
        this->contour_[1].size()<< "\n";
      
      // first (left) contour
      for (unsigned int i=0; i<this->contour_[0].size(); i++) 
      {
        fs2 << this->contour_[0][i].x() << " ";
        fs2 << this->contour_[0][i].y() << "\n";
      }
      // second (right) contour. 
      // The order is reverse to get a continuous overall contour.
      for (unsigned int i=this->contour_[1].size()-1; i>0; i--)
      {
        fs2 << this->contour_[1][i].x() << " ";
        fs2 << this->contour_[1][i].y() << "\n";
      }

      // finish saving the .con file
      fs2.close();

      vcl_cout << "Contour written to the specified files!\n";    
      this->seed_picked_ = false;

      // clear old data
      this->mouse_curve_.clear();
      for (unsigned int m=0; m<2; m++)
      {
        this->contour_[m].clear();
      }
    }
  }
  return false;
}




// ------------------------------------------------------------------------
//: Handle draw overlay event
bool dbdet_dmedial_wire_tool::
handle_draw_overlay()
{
  if (this->seed_picked()) 
  {
    this->contour_style_->apply_all();
    // draw the two livewire search box
    for (unsigned int i = 0; i < 2; ++i)
    {

      this->contour_style_->apply_all();

      // draw livewire search box
      // box dimensions
      vgl_box_2d<int > box = this->intsciss_[i].explored_region(); 

      // construct 4 points of the box
      float points_x[] = {box.min_x(), box.max_x(), box.max_x(), box.min_x()};
      float points_y[] = {box.min_y(), box.min_y(), box.max_y(), box.max_y()};

      vgui_soview2D_polygon box_vis(4, points_x, points_y);
    
      // draw the box
      box_vis.draw();

      // draw the contour
      this->contour_style_->apply_all();
      vgui_soview2D_point soview_point;

      // draw the 2 contours
      for(unsigned int i=0; i<2; ++i)
      {
        for (unsigned int m = 0; m<this->contour_[i].size(); m++)
        {
          soview_point.x = (float)this->contour_[i][m].x();
          soview_point.y = (float)this->contour_[i][m].y();
          soview_point.draw();
        }
      }

      // draw the optimized path return livewire
      this->path_style_->apply_all();
      for (unsigned int i=0; i<2; ++i)
      {
        for (unsigned int m=0; m<this->path_[i].size(); ++m)
        {
          soview_point.x = (float)this->path_[i][m].x();
          soview_point.y = (float)this->path_[i][m].y();
          soview_point.draw();
        } 
      }


      this->medial_style_->apply_all();
      for (unsigned int m=0; m<this->mouse_curve_.size(); ++m)
      {
        soview_point.x = (float)mouse_curve_[m].x();
        soview_point.y = (float)mouse_curve_[m].y();
        soview_point.draw();
      }
    }
  }
  return false;
}

//
////: this method is used to find a new seed point if user has dragged mouse outside of window
//// \TODO fix this function. Something is wrong.
//void dbdet_dmedial_wire_tool::
//find_cost(const vcl_vector<vcl_pair<int,int> >& coor, 
//          dbdet_lvwr& intsciss, 
//          vcl_vector< vgl_point_2d<double > >& contour, 
//          int& seedx, 
//          int& seedy)
//{
//  float cost1, cost2, cost_dif, cost;
//  int last_x(0), last_y(0);
//  cost1 = intsciss.get_global_cost(coor[0].second, coor[0].first);
//  cost_dif = 0;
//  for (unsigned int i = 1; i < coor.size(); i++) 
//  {
//    cost2 = intsciss.get_global_cost(coor[i].second, coor[i].first);
//    cost_dif += cost1-cost2;
//    cost1 = cost2;
//  }
//
//  // average cost difference
//  cost_dif /= coor.size(); 
//  last_x = seedx;
//  last_y = seedy;
//  cost1 = intsciss.get_global_cost(coor[0].second, coor[0].first);
//
//  for (unsigned int i = 1; i < coor.size(); i++) 
//  {
//    cost2 = intsciss.get_global_cost(coor[i].second, coor[i].first);
//    cost = cost1-cost2;
//    if (cost <= cost_dif) 
//    {
//      last_x = coor[i].second;
//      last_y = coor[i].first;
//      break;
//    }
//    cost1 = cost2;
//  }
//
//  // in case the new lowest cost points are the same as the existing points, choose the edge points
//
//  if(last_x==seedx && last_y==seedy)
//  {
//    last_x=coor[0].second;
//    last_y=coor[0].first;
//  }
//
//  //the new seed
//  seedx = last_x;         
//  seedy = last_y;
//
//  //store the new seed point in contour
//  if (coor.size() != 0) 
//  {
//    vcl_vector<vcl_pair<int, int> >::const_iterator q;
//    int i = contour.size();
//    bool start = false;
//    for (q = coor.begin(); q != coor.end()-1; q++) 
//    {
//      if (q->second == seedx && q->first == seedy) 
//        start = true;
//      if (start) 
//      {
//        contour.push_back(vgl_point_2d<double >(q->second, q->first));
//        i++;
//      }
//    }
//  }
//  
//  intsciss.compute_directions(seedx, seedy);
//}
//
//
//
//
//: intialize livewires
bool dbdet_dmedial_wire_tool::
init_livewires()
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

  
  this->intsciss_[0].set_image(greyscale_view);
  this->intsciss_[0].compute_static_cost_components();
  this->intsciss_[0].force_static_cost(this->manual_points_, 0);
  //this->intsciss_[0].compute_cursor_snap();
  this->intsciss_[1].copy(this->intsciss_[0]);

  return true;
}



//----------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------
bool dbdet_dmedial_wire_tool::
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
}








////: Handle run fast-mode
//bool dbdet_dmedial_wire_tool::
//handle_run_fast_mode()
//{
//  vcl_cout << "Running fast mode ... ";
//  for (unsigned int i=1; i < this->preloaded_curve_.size(); ++i)
//  {
//    vgl_point_2d<double > new_pt = this->preloaded_curve_[i];
//    this->handle_mouse_motion(new_pt.x(), new_pt.y());
//  }
//  vcl_cout << "done.\n";
//  vcl_cout << "Number of pts in right contour = " << this->contour[0].size() << vcl_endl;
//
//  vcl_cout << "Number of pts in left contour = " << this->contour[1].size() << vcl_endl;
//
//
//  return false;
//}
//
//
//
//
//
////-------------------------------------------------------------------
////: Handle translating preloaded medial curve
//bool dbdet_dmedial_wire_tool::
//handle_translate_preloaded_curve(double dx, double dy)
//{
//  for (unsigned int i=0; i<this->preloaded_curve_.size(); ++i)
//  {
//    vgl_point_2d<double > pt = this->preloaded_curve_[i];
//    this->preloaded_curve_[i].set(pt.x()+dx, pt.y()+dy);
//  }
//  return true;
//}
//
//
//
//
//
//
//
//
//
//
//#if 0
////: Handle fast mode
//bool dbdet_dmedial_wire_tool::
//handle_fast_mode(const vgui_event & e)
//{
//  //////////////////////////////////////////////////////////////////////////////////////////
//
//  //This event is useful in fast mode, to reposition the medial axis
//  //Moves medial axis upward
//  if(seed_picked && gesture_up(e))
//  {
//    mouse_curve.clear();
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {
//      mouse_coor[i].first=mouse_coor[i].first-1;
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  /////////////////////////////////////////////////////////////////////////////////////
//  //This event is useful in fast mode, to reposition the medial axis
//  //Moves medial axis downward
//  if(seed_picked && gesture_down(e))
//  {
//    mouse_curve.clear();
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {
//      mouse_coor[i].first=mouse_coor[i].first+1;
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  /////////////////////////////////////////////////////////////////////////////////////////
//  //This event is useful in fast mode, to reposition the medial axis
//  //Moves medial axis to the left
//  if(seed_picked && gesture_left(e))
//  {
//    mouse_curve.clear();
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {
//      mouse_coor[i].second=mouse_coor[i].second-1;
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  /////////////////////////////////////////////////////////////////////////
//  
//  //This event is useful in fast mode, to reposition the medial axis
//  //Moves medial axis to the right
//  if(seed_picked && gesture_right(e))
//  {
//    mouse_curve.clear();
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {
//      mouse_coor[i].second=mouse_coor[i].second+1;
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  ///////////////////////////////////////////////////////////////////////////////////////////
//  
//  //Use in fast mode to increase the scale of the medial axis
//  if(seed_picked && gesture_scale_up(e))
//  {
//    //find center of medial axis, assuming it represents a half-circle
//    int left_endX(0), right_endX(0), left_endY(0), right_endY(0), center_x(0), center_y(0);
//      
//    //left endpt of medial axis
//    left_endX=mouse_coor[0].second;     
//    left_endY=mouse_coor[0].first;                  
//
//    //right endpt of medial axis
//    right_endX=mouse_coor[mouse_coor.size()-1].second;       
//    right_endY=mouse_coor[mouse_coor.size()-1].first;
//    
//    //find center of half-circle
//    center_x=(left_endX+right_endX)/2;                       
//    center_y=(right_endX+right_endY)/2;
//    
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {                      
//      //increase the scale factor
//      mouse_coor[i].second=static_cast<int>(center_x+1.03*(mouse_coor[i].second-center_x));
//      mouse_coor[i].first=static_cast<int>(center_y+1.03*(mouse_coor[i].first-center_y));
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  ///////////////////////////////////////////////////////////////////////////////////////////////
//  
//  // Use in fast mode to decrease the scale of the medial axis
//  if(seed_picked && gesture_scale_down(e))
//  {
//
//  //find center of medial axis, assuming it represents a half-circle
//    int left_endX(0), right_endX(0), left_endY(0), right_endY(0), center_x(0), center_y(0);
//
//    //left endpt of medial axis
//    left_endX=mouse_coor[0].second;                          
//    left_endY=mouse_coor[0].first;
//
//    //right endpt of medial axis
//    right_endX=mouse_coor[mouse_coor.size()-1].second;       
//    right_endY=mouse_coor[mouse_coor.size()-1].first;
//
//    //find center of half-circle
//    center_x=(left_endX+right_endX)/2;                       
//    center_y=(right_endX+right_endY)/2;
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {                      
//      //decrease the scale factor
//      mouse_coor[i].second=static_cast<int>(center_x+0.97*(mouse_coor[i].second-center_x));
//      mouse_coor[i].first=static_cast<int>(center_y+0.97*(mouse_coor[i].first-center_y));
//      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
//    }
//
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//  
//  /////////////////////////////////////////////////////////////////////////////////////////////////
//  //After repositioning the medial axis in the center of the cartilage sheet, 
//  // run fast mode to obtain the contours
//  if(seed_picked && gesture_run_fastmode(e))
//  {
//    //initialize local variables 
//    int x1(0), y1(0), x2(0), y2(0);
//    double theta(0);
//    int curve_x[2];
//    int curve_y[2];
//
//    for(unsigned int i=0; i<mouse_coor.size(); i++)
//    {
//      for(unsigned int h=0; h<2; h++)
//      {                     
//        //delete the previous temp & coor before each mouse 'motion'
//        for(unsigned int n=0; n<temp[h].size(); n++)
//          delete temp[h][n];
//        temp[h].clear();
//        coor[h].clear();
//      }
//      x1=(mouse_coor[i].second);
//      y1=(mouse_coor[i].first);
//      x2=(mouse_coor[i+1].second);
//      y2=(mouse_coor[i+1].first);
//      theta=atan2(double(y2-y1),double(x2-x1));
//
//      // use medial point and tangent to plot 2 new points, to the left & right of the medial axis
//      curve_x[0]=static_cast<int>(x1+((vcl_cos(theta+(3.14/2)))*rad));
//      curve_y[0]=static_cast<int>(y1+((vcl_sin(theta+(3.14/2)))*rad));
//      curve_x[1]=static_cast<int>(x1+((vcl_cos(theta-(3.14/2)))*rad));
//      curve_y[1]=static_cast<int>(y1+((vcl_sin(theta-(3.14/2)))*rad));
//
//      //check to ensure new pts are not the same as the seed pt!
//      if (curve_x[0]==seed_x[0] && curve_y[0]==seed_y[0])
//        curve_x[0]=curve_x[0]-1;
//      if (curve_x[1]==seed_x[1] && curve_y[1]==seed_y[1])
//        curve_x[1]=curve_x[1]+1;
//
//      //out determines whether the new point is outside the window
//      bool out=false;            
//      for (unsigned int k=0; k<2; k++)
//      {
//        out=intsciss[k].get_path(curve_x[k], curve_y[k], coor[k]);
//        if (coor[k].size() > 0)
//        {
//          if (!out)
//          {
//            for (unsigned int m = 0; m < coor[k].size(); m++) 
//            {
//              temp[k].push_back(new vgui_soview2D_point(coor[k][m].second, coor[k][m].first));
//              temp[k][m]->set_style(style);
//            }
//          }
//          else
//          {              
//            //if new point is outside window, find a new seed point within the window
//            find_cost(coor[k], intsciss[k], contour[k], seed_x[k], seed_y[k], seed_gui[k]);
//            translate_rectangle(intsciss[k],seed_x[k],seed_y[k], k);
//          }
//        } 
//      }
//    }     //end of for loop
//    bvis1_manager::instance()->post_overlay_redraw();
//    return false;
//  }
//
//  return false;
//}
//
//
//#endif 
//
