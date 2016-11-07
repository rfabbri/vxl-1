// This is brcv/seg/dbdet/vis/dbdet_medial_livewire_tool.cxx
//:
// \file

#include "dbdet_medial_livewire_tool.h"

#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>

#include <vil/vil_convert.h>
#include <vil1/vil1_vil.h>

#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>


//: Constructor - protected
dbdet_medial_livewire_tool::
dbdet_medial_livewire_tool() : 
tableau_(0), seed_picked(false), 
mouse_x(0), mouse_y(0),
image_sptr(0),
livewires_initialzed(false),
rad(3.3), fast_mode(false)
{
  // define different gestures of the tool
  gesture_clear=vgui_event_condition(vgui_key('e'),vgui_MODIFIER_NULL,true);
  gesture_smooth = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  gesture_increase_radius=vgui_event_condition(vgui_key('r'),vgui_MODIFIER_NULL,true);
  gesture_decrease_radius=vgui_event_condition(vgui_key('r'),vgui_SHIFT,true);
  gesture_start=vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);

  gesture_reset = vgui_event_condition(vgui_key('r'), vgui_CTRL, true);

  gesture_up=vgui_event_condition(vgui_CURSOR_UP,vgui_MODIFIER_NULL,true);
  gesture_down=vgui_event_condition(vgui_CURSOR_DOWN,vgui_MODIFIER_NULL,true);
  gesture_left=vgui_event_condition(vgui_CURSOR_LEFT,vgui_MODIFIER_NULL,true);
  gesture_right=vgui_event_condition(vgui_CURSOR_RIGHT,vgui_MODIFIER_NULL,true);
  gesture_run_fastmode=vgui_event_condition(vgui_key('f'),vgui_MODIFIER_NULL,true);


  gesture_scale_up=vgui_event_condition(vgui_CURSOR_UP, vgui_SHIFT, true);
  gesture_scale_down=vgui_event_condition(vgui_CURSOR_DOWN, vgui_SHIFT, true);

  this->medial_style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  this->contour_style_ = vgui_style::new_style(0.0f, 0.0f, 3.0f, 3.0f, 0.0f);
}

//: This is called when the tool is activated
void dbdet_medial_livewire_tool::
activate() 
{
  fast_mode=false;
  seed_picked=false;
  this->livewires_initialzed = false;

  osl_canny_ox_params canny_params;
  dbdet_lvwr_params iparams;
  iparams.canny = true;
  iparams.window_h = 6;
  iparams.window_w = 6;
  get_intscissors_params(&iparams, &canny_params);

  for(unsigned int i=0; i<2; i++)
  {
    intsciss[i].set_params(iparams);
    intsciss[i].set_canny_params(canny_params);
    contour[i].clear();
  }
  this->mouse_curve.clear();

  //check for fast mode
  if(iparams.fast_mode)
  {
    fast_mode=true;
    // added Sep 10, 2005
    bpro1_storage_sptr storage = 
      bvis1_manager::instance()->repository()->get_data("vsol2D", 0, 0);
    vidpro1_vsol2D_storage_sptr input_vsol;
    input_vsol.vertical_cast(storage);

    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    {
      // get the first polyline or polygon
      if( vsol_list[b]->cast_to_curve() ) {
        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) 
        {
          vsol_polyline_2d_sptr polyline = 
            vsol_list[b]->cast_to_curve()->cast_to_polyline();
          for (unsigned int i=0; i<polyline->size(); ++i)
          {
            this->preloaded_curve_.push_back(polyline->vertex(i)->get_p());
          }
          break;
        }
      }
    }

    assert(!this->preloaded_curve_.empty());
    // smoothen the input curve
    bdgl_curve_algs::smooth_curve(this->preloaded_curve_, 1.0);

  }
  vcl_cout << "\nMedial-livewire tool is activated!!!\n";
}

//: Set the tableau to work with
bool dbdet_medial_livewire_tool::
set_tableau ( const vgui_tableau_sptr& tableau)
{
  if( tableau.ptr() != NULL && tableau->type_name() == "vgui_image_tableau" )
  {
    tableau_.vertical_cast(tableau);
    return true;
  }
  
  vcl_cout << "NON vgui_image_tableau is set!! name is : " << tableau->type_name() << " \n";
  tableau_ = NULL;
  return false;
}


