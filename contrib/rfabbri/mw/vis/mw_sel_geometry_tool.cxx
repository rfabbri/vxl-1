#include "mw_sel_geometry_tool.h"

#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#define MANAGER bvis1_manager::instance()

#include <dbdif/pro/dbdif_3rd_order_geometry_storage.h>

mw_sel_geometry_tool::
mw_sel_geometry_tool()
{
}


vcl_string mw_sel_geometry_tool::
name() const
{
  return "SEL Geometry";
}

void   
mw_sel_geometry_tool::
activate ()
{
  dbdet_sel_explorer_tool::activate();

  bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("3rdOrderGeometry",frame_v_[0]);

  gt_.vertical_cast(p);
  if(!gt_) {
    vcl_cerr << "Error: tool requires a 3rdOrderGeometry storage" << vcl_endl;
    return;
  }
  vcl_cout << "Geom Size: " << gt_->c().size() << vcl_endl;
}

void mw_sel_geometry_tool::
deactivate ()
{
  vcl_cout << "mw_sel_geometry_tool OFF\n";
  dbdet_sel_explorer_tool::deactivate();
}

bool mw_sel_geometry_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  bool stat = dbdet_sel_explorer_tool::handle(e,view);

  if (e.type == vgui_KEY_PRESS) {
    switch (e.key) {
      case 'g':
        print_ground_truth_info_at_edgel();
        return true;
        break;
      default:
        vcl_cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  return stat;
}

void mw_sel_geometry_tool::
print_ground_truth_info_at_edgel()
{
  if (!cur_edgel) {
    vcl_cout << "No selected edgel\n";
    return;
  }

  unsigned  i = cur_edgel->id -1;
  vcl_cout << 
    "Ground truth at id #" << i+1 <<
    "  k: " << gt_->c()[i].k <<
    "  kdot: " << gt_->c()[i].kdot << 
    vcl_endl;
}
