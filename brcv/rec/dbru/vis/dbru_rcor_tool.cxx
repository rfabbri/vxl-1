// This is brcv/rec/dbru/vis/dbru_rcor_tool.cxx
//:
// \file

#include <dbru/vis/dbru_rcor_tool.h>

#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vgui/vgui.h> 

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/vidpro1_repository.h>

#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>

#include <dbru/algo/dbru_rcor_generator.h>

#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbcvr/dbcvr_cv_cor.h>

//: Constructor - protected
dbru_rcor_tool::dbru_rcor_tool()
 :  tableau_(NULL), storage_(NULL), activation_ok_(false)
{
  gesture_click = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_print_histogram = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);

  float color[12][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, 
                         {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f},
                         {0.0f, 0.5f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.5f},
                         {0.5f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.0f}}; 
  colors_.resize(12,3);
  for (int i = 0; i<12; i++)
    for (int j = 0; j<3; j++)
      colors_[i][j] = color[i][j];
}

//: This is called when the tool is activated
void dbru_rcor_tool::activate() 
{
  activation_ok_ = true;
  current_pt0_.set(10, 10);
  current_pt1_.set(10, 10);

  current_ct0_.set(10, 10);
  current_ct1_.set(10, 10);

  fixed_set0_.clear();
  fixed_set1_.clear();

  //set up the views for optimal usage  
  vcl_vector<bvis1_view_tableau_sptr> views = bvis1_manager::instance()->get_views();
  if (views.size() < 2) {
    vcl_cout << "Need two views for this tool, please add the second view!\n";
    activation_ok_ = false;
    return;
  }
  view0_ = views[0];
  view1_ = views[1];

  views[0]->selector()->set_active("region_cor0");
  views[1]->selector()->set_active("region_cor0");
  this->set_tableau(bvis1_manager::instance()->active_tableau());

  views[0]->selector()->toggle("vsol2D1");
  views[0]->selector()->toggle("image1");
  views[0]->selector()->toggle("image0");
  views[0]->selector()->toggle("region_cor0"); // make it invisible

  views[1]->selector()->toggle("vsol2D0");
  views[1]->selector()->toggle("image0");
  views[1]->selector()->toggle("image1");
  views[1]->selector()->toggle("region_cor0"); // make it invisible

  if (storage_) {
    rcor_ = storage_->get_rcor();
    sil_cor_ = rcor_->get_sil_cor();

    curve11_ = new dbsol_interp_curve_2d();
    curve22_ = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(curve11_.ptr(), sil_cor_->get_poly1());
    dbsol_curve_algs::interpolate_linear(curve22_.ptr(), sil_cor_->get_poly2());
  }

  bvis1_manager::instance()->post_redraw();

  if (activation_ok_)
    vcl_cout << "The tool is activated OK!!!\n";
  else 
    vcl_cout << "The tool has PROBLEMS!!!\n";


}

//: Set the tableau to work with
bool dbru_rcor_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
    
  tableau_.vertical_cast(tableau);
  if(!tableau_.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))) {
    return true;
  }
  
  tableau_ = 0;
  return false;
}

//: Set the storage class for the active tableau
bool dbru_rcor_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if(!storage.ptr())
    return false;
    
  //make sure its a ishock storage class
  if (storage->type() == "region_cor"){
    storage_.vertical_cast(storage);
    return true;
  }

  storage_ = 0;
  return false;
}

