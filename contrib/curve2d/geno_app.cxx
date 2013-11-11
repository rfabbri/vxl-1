#include "geno_app.h"
#include <bpro1/bpro1_process.h>
#include <bgui/bgui_selector_tableau.h>
#include <bgui/bgui_selector_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vgui/vgui.h>
#include <vidpro1/vidpro1_repository.h>
#include <bpro1/bpro1_storage.h>
#include <bvis1/bvis1_tool_sptr.h>
#include <bvis1/menu/bvis1_command.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>


//: calls geno process w/default params
void 
call_geno_process()
{
   bpro1_process_sptr pro= MANAGER->process_manager()->get_process_by_name("Sample Curve");

   if (!MANAGER->active_tableau())
      return;

   vcl_string itype(MANAGER->storage_from_tableau(MANAGER->active_tableau())->type());
   if (itype != "vsol2D")
      return;

   vcl_string iname(MANAGER->storage_from_tableau(MANAGER->active_tableau())->name());

   vcl_vector<vcl_string> name_v;

   name_v.push_back(iname);
   pro->set_input_names(name_v);

   name_v[0] = "GENO Output";
   pro->set_output_names(name_v);

   vcl_set<bpro1_storage_sptr> modified;
   //now run the process
   MANAGER->process_manager()->run_process_on_current_frame(pro, &modified);


   // update the display for any modified storage objects
   for ( vcl_set<bpro1_storage_sptr>::iterator itr = modified.begin();
         itr != modified.end(); ++itr ) {
     bvis1_manager::instance()->add_to_display(*itr);
   }

   bvis1_manager::instance()->display_current_frame();
}

//: calls eno process w/default params
void 
call_eno_process()
{
   bpro1_process_sptr pro= MANAGER->process_manager()->get_process_by_name("Sample Function");

   if (!MANAGER->active_tableau())
      return;

   vcl_string itype(MANAGER->storage_from_tableau(MANAGER->active_tableau())->type());
   if (itype != "vsol2D")
      return;

   vcl_string iname(MANAGER->storage_from_tableau(MANAGER->active_tableau())->name());

   vcl_vector<vcl_string> name_v;

   name_v.push_back(iname);
   pro->set_input_names(name_v);

   name_v[0] = "ENO Output";
   pro->set_output_names(name_v);

   vcl_set<bpro1_storage_sptr> modified;
   //now run the process
   MANAGER->process_manager()->run_process_on_current_frame(pro, &modified);


   // update the display for any modified storage objects
   for ( vcl_set<bpro1_storage_sptr>::iterator itr = modified.begin();
         itr != modified.end(); ++itr ) {
     bvis1_manager::instance()->add_to_display(*itr);
   }

   bvis1_manager::instance()->display_current_frame();
}

void 
clear_all()
{
   vcl_cout << "clearing all\n";
   MANAGER->repository()->initialize(1);
   MANAGER->regenerate_all_tableaux();
   MANAGER->display_current_frame(true);
}


void
geno_reset()
{
   // Initialize/Clean up
   clear_all();
   geno_interactive();
}

void 
geno_interactive()
{
   // add grid if only one present
   vgui_grid_tableau_sptr pgrid;
   pgrid.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_grid_tableau"));

   if (pgrid->rows()*pgrid->cols() <= 1)
      MANAGER->add_new_view(0, false);
   MANAGER->display_current_frame();

   vcl_string type, name;
   // use active vsol if any; else create one for input
   if (MANAGER->active_tableau()) {
      type = MANAGER->storage_from_tableau(MANAGER->active_tableau())->type();
      if (type == "vsol2D")
         name = MANAGER->storage_from_tableau(MANAGER->active_tableau())->name();
   } 

   if (name.length() == 0) {
      type = "vsol2D";
      name = "GENO Input";
      bpro1_storage_sptr n_data = MANAGER->repository()->new_data(type,name);
      if (n_data) {
         MANAGER->add_to_display(n_data);
         MANAGER->display_current_frame();
      } else {
         vcl_cerr << "error: unable to register new data\n";
         return;
      }
   }

   vgui_tableau_sptr ptab;
   ptab = vgui_find_below_by_type_name(pgrid->get_tableau_at(1,0),"bgui_selector_tableau");

   // Makes input vsol invisible in 2nd view
   bgui_selector_tableau_sptr selector;
   selector.vertical_cast(ptab);

   selector->toggle(name);
   
   // Make new storage into a light yellow color
   bgui_vsol2D_tableau_sptr pvsol;
      pvsol.vertical_cast(selector->get_child(0));

   vgui_style_sptr sty = vgui_style::new_style(1,1,0,5,2);
   pvsol->set_vsol_polyline_2d_style(sty);
   MANAGER->post_redraw();

   // Activates polyline tool, in case no tool is activated
   if(!MANAGER->active_tool()) {
      bvis1_tool_sptr ptool = new bvis1_vsol2D_polyline_tool;
      bvis1_tool_command pcmd(ptool);
      pcmd.execute();
   }
}