//: Return name of the tool
vcl_string dbdet_medial_livewire_tool::
name() const
{
  return "Medial Livewire";
}

//=========================================================
//      EVENT HANDLERS
//=========================================================

//: the handle function
bool dbdet_medial_livewire_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view)
{
  
  // In fast mode
  if ( this->fast_mode)
  {
    // Event: run fast mode
    if (seed_picked && this->gesture_run_fastmode(e))
    {
      this->handle_run_fast_mode();
      vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
        "," << this->mouse_curve.back().y() << ")" << vcl_endl;
    
      bvis1_manager::instance()->post_overlay_redraw();
      // start here
      // increase radius dynamically
      return false;
    }
    
    // Event: translate preloaded curve
    if (this->gesture_down(e) || this->gesture_up(e) || this->gesture_left(e) ||
      this->gesture_right(e))
    {
      double dx = 0;
      double dy = 0;
      if (this->gesture_down(e)) dy = 0.1;
      if (this->gesture_up(e)) dy = -0.1;
      if (this->gesture_left(e)) dx = -0.1;
      if (this->gesture_right(e)) dx = 0.1;
      this->handle_translate_preloaded_curve(dx, dy);
    
      bvis1_manager::instance()->post_overlay_redraw();
      return false;
    }

  }

 
  if (this->gesture_reset(e))
  {
    this->handle_reset();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  mouse_x = ix;
  mouse_y = iy;

  // Event: choose initial seed point)
  if (!seed_picked && gesture_start(e)) 
  { 
    vgui::out << "Initial seed selected" << vcl_endl;
    this->handle_pick_initial_seed();
    //do the following no matter what mode we are in
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  } // end of first event - choose initial seed point
  

  // Event: change radius allows user to input a new search radius, depending on cartilage thickness
  //Livewire will use this radius to compute new seed points
  if(gesture_increase_radius(e))
  {
    this->handle_change_radius(0.1);
    vgui::out << "r =" << this->rad << vcl_endl;
    return false;
  }

  if(gesture_decrease_radius(e))
  {
    this->handle_change_radius(-0.1);
    vgui::out << "r =" << this->rad << vcl_endl;
    return false;
  }

  
  // Event: mouse motions
  if (seed_picked && (e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) 
  {
     if( !fast_mode)
     {      
       this->handle_mouse_motion(mouse_x, mouse_y);
       vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
         "," << this->mouse_curve.back().y() << ")" << vcl_endl;
       bvis1_manager::instance()->post_overlay_redraw();
     }
     return false;
  }

  // Event: smooth the curves for nicer results
  if (seed_picked && gesture_smooth(e)) 
  { 
    this->handle_smooth_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  // Event: delete end parts of contour
  if (seed_picked && gesture_clear(e)) 
  {
    this->handle_delete_end_segments();
    vgui::out << "last mouse point (x, y)=(" << this->mouse_curve.back().x() <<
         "," << this->mouse_curve.back().y() << ")" << vcl_endl;
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
  if (seed_picked && e.type == vgui_MOUSE_PRESS && e.button == vgui_RIGHT) 
  {
    this->handle_save_contour();
    bvis1_manager::instance()->post_overlay_redraw();
    vcl_cout<<"We are at the end of the Save event"<<"\n";
    return false;
  }   


  return false;
}    // end handle()


//: Handle reset event
bool dbdet_medial_livewire_tool::
handle_reset()
{
  this->seed_picked = false;
  this->contour[0].clear();
  this->contour[1].clear();
  this->mouse_curve.clear();
  return false;
}


//: Handle pick initial seed
bool dbdet_medial_livewire_tool::
handle_pick_initial_seed()
{
  vcl_cout<<"We are in Event 1: picking initial seed pt"<<"\n";

  if (! this->livewires_initialzed)
  {
    this->init_livewires();
    this->livewires_initialzed = true;
  }

  // range check of mouse position
  if (!(mouse_x >= 0 && mouse_x < (int)image_sptr->ni() && 
    mouse_y >= 0 && mouse_y < (int)image_sptr->nj())) 
  {
    vcl_cout << "Not a valid seed point on image!!\n";
    vcl_cout << "Select a new seed point inside the image!!\n"; 
    return false;
  }

  //in fast_mode, propagate the medial axis from the previous image to the next image
  // \TODO - fix fast mode code

  if(fast_mode)
  {    
    vcl_cout<< "We are in fast mode\n";
    seed_picked=true;



    //in fasst mode, simply display the preload mouse curve
    vcl_cout<<"We are in fast mode"<<"\n";
    
    //start over, with a new set of mouse coor
    mouse_curve.clear();
    this->contour[0].clear();
    this->contour[1].clear();

    assert(!this->preloaded_curve_.empty());
    
  
    //Seeds 1 and 2 (top & bottom cartilage boundaries, respectively), 
    //are set to the first mouse point
    seed_picked = true;
    vgl_point_2d<double > first_pt = this->preloaded_curve_.front();
    for (unsigned int i=0; i<2; i++)
    {
      seed_x[i]= (int)first_pt.x();
      seed_y[i]= (int)first_pt.y();
    }

    //store seed point in all 4 vectors
    mouse_curve.push_back(first_pt);
    contour[0].push_back(vgl_point_2d<double >(seed_x[0],seed_y[0]));
    contour[1].push_back(vgl_point_2d<double >(seed_x[1],seed_y[1]));
  }   //end of fast mode}
    
  else 
  {    
    //in Regular Mode, simply plot initial seed point
    vcl_cout<<"We are in regular mode"<<"\n";
    
    //start over, with a new set of mouse coor
    mouse_curve.clear();
    this->contour[0].clear();
    this->contour[1].clear();
  
    //Seeds 1 and 2 (top & bottom cartilage boundaries, respectively), 
    //are set to the first mouse point
    seed_picked = true;
    for (unsigned int i=0; i<2; i++)
    {
      seed_x[i]= (int)mouse_x;
      seed_y[i]= (int)mouse_y;
    }

    //store seed point in all 4 vectors
    mouse_curve.push_back(vgl_point_2d<double >(mouse_x,mouse_y));
    contour[0].push_back(vgl_point_2d<double >(mouse_x,mouse_y));
    contour[1].push_back(vgl_point_2d<double >(mouse_x,mouse_y));
  }//end of regular mode
    
  // livewires
  intsciss[0].compute_directions(seed_x[0], seed_y[0]);
  intsciss[1].compute_directions(seed_x[1], seed_y[1]);
  vcl_cout << "Paths are computed...\n";


  return false;
}


//------------------------------------------------------------------------
//: Handle change radius
bool dbdet_medial_livewire_tool::
handle_change_radius(double dr)
{
  vcl_cout << "\n---------------------------------------------\n";
  vcl_cout << "Current radius = " << this->rad << vcl_endl;
  if ((this->rad + dr > 0)&&(this->rad + dr < 10))
    this->rad += dr;
  vcl_cout << "New radius = " << this->rad << vcl_endl;
  vcl_cout << "---------------------------------------------\n";
  return false;
}



//------------------------------------------------------------------------
//: Handle mouse motion
bool dbdet_medial_livewire_tool::
handle_mouse_motion(double newx, double newy)
{   
  // compute distance relative to previous mouse point
  
  //find distance between the 2 pts, to gauge whether they are too far apart
  double dx = newx - this->mouse_curve.back().x();
  double dy = newy - this->mouse_curve.back().y();

  //distance
  double d = vcl_sqrt(dx*dx+dy*dy);

  //if new mouse pt is too close or too far from the previous mouse point
  // then ignore event
  if ( (d < 0.3) || (d > 4) )
  {
    return false;
  }
  else{
    //// compare current tangent with previous tangent
    //if ( unsigned int size = this->mouse_curve.size() > 2)
    //{
    //  double prev_dx = this->mouse_curve.back().x() - this->mouse_curve[size-2].x();
    //  double prev_dy = this->mouse_curve.back().y() - this->mouse_curve[size-2].y();
    //  if ((prev_dx*dx + prev_dy*dy) < 0)
    //    return false;
    //}

    //store new point in mouse_coor & mouse_curve
    mouse_curve.push_back(vgl_point_2d<double >(newx, newy));

    double theta = vcl_atan2(dy,dx);
    
    int curve_x[2];
    int curve_y[2];
    //Use tangent to plot 2 new points, to the left and right of the medial axis
    curve_x[0] = static_cast<int>(newx+((vcl_cos(theta+(3.14/2)))*rad));
    curve_y[0] = static_cast<int>(newy+((vcl_sin(theta+(3.14/2)))*rad));
    curve_x[1] = static_cast<int>(newx+((vcl_cos(theta-(3.14/2)))*rad));
    curve_y[1] = static_cast<int>(newy+((vcl_sin(theta-(3.14/2)))*rad));
    
    // check to ensure new pts are not the same as the seed pt!
    if (curve_x[0]==seed_x[0] && curve_y[0]==seed_y[0])
      return false;
    if (curve_x[1]==seed_x[1] && curve_y[1]==seed_y[1])
      return false;

    // use livewire to find the paths to connect old and new points.
    //decide which of 2 paths to take, depending on value of out 
    //(out determines whether new point is outside window)
    for(int i=0; i<2; i++)
    {
      vcl_vector<vcl_pair<int, int > > coor;
      bool out = intsciss[i].get_path(curve_x[i], curve_y[i], coor); 
      if (coor.size() > 0)
      {
        if (!out)
        {
          // \TODO add the new segment
 
        }
        else
        {         
          //if new point is outside the window, 
          // find a new seed point within the window
          this->find_cost(coor, intsciss[i], contour[i], seed_x[i], seed_y[i]);
        }
      } 
    } 
  }
  return false;
}


//: Handle smoothing contours
bool dbdet_medial_livewire_tool::
handle_smooth_contour()
{ 
  bdgl_curve_algs::smooth_curve(this->mouse_curve, 1.0);
  for(unsigned int i=0; i<2; i++)
  {
    // smooth the contours
    bdgl_curve_algs::smooth_curve(this->contour[i], 1.0);
    // update seed points
    seed_x[i] = int(0.5 + contour[i].back().x());
    seed_y[i] = int(0.5 + contour[i].back().y());
    
    intsciss[i].compute_directions(seed_x[i], seed_y[i]);
  }
  return false;
}





//: Handle delete end segments on contours and medial curve
bool dbdet_medial_livewire_tool::
handle_delete_end_segments()
{
  unsigned int num_pts = 10;
  // if the number of points in mouse_curve is <= num_pts+3, delete the whole curve
  // except for the initial seed
  if (this->mouse_curve.size() <= num_pts+3)
  {
    vgl_point_2d<double > init_seed(this->mouse_curve.front());
    this->mouse_curve.clear();
    this->contour[0].clear();
    this->contour[1].clear();

    // add back the initial seed
    this->mouse_curve.push_back(init_seed);
    this->contour[0].push_back(init_seed);
    this->contour[1].push_back(init_seed);
  }
  // the mouse_curve is long enough
  else
  {
    // in general case, remove num_pts from mouse_curve and 
    // shorten the two contours on two sides accordingly
    
    // remove num_pts from mouse_curve
    for (unsigned int i = 0; i < num_pts; ++i)
    {
      this->mouse_curve.pop_back();
    }
    
    
    // compute tangent vector at the end of mouse_curve
    // choose 2 points far apart for reliability
    vgl_point_2d<double > pt_s = this->mouse_curve.at(this->mouse_curve.size()-3);
    vgl_point_2d<double > pt_e = this->mouse_curve.at(this->mouse_curve.size()-1);
    
    vgl_vector_2d<double > v_tangent(pt_e.x()-pt_s.x(), pt_e.y()-pt_s.y());

    // shorten the two contours
    for (int i = 0; i < 2; ++i)
    {
      while( this->contour[i].size()>0 )
      {
        vgl_point_2d<double > last_pt = this->contour[i].back();
        vgl_vector_2d<double > v(last_pt.x()-pt_e.x(), last_pt.y()-pt_e.y());
        // check relative position of the last point to the tangent
        // if angle is < 2, i.e. the last point is ahead of the last poin
        // on mouse_curve, remove the last point
        if (dot_product<double >(v_tangent, v) > 0)
        {
          this->contour[i].pop_back();
        }
        else
          break;
      }
      // safety check
      if (this->contour[i].size() == 0)
      {
        this->contour[i].push_back(this->mouse_curve.front());
      }
    }
  }
  // reset the seed points
  for (int i = 0; i < 2; ++i)
  {
    this->seed_x[i] = static_cast<int>(this->contour[i].back().x());
    this->seed_y[i] = static_cast<int>(this->contour[i].back().y());

    // reset seed points in livewire
    this->intsciss[i].compute_directions(seed_x[i], seed_y[i]);
  }
  
  return false;
}

// ------------------------------------------------------------------------
//: Handle save contour event
bool dbdet_medial_livewire_tool::
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
      fs2 << "CONTOUR\nOPEN\n" << contour[0].size()+ contour[1].size()<< "\n";
      
      // first (left) contour
      for (unsigned int i=0; i<contour[0].size(); i++) 
      {
        fs2 << contour[0][i].x() << " ";
        fs2 << contour[0][i].y() << "\n";
      }
      // second (right) contour. 
      // The order is reverse to get a continuous overall contour.
      for (unsigned int i=contour[1].size()-1; i>0; i--)
      {
        fs2 << contour[1][i].x() << " ";
        fs2 << contour[1][i].y() << "\n";
      }

      // finish saving the .con file
      fs2.close();

      vcl_cout << "Contour written to the specified files!\n";    
      seed_picked = false;

      // clear old data
      this->mouse_curve.clear();
      for (unsigned int m=0; m<2; m++)
      {
        contour[m].clear();
        intsciss[m].free_buffers();
      }
    }
  }
  return false;
}



// ------------------------------------------------------------------------
//: Handle draw overlay event
bool dbdet_medial_livewire_tool::
handle_draw_overlay()
{
  if (seed_picked) 
  {
    this->contour_style_->apply_all();
    // draw the two livewire search box
    for (unsigned int i = 0; i < 2; ++i)
    {
      // box dimensions
      float box_w = this->intsciss[i].params_.window_w;
      float box_h = this->intsciss[i].params_.window_h;

      // construct 4 points of the box
      float points_x[4];
      float points_y[4];
      
      for (int p = 0; p < 2; ++p)
      {
        float dx = (p==0) ? -box_w/2 : box_w/2;

        for (int q = 0; q < 2; ++q)
        {
          float dy = ((q+p == 0)||(p+q == 2)) ? -box_h/2 : box_h/2;
          points_x[2*p+q] = (float)this->seed_x[i] + dx;
          points_y[2*p+q] = (float)this->seed_y[i] + dy;
        }
      }
      vgui_soview2D_polygon box(4, points_x, points_y,false);
      
      // draw the box
      box.draw();
    }

    // draw the 2 contours
    for(unsigned int i=0; i<2; ++i)
    {
      this->contour_style_->apply_all();
      for (unsigned int m = 0; m<contour[i].size(); m++)
      {
        vgui_soview2D_point con_point((float)contour[i][m].x(), 
          (float)contour[i][m].y());
        con_point.draw();
      }
    }

    this->medial_style_->apply_all();
    for (unsigned int m=0; m<mouse_curve.size(); ++m)
    {
      vgui_soview2D_point medial_point((float)mouse_curve[m].x(), 
          (float)mouse_curve[m].y());
      medial_point.draw();
    }
  }


  if (this->fast_mode)
  {
    vgui_style_sptr preload_style = 
      vgui_style::new_style(0.0f, 1.0f, 0.0f, 3.0f, 3.0f);
    preload_style->apply_all();
    for (unsigned int m=0; m<this->preloaded_curve_.size(); ++m)
    {
      vgui_soview2D_point preloaded_point((float)preloaded_curve_[m].x(), 
        (float)preloaded_curve_[m].y());
      preloaded_point.draw();
    }
  }
  return false;
}


//: this method is used to find a new seed point if user has dragged mouse outside of window
// \TODO fix this function. Something is wrong.
void dbdet_medial_livewire_tool::
find_cost(const vcl_vector<vcl_pair<int,int> >& coor, 
          dbdet_lvwr& intsciss, 
          vcl_vector< vgl_point_2d<double > >& contour, 
          int& seedx, 
          int& seedy)
{
  float cost1, cost2, cost_dif, cost;
  int last_x(0), last_y(0);
  cost1 = intsciss.get_global_cost(coor[0].second, coor[0].first);
  cost_dif = 0;
  for (unsigned int i = 1; i < coor.size(); i++) 
  {
    cost2 = intsciss.get_global_cost(coor[i].second, coor[i].first);
    cost_dif += cost1-cost2;
    cost1 = cost2;
  }

  // average cost difference
  cost_dif /= coor.size(); 
  last_x = seedx;
  last_y = seedy;
  cost1 = intsciss.get_global_cost(coor[0].second, coor[0].first);

  for (unsigned int i = 1; i < coor.size(); i++) 
  {
    cost2 = intsciss.get_global_cost(coor[i].second, coor[i].first);
    cost = cost1-cost2;
    if (cost <= cost_dif) 
    {
      last_x = coor[i].second;
      last_y = coor[i].first;
      break;
    }
    cost1 = cost2;
  }

  // in case the new lowest cost points are the same as the existing points, choose the edge points

  if(last_x==seedx && last_y==seedy)
  {
    last_x=coor[0].second;
    last_y=coor[0].first;
  }

  //the new seed
  seedx = last_x;         
  seedy = last_y;

  //store the new seed point in contour
  if (coor.size() != 0) 
  {
    vcl_vector<vcl_pair<int, int> >::const_iterator q;
    int i = contour.size();
    bool start = false;
    for (q = coor.begin(); q != coor.end()-1; q++) 
    {
      if (q->second == seedx && q->first == seedy) 
        start = true;
      if (start) 
      {
        contour.push_back(vgl_point_2d<double >(q->second, q->first));
        i++;
      }
    }
  }
  
  intsciss.compute_directions(seedx, seedy);
}




//: intialize livewires
bool dbdet_medial_livewire_tool::
init_livewires()
{
  // get image from the repository
  bpro1_storage_sptr input_storage_sptr = bvis1_manager::instance()->repository()->get_data("image",0,0);
  vidpro1_image_storage_sptr frame;
  frame.vertical_cast(input_storage_sptr);
  this->image_sptr = frame->get_image();

  // convert to gray scale if necessary
  vil_image_view< unsigned char > image_view = 
    image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  vil_image_view< unsigned char > greyscale_view;
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
  
  // obtain a vil1_memory_image_of to use with livewire
  vil1_memory_image_of<unsigned char > img = vil1_from_vil_image_view( greyscale_view );

  //must think of a better way to prevent Canny delay
  this->intsciss[0].prepare_buffers(img.width(), img.height(), seed_x[0], seed_y[0]);
  this->intsciss[1].prepare_buffers(img.width(), img.height(), seed_x[1], seed_y[1]);
  this->intsciss[0].Compute(img, seed_x[0], seed_y[0],intsciss[1]);
  return true;
}

//----------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------
bool dbdet_medial_livewire_tool::
get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* params)
{
  vgui_dialog* intscissors_dialog = new vgui_dialog("Medial Livewire");
  intscissors_dialog->message("Start by pressing Shift and left-clicking to select initial seed");
  intscissors_dialog->message("Press 'e' to delete the last 10 points of the contours");
  intscissors_dialog->message("Press 's' to smooth the current contours");
  intscissors_dialog->message("Press 'r' to change the search radius");
  intscissors_dialog->message("Press Shift and right-click to save the contours to the specified file and restart the tool");
  intscissors_dialog->message("\n");

  intscissors_dialog->message("In Fast Mode, the following features are available:");
  intscissors_dialog->message("Use the arrows to reposition the medial axis");
  intscissors_dialog->message("Press Shift + up arrow to increase the scaling and Shift + downward arrow to decrease the scaling");
  intscissors_dialog->message("Press 'f' to automatically find the contours");
  
  //intscissors_dialog->field("Weight for Laplacian Zero-Crossing", iparams->weight_z);
  //intscissors_dialog->field("Weight for Gradient Magnitude", iparams->weight_g);
  //intscissors_dialog->field("Weight for Gradient Direction", iparams->weight_d);
  //intscissors_dialog->field("Sigma for Gaussian smoothing", iparams->gauss_sigma);
  intscissors_dialog->field("Window width for optimum paths' evaluation", iparams->window_w);
  intscissors_dialog->field("Window height for optimum paths' evaluation", iparams->window_h);
  //intscissors_dialog->field("Weight for Path Length penalty", iparams->weight_l);
  //intscissors_dialog->field("Normalization factor for path length", iparams->path_norm);
  intscissors_dialog->checkbox("Use canny", iparams->canny);

  //Fast mode checkbox
  intscissors_dialog->checkbox("Fast mode", iparams->fast_mode);
  


  //intscissors_dialog->field("Weight for costs from canny edges", iparams->weight_canny);
  //intscissors_dialog->field("Weight for path length on using canny", iparams->weight_canny_l);
  return intscissors_dialog->ask();
}







