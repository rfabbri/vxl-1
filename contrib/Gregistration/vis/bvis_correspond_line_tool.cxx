// This is breye1/bvis1/tool/bvis1_correspond_line_tool.cxx
//:
// \file

#include "bvis_correspond_line_tool.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_projection_inspector.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vul/vul_file.h>
#include <vgl/vgl_line_2d.h>

//: The bitmap for a 16x16 crosshair 
static const GLubyte cross_bmp[] = 
{ 0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,    
  0x02,0x40,0x00,0x00,0xf8,0x1f,0x00,0x00,  
  0x00,0x00,0xf8,0x1f,0x00,0x00,0x02,0x40, 
  0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40 };

//: Render the crosshair on the image at (x,y)
static void render_cross(float x, float y)
{
  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  glRasterPos2d(x, y);
  glBitmap(16, 16, 8, 8, 0, 0, cross_bmp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
}



//Constructor
bvis1_correspond_line_tool::bvis1_correspond_line_tool( const vgui_event_condition& select )
 : gesture_select_(select), point_style_(vgui_style::new_style(1.0f, 0.0f, 0.0f, 2.0f, 1.0f)),
   modify_(false), object_(NULL), tableau_(NULL), correspondences_(),cnt(0),
   curr_corr_(correspondences_.end())
{
}


//: Destructor
bvis1_correspond_line_tool::~bvis1_correspond_line_tool()
{
}


//: Return the name of this tool
vcl_string
bvis1_correspond_line_tool::name() const
{
  return "Correspond Lines";
}


//: Set the tableau to work with
bool
bvis1_correspond_line_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL ) 
    return false;
  
  return true;
}

//: Handle events
bool
bvis1_correspond_line_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{ 
  float ix, iy;
  static float last_x=0.0f, last_y = 0.0f;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );


  if( e.type == vgui_DRAW_OVERLAY ) {
    float x=last_x, y=last_y;
    if(object_ != NULL){
      object_->get_centroid(&x,&y);
    }
    if(is_mouse_over){
      glColor3f(1.0, 1.0, 1.0);
      render_cross(x,y);
    }
    if( curr_corr_ != correspondences_.end() ){
      if(modify_)
        glColor3f(0.0, 0.0, 1.0);
      else
        glColor3f(0.0, 1.0, 0.0);
      for ( corr_map::const_iterator itr = curr_corr_->begin();
            itr != curr_corr_->end();  ++itr )
      {
        if( this->is_visible_child(view->selector(),itr->first) ){
          itr->second->get_centroid(&x,&y);
          render_cross(x,y);
        }
      }
    }

    return true;
  }

  // cache the current position as the last known position
  if( e.type == vgui_MOTION ) {
    tableau_->motion(e.wx, e.wy);
    last_x = ix;
    last_y = iy;
    
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
    if( curr_obj != object_ ){
      object_ = dynamic_cast<bgui_vsol_soview2D_line_seg*>(curr_obj); 
      if ( !modify_ ){
        if ( !object_ || !this->set_curr_corr(object_) )
          curr_corr_ = correspondences_.end();
      }
    }
    tableau_->post_overlay_redraw();
    return true;
  }

  if( gesture_select_(e) ) {
    if( modify_ ){
      if( object_ == NULL ){
        cnt=cnt-1;
        vsol_line_2d_sptr newline =  new vsol_line_2d(new vsol_point_2d(cnt,cnt),new vsol_point_2d(cnt,cnt));
        object_ = new bgui_vsol_soview2D_line_seg(newline);
      }
      
      if( !this->set_curr_corr(object_) ){
        if ( curr_corr_ == correspondences_.end() )
          curr_corr_ = correspondences_.insert(corr_map()).first;    
        const_cast<corr_map&>(*curr_corr_)[tableau_] = object_;
        object_->set_style(vgui_style::new_style(1.0,0.5,0.0,3.0,3.0));
        tab_set_.insert(tableau_);
      }
      
      tableau_->post_overlay_redraw();
    }
    else{
      if( object_ != NULL )
        if( this->set_curr_corr(object_) )
          tableau_->post_overlay_redraw();
    }
    return true;
  }
  if( e.type == vgui_KEY_PRESS && e.key == 'j' ) {
    tableau_->post_overlay_redraw();
    curr_corr_ = correspondences_.end();
    return true;
  }

  // toggle on/off modify mode
  if( e.type == vgui_KEY_PRESS && e.key == 'm' ) {
    modify_ = !modify_;
    tableau_->post_overlay_redraw();
    curr_corr_ = correspondences_.end();
    return true;
  }

  return false;
}