bool dbru_rcor_tool::handle( const vgui_event & e, 
                         const bvis1_view_tableau_sptr& view )
{
  if (!activation_ok_) {
    vcl_cout << "Problems in activation!\n";
    return false;
  }  

  
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  int mouse_x = static_cast<int>(vcl_floor(ix+0.5));
  int mouse_y = static_cast<int>(vcl_floor(iy+0.5));

  //get the necessary info from the region correspondence computation
  //dbru_rcor_sptr rcor_ = storage_->get_rcor();
  int min_x_ = rcor_->get_min_x();
  int min_y_ = rcor_->get_min_y();
  //int rcor_width_ = rcor_->get_upper1_x();
  //int rcor_height_ = rcor_->get_upper1_y();
  vbl_array_2d< vgl_point_2d<int> > & rcor_map_ = rcor_->get_map();
  

  if (e.modifier == vgui_MODIFIER_NULL && e.type == vgui_MOTION && view == view0_)
  {

    int x = mouse_x, y = mouse_y;
    vgui::out << "(" << mouse_x << ", " << mouse_y << ")" << vcl_endl;
    //: region_map starts from (0,0) 
    x = x - min_x_; y = y - min_y_;
    int rcor_map_size_int1 = static_cast<int>(rcor_map_.rows());
    if (x < 0 || (x >= rcor_map_size_int1 && rcor_map_size_int1 >= 0)) {
      //vcl_cout << "mouse point: (" << int(mouse_x) << ", " << int(mouse_y) << ") : is not in region 1's x limit! ";
      //vcl_cout << "x: " << x << " rcor_map_.size: " << rcor_map_.size() << "\n";
      return false;
    }

    int rcor_map_size_int2 = static_cast<int>(rcor_map_.cols());      
    if(y < 0 || (y >= rcor_map_size_int2 && rcor_map_size_int2 >= 0)) {
      //vcl_cout << "mouse point: (" << int(mouse_x) << ", " << int(mouse_y) << ") : is not in region 1's y limit! "; 
      //vcl_cout << "x: " << x << " y: " << y << " rcor_map_[x].size: " << rcor_map_[x].size() << "\n";
      return false;
    }

    //record the current mouse position
    current_pt0_.set(mouse_x, mouse_y);
    //record the corresponding region 2 pixel from map
    current_pt1_ = rcor_map_[x][y];

    vcl_pair<double, double> pair = dbru_rcor_generator::dt(rcor_->p1(), ix, iy);
    vsol_point_2d_sptr tmp_pt = curve11_->point_at(pair.first);
    current_ct0_.set(tmp_pt->x(), tmp_pt->y());
    tmp_pt = curve22_->point_at(sil_cor_->get_arclength_on_curve2(pair.first));
    current_ct1_.set(tmp_pt->x(), tmp_pt->y());

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_click(e) && view == view0_) {
    int x = mouse_x, y = mouse_y;
    
    //: region_map starts from (0,0) 
    x = x - min_x_; y = y - min_y_;
    int rcor_map_size_int3 = static_cast<int>(rcor_map_.rows());
    if (x < 0 || (x >= rcor_map_size_int3 && rcor_map_size_int3>=0)) {
      //vcl_cout << "mouse point: (" << int(mouse_x) << ", " << int(mouse_y) << ") : is not in region 1's x limit! ";
      //vcl_cout << "x: " << x << " rcor_map_.size: " << rcor_map_.size() << "\n";
      return false;
    }

    int rcor_map_size_int4 = static_cast<int>(rcor_map_.cols());
    if(y < 0 || (y >= rcor_map_size_int4 && rcor_map_size_int4>=0)) {
      //vcl_cout << "mouse point: (" << int(mouse_x) << ", " << int(mouse_y) << ") : is not in region 1's y limit! "; 
      //vcl_cout << "x: " << x << " y: " << y << " rcor_map_[x].size: " << rcor_map_[x].size() << "\n";
      return false;
    }

    //record the current mouse position
    vgl_point_2d<int> pt0(mouse_x, mouse_y);
    //record the corresponding region 2 pixel from map
    vgl_point_2d<int> pt1 = rcor_map_[x][y];

    fixed_set0_.push_back(pt0);
    fixed_set1_.push_back(pt1);

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_print_histogram(e) && view == view0_) {
    
    vbl_array_2d< vcl_vector< vcl_pair< vgl_point_2d<int>, int> > > & region1_hists_ = rcor_->get_region1_histograms();
    if (region1_hists_.rows() <= 0 || region1_hists_.cols() <= 0) {
      vcl_cout << "Vote distribution histograms are not saved!!!\n";
      return false;
    }

    int x = mouse_x, y = mouse_y;
    
    //: region_map starts from (0,0) 
    x = x - min_x_; y = y - min_y_;
    vbl_array_2d<int> region2(rcor_->get_upper2_x(), rcor_->get_upper2_y(), 0);
    int min2x = rcor_->get_min2_x();
    int min2y = rcor_->get_min2_y();
    vcl_cout << "Printing vote distribution histogram for pixel: " << x << " " << y << " size: ";
    vcl_cout << region1_hists_[x][y].size() << "\n";
    int minx = 10000, miny = 10000, maxx = 0, maxy = 0;
    for (unsigned int i = 0; i<region1_hists_[x][y].size(); i++) {
      vgl_point_2d<int> p = region1_hists_[x][y][i].first;
      if (p.x() < minx) minx = p.x();
      if (p.y() < miny) miny = p.y();
      if (p.x() > maxx) maxx = p.x();
      if (p.y() > maxy) maxy = p.y();
      vcl_cout << "i: " << i << " " << p << " # of votes: " << region1_hists_[x][y][i].second << "\n";
      region2[p.x()-min2x][p.y()-min2y] = region1_hists_[x][y][i].second;
    }
    
    vcl_ofstream of("temp_histogram.out");
    of << "Histogram of pixel x: " << mouse_x << " y: " << mouse_y << vcl_endl;
    minx -= min2x; maxx -= min2x; miny -= min2y; maxy -= min2y;
    of << "0\t";
    for (int j = miny; j<maxy; j++)
      of << j+min2y << "\t";
    of << "\n";
    for (int i = minx; i<maxx; i++) {
      of << i+min2x << "\t"; 
      for (int j = miny; j<maxy; j++) {
        of << region2[i][j] << "\t";
      }
      of << "\n";
    }
    of.close();

    return false;
  }

  if (e.type == vgui_DRAW_OVERLAY) 
  {
    //if (view->selector()->is_visible("vsol2D0")) {
    if (view == view0_) {
      glColor3f(0.0f, 0.0f, 1.0f);
      //draw the current point
      glPointSize(3.0);
      glBegin(GL_POINTS);
      glVertex2f(current_pt0_.x(), current_pt0_.y()); 
      glEnd();

      glColor3f(0.0f, 0.0f, 0.0f);
      //draw the current point
      glPointSize(6.0);
      glBegin(GL_POINTS);
      glVertex2f(current_ct0_.x(), current_ct0_.y()); 
      glEnd();
      
      //draw the first fixed set
      for (unsigned int i = 0; i<fixed_set0_.size(); i++) {
        glColor3f(colors_[i%5][0], colors_[i%5][1], colors_[i%5][2]);
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glVertex2f(fixed_set0_[i].x(), fixed_set0_[i].y()); 
        glEnd();
      }
    }
    else { 
      glColor3f(1.0f, 0.0f, 0.0f);
      //draw the current point
      glPointSize(3.0);
      glBegin(GL_POINTS);
      glVertex2f(current_pt1_.x(), current_pt1_.y()); 
      glEnd();

      glColor3f(0.0f, 0.0f, 0.0f);
      //draw the current point
      glPointSize(6.0);
      glBegin(GL_POINTS);
      glVertex2f(current_ct1_.x(), current_ct1_.y()); 
      glEnd();

      //draw the second fixed set
      for (unsigned int i = 0; i<fixed_set1_.size(); i++){
        glColor3f(colors_[i%5][0], colors_[i%5][1], colors_[i%5][2]);
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glVertex2f(fixed_set1_[i].x() , fixed_set1_[i].y()); 
        glEnd();
      }
    }
    return false;
  }

  return false;
}