//: Handle run fast-mode
bool dbdet_medial_livewire_tool::
handle_run_fast_mode()
{
  vcl_cout << "Running fast mode ... ";
  for (unsigned int i=1; i < this->preloaded_curve_.size(); ++i)
  {
    vgl_point_2d<double > new_pt = this->preloaded_curve_[i];
    this->handle_mouse_motion(new_pt.x(), new_pt.y());
  }
  vcl_cout << "done.\n";
  vcl_cout << "Number of pts in right contour = " << this->contour[0].size() << vcl_endl;

  vcl_cout << "Number of pts in left contour = " << this->contour[1].size() << vcl_endl;


  return false;
}





//-------------------------------------------------------------------
//: Handle translating preloaded medial curve
bool dbdet_medial_livewire_tool::
handle_translate_preloaded_curve(double dx, double dy)
{
  for (unsigned int i=0; i<this->preloaded_curve_.size(); ++i)
  {
    vgl_point_2d<double > pt = this->preloaded_curve_[i];
    this->preloaded_curve_[i].set(pt.x()+dx, pt.y()+dy);
  }
  return true;
}










#if 0
//: Handle fast mode
bool dbdet_medial_livewire_tool::
handle_fast_mode(const vgui_event & e)
{
  //////////////////////////////////////////////////////////////////////////////////////////

  //This event is useful in fast mode, to reposition the medial axis
  //Moves medial axis upward
  if(seed_picked && gesture_up(e))
  {
    mouse_curve.clear();

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {
      mouse_coor[i].first=mouse_coor[i].first-1;
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////
  //This event is useful in fast mode, to reposition the medial axis
  //Moves medial axis downward
  if(seed_picked && gesture_down(e))
  {
    mouse_curve.clear();

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {
      mouse_coor[i].first=mouse_coor[i].first+1;
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////
  //This event is useful in fast mode, to reposition the medial axis
  //Moves medial axis to the left
  if(seed_picked && gesture_left(e))
  {
    mouse_curve.clear();

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {
      mouse_coor[i].second=mouse_coor[i].second-1;
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  /////////////////////////////////////////////////////////////////////////
  
  //This event is useful in fast mode, to reposition the medial axis
  //Moves medial axis to the right
  if(seed_picked && gesture_right(e))
  {
    mouse_curve.clear();

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {
      mouse_coor[i].second=mouse_coor[i].second+1;
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  
  //Use in fast mode to increase the scale of the medial axis
  if(seed_picked && gesture_scale_up(e))
  {
    //find center of medial axis, assuming it represents a half-circle
    int left_endX(0), right_endX(0), left_endY(0), right_endY(0), center_x(0), center_y(0);
      
    //left endpt of medial axis
    left_endX=mouse_coor[0].second;     
    left_endY=mouse_coor[0].first;                  

    //right endpt of medial axis
    right_endX=mouse_coor[mouse_coor.size()-1].second;       
    right_endY=mouse_coor[mouse_coor.size()-1].first;
    
    //find center of half-circle
    center_x=(left_endX+right_endX)/2;                       
    center_y=(right_endX+right_endY)/2;
    
    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {                      
      //increase the scale factor
      mouse_coor[i].second=static_cast<int>(center_x+1.03*(mouse_coor[i].second-center_x));
      mouse_coor[i].first=static_cast<int>(center_y+1.03*(mouse_coor[i].first-center_y));
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  
  // Use in fast mode to decrease the scale of the medial axis
  if(seed_picked && gesture_scale_down(e))
  {

  //find center of medial axis, assuming it represents a half-circle
    int left_endX(0), right_endX(0), left_endY(0), right_endY(0), center_x(0), center_y(0);

    //left endpt of medial axis
    left_endX=mouse_coor[0].second;                          
    left_endY=mouse_coor[0].first;

    //right endpt of medial axis
    right_endX=mouse_coor[mouse_coor.size()-1].second;       
    right_endY=mouse_coor[mouse_coor.size()-1].first;

    //find center of half-circle
    center_x=(left_endX+right_endX)/2;                       
    center_y=(right_endX+right_endY)/2;

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {                      
      //decrease the scale factor
      mouse_coor[i].second=static_cast<int>(center_x+0.97*(mouse_coor[i].second-center_x));
      mouse_coor[i].first=static_cast<int>(center_y+0.97*(mouse_coor[i].first-center_y));
      mouse_curve.push_back(new vgui_soview2D_point(mouse_coor[i].second, mouse_coor[i].first));
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////
  //After repositioning the medial axis in the center of the cartilage sheet, 
  // run fast mode to obtain the contours
  if(seed_picked && gesture_run_fastmode(e))
  {
    //initialize local variables 
    int x1(0), y1(0), x2(0), y2(0);
    double theta(0);
    int curve_x[2];
    int curve_y[2];

    for(unsigned int i=0; i<mouse_coor.size(); i++)
    {
      for(unsigned int h=0; h<2; h++)
      {                     
        //delete the previous temp & coor before each mouse 'motion'
        for(unsigned int n=0; n<temp[h].size(); n++)
          delete temp[h][n];
        temp[h].clear();
        coor[h].clear();
      }
      x1=(mouse_coor[i].second);
      y1=(mouse_coor[i].first);
      x2=(mouse_coor[i+1].second);
      y2=(mouse_coor[i+1].first);
      theta=atan2(double(y2-y1),double(x2-x1));

      // use medial point and tangent to plot 2 new points, to the left & right of the medial axis
      curve_x[0]=static_cast<int>(x1+((vcl_cos(theta+(3.14/2)))*rad));
      curve_y[0]=static_cast<int>(y1+((vcl_sin(theta+(3.14/2)))*rad));
      curve_x[1]=static_cast<int>(x1+((vcl_cos(theta-(3.14/2)))*rad));
      curve_y[1]=static_cast<int>(y1+((vcl_sin(theta-(3.14/2)))*rad));

      //check to ensure new pts are not the same as the seed pt!
      if (curve_x[0]==seed_x[0] && curve_y[0]==seed_y[0])
        curve_x[0]=curve_x[0]-1;
      if (curve_x[1]==seed_x[1] && curve_y[1]==seed_y[1])
        curve_x[1]=curve_x[1]+1;

      //out determines whether the new point is outside the window
      bool out=false;            
      for (unsigned int k=0; k<2; k++)
      {
        out=intsciss[k].get_path(curve_x[k], curve_y[k], coor[k]);
        if (coor[k].size() > 0)
        {
          if (!out)
          {
            for (unsigned int m = 0; m < coor[k].size(); m++) 
            {
              temp[k].push_back(new vgui_soview2D_point(coor[k][m].second, coor[k][m].first));
              temp[k][m]->set_style(style);
            }
          }
          else
          {              
            //if new point is outside window, find a new seed point within the window
            find_cost(coor[k], intsciss[k], contour[k], seed_x[k], seed_y[k], seed_gui[k]);
            translate_rectangle(intsciss[k],seed_x[k],seed_y[k], k);
          }
        } 
      }
    }     //end of for loop
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  return false;
}


#endif 

