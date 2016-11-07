#include "mw_sel_tracing_tool.h"

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#define MANAGER bvis1_manager::instance()

#include <dbdet/sel/dbdet_sel.h>

#include <mw/mw_util.h>

mw_sel_tracing_tool::
mw_sel_tracing_tool()
  : 
  nviews_(1), 
  sel_storage_(0), 
  sel_(0), 
  p0_(0),
  p0_style_(vgui_style::new_style(1, 1, 0, 
                                    8.0f/*ptsize*/, 3.0f/*linesize*/))  // Yellow
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  frame_v_.resize(nviews_);
  tab_.resize(nviews_);
}


vcl_string mw_sel_tracing_tool::
name() const
{
  return "Multiview curvelet tracer";
}

bool mw_sel_tracing_tool::
set_tableau( const vgui_tableau_sptr& /*tableau*/ )
{
  return true;
}


void   
mw_sel_tracing_tool::
activate ()
{
  /* XXX must update for new SEL code
  vcl_cout << "mw_sel_tracing_tool ON\n";

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if (views.size() < nviews_) {
    vgui::out << "Error: need at least " << nviews_ << " views for this tool" << vcl_endl;
    vcl_cerr << "Error: need at least " << nviews_ << " views for this tool" << vcl_endl;
    return;
  }

  for (unsigned i=0; i<nviews_; ++i) {
    frame_v_[i] = views[i]->frame();
  }
  
  vcl_cout << "Working in frames ";
  for (unsigned i=0; i<nviews_; ++i) {
    vcl_cout << frame_v_[i] << "  " ;
  }
  vcl_cout << vcl_endl;

  bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("sel",frame_v_[0]);

  sel_storage_.vertical_cast(p);
  if(!sel_storage_) {
    vcl_cerr << "Error: tool requires a dbdet sel storage" << vcl_endl;
    return;
  }
  vcl_cout << "success" << vcl_endl;

  sel_ = sel_storage_->get_sel();

  // -------- Add tableaus to draw on

  vcl_string type("vsol2D");
  vcl_string name("mw_curve_tracer");

  for (unsigned i=0 ; i < nviews_; ++i) {

    bpro1_storage_sptr 
      n_data  = MANAGER->repository()->new_data_at(type,name,frame_v_[i]);

    if (n_data) {
       MANAGER->add_to_display(n_data);
    } else {
       vcl_cerr << "error: unable to register new data\n";
       return ;
    }

  }

  MANAGER->display_current_frame();

  for (unsigned i=0 ; i < nviews_; ++i) {
    vgui_tableau_sptr tab_ptr1 = views[i]->selector()->get_tableau(name);
    if (tab_ptr1) {
      tab_[i].vertical_cast(tab_ptr1);
    } else {
      vcl_cerr << "error: Could not find child tableaus in selector\n";
      return ;
    }

    vcl_string active;
    active = views[i]->selector()->active_name();

    views[i]->selector()->set_active(name);
    views[i]->selector()->active_to_top();
    views[i]->selector()->set_active(active);
  }
  */

}

void mw_sel_tracing_tool::
deactivate ()
{
  vcl_cout << "mw_sel_tracing_tool OFF\n";
}


