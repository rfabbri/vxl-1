// This is brcv/dbsk2d/vis/dbsk2d_ishock_detector_debug_tool.cxx

//:
// \file

#include "dbsk2d_ishock_detector_debug_tool.h"

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>

//: Constructor
dbsk2d_ishock_detector_debug_tool::dbsk2d_ishock_detector_debug_tool()
{
  clear_shocks = vgui_event_condition(vgui_key('c'), vgui_CTRL, false);
  re_init_shocks = vgui_event_condition(vgui_key('i'), vgui_CTRL, false);
  prop_until_end = vgui_event_condition(vgui_key('k'), vgui_CTRL, false);
  init_shocks_within_cell = vgui_event_condition(vgui_key('i'), vgui_SHIFT, false);
  next_prop = vgui_event_condition(vgui_key('n'), vgui_CTRL, false);
  next_jump_prop = vgui_event_condition(vgui_key('j'), vgui_CTRL, false);
  merge_cells = vgui_event_condition(vgui_key('m'), vgui_CTRL, false);
  
  sh_det_ = 0;
  disp_foots = false;
}

//: Destructor
dbsk2d_ishock_detector_debug_tool::~dbsk2d_ishock_detector_debug_tool()
{
  if (sh_det_)
    delete sh_det_;
}

//: Set the storage class for the active tableau
bool 
dbsk2d_ishock_detector_debug_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if(!storage.ptr())
    return false;
    
  //make sure its a ishock storage class
  if (storage->type() == "shock"){
    storage_.vertical_cast(storage);
    
    //if detector has not been initialized, initialize here
    if (!sh_det_){
      //first make sure that this storage class has an intrinsic shock object
      if (storage_->get_ishock_graph())
        sh_det_ = new dbsk2d_ishock_detector(storage_->get_ishock_graph()->boundary());
      else {
        sh_det_ = 0;
        return false;
      }
    }
    else if (sh_det_->ishock_graph() != storage_->get_ishock_graph())
    {
      //this tool is operating on a different tableau
      delete sh_det_;
      
      //initialize a new one for this tableau
      //first make sure that this storage class has an intrinsic shock object
      if (storage_->get_ishock_graph())
        sh_det_ = new dbsk2d_ishock_detector(storage_->get_ishock_graph()->boundary());
      else {
        sh_det_ = 0;
        return false;
      }
    }  
    return true;
  }

  storage_ = 0;
  return false;
}

//: Return the name of this tool
vcl_string dbsk2d_ishock_detector_debug_tool::name() const
{
  return "Debug Shock Computation";
}