//: Return the corresponding points as a vector of vectors
//  - corresponding points are in the same order in each vector
//  - one vector is returned for each tableau in \p tabs in the same order
//  - NULL points are returned for missing correspondences
vcl_vector<vcl_vector<vsol_line_2d_sptr> >
bvis1_correspond_line_tool::correspondences(
                          const vcl_vector<vgui_displaylist2D_tableau_sptr>& tabs) const
{
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > result(tabs.size());

  for ( vcl_set<corr_map>::const_iterator citr = correspondences_.begin();
        citr != correspondences_.end();  ++citr )
  {
    for ( unsigned int i=0; i<tabs.size(); ++i )
    {
      corr_map::const_iterator fitr = citr->find(tabs[i]);
      if ( fitr == citr->end() )
        result[i].push_back(NULL);
      else
        result[i].push_back(fitr->second->sptr());
    }
  }
  return result;
}


//: Search the correspondences for this point and make the set active 
bool
bvis1_correspond_line_tool::set_curr_corr(const bgui_vsol_soview2D_line_seg* pt)
{
  for ( vcl_set<corr_map>::iterator itr = correspondences_.begin();
        itr != correspondences_.end();  ++itr )
  {
    corr_map::const_iterator itr2 = itr->find(this->tableau_);
    if ( itr2 != itr->end() ){
      if( itr2->second == pt ){
        curr_corr_ = itr;
        return true;
      }
    }
  }
  return false;
}


//: Returns true if \p tableau is a visible child of \p selector
bool 
bvis1_correspond_line_tool::is_visible_child( const vgui_selector_tableau_sptr& selector,
                                              const vgui_tableau_sptr& tableau ) const
{
  vcl_vector<vcl_string> names(selector->child_names());
  for ( vcl_vector<vcl_string>::const_iterator itr = names.begin();
        itr != names.end();  ++itr )
  {
    if ( tableau == selector->get_tableau(*itr) ){
      if( selector->is_visible(*itr) )
        return true;
      return false;
    }
  }
  return false;
}



//----------------------------------------------------------------------------
//: A vgui command used to compute the fundamental matrix
class bvis1_corr_io_command : public vgui_command
{
  public:
  bvis1_corr_io_command(bvis1_correspond_line_tool* tool) : tool_(tool) {}
  void execute()
  {
    vcl_set<vgui_displaylist2D_tableau_sptr> tab_set(tool_->tableaux());
    vcl_vector<vgui_displaylist2D_tableau_sptr> tabs;
    vcl_vector<vcl_string> choices;
    for ( vcl_set<vgui_displaylist2D_tableau_sptr>::iterator itr = tab_set.begin();
          itr != tab_set.end();  ++itr )
    {
      tabs.push_back(*itr);
      bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(*itr);
      if( storage != NULL )
        choices.push_back(storage->name());
      else
        choices.push_back("");
    }
    unsigned int val1=0, val2=0;
    if(choices.size() > 1)
      val2 = 1;
    vgui_dialog tab_dlg("Select Tableau");
    tab_dlg.choice("View 1", choices, val1);
    tab_dlg.choice("View 2", choices, val2);

    static vcl_string filename="";
    static vcl_string ext="*.*";
    tab_dlg.file("Output Filename",ext,filename);
    //static bool is_skew = false;
    //tab_dlg.checkbox("Translation only", is_skew);
    if (!tab_dlg.ask())
      return; // cancelled


    vcl_ofstream ofile;

    if(vul_file::exists(filename))
        ofile.open(filename.c_str(),vcl_ios::app);
    else
    {
        ofile.open(filename.c_str());
        ofile<<"// edges from "<<choices[val1]<<"to  "<<choices[val2]<<"\n";
    }

    vcl_vector<vgui_displaylist2D_tableau_sptr> selected(2);
    selected[0] = tabs[val1];
    selected[1] = tabs[val2];

    vcl_vector<vcl_vector<vsol_line_2d_sptr> > corr_lines =
      tool_->correspondences(selected);


    for ( unsigned int i=0; i<corr_lines[0].size(); ++i ){
        if(corr_lines[0][i] && corr_lines[1][i])
        {
            vsol_point_2d_sptr midpoint0=corr_lines[0][i]->middle();
            vgl_vector_2d<double> direction0=corr_lines[0][i]->direction();
            vsol_point_2d_sptr midpoint1=corr_lines[1][i]->middle();
            vgl_vector_2d<double> direction1=corr_lines[1][i]->direction();

            ofile<<"["<<midpoint0->x()<<" "<<midpoint0->y()<<" "<<direction0.x()<<" "<<direction0.y()<<"]"<<"\t"
                <<"["<<midpoint1->x()<<" "<<midpoint1->y()<<" "<<direction1.x()<<" "<<direction1.y()<<"]\n";
        }      
    }

  }

  bvis1_correspond_line_tool* tool_;
};


//: Add popup menu items
void 
bvis1_correspond_line_tool::get_popup(const vgui_popup_params& params, 
                                            vgui_menu &menu)
{
  menu.add("Corr IO", new bvis1_corr_io_command(this));
}