bool mw_sel_tracing_tool::
handle( const vgui_event & /*e*/, 
        const bvis1_view_tableau_sptr& /*view*/ )
{

  /* XXX must update for new SEL code
  if (e.type == vgui_KEY_PRESS) {
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;

    switch (e.key) {
      case 'p': { // print info on the storage
        vcl_cout << "Name: " << sel_storage_->name() << vcl_endl;
        print_sel_stats();

        return true;
        break;
      }

      default:
        vcl_cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  if (gesture0_(e)) {
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;

    if (p0_) {
      tab_[0]->remove(p0_);
      tab_[0]->post_redraw();
    }

    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // I) Find edgel closest to ix,iy

    unsigned row_cell = (unsigned)iy;
    unsigned col_cell = (unsigned)ix;

    if (row_cell < 1 || col_cell < 1 ||
        row_cell > (unsigned )(sel_->nrows())-2 || col_cell > (unsigned )(sel_->ncols())-2) {
      vcl_cout << "Click out of bounds\n";
      return false;
    }
    vcl_cout << "ix,iy: " << ix << "," << iy << vcl_endl;

    double dmin = vcl_numeric_limits<double>::infinity();
    unsigned jcell_min = 0, icell_min = 0;
    unsigned imin = 0;

    for (unsigned icell = row_cell-1; icell <= row_cell+1; ++icell)
    for (unsigned jcell = col_cell-1; jcell <= col_cell+1; ++jcell) {
      for (unsigned i=0; i < sel_->cells()[icell][jcell].size(); ++i) {
        dbdet_edgel *edgel = sel_->cells()[icell][jcell][i];
        double dx =edgel->pt.x() - ix; 
        double dy =edgel->pt.y() - iy; 
        double d = dx*dx+dy*dy;
        if (d < dmin) {
          imin = i;
          dmin = d;
          jcell_min = jcell;
          icell_min = icell;
        }
      }
    }

    if (dmin == vcl_numeric_limits<double>::infinity()) {
      vcl_cout << "No closest edgel\n";
    } else {
      vgl_point_2d<double> pt = sel_->cells()[icell_min][jcell_min][imin]->pt;
      vcl_cout << "Closest edgel: " << pt << vcl_endl;
      // II) Add a point to the vsol tableau
      for (unsigned i=0 ; i < nviews_; ++i) 
        tab_[i]->set_current_grouping( "Drawing" );


      p0_ = tab_[0]->add_point(pt.x(),pt.y());
      p0_->set_style(p0_style_);

      tab_[0]->post_redraw();
      
      // III) Display info about the selected edgel
      vcl_cout << vcl_endl;
      print_edgel_stats(sel_->cells()[icell_min][jcell_min][imin]);
    }

  }
*/

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

/*
void mw_sel_tracing_tool::
print_sel_stats() const
{
  
  unsigned nquads = 0, nedgels = 0, max_nquads=0, min_nquads=(unsigned)-1;

  // print quads per edgel, max quads per edgel
  for (unsigned i = 0; i < (unsigned ) sel_->nrows(); ++i)
    for (unsigned j = 0; j < (unsigned )sel_->ncols(); ++j)
      for (unsigned k = 0; k < (unsigned )sel_->cells()[i][j].size() ; ++k) {
        dbdet_edgel *edgel = sel_->cells()[i][j][k];
        unsigned current_nquads
            = edgel->local_curvelets[2].size();
        nquads += current_nquads;
        if (current_nquads > max_nquads)
          max_nquads = current_nquads;
        if (current_nquads < min_nquads)
          min_nquads = current_nquads;
        nedgels++;
      }
  
  vcl_cout << "#edgels: " << nedgels << vcl_endl;
  vcl_cout << "#quads: " << nquads << vcl_endl;
  vcl_cout << "max  #quads per edgel: " << max_nquads << vcl_endl;
  vcl_cout << "min  #quads per edgel: " << min_nquads << vcl_endl;
  vcl_cout << "mean #quads per edgel: " << (double)nquads/(double)nedgels << vcl_endl;
}

void mw_sel_tracing_tool::
print_edgel_stats(dbdet_edgel * e) const
{
  vcl_cout << "========= EDGEL =========\n";
  vcl_cout << "    id: " << e->id << vcl_endl;  
  vcl_cout << "#pairs: " << e->local_curvelets[0].size() << vcl_endl;  
  vcl_cout << "#trips: " << e->local_curvelets[1].size() << vcl_endl;  
  vcl_cout << "#quads: " << e->local_curvelets[2].size() << vcl_endl;  

  // - number of curve bundles per curvelet
  // - max curvature, min curvature

  curvelet_list_iter it = e->local_curvelets[2].begin();
  unsigned n=1;
  for (; it != e->local_curvelets[2].end(); ++it, ++n) {
    vcl_cout << "   ==== Curvelet #" << n << " ====" << vcl_endl;
    vcl_cout << "Chain:"; 
    for (unsigned i=0; i < (*it)->edgel_chain.size(); ++i) {
      vcl_cout << " " << (*it)->edgel_chain[i]->id;
    }
    vcl_cout << vcl_endl;
    vcl_cout << "ref_edgel: " << (*it)->ref_edgel->id << "    " << vcl_endl;
    vcl_cout << " --- Curve Model:"; 
    if ( (*it)->curve_model ) {

      dbdet_ES_curve_model *crv_model = dynamic_cast<dbdet_ES_curve_model *> ((*it)->curve_model);
      if (!crv_model) {
        vcl_cerr << "Curve model must be Eulerspiral\n";
        abort();
      }

      vcl_cout << "k,kdot: " << crv_model->k <<
        ", " <<  crv_model->gamma << "    ";
    } else {
      vcl_cout << "NULL    ";
    }
    vcl_cout << vcl_endl;
  }
}
*/