//: Handle events
bool dbsk2d_ishock_detector_debug_tool::handle( const vgui_event & e, 
                                                const bvis1_view_tableau_sptr& view )
{
  if (sh_det_){

    if(clear_shocks(e)){
      vcl_cout << "Clear all shocks." << vcl_endl;
      sh_det_->clear();

      tableau()->post_redraw();
      return true;
    }
    
    if(re_init_shocks(e)){
      vcl_cout << "Re-Initialize shocks: " << vcl_endl;
      sh_det_->clear();
      sh_det_->initialize_shocks();

      sh_det_->ishock_graph()->update_shocks();
      tableau()->post_redraw();
      return true;
    }
    
    if(prop_until_end(e)){
      vcl_cout << "Propagate shocks." << vcl_endl;
      sh_det_->propagate_shocks_without_merge();

      sh_det_->ishock_graph()->update_shocks();
      tableau()->post_redraw();
      return true;
    }
    
    if (init_shocks_within_cell(e)){
      if (sh_det_->cells().size()>0)
      {
        vcl_cout << "Initialize: Cell (" << sh_det_->cur_row() << ", " 
                 << sh_det_->cur_col() << ") " << vcl_endl;

        sh_det_->cell(sh_det_->cur_row(),sh_det_->cur_col()).initialize_shocks();

        sh_det_->ishock_graph()->update_shocks();
        tableau()->post_redraw();
      }
      return true;
    }

    if(next_prop(e)){
      //vcl_cout << "Single Propagation: " ;
      sh_det_->propagate_next_active_shock();

      sh_det_->ishock_graph()->update_shocks();
      tableau()->post_redraw();
      return true;
    }
    
    if(next_jump_prop(e)){
      //vcl_cout << "Jump Propagation." << vcl_endl;
      sh_det_->propagate_a_bunch_of_shocks();

      sh_det_->ishock_graph()->update_shocks();
      tableau()->post_redraw();
      return true;
    }

    if (merge_cells(e)){
      sh_det_->merge_next_scheduled_cells();

      sh_det_->ishock_graph()->update_shocks();
      tableau()->post_redraw();
      return true;
    }

    if(left_click(e)){
      //select the active lagrangian cell for debug
      if (current_){
        dbsk2d_ishock_bcurve* bcurve = dynamic_cast<dbsk2d_ishock_bcurve*>(current_);
        if (bcurve){
          sh_det_->set_cur_row(bcurve->bnd_edge()->cells().front()->index().row);
          sh_det_->set_cur_col(bcurve->bnd_edge()->cells().front()->index().col);
        }
        vcl_cout << "Current Cell: (" << sh_det_->cur_row() << ", " 
                 << sh_det_->cur_col() << ")" << vcl_endl;
      }
    }

    //for debug
    //render the candidate second order nodes
    if (e.type == vgui_DRAW)
    {
      if (sh_det_->cells().size()>0)
      {
        //draw the current cells that are active
        for (int i=0; i<sh_det_->boundary()->num_rows(); ++i){
          for (int j=0; j<sh_det_->boundary()->num_cols(); ++j){
            if (sh_det_->cell(i, j).is_valid())
            {
              double min_x = sh_det_->cell(i, j).left_bnd()->loc();
              double max_x = sh_det_->cell(i, j).right_bnd()->loc();
              double min_y = sh_det_->cell(i, j).bottom_bnd()->loc();
              double max_y = sh_det_->cell(i, j).top_bnd()->loc();

              glColor3f( 0.0f , 0.8f , 0.0f );
              glBegin( GL_LINE_STRIP );
              glVertex2d(min_x, min_y);
              glVertex2d(max_x, min_y);      
              glVertex2d(max_x, max_y);      
              glVertex2d(min_x, max_y);
              glVertex2d(min_x, min_y);
              glEnd();
            }
          }
        }

        //draw the candidate sources of the current active cell
        int cur_row = sh_det_->cur_row();
        int cur_col = sh_det_->cur_col();
        ordered_src_list_iter curS = sh_det_->cell(cur_row,cur_col).cand_src_list().begin();
        for (; curS!=sh_det_->cell(cur_row,cur_col).cand_src_list().end(); curS++){
          dbsk2d_ishock_node* curSrc = (curS->second);
          
          glColor3f( 1.0 , 0.0 , 0.0 );
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f(curSrc->origin().x() , curSrc->origin().y() ); 
          glEnd();
        }
      }
    }

    //display foot projections of the shocks
    if (e.type == vgui_DRAW_OVERLAY){
      if (disp_foots && current_){
        dbsk2d_ishock_edge* sedge = dynamic_cast<dbsk2d_ishock_edge*>(current_);
        if (sedge){
          vgl_point_2d<double> sh_pt, lft_pt, rft_pt;

          //at the start point
          sh_pt = sedge->getStartPt(); 
          lft_pt = sedge->getLFootPt(sedge->sTau());
          rft_pt = sedge->getRFootPt(sedge->sTau());


          glColor3f( 0.0f , 0.5f , 0.0f );
          glLineWidth( 1.0f);
          glBegin( GL_LINE_STRIP );
          glVertex2d(lft_pt.x(), lft_pt.y());
          glVertex2d(sh_pt.x(), sh_pt.y()); 
          glEnd();

          glBegin( GL_LINE_STRIP );
          glVertex2d(rft_pt.x(), rft_pt.y());
          glVertex2d(sh_pt.x(), sh_pt.y()); 
          glEnd();

          //at the end point
          sh_pt = sedge->getEndPt(); 
          lft_pt = sedge->getLFootPt(sedge->eTau());
          rft_pt = sedge->getRFootPt(sedge->eTau());

          glColor3f( 0.0f , 0.5f , 0.0f );
          glLineWidth( 1.0f);
          glBegin( GL_LINE_STRIP );
          glVertex2d(lft_pt.x(), lft_pt.y());
          glVertex2d(sh_pt.x(), sh_pt.y()); 
          glEnd();

          glBegin( GL_LINE_STRIP );
          glVertex2d(rft_pt.x(), rft_pt.y());
          glVertex2d(sh_pt.x(), sh_pt.y()); 
          glEnd();

        }
      }
    }
  }

  return dbsk2d_ishock_getinfo_tool::handle(e, view);
}

void dbsk2d_ishock_detector_debug_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                   vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add(((disp_foots)?on:off)+"Display shock projections", 
            bvis1_tool_toggle, (void*)(&disp_foots) );

}

